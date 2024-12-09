/*=============================================*/
/*       CONFIGURE LCD SMART CONTROLER         */
/*=============================================*/
#include "LCDML_liquidcrystal_zenith.h"

#define LCD_PINS_RS 16
#define LCD_PINS_ENABLE 17
#define LCD_PINS_D4 23
#define LCD_PINS_D5 19 //changed from 25
#define LCD_PINS_D6 27
#define LCD_PINS_D7 29

#define BEEPER_PIN 37

#define BTN_EN1 33
#define BTN_EN2 31
#define BTN_ENC 35

#define SD_DETECT_PIN 49
#define KILL_PIN 41

#define LCD_BACKLIGHT_PIN 39


    
/*=============================================*/
/*            METHODS DECLARATION  PANEL       */
/*=============================================*/
void init_lcd_panel();
void init_lcd_panel_pins();



void init_menu()
{
  zenith_LCDML_setup();
}

void loop_menu(){
  zenith_LCDML_loop();
}


/*=============================================*/
/*            METHODS  PANEL                   */
/*=============================================*/
void init_lcd_panel(){

  Serial.println(F("Initialisation: smart panel"));
  init_lcd_panel_pins();
}



void init_lcd_panel_pins(){
  
  pinMode(SD_DETECT_PIN, INPUT);
  
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  pinMode(BEEPER_PIN, OUTPUT);
  pinMode(KILL_PIN, OUTPUT);
}






