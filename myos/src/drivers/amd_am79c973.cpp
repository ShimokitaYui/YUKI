#include <drivers/amd_am79c973.h>


using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

/*

struct InitializationBlock {
    myos::common::uint16_t mode;
    unsigned reserved1 : 4;
    unsigned numSendBuffers : 4;
    unsigned reserved2 : 4;
    unsigned numRecvBUffers : 4;
    myos::common::uint64_t physicalAddress : 48;
    myos::common::uint16_t reserved3;
    myos::common::uint64_t logicalAddress;
    myos::common::uint32_t recvBufferDescrAddress;
    myos::common::uint32_t sendBufferDescrAddress;
} __attribute__((packed));
struct BufferDescriptor {
    myos::common::uint32_t address;
    myos::common::uint32_t flags;
    myos::common::uint32_t flags2;
    myos::common::uint32_t avail;
} __attribute__((packed));


InitializationBlock init8Block;
BufferDescriptor* sendBUfferDescr;
myos::common::uint8_t sendBufferDescMemory[2048 + 15];
myos::common::uint8_t sendBuffers[2 * 1024 + 15][8];
myos::common::uint8_t currentSendBuffer;

BufferDescriptor* recvBUfferDescr;
myos::common::uint8_t recvBufferDescMemory[2048 + 15];
myos::common::uint8_t recvBuffers[2 * 1024 + 15][8];
myos::common::uint8_t currentRecvBuffer;
*/
void printfHex(uint8_t key);
void printf(char*);
amd_am79c973::amd_am79c973(PeripheralComponentInterconnectDeviceDescriptor* dev, InterruptManager* interrupts) 
: Driver(),
    InterruptHandler(dev->interrupt + interrupts->HardwareInterruptOffset, interrupts),
    MACAddress0Port(dev->portBase),
    MACAddress2Port(dev->portBase + 0x02),
    MACAddress4Port(dev->portBase + 0x04),
    registerDataPort(dev->portBase + 0x10),
    registerAddressPort(dev->portBase + 0x12),
    resetPort(dev->portBase + 0x14),
    busControlRegisterDataPort(dev->portBase + 0x16)
{
    currentSendBuffer = 0;
    currentSendBuffer = 0;
    
    //read MAC Address
    uint64_t MAC0 = MACAddress0Port.Read() % 256;
    uint64_t MAC1 = MACAddress0Port.Read() / 256;
    uint64_t MAC2 = MACAddress2Port.Read() % 256;
    uint64_t MAC3 = MACAddress2Port.Read() / 256;
    uint64_t MAC4 = MACAddress4Port.Read() % 256;
    uint64_t MAC5 = MACAddress4Port.Read() / 256;

    uint64_t MAC = MAC5 << 40 
                |  MAC4 << 32
                |  MAC3 << 24
                |  MAC2 << 16
                |  MAC1 << 8
                |  MAC0;
    //set 32 bit mode
    registerAddressPort.Write(20);
    busControlRegisterDataPort.Write(0x102); //20 register write 0x102

    //STOP reset
    registerAddressPort.Write(0);
    registerDataPort.Write(0x04);

    // initBlock
    init8Block.mode = 0x0000; //promiscuous mode = false
    init8Block.reserved1 = 0;
    init8Block.numSendBuffers = 3; //8 
    init8Block.reserved2 = 0;
    init8Block.numRecvBUffers = 3;
    init8Block.physicalAddress = MAC;
    init8Block.reserved3 = 0;
    init8Block.logicalAddress = 0;
    
    sendBufferDescr = (BufferDescriptor*)(((uint32_t)(&sendBufferDescMemory[0]) + 15) & ~((uint32_t)0xF));
    init8Block.sendBufferDescrAddress = (uint32_t)sendBufferDescr;

    recvBufferDescr = (BufferDescriptor*)(((uint32_t)(&recvBufferDescMemory[0]) + 15) & ~((uint32_t)0xF));
    init8Block.recvBufferDescrAddress = (uint32_t)recvBufferDescr;

    for(uint8_t i = 0; i < 8; i++) {
        sendBufferDescr[i].address = (((uint32_t)&sendBufferDescMemory[i]) + 15) & ~(uint32_t)0xF;
        sendBufferDescr[i].flags = 0x7FF
                                |  0xF000;
        sendBufferDescr[i].flags2 = 0;
        sendBufferDescr[i].avail = 0;

        recvBufferDescr[i].address = (((uint32_t)&recvBufferDescMemory[i]) + 15) & ~(uint32_t)0xF;
        recvBufferDescr[i].flags = 0xF7FF
                                |  0x80000000; // tells it is recvBuffer
        recvBufferDescr[i].flags2 = 0;
        recvBufferDescr[i].avail = 0;
    }

    registerAddressPort.Write(1);
    registerDataPort.Write((uint32_t)(&init8Block) & 0xFFFF);

    registerAddressPort.Write(2);
    registerDataPort.Write(((uint32_t)(&init8Block) >> 16) & 0xFFFF);
}

amd_am79c973::~amd_am79c973() {

}

void amd_am79c973::Activate() {
    registerAddressPort.Write(0);
    registerDataPort.Write(0x41);//enable interrupts
    
    registerAddressPort.Write(4);
    uint32_t temp = registerDataPort.Read();
    registerAddressPort.Write(4);
    registerDataPort.Write(temp | 0xC00);

    registerAddressPort.Write(0);
    registerDataPort.Write(0x42);
}

int amd_am79c973::Reset() {
    resetPort.Read();
    resetPort.Write(0);
    return 10; //wait for ten seconds
}


uint32_t amd_am79c973::HandleInterrupt(uint32_t esp) {
    printf("INTERRUPT FROM AMD am79c973\n");

    registerAddressPort.Write(0);
    uint32_t temp = registerDataPort.Read();

    if((temp & 0x8000) == 0x8000) printf("AMD am79c973 ERROR\n");
    if((temp & 0x2000) == 0x2000) printf("AMD am79c973 COLLISION ERROR\n");
    if((temp & 0x1000) == 0x1000) printf("AMD am79c973 MISSED FRAME\n");
    if((temp & 0x0800) == 0x0800) printf("AMD am79c973 MEMORY ERROR\n");
    if((temp & 0x0400) == 0x0400) Receive();
    if((temp & 0x0200) == 0x0200) printf("AMD am79c973 DATA SENT\n");

    //acknoledge
    registerAddressPort.Write(0);
    registerDataPort.Write(temp);

    if((temp & 0x0100) == 0x0100) {
        printf("AMD am79c973 INIT DONE\n"); 
    }
    return esp;
}

void amd_am79c973::Send(uint8_t* buffer, int32_t size) { 
    int32_t sendDescriptor = currentSendBuffer;
    currentSendBuffer = (currentSendBuffer + 1) % 8;

    if(size > 1518) { //需要分片
        size = 1518;//丢弃了剩下的内容
    }
    for(uint8_t *src = buffer + size - 1,
                *dst = (uint8_t*)(sendBufferDescr[sendDescriptor].address + size - 1);
                src >= buffer; src--, dst--) { // 把内容复制到发送缓存区
        *dst = *src;
    }
    sendBufferDescr[sendDescriptor].avail = 0;
    sendBufferDescr[sendDescriptor].flags2 = 0; //清楚错误信息
    sendBufferDescr[sendDescriptor].flags = 0x8300F000
                                            | ((uint16_t)((-size) & 0xFFFF));
    registerAddressPort.Write(0);
    registerDataPort.Write(0x48);//command
}

void amd_am79c973::Receive() { 
    printf("AMD am79c973 DATA RECEIVED\n");

    for(;(recvBufferDescr[currentRecvBuffer].flags & 0x80000000) == 0; currentRecvBuffer = (currentRecvBuffer + 1) % 8) {
        if(!(recvBufferDescr[currentRecvBuffer].flags & 0x40000000) //检查error bit
        &&  ((recvBufferDescr[currentRecvBuffer].flags & 0x03000000) == 0x03000000)) { // 检查start of packet and end of packet bits
            uint32_t size = recvBufferDescr[currentRecvBuffer].flags & 0xFFF;
            if(size > 64) {
                size -= 4; // 忽略校验和
            }
            uint8_t* buffer = (uint8_t*)(recvBufferDescr[currentRecvBuffer].address);
            for(int32_t i = 0; i < size; i++) {
                printfHex(buffer[i]);
                printf(" ");
            }
            printf("\n");
        }
        recvBufferDescr[currentRecvBuffer].flags2 = 0;
        recvBufferDescr[currentRecvBuffer].flags = 0x8000F7FF;
    }
}