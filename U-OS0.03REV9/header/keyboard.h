#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#define KEYBOARD_ADDRESS 0x60
#define LED_SCROLL 0x01
#define LED_NUML   0x02
#define LED_CAPS   0x04
  
void initKEYBOARD();
void KEYBOARD_irq();
int _kgetch();
void _ksetleds(char);

#endif
