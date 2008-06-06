#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

const prog_char bars[] PROGMEM = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

const prog_char left_bar[] PROGMEM = {
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b00000
};

const prog_char right_bar[] PROGMEM = {
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00000
};

void display_values(unsigned int *values, unsigned int max)
{
  unsigned char i;

  unsigned char characters[] = {' ',0,1,2,3,4,5,255};

  lcd_goto_xy(0,1);
  for(i=0;i<5;i++)
  {
    // get characters[0] to characters[7]
    print_character(characters[values[i]*8/(max+1)]);
  }
}

void test_qtr()
{
  clear();

  // Load bar graph characters.
  // Together with space and the solid block at 255, this makes almost
  // all possible bar heights, with two to spare.
  unsigned char i;
  for(i=0;i<6;i++)
  {
    lcd_load_custom_character(bars+i,i);
  }
  lcd_load_custom_character(left_bar,6);
  lcd_load_custom_character(right_bar,7);

  // test RC
  unsigned char pins[5] = {14,15,16,17,18};
  unsigned int values[5];

  qtr_rc_init(pins,5,1000,19);

  // raw values
  while(!button_is_pressed(ALL_BUTTONS))
  {
    clear();

    qtr_read(values);

    printf("Raw %4d",values[0]);

    display_values(values,1000);
    qtr_calibrate();
    delay_ms(50);
  }

  wait_for_button(ALL_BUTTONS);

  // calibrated values
  while(!button_is_pressed(ALL_BUTTONS))
  {
    qtr_read_calibrated(values);

    clear();
    printf("Cal %4d",values[0]);

    display_values(values,1000);
    delay_ms(50);
  }

  wait_for_button(ALL_BUTTONS);

  // line detection
  while(!button_is_pressed(ALL_BUTTONS))
  {
    unsigned int pos = qtr_read_line(values);

    clear();

    // we want pos = 0-499 to print zero spaces, etc
    unsigned char spaces = (pos+500)/1000;
    for(i=0;i<spaces;i++)
      print_character(' ');

    // print the bar to the left, middle, or right, as appropriate
    if(pos == 0)
      print_character(6); // all the way on the left
    else if(pos == 4000)
      print_character(7); // all the way on the right
    else if((pos+500)%1000 <= 333)
      print_character(6);
    else if((pos+500)%1000 <= 666)
      print_character('|');
    else print_character(7);

    display_values(values,1000);
    delay_ms(50);
  }

  wait_for_button(ALL_BUTTONS);

}
