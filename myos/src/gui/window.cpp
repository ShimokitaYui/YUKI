            
#include <gui/window.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::gui;
            
            
            
Window::Window( Widget* parent, 
        myos::common::int32_t x, myos::common::int32_t y, myos::common::int32_t w, myos::common::int32_t h, 
        myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b) 
:CompositeWidget(parent, x, y ,w, h, r, g, b) {
    Dragging = false;
}

Window::~Window() {

}

void Window::OnMouseDown(myos::common::int32_t x, myos::common::int32_t y, common::uint8_t button) {
    Dragging = button == 1;
    CompositeWidget::OnMouseDown(x, y, button);
}

void Window::OnMouseUp(myos::common::int32_t x, myos::common::int32_t y, common::uint8_t button) {
    Dragging = false;
    CompositeWidget::OnMouseUp(x, y, button);
}

void Window::OnMouseMove(myos::common::int32_t oldx, myos::common::int32_t oldy, myos::common::int32_t newx, myos::common::int32_t newy) {
    if(Dragging) {
        this->x += newx - oldx;
        this->y += newy - oldy;
    }
    CompositeWidget::OnMouseMove(oldx, oldy, newx, newy);
}
