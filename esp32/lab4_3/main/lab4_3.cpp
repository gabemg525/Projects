/*!
  * file HelloWorld.ino
  * brief helloworld.
  *
  * Copyright	[DFRobot](http://www.dfrobot.com), 2016
  * Copyright	GNU Lesser General Public License
  *
  * version  V1.0
  * date  2018-1-13
  */


#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "DFRobot_LCD.h"
#include "dfrobot_c_lib.h"

DFRobot_LCD lcd(16,2);
uint8_t heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
};

uint8_t smiley[8] = {
    0b00000,
    0b00000,
    0b01010,
    0b00000,
    0b00000,
    0b10001,
    0b01110,
    0b00000
};

uint8_t frownie[8] = {
    0b00000,
    0b00000,
    0b01010,
    0b00000,
    0b00000,
    0b00000,
    0b01110,
    0b10001
};

uint8_t armsDown[8] = {
    0b00100,
    0b01010,
    0b00100,
    0b00100,
    0b01110,
    0b10101,
    0b00100,
    0b01010
};

uint8_t armsUp[8] = {
    0b00100,
    0b01010,
    0b00100,
    0b10101,
    0b01110,
    0b00100,
    0b00100,
    0b01010
};



extern "C" {void app_main();}

void app_main() {
 
    printf("Hello wurl\n");
    

    while(true){
      lcd.init();
      printf("Hello wurl\n");
        // create a new character
        lcd.customSymbol(0, heart);
        char temp_buff[100], hum_buff[100];
        float t = 74.2, h=38.3;
        sprintf(temp_buff, "Temp: %.1fC",t);
        sprintf(hum_buff, "Hum : %.1f%%",h);
        // lcd.printstr(temp_buff, 0);
        // lcd.printstr(hum_buff, 1);
        lcd.T_H_display();
        

  
        // create a new character
        // lcd.customSymbol(1, smiley);
        // // create a new character
        // lcd.customSymbol(2, frownie);
        // // create a new character
        // lcd.customSymbol(3, armsDown);
        // // create a new character
        // lcd.customSymbol(4, armsUp);

        // set up the lcd's number of columns and rows:
        
        
        // lcd.setCursor(0, 1);
        // // Print a message to the lcd.

        // // lcd.write((unsigned char)0);

        // lcd.write(1);
        // set the cursor to the bottom row, 5th position:
        // lcd.setCursor(3, 0);
        // // // draw the little man, arms down:
        // lcd.write('a');
        // lcd.setCursor(4, 0);
        // // // draw the little man, arms down:
        // lcd.write('b');
        // lcd.setCursor(5, 0);
        // // // draw the little man, arms down:
        // lcd.write('c');
        // lcd.setCursor(6, 0);
        // // // draw the little man, arms down:
        // lcd.write('d');
        
        
        vTaskDelay(pdMS_TO_TICKS(1000)); 
   


    }
}