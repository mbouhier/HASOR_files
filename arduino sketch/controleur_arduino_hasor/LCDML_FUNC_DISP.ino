/* ===================================================================== *
 *                                                                       *
 * DISPLAY SYSTEM                                                        *
 *                                                                       *
 * ===================================================================== *
 * every "disp menu function" needs three functions 
 * - void LCDML_DISP_setup(func_name)    
 * - void LCDML_DISP_loop(func_name)     
 * - void LCDML_DISP_loop_end(func_name)
 *
 * EXAMPLE CODE:
    void LCDML_DISP_setup(..menu_func_name..) 
    {
      // setup
      // is called only if it is started

      // starts a trigger event for the loop function every 100 millisecounds
      LCDML_DISP_triggerMenu(100);  
    }
    
    void LCDML_DISP_loop(..menu_func_name..)
    { 
      // loop
      // is called when it is triggert
      // - with LCDML_DISP_triggerMenu( millisecounds ) 
      // - with every button status change

      // check if any button is presed (enter, up, down, left, right)
      if(LCDML_BUTTON_checkAny()) {         
        LCDML_DISP_funcend();
      } 
    }
    
    void LCDML_DISP_loop_end(..menu_func_name..)
    {
      // loop end
      // this functions is ever called when a DISP function is quit
      // you can here reset some global vars or do nothing  
    } 
 * ===================================================================== *
 */

#define LIST_DELIMITER ","


// *********************************************************************
// *************          LAUNCH RTI PROCESS         *******************
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_launchRtiProcess)
{
  //Pour une progressbar voir https://www.carnetdumaker.net/snippets/4/
  
  // setup function 
  lcd.setCursor(0, 0);
  lcd.print(F("    ACQUISITION     "));
  lcd.setCursor(0, 1);
  lcd.print(F("    IN PROGRESS     "));

  LCDML_DISP_triggerMenu(100);  // starts a trigger event for the loop function every 100 millisecounds

  acquisition.run();
}

#define BLINK_PERIOD  300
#define BLINK_POINTS_COUNT 11
byte blink_flag = 0;
unsigned long blink_timer = 0;


void LCDML_DISP_loop(LCDML_FUNC_launchRtiProcess) 
{
  if(LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();
    LCDML_DISP_funcend();
  }

  //===============================================
  //            Progress bar effect
  //===============================================
  if((millis() - blink_timer) >= BLINK_PERIOD) {
    blink_timer = millis();
    lcd.setCursor(4,2);
    lcd.print(F("..........."));//same size as blink_points_count
    lcd.setCursor(4 + blink_flag,2);
    lcd.print(" ");
    if(blink_flag > BLINK_POINTS_COUNT - 2) blink_flag=0;
    else blink_flag++;
    }
  //===============================================

  //===============================================
  //            Acquisition infos
  //===============================================
//  int state = acquisition.state;
//  lcd.setCursor(4,3);
//  lcd.print(state);


  if(acquisition.isSequenceRunning()){
    
      char str_buffer[20];
      sprintf(str_buffer, "%d%%", int(100.*acquisition.getProgress()));
      
      int start_pos = int(0.5*( 20 - strlen(str_buffer)));
      
      lcd.setCursor(0,3);
      lcd.print(F("                    "));
      lcd.setCursor(start_pos,3);
      lcd.print(str_buffer);
  }
  

  if(acquisition.state == AS_END_ACQUISITION){
    lcd.clear();
    lcd.setCursor(4,1);
    lcd.print(F("ACQUISITION"));
    lcd.setCursor(4,2);
    lcd.print(F(" COMPLETED! "));
    delay(SPLASH_SCREEN_DURATION);
    LCDML_DISP_funcend();
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_launchRtiProcess)
{
  acquisition.stop();
}  



// *********************************************************************
// *************           MOTOR TEST          *************************
// *********************************************************************

int motor_position_requested = 0;
const int motor_position_min_value = 0;
const int motor_position_max_value = 100;
const int motor_position_increment = 5;

char string_buffer[20];


void LCDML_DISP_setup(LCDML_FUNC_MotorTest)
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(F("Searching endstop..."));
  int r_code = motor.calibrate();
  lcd.setCursor(0, 2);
  lcd.print(F("      Done"));
  delay(SPLASH_SCREEN_DURATION/2);
  lcd.clear();

  
  // setup function 
  lcd.setCursor(0, 0);
  lcd.print(F("MOTOR TEST (USE L/R)"));
  lcd.setCursor(0, 1);
  lcd.print(F("Position:"));
  lcd.setCursor(0, 2);
  lcd.print(F("Left switch:"));
  lcd.setCursor(0, 3);
  lcd.print(F("Right switch:"));

  LCDML_DISP_triggerMenu(100);//to update switch states

  motor_position_requested = long(motor_position_max_value * motor.getRelativePosition());
}

void LCDML_DISP_loop(LCDML_FUNC_MotorTest) 
{
  if(LCDML_BUTTON_checkEnter()) { 
    LCDML_DISP_funcend();
  }
  if(LCDML_BUTTON_checkUp()) {
    LCDML_BUTTON_resetUp();
    if(motor_position_requested < motor_position_max_value){ 
      motor_position_requested += motor_position_increment;
      motor.moveToRelativePosition(motor_position_requested/100.);
    }
    
  }
  if(LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetDown();
    if(motor_position_requested > motor_position_min_value){
      motor_position_requested -= motor_position_increment;
      motor.moveToRelativePosition(motor_position_requested/100.);
    }
  }

    sprintf(string_buffer, "%d     ",motor_position_requested);
    lcd.setCursor(10, 1);
    lcd.print(string_buffer);
    lcd.setCursor(13, 2);
    lcd.print(motor.end_switch_left_state);
    lcd.setCursor(14, 3);
    lcd.print(motor.end_switch_right_state);
}

void LCDML_DISP_loop_end(LCDML_FUNC_MotorTest)
{

}  

// *********************************************************************
// **      Generic method to display/update LONG INT values           **
// *********************************************************************
void genericMethodForIntegerValue_setup(const char *var_name)
{
  int str_len = strlen(var_name);

  lcd.clear();
  lcd.setCursor(int(0.5*( 20 - str_len)), 0);
  lcd.print(var_name);
}
void genericMethodForIntegerValue_loop(const char *var_name, long &var, long min_value, long max_value, long increment, const char* unit)
{
 if(LCDML_BUTTON_checkUp()) {
    LCDML_BUTTON_resetUp();
    if(var <= (max_value - increment)) var += increment;
  }
  if(LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetDown();
    if(var >= (min_value + increment)) var -= increment;
  }
  if(LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();//a mettre?
    LCDML_DISP_funcend();
  }
  
  lcd.setCursor(0,2);
  lcd.print("                    ");
  sprintf(string_buffer, "%ld %s", var, unit);

  int start_pos = int(0.5*( 20 - strlen(string_buffer)));
  lcd.setCursor(start_pos,2);
  lcd.print(string_buffer);
}
void genericMethodForIntegerValue_end()
{
  saveParametersInEeprom();
}

// *********************************************************************
// **      Generic method to display/update BYTES values              **
// *********************************************************************
void genericMethodForIntegerValue_loop(const char *var_name, byte &var, byte min_value, byte max_value, byte increment, const char* unit)
{
 if(LCDML_BUTTON_checkUp()) {
    LCDML_BUTTON_resetUp();
    if(var <= (max_value - increment)) var += increment;
  }
  if(LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetDown();
    if(var >= (min_value + increment)) var -= increment;
  }
  if(LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();//a mettre?
    LCDML_DISP_funcend();
  }
  
  lcd.setCursor(0,2);
  lcd.print("                    ");
  sprintf(string_buffer, "%b %s", var, unit);

  int start_pos = int(0.5*( 20 - strlen(string_buffer)));
  lcd.setCursor(start_pos,2);
  lcd.print(string_buffer);
}

// *********************************************************************
// **    Generic method to display/update INT values                  **
// *********************************************************************
void genericMethodForIntegerValue_loop(const char *var_name, int &var, int min_value, int max_value, int increment, const char* unit)
{
 if(LCDML_BUTTON_checkUp()) {
    LCDML_BUTTON_resetUp();
    if(var <= (max_value - increment)) var += increment;
  }
  if(LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetDown();
    if(var >= (min_value + increment)) var -= increment;
  }
  if(LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();//a mettre?
    LCDML_DISP_funcend();
  }
  
  lcd.setCursor(0,2);
  lcd.print("                    ");
  sprintf(string_buffer, "%d %s", var, unit);

  int start_pos = int(0.5*( 20 - strlen(string_buffer)));
  lcd.setCursor(start_pos,2);
  lcd.print(string_buffer);
}


// *********************************************************************
// **      Generic method to display/update LONG INT values           **
// *********************************************************************
void genericMethodForFloatValue_setup(const char *var_name)
{
    genericMethodForIntegerValue_setup(var_name);
}
void genericMethodForFloatValue_loop(const char *var_name, float &var, float min_value, float max_value, float increment, const char* unit)
{
 if(LCDML_BUTTON_checkUp()) {
    LCDML_BUTTON_resetUp();
    if(var <= (max_value - increment)) var += increment;
  }
  if(LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetDown();
    if(var >= (min_value + increment)) var -= increment;
  }
  if(LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();//a mettre?
    LCDML_DISP_funcend();
  }
  
  lcd.setCursor(0,2);
  lcd.print("                    ");
  sprintf(string_buffer, "%d.%02d %s", (int)var, (int)(var*100)%100, unit);

  int start_pos = int(0.5*( 20 - strlen(string_buffer)));
  lcd.setCursor(start_pos,2);
  lcd.print(string_buffer);
}

void genericMethodForFloatValue_end()
{
  saveParametersInEeprom();
}


// *********************************************************************
// **   Generic method to display/update STRING values from a list    **
// *********************************************************************
void genericMethodForStringValues_setup(const char *var_name)
{
  int str_len = strlen(var_name);

  lcd.clear();
  lcd.setCursor(int(0.5*( 20 - str_len)), 0);
  lcd.print(var_name);
}
void genericMethodForStringValues_loop(const char *var_name, byte &selected_position, const char* strings_list[], const byte element_count)
{
 
 if(LCDML_BUTTON_checkUp()) {
    LCDML_BUTTON_resetUp();
    if(selected_position < element_count - 1) selected_position++;
  }
  if(LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetDown();
    if(selected_position > 0) selected_position--;
  }
  if(LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();
    LCDML_DISP_funcend();
  }
  
  lcd.setCursor(0,2);
  lcd.print("                    ");

  int start_pos = int(0.5*( 20 - strlen(strings_list[selected_position])));
  lcd.setCursor(start_pos,2);
  lcd.print(strings_list[selected_position]);
}
void genericMethodForStringValues_end()
{
  saveParametersInEeprom();
}


// *********************************************************************
// **************END OF GENERIC SETTING METHODS*************************
// *********************************************************************


// *********************************************************************
// **********          TRIGGER DURATION         ************************
// *********************************************************************
//long trigger_duration = 0;
#define SHUTTER_DURATION_MIN 0L
#define SHUTTER_DURATION_MAX 5000L
#define SHUTTER_DURATION_INCREMENT 10L

void LCDML_DISP_setup(LCDML_FUNC_setShutterDuration)
{
  genericMethodForIntegerValue_setup("Shutter Duration");
}

void LCDML_DISP_loop(LCDML_FUNC_setShutterDuration) 
{
  genericMethodForIntegerValue_loop("Shutter Duration", params.shutter_duration, SHUTTER_DURATION_MIN, SHUTTER_DURATION_MAX, SHUTTER_DURATION_INCREMENT, "ms");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setShutterDuration)
{
  shutter.setShutterDuration(params.shutter_duration);
}


// *********************************************************************
// **********          HOW MANY LED BY MODULE         ******************
// *********************************************************************
//long trigger_duration = 0;
#define LED_BY_MODULE_MIN 1L
#define LED_BY_MODULE_INCREMENT 1L

void LCDML_DISP_setup(LCDML_FUNC_setLedsByModuleCount)
{
  genericMethodForIntegerValue_setup("Leds by module");
}

void LCDML_DISP_loop(LCDML_FUNC_setLedsByModuleCount) 
{
  genericMethodForIntegerValue_loop("Leds by module", params.led_by_module, LED_BY_MODULE_MIN, LED_BY_MODULE_MAX, LED_BY_MODULE_INCREMENT, "");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setLedsByModuleCount)
{
  genericMethodForIntegerValue_end();
}


// *********************************************************************
// **********          HOW MANY MODULES          ***********************
// *********************************************************************
//long trigger_duration = 0;
#define MODULE_COUNT_MIN 1L
#define MODULE_COUNT_INCREMENT 1L

void LCDML_DISP_setup(LCDML_FUNC_setModulesCount)
{
  genericMethodForIntegerValue_setup("Module count");
}

void LCDML_DISP_loop(LCDML_FUNC_setModulesCount) 
{
  genericMethodForIntegerValue_loop("Module count", params.led_modules_count, MODULE_COUNT_MIN, MODULE_COUNT_MAX, MODULE_COUNT_INCREMENT, "");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setModulesCount)
{
  genericMethodForIntegerValue_end();
}



// *********************************************************************
// **********          Tune Leds Insensity (global)  *******************
// *********************************************************************
#define MAX_LED_INTENSITY_MIN 0.
#define MAX_LED_INTENSITY_MAX 1.
#define MAX_LED_INTENSITY_INCREMENT 0.01
#define TEST_LED_ID                 0
#define TEST_LED_MAX_DURATION       5000
byte saved_previous_brightness_value;
//float temporary_brightness_value;

void LCDML_DISP_setup(LCDML_FUNC_setLedsMaxIntensity)
{
  genericMethodForFloatValue_setup("Max. Leds Intensity");
  saved_previous_brightness_value = params.leds_weights[params.preset_id][TEST_LED_ID];
}

void LCDML_DISP_loop(LCDML_FUNC_setLedsMaxIntensity) 
{
  
  genericMethodForFloatValue_loop("Max. Leds Intensity", params.max_leds_intensity, MAX_LED_INTENSITY_MIN, MAX_LED_INTENSITY_MAX, MAX_LED_INTENSITY_INCREMENT, "");
      
//  if(INVERT_LED_LOGIC) params.max_leds_intensity = temporary_brightness_value;
//  else                 params.max_leds_intensity = temporary_brightness_value;

  //set led to max intensity overriding user led dimming settings
  params.leds_weights[params.preset_id][TEST_LED_ID] = params.max_leds_intensity*100;
  switch_off_led_by_id(TEST_LED_ID);
  switch_on_led_by_id(TEST_LED_ID, TEST_LED_MAX_DURATION);
}

void LCDML_DISP_loop_end(LCDML_FUNC_setLedsMaxIntensity)
{
  genericMethodForFloatValue_end();
  switch_off_led_by_id(TEST_LED_ID);
  params.leds_weights[params.preset_id][TEST_LED_ID] = saved_previous_brightness_value;
}


// *********************************************************************
// **********          Tune Led Insensity (shadding)   *****************
// *********************************************************************
byte selected_led_id;
byte selected_led_cancel_id;
byte selected_led_reset_id;
byte max_led_count;
bool is_tuning_mode;
byte led_weight;
#define MODULE_CANCEL  "Exit"
#define MODULE_RESET   "Reset weights"

void LCDML_DISP_setup(LCDML_FUNC_setLedsFading)
{
  genericMethodForStringValues_setup("Led Tuning");
  
  selected_led_id         = 0;
  led_weight              = params.leds_weights[params.preset_id][selected_led_id];
  max_led_count           = params.led_modules_count*params.led_by_module;
  selected_led_reset_id   = max_led_count;
  selected_led_cancel_id  = selected_led_reset_id + 1;

  Serial.println("selected_led_reset_id");
  Serial.println(selected_led_reset_id);

  Serial.println("selected_led_cancel_id");
  Serial.println(selected_led_cancel_id);
  is_tuning_mode = false;
}

void LCDML_DISP_loop(LCDML_FUNC_setLedsFading) 
{
  moduleSelection_loop();
}

void LCDML_DISP_loop_end(LCDML_FUNC_setLedsFading)
{
  switch_off_all_leds();
  genericMethodForStringValues_end();
}

void moduleSelection_loop(){
  
    if(LCDML_BUTTON_checkUp()) {
      LCDML_BUTTON_resetUp();
      
      if(!is_tuning_mode){
        if(selected_led_id < selected_led_cancel_id ){
          switch_off_led_by_id(selected_led_id);
          selected_led_id++;
          led_weight = params.leds_weights[params.preset_id][selected_led_id];
        }
      }
      else{
        if(led_weight < 100){
          led_weight++;
          params.leds_weights[params.preset_id][selected_led_id] = led_weight;
          switch_on_led_by_id(selected_led_id, TEST_LED_MAX_DURATION);
        }
      }
      Serial.println("selected_lde_id");
      Serial.println(selected_led_id);
      
    }
    if(LCDML_BUTTON_checkDown()) {
      LCDML_BUTTON_resetDown();
      if(!is_tuning_mode){
        if(selected_led_id > 0){
          switch_off_led_by_id(selected_led_id);
          selected_led_id--;
          led_weight = params.leds_weights[params.preset_id][selected_led_id];
        }
      }
      else{
        if(led_weight > 0){
          led_weight--;
          params.leds_weights[params.preset_id][selected_led_id] = led_weight;
          switch_on_led_by_id(selected_led_id, TEST_LED_MAX_DURATION);
        }
      }
    }
    

    if(LCDML_BUTTON_checkEnter()) {
      LCDML_BUTTON_resetEnter();

      if(selected_led_id < max_led_count ) is_tuning_mode = !is_tuning_mode;
      
      else if(selected_led_id == selected_led_cancel_id) LCDML_DISP_funcend();

      //set all leds weights to 100%
      else if (selected_led_id == selected_led_reset_id){ 
        resetLedsWeightsForCurrentPreset();
        switch_off_all_leds();
      }
    
    }
    
    lcd.setCursor(0,2);
    lcd.print("                    ");

    if(selected_led_id < max_led_count){
        if(is_tuning_mode) sprintf(string_buffer, "set Led %d to %d%%", selected_led_id, led_weight);
        else               sprintf(string_buffer, "Led %d (%d%%)", selected_led_id, led_weight);
        
    }
        
    else if(selected_led_id == selected_led_cancel_id)
        sprintf(string_buffer, "%s", MODULE_CANCEL);

    else if (selected_led_id == selected_led_reset_id)
        sprintf(string_buffer, "%s", MODULE_RESET);
    
    int start_pos = int(0.5*( 20 - strlen(string_buffer)));
    lcd.setCursor(start_pos,2);
    lcd.print(string_buffer);
}

// *********************************************************************
// **********          First Led Index          *******************
// *********************************************************************
#define FIRST_LED_INDEX_INCREMENT 1L

void LCDML_DISP_setup(LCDML_FUNC_setFirstLedIndex)
{
  genericMethodForIntegerValue_setup("First led module index");
}

void LCDML_DISP_loop(LCDML_FUNC_setFirstLedIndex) 
{
  genericMethodForIntegerValue_loop("First led Module index", params.first_led_index, 0, params.led_modules_count, FIRST_LED_INDEX_INCREMENT, "");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setFirstLedIndex)
{
  genericMethodForIntegerValue_end();
}


// *********************************************************************
// **********          MOTOR CALIBRATION       *************************
// *********************************************************************

byte r_code = 0;

void LCDML_DISP_setup(LCDML_FUNC_processMotorCalibration)
{
     lcd.print("Calibrating...");
}

void LCDML_DISP_loop(LCDML_FUNC_processMotorCalibration) 
{
    //lcd.setCursor(
    r_code = motor.calibrate();
    LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_processMotorCalibration)
{
  lcd.clear();
  switch(r_code){
    case MOTOR_CALIBRATION_TIMEOUT: {lcd.print(F("Error: Timeout"));break;}
    case MOTOR_CALIBRATION_OK: {lcd.print(F("Done!"));break;}
    default: break;
  }

  delay(SPLASH_SCREEN_DURATION);
  
}  




// *********************************************************************
// **********          MOTOR SPEED          ****************************
// *********************************************************************
//long motor_speed = 0;
#define MOTOR_SPEED_MIN 0.01
#define MOTOR_SPEED_MAX 1.
#define MOTOR_SPEED_INCREMENT 0.01

void LCDML_DISP_setup(LCDML_FUNC_setMotorSpeed)
{
  genericMethodForFloatValue_setup("Motor Speed");
}

void LCDML_DISP_loop(LCDML_FUNC_setMotorSpeed) 
{
  genericMethodForFloatValue_loop("Motor Speed", params.motor_speeds[params.preset_id], MOTOR_SPEED_MIN, MOTOR_SPEED_MAX, MOTOR_SPEED_INCREMENT, "");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setMotorSpeed)
{
  //TODO prendre la vitesse en compte directement dans la loop moteur
  genericMethodForFloatValue_end();
  motor.set_speed(long(MOTOR_MAX_SPEED*params.motor_speeds[params.preset_id]));
}  

// *********************************************************************
// **********          STABILIZATION DELAY          ********************
// *********************************************************************
//long stabilization_delay = 0;
#define STABILIZATION_DELAY_MIN 0L
#define STABILIZATION_DELAY_MAX 2000L
#define STABILIZATION_DELAY_INCREMENT 10L

void LCDML_DISP_setup(LCDML_FUNC_setStabDelay)
{
  genericMethodForIntegerValue_setup("Stab. Delay");
}

void LCDML_DISP_loop(LCDML_FUNC_setStabDelay) 
{
  genericMethodForIntegerValue_loop("Stab. Delay", params.stabilization_delay, STABILIZATION_DELAY_MIN, STABILIZATION_DELAY_MAX, STABILIZATION_DELAY_INCREMENT, "ms");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setStabDelay)
{
  genericMethodForIntegerValue_end();
}  

// *********************************************************************
// **********          LED_ON DELAY                  ********************
// *********************************************************************
//long led_on_delay = 0;
#define LED_ON_DURATION_MIN 0L
#define LED_ON_DURATION_MAX 5000L
#define LED_ON_DURATION_INCREMENT 10L

void LCDML_DISP_setup(LCDML_FUNC_setLedOnDuration)
{
  genericMethodForIntegerValue_setup("LedOn Delay");
}

void LCDML_DISP_loop(LCDML_FUNC_setLedOnDuration) 
{
  genericMethodForIntegerValue_loop("LedOn Delay", params.led_on_duration, LED_ON_DURATION_MIN, LED_ON_DURATION_MAX, LED_ON_DURATION_INCREMENT, "ms");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setLedOnDuration)
{
  genericMethodForIntegerValue_end();
}  

// *********************************************************************
// **********          SHUTTER TYPE             ************************
// *********************************************************************

void LCDML_DISP_setup(LCDML_FUNC_setShutterType)
{
  genericMethodForStringValues_setup("Shutter type");
}

void LCDML_DISP_loop(LCDML_FUNC_setShutterType) 
{
  const char *shutter_types[] PROGMEM = {"Relay", "IR"};

  genericMethodForStringValues_loop("Shutter type", params.shutter_type, shutter_types, 2);
}

void LCDML_DISP_loop_end(LCDML_FUNC_setShutterType)
{
  shutter.setShutterType(params.shutter_type);
  genericMethodForStringValues_end();
}

// *********************************************************************
// **********        LOAD PREVIOUS PARAMETERS        *******************
// *********************************************************************
//edit CONFIG_COUNT in configuration.h if you nedd to add/remove presets
#define PRESET_1       "Config. 1"
#define PRESET_2       "Config. 2"
#define PRESET_3       "Config. 3"
#define PRESET_4       "Config. 4"
#define PRESET_5       "Config. 5"
#define PRESET_6       "Config. 6"
#define PRESET_7       "Config. 7"
#define PRESET_CANCEL  "Cancel"

const char *acquisition_modes[] = {PRESET_1, PRESET_2, PRESET_3, PRESET_4, PRESET_5, PRESET_6, PRESET_7, PRESET_CANCEL};
byte selected_preset_id;

void LCDML_DISP_setup(LCDML_FUNC_loadParameters)
{
  genericMethodForStringValues_setup("Load Parameters");
}

void LCDML_DISP_loop(LCDML_FUNC_loadParameters) 
{
  genericMethodForStringValues_loop("Load Parameters", selected_preset_id, acquisition_modes, CONFIG_COUNT + 1);
}

void LCDML_DISP_loop_end(LCDML_FUNC_loadParameters)
{
  //genericMethodForStringValues_end();

  if(selected_preset_id != 7){
      params.preset_id = selected_preset_id;
      lcd.clear();
      lcd.setCursor(0,1);

      if(loadParametersFromEeprom()) lcd.print(F(" Parameters loaded! "));
      else                           lcd.print(F(" Config undefined!  "));
      
      delay(SPLASH_SCREEN_DURATION);
  }
  //TODO: prendre en compte dans la loop motor
  motor.set_speed(long(MOTOR_MAX_SPEED*params.motor_speeds[params.preset_id]));

}

// *********************************************************************
// **********        SAVE PARAMETERS IN PRESET       *******************
// *********************************************************************


void LCDML_DISP_setup(LCDML_FUNC_saveParameters)
{
  genericMethodForStringValues_setup("Save Parameters");
}

void LCDML_DISP_loop(LCDML_FUNC_saveParameters) 
{
  genericMethodForStringValues_loop("Save Parameters", selected_preset_id, acquisition_modes, 8);
}

void LCDML_DISP_loop_end(LCDML_FUNC_saveParameters)
{
  genericMethodForStringValues_end();
  
  if(selected_preset_id != CONFIG_COUNT + 1){
      params.preset_id = selected_preset_id;
    
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print(F(" Parameters saved!  "));
      
      delay(SPLASH_SCREEN_DURATION);
  }
}





// *********************************************************************
// **********          IR Brand selection              *****************
// *********************************************************************

//***************   multiCameraIrControl Library    ********************
//http://sebastian.setz.name/arduino/my-libraries/multi-camera-ir-control/

void LCDML_DISP_setup(LCDML_FUNC_setIRBrand)
{
  genericMethodForStringValues_setup("IR Mode");
}

void LCDML_DISP_loop(LCDML_FUNC_setIRBrand) 
{
  const char *ir_modes[] PROGMEM = {"Nikon", "Canon", "CanonWLDC100", "Pentax", "Olympus", "Minolta", "Sony", "Custom"};

  genericMethodForStringValues_loop("Selected Mode", params.IR_signal_id, ir_modes, 8);
}

void LCDML_DISP_loop_end(LCDML_FUNC_setIRBrand)
{
  genericMethodForStringValues_end();
}



// *********************************************************************
// **********          POLAR STEPS COUNT                ****************
// *********************************************************************
#define STEPS_COUNT_MIN 2
#define STEPS_COUNT_MAX 50

void LCDML_DISP_setup(LCDML_FUNC_setPolarSteps)
{
  genericMethodForIntegerValue_setup("Steps count");
}

void LCDML_DISP_loop(LCDML_FUNC_setPolarSteps) 
{
  genericMethodForIntegerValue_loop("Steps count", params.steps_count, STEPS_COUNT_MIN, STEPS_COUNT_MAX, 1, "");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setPolarSteps)
{
  genericMethodForIntegerValue_end();
}

// *********************************************************************
// **********       POLAR MIN POLAR POSITION            ****************
// *********************************************************************
#define POLAR_POSITION_MIN 0.
#define POLAR_POSITION_MAX 1.
#define POLAR_POSITION_INCREMENT 0.01
void LCDML_DISP_setup(LCDML_FUNC_setPolarMin)
{
  genericMethodForIntegerValue_setup("Min. polar position");
}

void LCDML_DISP_loop(LCDML_FUNC_setPolarMin) 
{
  const int max_value = params.max_polar_position - POLAR_POSITION_INCREMENT; //max value must be > to previously set min_value ...
  genericMethodForFloatValue_loop("Min. polar position", params.min_polar_position, POLAR_POSITION_MIN, max_value, POLAR_POSITION_INCREMENT, "");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setPolarMin)
{
  genericMethodForIntegerValue_end();
}

// *********************************************************************
// **********       POLAR MAX POLAR POSITION            ****************
// *********************************************************************

void LCDML_DISP_setup(LCDML_FUNC_setPolarMax)
{
  genericMethodForIntegerValue_setup("Max. polar position");
}

void LCDML_DISP_loop(LCDML_FUNC_setPolarMax) 
{
  const float min_value = params.min_polar_position + POLAR_POSITION_INCREMENT; //max value must be > to previously set min_value ...
  genericMethodForFloatValue_loop("Max. polar position", params.max_polar_position, min_value, POLAR_POSITION_MAX, POLAR_POSITION_INCREMENT, "");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setPolarMax)
{

  genericMethodForIntegerValue_end();
}




// *********************************************************************
// **********          2 Snaps Wait Delay               ****************
// *********************************************************************
#define SNAPS_DELAY_MIN 0
#define SNAPS_DELAY_MAX 2000

  
void LCDML_DISP_setup(LCDML_FUNC_setSnapsDelay)
{
  genericMethodForIntegerValue_setup("Snaps Delay");
}

void LCDML_DISP_loop(LCDML_FUNC_setSnapsDelay) 
{
  genericMethodForIntegerValue_loop("Snaps Delay", params.snaps_delay, SNAPS_DELAY_MIN, SNAPS_DELAY_MAX, 5, "ms");
}

void LCDML_DISP_loop_end(LCDML_FUNC_setSnapsDelay)
{
  genericMethodForIntegerValue_end();
}



// *********************************************************************
// **********          Shutter Test                    *****************
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_ShutterTest){}

void LCDML_DISP_loop(LCDML_FUNC_ShutterTest) 
{
    Serial.println("Shutter test");
    shutter.shutter_now();
    LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_ShutterTest){}  


// *********************************************************************
// **********          Relay Shutter Test              *****************
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_RelayTest){}

void LCDML_DISP_loop(LCDML_FUNC_RelayTest) 
{
    Serial.println("Relay shutter test");
    shutter.shutter_now_Relay();
    LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_RelayTest){}  


// *********************************************************************
// **********          IR Shutter Test                 *****************
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_IRTest){}

void LCDML_DISP_loop(LCDML_FUNC_IRTest) 
{
    Serial.println("IR shutter test");
    shutter.shutter_now_IR();
    LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_IRTest){}  


// *********************************************************************
// **********             Leds Test                    *****************
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_LedsTest){}

void LCDML_DISP_loop(LCDML_FUNC_LedsTest) 
{
    Serial.println("Leds sequencial test");

    launch_leds_sequential_test();
    
    LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_LedsTest){}  




// *********************************************************************
// **********             Back in Menu                 *****************
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_backInMenu){}


void LCDML_DISP_loop(LCDML_FUNC_backInMenu) 
{
    LCDML.goBack();
    LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_backInMenu){
}  


// *********************************************************************
// **********          Infos about firmware            *****************
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_displayInfos)
{
  // setup function 
  lcd.setCursor(0, 1);
  lcd.print(F("  ZENITH OPEN RTI"));
  lcd.setCursor(0, 2);
  lcd.print(F("   Firmware v1.0"));
}

void LCDML_DISP_loop(LCDML_FUNC_displayInfos) 
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function
  
  if(LCDML_BUTTON_checkAny()) { // check if any button is presed (enter, up, down, left, right)
    // LCDML_DISP_funcend calls the loop_end function
    LCDML_DISP_funcend();
  } 
}

void LCDML_DISP_loop_end(LCDML_FUNC_displayInfos)
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing  
}  
