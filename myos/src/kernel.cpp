#include <common/types.h>
#include <common/multiboot.h>
#include <gdt.h>
#include <multitasking.h>
#include <memorymagement.h>
#include <syscalls.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <gui/widget.h>
#include <gui/desktop.h>
#include <gui/window.h>

#include <drivers/amd_am79c973.h>

using namespace myos;
using namespace myos::drivers;
using namespace myos::common;
using namespace myos::hardwarecommunication;
using namespace myos::gui;



// #define GRAPHICSMODE

void printf(char* str) {
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x = 0, y = 0;
    for(int32_t i = 0; str[i] != '\0'; i++) {
        switch(str[i]) {
            case '\n':
                y++; x = 0;
                break;
            default:
                VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
                x++;
                break;
        } 
        if(x >= 80) {
            x = 0;
            y++;
        }

        if(y >= 25) {
            for(y = 0; y < 25; y++) {
                for(x = 0; x < 80; x++) {
                    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | ' ';
                }
            }
            x = 0;
            y = 0;
        }
    }

}

void printfHex(uint8_t key) {
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0x0F];
    foo[1] = hex[key& 0x0F];
    printf(foo);
}

class PrintKeyboardEventHandler : public KeyboardEventHandler {
public:
    void OnKeyDown(char c) {
        char* foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler {
    int8_t x,y;
public:
    MouseToConsole() {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4)
                                | ((VideoMemory[80 * y + x] & 0x0F00) << 4)
                                | (VideoMemory[80 * y + x] & 0x00FF);
    }

    void OnActivate() {

    }

    void OnMouseMove(int32_t xoffset, int32_t yoffset) {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4)
                        | ((VideoMemory[80 * y + x] & 0x0F00) << 4)
                        | (VideoMemory[80 * y + x] & 0x00FF);
        x += xoffset;
        if(x < 0) x = 0;
        if(x >= 80) x = 79;
        y += yoffset;
        if(y < 0) y = 0;
        if(y >= 24) y = 23;
        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4)
                                | ((VideoMemory[80 * y + x] & 0x0F00) << 4)
                                | (VideoMemory[80 * y + x] & 0x00FF);
    }

};
void sysprintf(char*str) {
    asm("int $0x80" : : "a" (4), "b" (str));
}
void taskA() {
    while (true)
    {
        sysprintf("A");
    }
}

void taskB() {
    while (true)
    {
        sysprintf("B");
    }
    
}


typedef void (*constructor) ();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors() {
    for(constructor* i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

extern "C" void kernelMain(multiboot_info* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("-------------------------- YuKiNo Opertaing System -----------------------------");
    GlobalDescriptorTable gdt;
    uint32_t* memupper = (uint32_t*)multiboot_structure->mem_upper;
    size_t heap = 10 * 1024 * 1024;//10MB
    MemoryManager memoryManager(heap, (*memupper) * 1024 - heap - 10 * 1024);
    
    printf("sizeof(MemoryChunk):0x");
    printfHex(sizeof(MemoryChunk));

    printf("\nheap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8 ) & 0xFF);
    printfHex((heap      ) & 0xFF);

    void* allocated = memoryManager.malloc(1024);

    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xFF);
    printfHex(((size_t)allocated >> 16) & 0xFF);
    printfHex(((size_t)allocated >> 8 ) & 0xFF);
    printfHex(((size_t)allocated      ) & 0xFF);
    printf("\n");

    void* allocated_1 = memoryManager.malloc(1024);

    printf("\nallocated_1: 0x");
    printfHex(((size_t)allocated_1 >> 24) & 0xFF);
    printfHex(((size_t)allocated_1 >> 16) & 0xFF);
    printfHex(((size_t)allocated_1 >> 8 ) & 0xFF);
    printfHex(((size_t)allocated_1      ) & 0xFF);
    printf("\n");

    TaskManager taskManager;
    /*
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    */
    InterruptManager interrupts(0x20, &gdt, &taskManager);
    SyscallHandler syscalls(0x80, &interrupts);
    printf("Initializing Hardware, Stage 1\n");
    #ifdef GRAPHICSMODE
    Desktop desktop(320, 200, 0x00, 0x00, 0xA8);
    #endif
    DriverManager drvManager;

    #ifdef GRAPHICSMODE
        MouseDriver mouse(&interrupts, &desktop);
    #endif
    #ifndef GRAPHICSMODE
        MouseToConsole mousehandler;
        MouseDriver mouse(&interrupts, &mousehandler);
    #endif
        drvManager.AddDriver(&mouse);
    #ifdef GRAPHICSMODE
        KeyboardDriver keyboard(&interrupts, &desktop);
    #endif
    #ifndef GRAPHICSMODE
        PrintKeyboardEventHandler kbhandler;
        KeyboardDriver keyboard(&interrupts, &kbhandler);
    #endif
        drvManager.AddDriver(&keyboard);

        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);
    #ifdef GRAPHICSMODE
        VideoGraphicsArray vga;
    #endif

        printf("Initializing Hardware, Stage 2\n");
        drvManager.ActivateAll();
    printf("Initializing Hardware, Stage 3\n");
    #ifdef GRAPHICSMODE
    vga.SetMode(320, 200, 8);

    Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
    desktop.AddChild(&win1);
    Window win2(&desktop, 30, 15, 30, 30, 0x00, 0xA8, 0x00);
    desktop.AddChild(&win2);
    #endif
    /*
    amd_am79c973* eth8 = (amd_am79c973*)(drvManager.drivers[2]); //第三个设备
    eth8->Send((uint8_t*)"Hello Network", 13);
    */

   //interrupt 14

    AdvancedTechologyAttachment ata0m(0x1F0, true);
    printf("ATA Primary Master : ");
    ata0m.Identify();
    AdvancedTechologyAttachment ata0s(0X1F0, false);
    printf("ATA Primary Slave : ");
    ata0s.Identify();
    uint8_t yuki = 20;
    uint8_t* atabuffer = &yuki;
    ata0s.Write28(0, (uint8_t*)atabuffer, 1);
    ata0s.Flush();
    ata0s.Read28(0, (uint8_t*)atabuffer, 1);

    //interrupt 15
    AdvancedTechologyAttachment ata1m(0x170, true);
    AdvancedTechologyAttachment ata1s(0X170, false);

    //third 0x1E8
    //forth 0x168

    interrupts.Activate();
    while(1) {
    #ifdef GRAPHICSMODE
        desktop.Draw(&vga);
    #endif
    }
}
