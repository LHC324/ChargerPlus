#ifndef __ALLINT_H_
#define __ALLINT_H_



#define FALL_AND_RISE_EDGE 0  //�½����������ж�
#define FALL_EDGE  1     //�½����ж�

#define EX2             0x10  //ʹ���ж�2
#define EX3             0x20  //ʹ���ж�3
#define EX4             0x40  //ʹ���ж�4

void Int0_init(unsigned char edge);
void Int1_init(unsigned char edge);
void int_234_choose(unsigned char INTC);

#endif