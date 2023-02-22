#ifndef __MYOS__DRIVERS__ATA_H
#define __MYOS__DRIVERS__ATA_H

#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <hardwarecommunication/port.h>

namespace myos{
    namespace drivers {
        class AdvancedTechologyAttachment{
        protected:
            myos::hardwarecommunication::Port16Bit dataPort;
            myos::hardwarecommunication::Port8Bit errorPort;
            myos::hardwarecommunication::Port8Bit sectorCountPort;
            myos::hardwarecommunication::Port8Bit lbaLowPort;
            myos::hardwarecommunication::Port8Bit lbaMidPort;
            myos::hardwarecommunication::Port8Bit lbaHiPort;
            myos::hardwarecommunication::Port8Bit devicePort;
            myos::hardwarecommunication::Port8Bit commandPort;
            myos::hardwarecommunication::Port8Bit controlPort;
            
            bool master;
            common::uint16_t bytesPerSector;
        public:
            AdvancedTechologyAttachment(common::uint16_t portBase, bool master);
            ~AdvancedTechologyAttachment();

            void Identify();
            void Read28(myos::common::uint32_t sector, myos::common::uint8_t* data, myos::common::int32_t count);
            void Write28(myos::common::uint32_t sector, myos::common::uint8_t* data, myos::common::int32_t count);
            void Flush();
        };
    }
}



#endif