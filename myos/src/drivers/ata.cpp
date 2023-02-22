#include <drivers/ata.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char*);

AdvancedTechologyAttachment::AdvancedTechologyAttachment(uint16_t portBase, bool master) 
: dataPort(portBase),
errorPort(portBase + 1),
sectorCountPort(portBase + 2),
lbaLowPort(portBase + 3),
lbaMidPort(portBase + 4),
lbaHiPort(portBase + 5),
devicePort(portBase + 6),
commandPort(portBase + 7),
controlPort(portBase + 0x206) {
    bytesPerSector = 512;
    this->master = master;
}

AdvancedTechologyAttachment::~AdvancedTechologyAttachment() {

}

void AdvancedTechologyAttachment::Identify() {
    devicePort.Write(master ? 0xA0 : 0xB0);
    controlPort.Write(0); //clear hov

    devicePort.Write(0xA0);
    uint8_t status = commandPort.Read();
    if(status == 0xFF) {
        return;
    }
    devicePort.Write(master ? 0xA0 : 0xB0);
    sectorCountPort.Write(0);
    lbaLowPort.Write(0);
    lbaMidPort.Write(0);
    lbaHiPort.Write(0);
    commandPort.Write(0xEC);

    status = commandPort.Read();
    if(status == 0x00) {
        return; //no device
    }

    while(((status & 0x80) == 0x80) //busy 
        && ((status & 0x01) != 0x01))//error
        status = commandPort.Read();
    if(status & 0x01) {
        printf("ERROR");
        return;
    }

    for(uint16_t i = 0; i < 256; i++) {
        uint16_t data = dataPort.Read();
        char* foo = "  \0";
        foo[1] = (data >> 8) & 0x00FF;
        foo[0] = data & 0x00FF;
        printf(foo);
    }

}

void printfHex(uint8_t key);

void AdvancedTechologyAttachment::Read28(uint32_t sector, uint8_t* data, int32_t count) {
    if(sector & 0xF0000000) 
        return;  
    if(count > bytesPerSector) 
        return;
    /*
    8bit:
    1 MBS
    1 LAB
    1 MBS
    0 MASTER | 1 SLAVE
    .
    .
    .
    .LAB 24-27
    */
    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));

    errorPort.Write(0);
    sectorCountPort.Write(1);//读取或写入一个扇区

    lbaLowPort.Write(sector & 0x000000FF);
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHiPort.Write((sector & 0x00FF0000) >> 16);
    commandPort.Write(0x20);

    uint8_t status = commandPort.Read();

    while(((status & 0x80) == 0x80) //busy 
        && ((status & 0x01) != 0x01))//error
        status = commandPort.Read();
    if(status & 0x01) {
        printf("ERROR");
        return;
    }

    printf(" Reading from ATA: ");
    for(uint16_t i = 0; i < count; i += 2) {
        uint16_t wdata = dataPort.Read();
        
        // char* foo = "  \0";
        // foo[1] = (char)((wdata >> 8) & 0x00FF);
        // foo[0] = (char)(wdata & 0x00FF);
        
        printfHex((uint8_t)((wdata >> 8) & 0x00FF));
        printfHex((uint8_t)(wdata & 0x00FF));
        data[i] = (uint8_t)(wdata & 0x00FF);
        if(i + 1 < count) {
            data[i + 1] = (uint8_t)((wdata >> 8) & 0x00FF);
        }
    }

    for(int i = count + (count % 2); i < bytesPerSector; i += 2) {
        dataPort.Read();
    }

}

void AdvancedTechologyAttachment::Write28(uint32_t sector, uint8_t* data, int32_t count) {
    if(sector & 0xF0000000) 
        return;  
    if(count > bytesPerSector) 
        return;
    /*
    8bit:
    1 MBS
    1 LAB
    1 MBS
    0 MASTER | 1 SLAVE
    .
    .
    .
    .LAB 24-27
    */
    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));

    errorPort.Write(0);
    sectorCountPort.Write(1);//读取或写入一个扇区

    lbaLowPort.Write(sector & 0x000000FF);
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHiPort.Write((sector & 0x00FF0000) >> 16);
    commandPort.Write(0x30);

    printf("Writing to ATA: ");
    for(uint16_t i = 0; i < count; i += 2) {
        uint16_t wdata =  data[i];
        printfHex(data[i]);
        if(i + 1 < count) {
            wdata |= ((uint16_t)data[i + 1]) << 8;
            printfHex(data[i + 1]);
        }
        // char* foo = "  \0";
        // foo[1] = (wdata >> 8) & 0x00FF;
        // foo[0] = wdata & 0x00FF;
        // printf(foo);
        
        dataPort.Write(wdata);
    }

    for(uint16_t i = count + (count % 2); i < bytesPerSector; i += 2) {
        dataPort.Write(0x0000);
    }
}

void AdvancedTechologyAttachment::Flush() {
    devicePort.Write((master ? 0xE0 : 0xF0) );
    commandPort.Write(0xE7);

    uint8_t status = commandPort.Read();

    while(((status & 0x80) == 0x80) //busy 
        && ((status & 0x01) != 0x01))//error
        status = commandPort.Read();
    if(status & 0x01) {
        printf("ERROR");
        return;
    }

}
