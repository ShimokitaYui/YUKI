#ifndef __MYOS__HARDWARECOMMUNICATION__INTERRUPTS_H
#define __MYOS__HARDWARECOMMUNICATION__INTERRUPTS_H

#include <gdt.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <multitasking.h>


namespace myos {
    namespace hardwarecommunication{

        class InterruptManager;

        class InterruptHandler{
        protected:
            myos::common::uint8_t interruptNumber;
            InterruptManager* interruptManager;

            InterruptHandler(myos::common::uint8_t interruptNumber, InterruptManager* interruptManager);
            ~InterruptHandler();
        public:
            virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
        };

        class InterruptManager{
        friend class InterruptHandler;
        protected:

            static InterruptManager* ActiveInterruptManager;
            myos::TaskManager* taskManager;
            InterruptHandler* handlers[256];


            struct GateDescriptor {
                myos::common::uint16_t handlerAddressLowBits;
                myos::common::uint16_t gdt_codeSegmentSelector;
                myos::common::uint8_t reserved;
                myos::common::uint8_t accesss;
                myos::common::uint16_t handlerAddressHighBits;
            } __attribute__((packed));

            static GateDescriptor interruptDescriptorTable[256];//IDT

            struct  InterruptDescriptorTablePointer {
                myos::common::uint16_t size;
                myos::common::uint32_t base;
            } __attribute__((packed));


            static void SetInterruptDescriptorTableEntry(
                myos::common::uint8_t interruptNumber,
                myos::common::uint16_t codeSegmentSelectorOffset,
                void (*handler)(),
                myos::common::uint8_t DescriptorPrivilegeLevel,
                myos::common::uint8_t DescriptorType
            );

            Port8BitSlow programmableInterruptControllerMasterCommand;
            Port8BitSlow programmableInterruptControllerMasterData;
            Port8BitSlow programmableInterruptControllerSlaveCommand;
            Port8BitSlow programmableInterruptControllerSlaveData;

        public:
            InterruptManager(myos::common::uint16_t hardwareInterruptOffset, myos::GlobalDescriptorTable* gdt, myos::TaskManager* taskManager);
            ~InterruptManager();

            myos::common::uint16_t HardwareInterruptOffset;

            void Activate();
            void Deactivate();

            static myos::common::uint32_t handleInterrupt(myos::common::uint8_t interruptNumber, myos::common::uint32_t esp);
            myos::common::uint32_t DoHandleInterrupt(myos::common::uint8_t interruptNumber, myos::common::uint32_t esp);
            static void IgnoreInterruptRequest();            
            static void HandleException0x00();
            static void HandleException0x01();
            static void HandleException0x02();
            static void HandleException0x03();
            static void HandleException0x04();
            static void HandleException0x05();
            static void HandleException0x06();
            static void HandleException0x07();
            static void HandleException0x08();
            static void HandleException0x09();
            static void HandleException0x0A();
            static void HandleException0x0B();
            static void HandleException0x0C();
            static void HandleException0x0D();
            static void HandleException0x0E();
            static void HandleException0x0F();
            static void HandleException0x10();
            static void HandleException0x11();
            static void HandleException0x12();
            static void HandleException0x13();

            static void HandleInterruptRequest0x00();
            static void HandleInterruptRequest0x01();
            static void HandleInterruptRequest0x02();
            static void HandleInterruptRequest0x03();
            static void HandleInterruptRequest0x04();
            static void HandleInterruptRequest0x05();
            static void HandleInterruptRequest0x06();
            static void HandleInterruptRequest0x07();
            static void HandleInterruptRequest0x08();
            static void HandleInterruptRequest0x09();
            static void HandleInterruptRequest0x0A();
            static void HandleInterruptRequest0x0B();
            static void HandleInterruptRequest0x0C();
            static void HandleInterruptRequest0x0D();
            static void HandleInterruptRequest0x0E();
            static void HandleInterruptRequest0x0F();
            static void HandleInterruptRequest0x31();
            static void HandleInterruptRequest0x80();


        };
    }
}
#endif