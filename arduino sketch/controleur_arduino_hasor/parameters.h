// *********************************************************************
// *************          LOAD/SAVE PARAMETERS         *****************
// *********************************************************************
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <EEPROM.h>

#define  DEFAULT_PRESET_ID              0
#define  DEFAULT_SHUTTER_TYPE           SHUTTER_TYPE_RELAY
#define  DEFAULT_MOTOR_SPEED            500
#define  DEFAULT_STABILIZATION_DELAY    1000
#define  DEFAULT_SHUTTER_DURATION       500
#define  DEFAULT_LED_ON_DURATION        1000
#define  DEFAULT_SNAP_DELAY             1000
#define  DEFAULT_STEPS_COUNT            12
#define  DEFAULT_MIN_POLAR_POSITION     0
#define  DEFAULT_MAX_POLAR_POSITION     1.0
#define  DEFAULT_IR_SIGNAL_ID           0
#define  DEFAULT_IR_SIGNAL              222
#define  DEFAULT_LED_MODULES_COUNT      LED_MODULES_COUNT
#define  DEFAULT_LED_COUNT_IN_MODULE    LED_COUNT_IN_MODULE
#define  DEFAULT_LAST_LED_WEIGHT        100
#define  DEFAULT_FIRST_LED_INDEX        0
#define  DEFAULT_MAX_LEDS_INTENSITY     1.
#define  DEFAULT_LED_WEIGHT             100


//Attention, pas de prise en compte dynamique des CONFIG_COUNT, MODULE_COUNT_MAX, LED_BY_MODULE_MAX pour l'initialisation (TODO)
//Initialiser avec {CONFIG_COUNT x {MODULE_COUNT_MAX*LED_BY_MODULE_MAX}}
#define  DEFAULT_LED_WEIGHTS_ARRAY      {{100,100,100,100,100,100,100},{100,100,100,100,100,100,100},{100,100,100,100,100,100,100},{100,100,100,100,100,100,100},{100,100,100,100,100,100,100},{100,100,100,100,100,100,100}}
//Initialiser avec {CONFIG_COUNT x 1.}                                   
#define  DEFAULT_MOTOR_SPEEDS_ARRAY     {1.,1.,1.,1.,1.,1.}
                                     

//class ZenithParametersClass{
//  private:

//  public:
//    void saveParametersInEeprom();
//    void loadParametersFromEeprom();
//}

void saveParametersInEeprom();
bool loadParametersFromEeprom();
byte isValidParameters();


//https://www.approxion.com/?p=24
float *default_motor_speeds_array();
byte *default_led_weights_array();


struct ZenithParameters{
  char parameters_version[4];
  byte preset_id;
  byte shutter_type;
  float motor_speeds[CONFIG_COUNT];
  long stabilization_delay;
  long shutter_duration;
  long led_on_duration;
  long snaps_delay;
  int steps_count;
  float min_polar_position;
  float max_polar_position;
  byte IR_signal_id;
  byte IR_Signal;
  long led_modules_count;
  long led_by_module;
  long last_led_weight;
  int first_led_index;
  float max_leds_intensity;
  byte leds_weights[CONFIG_COUNT][MODULE_COUNT_MAX*LED_BY_MODULE_MAX];//values from 0 to 100
  
} params = {
  PARAMETERS_VERSION,
  DEFAULT_PRESET_ID,
  DEFAULT_SHUTTER_TYPE,
  DEFAULT_MOTOR_SPEEDS_ARRAY,
  DEFAULT_STABILIZATION_DELAY,
  DEFAULT_SHUTTER_DURATION,
  DEFAULT_LED_ON_DURATION,
  DEFAULT_SNAP_DELAY,
  DEFAULT_STEPS_COUNT,
  DEFAULT_MIN_POLAR_POSITION,
  DEFAULT_MAX_POLAR_POSITION,
  DEFAULT_IR_SIGNAL_ID,
  DEFAULT_IR_SIGNAL,
  DEFAULT_LED_MODULES_COUNT,
  DEFAULT_LED_COUNT_IN_MODULE,
  DEFAULT_LAST_LED_WEIGHT,
  DEFAULT_FIRST_LED_INDEX,
  DEFAULT_MAX_LEDS_INTENSITY,
  DEFAULT_LED_WEIGHTS_ARRAY
};


void saveParametersInEeprom(){
  EEPROM.put(PARAMETERS_ADRESS + params.preset_id*sizeof(ZenithParameters), params);
  Serial.println(F("Parameters saved in eeprom"));
}


bool loadParametersFromEeprom(){
  //Check if parameters structure already exists in memory
  if(isValidParameters())
  {
    EEPROM.get(PARAMETERS_ADRESS + params.preset_id*sizeof(ZenithParameters), params);
    Serial.println(F("Parameters loaded from EEPROM"));
    return true;
  }
  else{
    Serial.println(F("Invalid parameters in eeprom"));
    return false;
  }
}

byte isValidParameters(){
  if (EEPROM.read(PARAMETERS_ADRESS + params.preset_id*sizeof(ZenithParameters) + 0) == PARAMETERS_VERSION[0] &&
      EEPROM.read(PARAMETERS_ADRESS + params.preset_id*sizeof(ZenithParameters) + 1) == PARAMETERS_VERSION[1] &&
      EEPROM.read(PARAMETERS_ADRESS + params.preset_id*sizeof(ZenithParameters) + 2) == PARAMETERS_VERSION[2]){
    return true;
  }
  else{
    return false;
  }
}
#endif

