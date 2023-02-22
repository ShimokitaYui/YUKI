#ifndef __MYOS__GUI__DESKTOP_H
#define __MYOS__GUI__DESKTOP_H
#include <common/graphicscontext.h>
#include <common/types.h>
#include <gui/widget.h>
#include <drivers/mouse.h>
#include <drivers/keyboard.h>

namespace myos {
    namespace gui{
        class Desktop : public myos::gui::CompositeWidget , public myos::drivers::MouseEventHandler {
        protected:
            myos::common::uint32_t MouseX;
            myos::common::uint32_t MouseY;
        
        public:
            Desktop(myos::common::int32_t w, myos::common::int32_t h, 
                    myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b);
            ~Desktop();

            void Draw(myos::common::GraphicsContext* gc);
        
            void OnMouseDown(myos::common::uint8_t button);
            void OnMouseUp(myos::common::uint8_t button);
            void OnMouseMove(myos::common::int32_t xoffset, myos::common::int32_t yoffset);
        };
    }
}

#endif