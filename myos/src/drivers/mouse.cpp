#include <drivers/mouse.h>
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char* str);


MouseEventHandler::MouseEventHandler() {
    
}
void MouseEventHandler::OnActivate() {

}

void MouseEventHandler::OnMouseDown(uint8_t button) {

}

void MouseEventHandler::OnMouseUp(uint8_t button) {

}

void MouseEventHandler::OnMouseMove(int32_t xoffset, int32_t yoffset) {

}



MouseDriver::MouseDriver(InterruptManager* manager,MouseEventHandler* mhandler) 
: InterruptHandler(0x2C, manager),
handler(mhandler),
dataport(0x60),
commandport(0x64) {

}

MouseDriver::~MouseDriver() {
}

void MouseDriver::Activate() {
    
    offset = 0;
    buttons = 0;

    commandport.Write(0xAB);//tells the pic to start send a mouse interrupt 
                            //tells mouse to communicate with interrupt
    commandport.Write(0x20);//give us your current state
    uint8_t status = dataport.Read() | 2;

    commandport.Write(0x60);//set state
    dataport.Write(status);

    commandport.Write(0XD4);
    dataport.Write(0xF4);//Activate Mouse
    dataport.Read();
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp) {
    uint8_t status = commandport.Read();
    if(!(status & 0x20))
        return esp;

    buffer[offset] = dataport.Read();
    if((!(status & 0x20)) || handler == 0) 
        return esp;

    offset = (offset + 1) % 3;

    if(offset == 0) {
        if(buffer[1] != 0 || buffer[2] != 0) { 
            handler->OnMouseMove((int8_t)buffer[1],-((int8_t)buffer[2]));
        }

        for(uint8_t i = 0; i < 3; i++) {
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i))) { 
                if(buttons & (0x1 << 1))
                    handler->OnMouseUp(i + 1);
                else 
                    handler->OnMouseDown(i + 1);
            }
        }
    buttons = buffer[0];
    }
    return esp;
}