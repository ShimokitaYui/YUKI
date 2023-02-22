#ifndef __MYOS__GDT_H
#define __MYOS__GDT_H

#include <common/types.h>

namespace myos{
    class GlobalDescriptorTable {
    public:
        class SegmentDescriptor{
        private:
            myos::common::uint16_t limit_lo;
            myos::common::uint16_t base_lo;
            myos::common::uint8_t base_hi;
            myos::common::uint8_t type;
            myos::common::uint8_t flags_limit_hi;
            myos::common::uint8_t base_vhi;
        public:
            SegmentDescriptor(myos::common::uint32_t base, myos::common::uint32_t limit, myos::common::uint8_t flags);
            myos::common::uint32_t Base();
            myos::common::uint32_t Limit();

        } __attribute__((packed));//是告诉编译器取消结构在编译过程中的优化对齐,按照实际占用字节数进行对齐
        SegmentDescriptor nullSegmentSelector;      // empty one
        SegmentDescriptor unusedSegmentSelector;    //
        SegmentDescriptor codeSegmentSelector;      //代码段
        SegmentDescriptor dataSegmentSelector;      //数据段
    public:
        GlobalDescriptorTable();
        ~GlobalDescriptorTable();

        myos::common::uint16_t CodeSegmentSelector();
        myos::common::uint16_t DataSegmentSelector();
    };
}
#endif