#include "FastLED.h"

/*=============================================*/
/*            METHODS DECLARATION              */
/*=============================================*/
void init_led_modules();
void switch_on_led(int, int, unsigned long);
void switch_on_led_by_id(int, unsigned long);
void switch_off_led_by_id(int);
void switch_off_all_leds();
void led_modules_loop();

unsigned long ellapsed_led_ON = 0;

struct Led_state{
  boolean is_ON;
  unsigned long on_time;
  unsigned long on_delay;
  byte brightness;
};


//TODO: regler dynamiquement le nombre de LEDs ou supprimer l'option depuis la GUI
//CRGB leds[LED_MODULES_COUNT*LED_COUNT_IN_MODULE];
CRGB leds[LED_MODULES_COUNT];
//Led_state led_states[LED_MODULES_COUNT*LED_COUNT_IN_MODULE];
Led_state led_states[LED_MODULES_COUNT];


void init_led_modules(){
  //FastLED.addLeds<LED_TYPE, LEDS_DATA_PIN>(leds, LED_MODULES_COUNT*LED_COUNT_IN_MODULE);
  FastLED.addLeds<LED_TYPE, LEDS_DATA_PIN>(leds, LED_MODULES_COUNT);
  FastLED.setBrightness(255);
  
  Serial.println("switching off all leds");
  if(INVERT_LED_LOGIC){
    switch_off_all_leds();
  }
}


int get_brightness_by_led_id(int led_id){
  //return brightness from 0 to 100
  Serial.print("brightness:");
  Serial.println((byte)params.leds_weights[params.preset_id][led_id]);
  return (byte)params.leds_weights[params.preset_id][led_id];
}


void switch_on_led(int module, int led, unsigned long duration){
  
  if(module > LED_MODULES_COUNT)   Serial.println(F("incorrect led module ID"));
  if(led    > LED_COUNT_IN_MODULE) Serial.println(F("incorrect led ID"));


  byte brightness = get_brightness_by_led_id(module);
  
  char print_buffer[90];
  sprintf(print_buffer, "Switching on led %d on module %d for %lu ms at brightness %d%%", led, module, duration, brightness);
  Serial.println(print_buffer);
  
  int led_id = module + led;
  
  switch_on_led_by_id(led_id, duration);
}

void switch_on_led_by_id(int id, unsigned long duration){
  //leds[id] = CRGB::Red; //correspond a output1 = max sur WS2811

  byte brightness = get_brightness_by_led_id(id);

  byte inverted_value = (byte)255.*(1. - params.max_leds_intensity*brightness/100.);
  byte value = (byte)255.*params.max_leds_intensity*brightness/100.;
  
  //if(INVERT_LED_LOGIC) leds[id] = CRGB::Black; //correspond a output1 = 0 sur WS2811
  //else                 leds[id] = CRGB::white; //correspond a output1-3 = max sur WS2811
  
  if(INVERT_LED_LOGIC) leds[id].setRGB(inverted_value,255,255);
  else                 leds[id].setRGB(value,0,0);
  
  //FastLED.setBrightness(value);
  
  Serial.println("switch_on_led_by_id:");
  Serial.print("id:");
  Serial.println(id);
  Serial.print("leds[id]:");
  Serial.println(leds[id].r);
  Serial.println(leds[id].g);
  Serial.println(leds[id].b);
  Serial.print("brightness:");
  Serial.println(brightness);
  Serial.print("value:");
  Serial.println(value);
  Serial.print("inverted_value:");
  Serial.println(inverted_value);
  
  led_states[id] = {true, millis(), duration, brightness};
  
  led_states[id].is_ON = true;
  
  FastLED.show();
}

void switch_off_led_by_id(int id){
  
  //if(INVERT_LED_LOGIC) leds[id] = CRGB::White;
  //else                 leds[id] = CRGB::Black;
  if(INVERT_LED_LOGIC) leds[id] = CRGB::White;
  else                 leds[id] = CRGB::Black;

  
  led_states[id].is_ON = false;
  
  FastLED.show();
}

void switch_off_all_leds(){
  for(int i=0;i < LED_MODULES_COUNT;i++) switch_off_led_by_id(i);
}


void launch_leds_sequential_test(){
    long counter = 0;
    for(int i=0;i < LED_MODULES_COUNT;i++){
      //switch_off_led(i,0,2000L + counter);
      switch_on_led(i,0,1000L);
      delay(500);
      switch_off_led_by_id(i);
      //counter += 800;
    }
}

void led_modules_loop(){

  for(int i = 0; i < LED_MODULES_COUNT; i++){
    
    ellapsed_led_ON = millis() - led_states[i].on_time;
    
    if (led_states[i].is_ON and ellapsed_led_ON >= led_states[i].on_delay){
      char print_buffer[40];
      sprintf(print_buffer, "Switching off led %d after %ld ms", i, ellapsed_led_ON);
      Serial.println(print_buffer);
      switch_off_led_by_id(i);
    }
  }
}

void resetLedsWeightsForCurrentPreset(){
  for(int i=0 ; i < MODULE_COUNT_MAX*LED_BY_MODULE_MAX;i++) params.leds_weights[params.preset_id][i] = 100;
}
        













//#include "FastLED.h"
//
///*=============================================*/
///*            METHODS DECLARATION              */
///*=============================================*/
//void init_led_modules();
//void switch_on_led(int, int, unsigned long);
//void switch_on_led_by_id(int);
//void switch_off_led_by_id(int);
//void led_modules_loop();
//
//unsigned long ellapsed_led_ON = 0;
//
//struct Led_state{
//  boolean is_ON;
//  unsigned long on_time;
//  unsigned long on_delay;
//  byte brightness;
//};
//
//
////TODO: regler dynamiquement le nombre de LEDs ou supprimer l'option depuis la GUI
////CRGB leds[LED_MODULES_COUNT*LED_COUNT_IN_MODULE];
//CRGB leds[LED_MODULES_COUNT];
////Led_state led_states[LED_MODULES_COUNT*LED_COUNT_IN_MODULE];
//Led_state led_states[LED_MODULES_COUNT];
//
//
//void init_led_modules(){
//  //FastLED.addLeds<LED_TYPE, LEDS_DATA_PIN>(leds, LED_MODULES_COUNT*LED_COUNT_IN_MODULE);
//  FastLED.addLeds<LED_TYPE, LEDS_DATA_PIN>(leds, LED_MODULES_COUNT;
//}
//
//
//
//void switch_on_led(int module, int led, unsigned long duration){
//  
//  if(module > LED_MODULES_COUNT) Serial.println(F("incorrect led module ID"));
//  if(led > LED_COUNT_IN_MODULE)  Serial.println(F("incorrect led ID"));
//  char print_buffer[60];
//  sprintf(print_buffer, "Switching on led %d on module %d for %lu ms", led, module, duration);
//  Serial.println(print_buffer);
//
//
//  int led_id;
//  
//  if(WIRING_TYPE == WIRING_TYPE_CHAIN){
//    led_id = module*LED_COUNT_IN_MODULE + led;
//  }
//  else if(WIRING_TYPE == WIRING_TYPE_ZIG_ZAG){
//    //NOT IMPLEMENTED
//    return;
//  }
//  
//  led_states[led_id] = {true, millis(), duration, 255};
//  
//  switch_on_led_by_id(led_id);
//}
//
//void switch_on_led_by_id(int id){
//  leds[id] = CRGB::Red; //correspond a output1 = max sur WS2811
//  FastLED.show();
//}
//
//void switch_off_led_by_id(int id){
//  leds[id] = CRGB::Black; //correspond a output1 = 0 sur WS2811
//  led_states[id].is_ON = false;
//  FastLED.show();
//}
//
//void launch_leds_sequential_test(){
//    long counter = 0;
//    for(int i=0;i<LED_MODULES_COUNT*LED_COUNT_IN_MODULE;i++){
//      switch_on_led(i/LED_COUNT_IN_MODULE,i%LED_COUNT_IN_MODULE,2000L + counter);
//      counter+=100;
//    }
//}
//
//void led_modules_loop(){
//
//  for(int i = 0; i < LED_MODULES_COUNT*LED_COUNT_IN_MODULE; i++){
//    
//    ellapsed_led_ON = millis() - led_states[i].on_time;
//    
//    if (led_states[i].is_ON and ellapsed_led_ON >= led_states[i].on_delay){
//      char print_buffer[40];
//      sprintf(print_buffer, "Switching off led %d after %ld ms", i, ellapsed_led_ON);
//      Serial.println(print_buffer);
//      switch_off_led_by_id(i);
//    }
//  }
//}
//
