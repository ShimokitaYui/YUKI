#include <drivers/keyboard.h>
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char*);
void printfHex(uint8_t);


KeyboardEventHandler::KeyboardEventHandler() {

}

void KeyboardEventHandler::OnKeyDown(int8_t) {

}

void KeyboardEventHandler::OnKeyUp(int8_t) {

}


KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* kbhandler) 
: InterruptHandler(0x21, manager),
handler(kbhandler),
dataport(0x60),
commandport(0x64) {
}

KeyboardDriver::~KeyboardDriver() {

}

void KeyboardDriver::Activate() {
    while(commandport.Read() & 0x1) {
        dataport.Read();
    }
    commandport.Write(0xAE);//tells the pic to start send a keyboard interrupt 
                            //tells keyboard to communicate with interrupt
    commandport.Write(0x20);//give us your current state
    uint8_t status = (dataport.Read() | 1) & ~0x10;

    commandport.Write(0x60);//set state
    dataport.Write(status);

    dataport.Write(0xF4);//Activate Keyboard
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp) {
    uint8_t key = dataport.Read();

    if(handler == 0)
        return esp;
    switch (key) { 
        case 0x02: handler->OnKeyDown('1');break;
        case 0x03: handler->OnKeyDown('2');break;
        case 0x04: handler->OnKeyDown('3');break;
        case 0x05: handler->OnKeyDown('4');break;
        case 0x06: handler->OnKeyDown('5');break;
        case 0x07: handler->OnKeyDown('6');break;
        case 0x08: handler->OnKeyDown('7');break;
        case 0x09: handler->OnKeyDown('8');break;
        case 0x0A: handler->OnKeyDown('9');break;
        case 0x0B: handler->OnKeyDown('0');break;
        case 0x0C: handler->OnKeyDown('-');break;
        case 0x0D: handler->OnKeyDown('+');break;

        case 0x0E: printf("key-Backspace");break;
        case 0x0F: printf("key-Tab");break;

        case 0x10: handler->OnKeyDown('q');break;
        case 0x11: handler->OnKeyDown('w');break;
        case 0x12: handler->OnKeyDown('e');break;
        case 0x13: handler->OnKeyDown('r');break;
        case 0x14: handler->OnKeyDown('t');break;
        case 0x15: handler->OnKeyDown('y');break;
        case 0x16: handler->OnKeyDown('u');break;
        case 0x17: handler->OnKeyDown('i');break;
        case 0x18: handler->OnKeyDown('o');break;
        case 0x19: handler->OnKeyDown('p');break;

        case 0x1A: handler->OnKeyDown('[');break;
        case 0x1B: handler->OnKeyDown(']');break;
        case 0x1C: handler->OnKeyDown('\n');break;
        case 0x1D: printf("key-left-ctrl");break;
        case 0x1E: handler->OnKeyDown('a');break;
        case 0x1F: handler->OnKeyDown('s');break;
        case 0x20: handler->OnKeyDown('d');break;
        case 0x21: handler->OnKeyDown('f');break;
        case 0x22: handler->OnKeyDown('g');break;
        case 0x23: handler->OnKeyDown('h');break;
        case 0x24: handler->OnKeyDown('j');break;
        case 0x25: handler->OnKeyDown('k');break;
        case 0x26: handler->OnKeyDown('l');break;

        case 0x27: handler->OnKeyDown(';');break;
        case 0x28: handler->OnKeyDown('\'');break;
        case 0x29: handler->OnKeyDown('`');break;
        case 0x2A: break;//shift
        case 0x2B: handler->OnKeyDown('\\');break;

        case 0x2C: handler->OnKeyDown('z');break;
        case 0x2D: handler->OnKeyDown('x');break;
        case 0x2E: handler->OnKeyDown('c');break;
        case 0x2F: handler->OnKeyDown('v');break;
        case 0x30: handler->OnKeyDown('b');break;
        case 0x31: handler->OnKeyDown('n');break;
        case 0x32: handler->OnKeyDown('m');break;

        case 0x33: handler->OnKeyDown(',');break;
        case 0x34: handler->OnKeyDown('.');break;
        case 0x35: handler->OnKeyDown('/');break;
        case 0x36: break;//shift 
        case 0x37: handler->OnKeyDown('*');break;
        case 0x38: printf("key-Alt");break;
        case 0x39: handler->OnKeyDown(' ');break;
        case 0x3A: break;//CapsLock
        case 0x3B: printf("key-F1");break;
        case 0x3C: printf("key-F2");break;
        case 0x3D: printf("key-F3");break;
        case 0x3E: printf("key-F4");break;
        case 0x3F: printf("key-F5");break;
        case 0x40: printf("key-F6");break;
        case 0x41: printf("key-F7");break;
        case 0x42: printf("key-F8");break;
        case 0x43: printf("key-F9");break;
        case 0x44: printf("key-F10");break;
        case 0x45: printf("key-NumLock");break;
        case 0x47: printf("key-Home");break;
        case 0x48: printf("key-Up");break;
        case 0x49: printf("key-PageUp");break;
        case 0x4A: handler->OnKeyDown('-');break;
        case 0x4B: printf("key-Left");break;

        case 0x4D: printf("key-Right");break;
        case 0x4E: handler->OnKeyDown('+');break;
        case 0x4F: printf("key-End");break;
        case 0x50: printf("key-Down");break;
        case 0x51: printf("key-PageDown");break;
        case 0x52: printf("key-Insert");break;
        case 0x53: printf("key-Delete");break;

        case 0x57: printf("key-F11");break;
        case 0x58: printf("key-F12");break;

        case 0xC5: break;
        case 0xAA: case 0x9C:break;//shift-realse
        case 0xFA: break;

        default:
            if(key < 0x80) {
                printf("KEYBOARD 0x");
                printfHex(key);
                break;
            }
    }


    // char* ans = " ";
    // static bool Shift = false;
    // static bool CapsLock = false;
    // switch (key) { 
    //     case 0x02: if(Shift) printf("!"); else printf("1");break;
    //     case 0x03: if(Shift) printf("@"); else printf("2");break;
    //     case 0x04: if(Shift) printf("#"); else printf("3");break;
    //     case 0x05: if(Shift) printf("$"); else printf("4");break;
    //     case 0x06: if(Shift) printf("%"); else printf("5");break;
    //     case 0x07: if(Shift) printf("^"); else printf("6");break;
    //     case 0x08: if(Shift) printf("&"); else printf("7");break;
    //     case 0x09: if(Shift) printf("*"); else printf("8");break;
    //     case 0x0A: if(Shift) printf("("); else printf("9");break;
    //     case 0x0B: if(Shift) printf(")"); else printf("0");break;
    //     case 0x0C: if(Shift) printf("_"); else printf("-");break;
    //     case 0x0D: if(Shift) printf("+"); else printf("+");break;

    //     case 0x0E: printf("key-Backspace");break;
    //     case 0x0F: printf("key-Tab");break;

    //     case 0x10: if(Shift || CapsLock) printf("Q"); else printf("q");break;
    //     case 0x11: if(Shift || CapsLock) printf("W"); else printf("w");break;
    //     case 0x12: if(Shift || CapsLock) printf("E"); else printf("e");break;
    //     case 0x13: if(Shift || CapsLock) printf("R"); else printf("r");break;
    //     case 0x14: if(Shift || CapsLock) printf("T"); else printf("t");break;
    //     case 0x15: if(Shift || CapsLock) printf("Y"); else printf("y");break;
    //     case 0x16: if(Shift || CapsLock) printf("U"); else printf("u");break;
    //     case 0x17: if(Shift || CapsLock) printf("I"); else printf("i");break;
    //     case 0x18: if(Shift || CapsLock) printf("O"); else printf("o");break;
    //     case 0x19: if(Shift || CapsLock) printf("P"); else printf("p");break;

    //     case 0x1A: if(Shift) printf("{"); else printf("[");break;
    //     case 0x1B: if(Shift) printf("}"); else printf("]");break;
    //     case 0x1C: printf("\n");break;
    //     case 0x1D: printf("key-left-ctrl");break;
    //     case 0x1E: if(Shift || CapsLock) printf("A"); else printf("a");break;
    //     case 0x1F: if(Shift || CapsLock) printf("S"); else printf("s");break;
    //     case 0x20: if(Shift || CapsLock) printf("D"); else printf("d");break;
    //     case 0x21: if(Shift || CapsLock) printf("F"); else printf("f");break;
    //     case 0x22: if(Shift || CapsLock) printf("G"); else printf("g");break;
    //     case 0x23: if(Shift || CapsLock) printf("H"); else printf("h");break;
    //     case 0x24: if(Shift || CapsLock) printf("J"); else printf("j");break;
    //     case 0x25: if(Shift || CapsLock) printf("K"); else printf("k");break;
    //     case 0x26: if(Shift || CapsLock) printf("L"); else printf("l");break;

    //     case 0x27: if(Shift) printf(":"); else printf(";");break;
    //     case 0x28: if(Shift) printf("\""); else printf("'");break;
    //     case 0x29: if(Shift) printf("~"); else printf("`");break;
    //     case 0x2A: Shift = true;break;
    //     case 0x2B: if(Shift) printf("|"); else printf("\\");break;

    //     case 0x2C: if(Shift || CapsLock) printf("Z"); else printf("z");break;
    //     case 0x2D: if(Shift || CapsLock) printf("X"); else printf("x");break;
    //     case 0x2E: if(Shift || CapsLock) printf("C"); else printf("c");break;
    //     case 0x2F: if(Shift || CapsLock) printf("V"); else printf("v");break;
    //     case 0x30: if(Shift || CapsLock) printf("B"); else printf("b");break;
    //     case 0x31: if(Shift || CapsLock) printf("N"); else printf("n");break;
    //     case 0x32: if(Shift || CapsLock) printf("M"); else printf("m");break;

    //     case 0x33: if(Shift) printf("<"); else printf(",");break;
    //     case 0x34: if(Shift) printf(">"); else printf(".");break;
    //     case 0x35: if(Shift) printf("?"); else printf("/");break;
    //     case 0x36:Shift = true;break; 
    //     case 0x37: printf("*");break;
    //     case 0x38: printf("key-Alt");break;
    //     case 0x39: printf(" ");break;
    //     case 0x3A: CapsLock = !CapsLock;break;
    //     case 0x3B: printf("key-F1");break;
    //     case 0x3C: printf("key-F2");break;
    //     case 0x3D: printf("key-F3");break;
    //     case 0x3E: printf("key-F4");break;
    //     case 0x3F: printf("key-F5");break;
    //     case 0x40: printf("key-F6");break;
    //     case 0x41: printf("key-F7");break;
    //     case 0x42: printf("key-F8");break;
    //     case 0x43: printf("key-F9");break;
    //     case 0x44: printf("key-F10");break;
    //     case 0x45: printf("key-NumLock");break;
    //     case 0x47: printf("key-Home");break;
    //     case 0x48: printf("key-Up");break;
    //     case 0x49: printf("key-PageUp");break;
    //     case 0x4A: printf("-");break;
    //     case 0x4B: printf("key-Left");break;

    //     case 0x4D: printf("key-Right");break;
    //     case 0x4E: printf("+");break;
    //     case 0x4F: printf("key-End");break;
    //     case 0x50: printf("key-Down");break;
    //     case 0x51: printf("key-PageDown");break;
    //     case 0x52: printf("key-Insert");break;
    //     case 0x53: printf("key-Delete");break;

    //     case 0x57: printf("key-F11");break;
    //     case 0x58: printf("key-F12");break;

    //     case 0xC5: break;
    //     case 0xAA: case 0x9C:Shift = false;break;
    //     case 0xFA: break;
        
    //     default:
    //         if(key <= 0x80) {
    //             printf("KEYBOARD 0x");
    //             printfHex(key);
    //         }
    //         break;
    // }
    return esp;
}