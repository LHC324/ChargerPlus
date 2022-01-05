#ifndef __ALLINT_H_
#define __ALLINT_H_



#define FALL_AND_RISE_EDGE 0  //下降或上升沿中断
#define FALL_EDGE  1     //下降沿中断

#define EX2             0x10  //使能中断2
#define EX3             0x20  //使能中断3
#define EX4             0x40  //使能中断4

void Int0_init(unsigned char edge);
void Int1_init(unsigned char edge);
void int_234_choose(unsigned char INTC);

#endif