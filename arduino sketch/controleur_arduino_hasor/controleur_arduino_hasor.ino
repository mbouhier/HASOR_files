// A4988 driver test routine
//http://www.instructables.com/id/Drive-a-Stepper-Motor-with-an-Arduino-and-a-A4988-/?ALLSTEPS

/*=============================================*/
/*              IMPORT LiBRARIES               */
/*=============================================*/
#include "elapsedMillis.h"

#include "configuration.h"
#include "parameters.h"
#include "stepper_control.h"
#include "lcd_panel.h"
#include "camera_shutter.h"
#include "led_modules.h"
#include "temperature_ctrl.h"
#include "acquisition_modes.h"
/*=============================================*/
/*       
/*=============================================*/



void setup() {
  Serial.begin(SERIAL_SPEED);
  while (!Serial); // wait for serial monitor to connect

  loadParametersFromEeprom();

  pinMode(PIN_INFO_LED, OUTPUT);
  digitalWrite(PIN_INFO_LED, HIGH);

  init_temperature_ctrl();
  init_lcd_panel();
  init_menu();

  motor.init(PIN_MC_ENABLE, PIN_MC_STEP, PIN_MC_DIR, PIN_SENSOR_ENDSWITCH);
  motor.setEnableSwitches(true, false);
  motor.max_position_in_step = STEPS_IN_TOTAL_TRAVEL;
  
  attachInterrupt(digitalPinToInterrupt(motor.pin_sensor_end_switch_left), motor_end_switch_left_ISR, CHANGE);

  
//  motor.init(PIN_MC_ENABLE, PIN_MC_STEP, PIN_MC_DIR, PIN_SENSOR_ENDSWITCH);
//  motor.set_speed(400);
//  attachInterrupt(digitalPinToInterrupt(motor.pin_sensor_end_switch_left), motor_end_switch_left_ISR, CHANGE);


  
  //init_stepper_control();
  shutter.init(params);
  init_led_modules();
}



/*=============================================*/
/*         MISCELLANEOUS METHODS               */
/*=============================================*/


/*=============================================*/
/*              MAIN LOOP                      */
/*=============================================*/
void loop() {
  
  loop_menu();
  shutter.loop();
  acquisition.loop();
  led_modules_loop();
  motor.loop();
  
}
