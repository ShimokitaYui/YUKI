#include <gui/widget.h>

using namespace myos;
using namespace myos::common;
using namespace myos::gui;

Widget::Widget(Widget* parent,  int32_t x, int32_t y, int32_t w, int32_t h, 
                                uint8_t r, uint8_t g, uint8_t b) 
: KeyboardEventHandler(){
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->r = r;
    this->g = g;
    this->b = b;
    this->Focusable = true;
}

Widget::~Widget() {

}
void Widget::GetFocus(Widget* widget) {
    if(parent != 0) {
        parent->GetFocus(widget);
    }
}

void Widget::ModelToScreen(int32_t &x, int32_t &y) {
    if(parent != 0) {
        parent->ModelToScreen(x,y);
    } else {
        x += this->x;
        y += this->y;
    }
}

void Widget::Draw(GraphicsContext* gc) {
    int X = x;
    int Y = y;
    ModelToScreen(X,Y);
    gc->FillRectangle(X, Y, w, h, r, g, b);
}

void Widget::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    if(Focusable) {
        GetFocus(this);
    }
}

void Widget::OnMouseUp(int32_t x, int32_t y, uint8_t button) {

}

void Widget::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {

}


bool Widget::ContainsCoordinate(myos::common::int32_t x, myos::common::int32_t y) {
    return this->x <= x && x < this->w + this->x 
        && this->y <= y && y < this->h + this->y;
}


CompositeWidget::CompositeWidget(Widget* parent, 
        int32_t x, int32_t y, int32_t w, int32_t h, 
        uint8_t r, uint8_t g, uint8_t b) : Widget(parent, x, y, w, h, r, g, b) {
    focussedChild = 0;
    numChildren = 0;
}

CompositeWidget::~CompositeWidget() {

}

void CompositeWidget::GetFocus(Widget* widget) {
    this->focussedChild = widget;
    if(parent != 0) {
        parent->GetFocus(this);
    }
}

bool CompositeWidget::AddChild(Widget* child) {
    if(numChildren >= 100) {
        return false;
    }
    children[numChildren++] = child;
    return true;
}

void CompositeWidget::Draw(GraphicsContext* gc) {
    Widget::Draw(gc);
    for(int i = numChildren - 1; i >= 0; i--) {
        children[i]->Draw(gc);
    }
}

void CompositeWidget::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    for(int i = 0; i < numChildren; i++) {
        if(children[i]->ContainsCoordinate(x - this->x, y - this->y)) {
            children[i]->OnMouseDown(x - this->x, y - this->y, button);
            break;
        }
    }
}

void CompositeWidget::OnMouseUp(int32_t x, int32_t y, uint8_t button) {
    for(int i = 0; i < numChildren; i++) {
        if(children[i]->ContainsCoordinate(x - this->x, y - this->y)) {
            children[i]->OnMouseUp(x - this->x, y - this->y, button);
            break;
        }
    }
}

void CompositeWidget::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {
    int32_t firschild = -1;
    for(int32_t i = 0; i < numChildren; i++) {
        if(children[i]->ContainsCoordinate(oldx - this->x, oldy - this->y)) {
            children[i]->OnMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            firschild = i;
            break;
        }
    }

    for(int32_t i = 0; i < numChildren; i++) {
        if(children[i]->ContainsCoordinate(newx - this->x, newy - this->y)) {
            if(firschild != i) {
                children[i]->OnMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            }
            break;
        }
    }
}

void CompositeWidget::OnKeyDown(int8_t str) {
    if(focussedChild != 0) {
        focussedChild->OnKeyDown(str);
    }
}

void CompositeWidget::OnKeyUp(int8_t str) {
    if(focussedChild != 0) {
        focussedChild->OnKeyUp(str);
    }
}
