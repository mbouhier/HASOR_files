/*=============================================*/
/*       CONFIGURE LCD SMART CONTROLER         */
/*=============================================*/
#define PIN_FAN_ON 44
#define PIN_TEMP_SENSOR 0 //must be analog pin


    
/*=============================================*/
/*            METHODS DECLARATION              */
/*=============================================*/
void init_temperature_ctrl();
void temperature_ctrl_loop();




void init_temperature_ctrl()
{
  //Temporary, TODO: link fan with inside temperature, put in separate module
  pinMode(PIN_TEMP_SENSOR, INPUT);
  pinMode(PIN_FAN_ON, OUTPUT);
  digitalWrite(PIN_FAN_ON, HIGH);

}

void temperature_ctrl_loop(){

}



