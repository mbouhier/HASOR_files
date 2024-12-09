#define DEBUG 1


#define PIN_INFO_LED 13


#define LED_MODULES_COUNT 6
#define LED_COUNT_IN_MODULE 1
#define LEDS_DATA_PIN 9
#define MODULE_COUNT_MAX 7L      //used to record values in epprom
#define LED_BY_MODULE_MAX 1L

//Chip controller the leds
#define LED_TYPE  WS2811//NEOPIXEL//WS2812B 
#define WIRING_TYPE_CHAIN 0   //all module in same way
#define WIRING_TYPE_ZIG_ZAG 1 //modules alternate
#define WIRING_TYPE WIRING_TYPE_CHAIN
#define INVERT_LED_LOGIC 1

#define SPLASH_SCREEN_DURATION 1000
  
#define PIN_SHUTTER_IR_LED 3 // HACK 15/11/24 anciennement 42, mise sur la broche du endsgtop -X inutilisée
#define PIN_SHUTTER_RELAY 5
#define PIN_FOCUS_RELAY 4

#define PIN_MC_ENABLE 24
#define PIN_MC_STEP 26
#define PIN_MC_DIR 28

#define MC_SPEED 1000
#define STEPS_IN_TOTAL_TRAVEL 8240//1030 setRangeInStep//TMC2208 works by default at 1/16 step
#define PIN_SENSOR_ENDSWITCH 2
#define SERIAL_SPEED 57600

#define CALIBRATION_TIMEOUT 5000
#define CALIBRATE_ON_STARTUP 1
#define PARAMETERS_VERSION "pv1" //Change this when modifiing params structure
#define PARAMETERS_ADRESS  32
#define CONFIG_COUNT 6 

#define RELAY_OPEN LOW
#define RELAY_CLOSED HIGH
#define SHUTTER_TYPE_RELAY 0
#define SHUTTER_TYPE_IR 1

#define IR_NIKON_ID 0
#define IR_CANON_ID 1
#define IR_CANONWLDC100_ID 2
#define IR_PENTAX_ID 3
#define IR_OLYMPUS_ID 4
#define IR_MINOLTA_ID 5
#define IR_SONY_ID 6
#define IR_CUSTOM_ID 7
