
#include <drivers/vga.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
/*
Port8Bit miscPort;
Port8Bit ctrcIndexPort;
Port8Bit crtcDataPort;
Port8Bit sequencerIndexPort;
Port8Bit sequencerDataPort;
Port8Bit graphicsControllerIndexPort;
Port8Bit graphicsControllerDataPort;
Port8Bit attributeControllerIndexPort;
Port8Bit attributeControllerReadPort;
Port8Bit attributeControllerWritePort;
Port8Bit attributeControllerResetPort;
*/
VideoGraphicsArray::VideoGraphicsArray()
:miscPort(0x3C2), //这是杂项输出寄存器。它使用端口0x3C2进行写入，使用0x3CC进行读取。此寄存器的位0控制其他几个寄存器的位置：如果清除，则端口0x3D4映射到0x3B4，端口0x3DA映射到0x3A。为了可读性，只列出了第一个端口，假设设置了位0。
ctrcIndexPort(0x3D4),
crtcDataPort(0x3D5),
sequencerIndexPort(0x3C4),
sequencerDataPort(0x3C5),
graphicsControllerIndexPort(0x3CE),
graphicsControllerDataPort(0x3CF),
attributeControllerIndexPort(0x3C0),//将索引和数据字节写入同一端口。VGA会跟踪下一次写入应该是索引还是数据字节。然而，初始状态未知。通过从端口0x3A读取，它将进入索引状态。要读取内容，请将索引输入到0x3C0端口，然后从0x3C1读取值（然后读取0x3DA，因为VGA下一步需要数据字节还是索引字节尚未定义）。
attributeControllerReadPort(0x3C1),
attributeControllerWritePort(0x3C0),
attributeControllerResetPort (0x3DA) {

}

VideoGraphicsArray::~VideoGraphicsArray() {

}

void VideoGraphicsArray::WriteRegisters(uint8_t* registers) {
    // misc
    miscPort.Write(*(registers++));

    // sequence
    for(uint8_t i = 0; i < 5; i++) {
        sequencerIndexPort.Write(i);
        sequencerDataPort.Write(*(registers++));
    }

    // cathode ray tube controller
    ctrcIndexPort.Write(0x03);// Lock Controller
    crtcDataPort.Write(crtcDataPort.Read() | 0x80);
    ctrcIndexPort.Write(0x11);
    crtcDataPort.Write(crtcDataPort.Read() & ~0x80);

    registers[0x03] = registers[0x03] | 0x80;
    registers[0x11] = registers[0x11] & ~0x80;
    for(uint8_t i = 0; i < 25; i++) {
        ctrcIndexPort.Write(i);
        crtcDataPort.Write(*(registers++));
    }
    
    // graphics controller
    for(uint8_t i = 0; i < 9; i++) {
        graphicsControllerIndexPort.Write(i);
        graphicsControllerDataPort.Write(*(registers++));
    }

    // attribute controller
    for(uint8_t i = 0; i < 21; i++) {
        attributeControllerResetPort.Read();    //Reset
        attributeControllerIndexPort.Write(i);
        attributeControllerWritePort.Write(*(registers++));

    }

    attributeControllerResetPort.Read();    //Reset
    attributeControllerIndexPort.Write(0x20);

}

bool VideoGraphicsArray::SupportsMode(uint32_t width, uint32_t height, uint32_t colordepth) {
    return width == 320 && height == 200 && colordepth == 8;
}

bool VideoGraphicsArray::SetMode(uint32_t width, uint32_t height, uint32_t colordepth) {
    if(!SupportsMode(width, height, colordepth)) 
        return false;
    uint8_t g_320x200x256[] = {
    /* MISC */
        0x63,
    /* SEQ */
        0x03, 0x01, 0x0F, 0x00, 0x0E,
    /* CRTC */
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
        0xFF,
    /* GC */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
        0xFF,
    /* AC */
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x41, 0x00, 0x0F, 0x00,	0x00
    };

    WriteRegisters(g_320x200x256);
    return true;
}

uint8_t* VideoGraphicsArray::GetFrameBUfferSegment() {
    graphicsControllerIndexPort.Write(0x06);
    uint8_t segmentNumber = ((graphicsControllerDataPort.Read() >> 2) & 0x03);
    switch(segmentNumber) {
        default:
        case 0:
        return (uint8_t*)0x00000;
        break;
        
        case 1:
        return (uint8_t*)0xA0000;
        break;
        
        case 2:
        return (uint8_t*)0xB0000;
        break;
        
        case 3:
        return (uint8_t*)0xB8000;
        break;
    }
}

void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint8_t colorIndex) {
    if(x < 0 || x >= 320 || y < 0 || y >= 200) return ;
    uint8_t* pixelAddress = GetFrameBUfferSegment() + 320 * y + x;
    *pixelAddress = colorIndex;
}

uint8_t VideoGraphicsArray::GetColorIndex(uint8_t r, uint8_t g, uint8_t b) {
    if(r == 0x00 && g == 0x00 && b == 0x00) return 0x00; //black
    if(r == 0x00 && g == 0x00 && b == 0xA8) return 0x01; //blue
    if(r == 0x00 && g == 0xA8 && b == 0x00) return 0x02; //green
    if(r == 0xA8 && g == 0x00 && b == 0x00) return 0x04; //red
    if(r == 0xFF && g == 0xFF && b == 0xFF) return 0x3F; //white
    return 0x01;
}

void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b) {
    PutPixel(x,y, GetColorIndex(r, g, b));
}

void VideoGraphicsArray::FillRectangle(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b) {
    for(uint32_t Y = y; Y < y + h; Y++) {
        for(uint32_t X = x; X < x + w; X++) {
            PutPixel(X, Y, r, g, b);
        }
    }
}