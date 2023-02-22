
#ifndef __MYOS__DRIVERS__DRIVER_H
#define __MYOS__DRIVERS__DRIVER_H
#include <common/types.h>

namespace myos {
    namespace drivers{
        class Driver {
        public:
            Driver();
            ~Driver();
        
            virtual void Activate();
            virtual int Reset();
            virtual void Deactivate();
        };
        
        class DriverManager {
        public: // private is a better idea
            Driver* drivers[256];
            myos::common::int32_t numDrivers;
        
        public:
            DriverManager();
            void AddDriver(Driver*);
            void ActivateAll();
        
        };
    }
}
#endif