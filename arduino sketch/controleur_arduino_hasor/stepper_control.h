/**
 * Zenith Open RTI
 * Copyright (C) 2017
**/

#define CLOCKWISE 0
#define ANTICLOCKWISE 1

#define END_SWITCH_REACHED  LOW /*sensor state when triggered*/

#define MOTOR_MAX_SPEED 4500//7000
#define MOTOR_SLOW_SPEED_COEFF 0.4
#define MOTOR_ACCELERATION 2000//3000

#define MOTOR_CALIBRATION_OK 1
#define MOTOR_CALIBRATION_TIMEOUT 7000

#define COARSE_INCREMENT_DURING_CALIBRATION 30
#define FINE_INCREMENT_DURING_CALIBRATION 1

#define MOTOR_ON   0
#define MOTOR_OFF  1

#define ENABLE_AUTOSLEEP 1
#define AUTOSLEEP_DELAY 10000

#include <AccelStepper.h>



/*=============================================*/
/*            VARIABLES DECLARATION            */
/*=============================================*/



class Motor{
  

  
  private:
    void _update_motor_position_counter(int, byte);
    void _reset_motor_position_counter();
    void _pulse_MC_driver();
    byte _is_destination_in_range(int);
    void _update_flags();
    void _reset_position_counter();
    byte _findSwitch(byte *, const byte, long *);
    void _checkAutosleep();
    byte _motor_state;

  public:
    void init(byte, byte, byte, byte);
    void stepNow(int, byte);
    void set_speed(long);
    void setNormalSpeedMode();
    void setSlowSpeedMode();
    void set_state(byte);
    void set_direction(byte);
    void setRangeInStep(long,long);
    void end_switch_left_ISR();
    void end_switch_right_ISR(); 
    byte findLeftSwitch();
    byte findRightSwitch();
    byte calibrate();
    void setEnableSwitches(boolean, boolean);
    void moveToRelativePosition(float);
    float getRelativePosition();
    void loop();

    
    int steps_in_complete_rotation;
    long position = 0; //ex int
    int destination = 0;
    byte direction = CLOCKWISE;
    long speed = MOTOR_MAX_SPEED;
    long start_time = millis(); //debut du mouvement
    long end_time = millis();   //fin du mouvement
    long last_step_time = millis();

    int min_position_in_step = 0;
    int max_position_in_step = 200; 
    
    byte destination_reached = false;
    volatile byte end_switch_left_state = !END_SWITCH_REACHED;
    volatile byte end_switch_right_state = !END_SWITCH_REACHED;
    byte pin_mc_enable;
    byte pin_mc_step;
    byte pin_mc_dir;
    byte pin_sensor_end_switch_left;
    byte pin_sensor_end_switch_right;
    boolean _left_switch_enable = false;
    boolean _right_switch_enable = false;
    long _offset_from_switch_in_step = 0; //if we whant to stop moving before endswitchs
    
    AccelStepper stepper;

} motor;

//A wrapper for ISR because attachInterrupt whant only non-member function
//http://arduino.stackexchange.com/questions/4862/attachinterrupt-in-a-library
void motor_end_switch_left_ISR(){
  motor.end_switch_left_ISR();
}
void motor_end_switch_right_ISR(){
  motor.end_switch_right_ISR();
}



void Motor::init(byte pin_mc_enable, byte pin_mc_step, byte pin_mc_dir, byte pin_sensor_end_switch_left){
  /*=============================================*/
  /*               DEFINE OUTPUTS                */
  /*=============================================*/
  pinMode(pin_mc_enable, OUTPUT);
  stepper = AccelStepper(AccelStepper::DRIVER, uint8_t(pin_mc_step), uint8_t(pin_mc_dir));
//  stepper.setEnablePin(pin_mc_enable);
//  stepper.enableOutputs();
  this->set_speed(long(MOTOR_MAX_SPEED*params.motor_speeds[params.preset_id]));

  /*=============================================*/
  /*               DEFINE INPUTS                 */
  /*=============================================*/
  pinMode(pin_sensor_end_switch_left,INPUT);
  //pinMode(pin_sensor_end_switch_right,INPUT);
  
  /*=============================================*/
  /*               INITIALISATION                */
  /*=============================================*/ 

  //Semble ne pas fonctionner quand placé ici
  //this->end_switch_left_ISR();
  //this->end_switch_right_ISR();

  
  //interrupt have to be attached outside
  //attachInterrupt(digitalPinToInterrupt(pin_sensor_end_switch_left), this->end_switch_left_ISR, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(pin_sensor_end_switch_right), this->end_switch_right_ISR, CHANGE);
  
  this->pin_mc_enable = pin_mc_enable;
  this->pin_mc_step = pin_mc_step;
  this->pin_mc_dir = pin_mc_dir;
  this->pin_sensor_end_switch_left = pin_sensor_end_switch_left;
  //this->pin_sensor_end_switch_right = pin_sensor_end_switch_right;
}



/*=============================================*/
/*               MOTOR METHODS                 */
/*=============================================*/


void Motor::setEnableSwitches(boolean left_switch, boolean right_switch){
  this->_left_switch_enable  = left_switch;
  this->_right_switch_enable = right_switch;
}

void Motor::set_speed(long m_speed){
  this->speed = m_speed;
  stepper.setMaxSpeed(m_speed);
  stepper.setAcceleration(MOTOR_ACCELERATION);
}


void Motor::_checkAutosleep(){
  
  if(this->_motor_state == MOTOR_ON and ENABLE_AUTOSLEEP and this->destination_reached){
    if((millis() - this->end_time) > AUTOSLEEP_DELAY){
      if(DEBUG) Serial.println(F("Motor autosleep"));
      this->set_state(MOTOR_OFF);
    }
  }
  if(!this->destination_reached and this->_motor_state == MOTOR_OFF){
    this->set_state(MOTOR_ON);
  }
  
}

void Motor::loop(){
  this->_update_flags();

  this->_checkAutosleep();
  
  stepper.run();
}

void Motor::setRangeInStep(long min_step,long max_step){
  this->min_position_in_step = min_step;
  this->max_position_in_step = max_step;
}

void Motor::moveToRelativePosition(float pos){
  //Go from 0 to 1: where 1 is the maximum position of the motor

  long position_in_step = long(this->min_position_in_step + pos*(this->max_position_in_step - this->min_position_in_step));

//  char print_buffer[45];
//  Serial.println(pos);
//  Serial.println(this->min_position_in_step);
//  Serial.println(this->max_position_in_step);
//  sprintf(print_buffer,"Going to position %d.%02d (step %ld)", (int)pos, (int)(pos*100)%100, position_in_step);
//  Serial.println(print_buffer);

  this->stepper.moveTo(position_in_step);
}

float Motor::getRelativePosition(){

  float relative_position = 1.0*(this->stepper.currentPosition() - this->min_position_in_step) / (this->max_position_in_step - this->min_position_in_step);
  
  return relative_position;
}


void Motor::_reset_position_counter(){
  this->stepper.setCurrentPosition(0);
  this->_update_flags();
  Serial.print("Motor position reset");
}


void Motor::_update_flags(){
   if(this->stepper.distanceToGo()==0){
      if(!this->destination_reached){
        this->destination_reached = true;
        this->end_time = millis();
      }
   }
   else{
      this->destination_reached = false;
   }
}

void Motor::setNormalSpeedMode(){
  
  long m_speed  = long(MOTOR_MAX_SPEED*params.motor_speeds[params.preset_id]);
  
  Serial.print("setting Normal speed Mode:");
  Serial.println(m_speed);
  
  this->set_speed(m_speed);
}
void Motor::setSlowSpeedMode(){

  long m_speed  = long(MOTOR_MAX_SPEED*params.motor_speeds[params.preset_id]);
  m_speed = MOTOR_SLOW_SPEED_COEFF * m_speed; 
  
  Serial.print("setting Slow speed Mode:");
  Serial.println(m_speed);

  this->set_speed(m_speed);
}

/*=============================================*/
/*           CALIBRATION METHODS               */
/*=============================================*/

byte Motor::calibrate(){
  //Caution: Blocking method

  this->set_state(MOTOR_ON);
  Serial.println(F("Starting calibration process..."));


  //Manual call of ISRs, cause state is not good on startup to work on init
  this->end_switch_left_ISR();
  this->end_switch_right_ISR();
  delay(100);//delay to process ISR

 
  byte result = 0;
  
  if(this->_left_switch_enable){
    result  = this->findLeftSwitch();
    if(result != MOTOR_CALIBRATION_OK) return result;
  }
  if(this->_right_switch_enable){
    result  = this->findRightSwitch();
    if(result != MOTOR_CALIBRATION_OK) return result;

    //Retour au left_switch
    this->moveToRelativePosition(0);
  }
  
  return result;
}

byte Motor::_findSwitch(byte *switch_state, const byte direction, long *position){
  
  //Return TIMEOUT OR OK if switch founded 
  //position: Position of the switch in step


  //Reducing speed in this mode
  //this->set_speed(MOTOR_SLOW_SPEED);
  this->setSlowSpeedMode();
  
  elapsedMillis timeElapsed;

  int dir_sign = ( direction == CLOCKWISE ) ? 1 : -1;

  Serial.print(F("Searching end switch in "));
  if(direction == CLOCKWISE) Serial.println("CLOCKWISE direction");
  else                       Serial.println("ANTICLOCKWISE direction");


  Serial.print("Switch state on startup:");
  Serial.println(*switch_state);
  //==================================================================
  // If we are not already homed
  //==================================================================
  
  if(*switch_state != END_SWITCH_REACHED){
    
      //==============================================================
      //We search rapidely (coarse steps) the switch, then more finely (TODO)
      //==============================================================
    
      //Les capteurs ne peuvent être qu'au plus loin a STEPS_IN_TOTAL_TRAVEL 
      //de la position actuel, on prend un peus de marge avec le coefficient
    
      long requ_travel = int(1.10*STEPS_IN_TOTAL_TRAVEL);
      
      this->stepper.move(dir_sign*requ_travel); 
      
      while(!stepper.distanceToGo() == 0){
        
        //====================================
        //Check if motor is not in timeout
        //====================================
        if(timeElapsed > CALIBRATION_TIMEOUT){
          Serial.println(F("\nerror: motor timeout"));
          return MOTOR_CALIBRATION_TIMEOUT;
        }
        //====================================
        //Check if switch reached
        //====================================
        if(*switch_state == END_SWITCH_REACHED){
          position = this->stepper.currentPosition();
          this->stepper.stop();
          break;
        }
    
        this->stepper.run();
      }

      this->end_time = millis();
      Serial.println(F("Done"));
  }

  else{
    Serial.println(F("Already homed"));
  }
  
  
  
  return MOTOR_CALIBRATION_OK;
  
}



byte Motor::findLeftSwitch(){
  //Switch servant de reference step=0

  byte r;

  Serial.println(F("Searching Left switch"));
  long switch_position; //not used here
  r = this->_findSwitch(&this->end_switch_left_state, ANTICLOCKWISE, switch_position);

  Serial.print(F("Setting current position ("));
  Serial.print(this->stepper.currentPosition());
  Serial.println(F(") to 0"));
  this->stepper.setCurrentPosition(0);
  
  this->min_position_in_step =  0;
  if(this->_offset_from_switch_in_step){
    this->stepper.moveTo(-this->_offset_from_switch_in_step);
    while(!stepper.distanceToGo() == 0)this->stepper.run();
  }

  this->min_position_in_step = this->stepper.currentPosition();

  return r;
}


byte Motor::findRightSwitch(){

  byte r;
    
  long switch_position;

  Serial.println(F("Searching Right switch"));
  this->_findSwitch(&this->end_switch_right_state, CLOCKWISE, switch_position);

  if(this->_offset_from_switch_in_step){
    this->stepper.move(this->_offset_from_switch_in_step);
    while(!stepper.distanceToGo() == 0)this->stepper.run();
  }

  Serial.print(F("Max position is now"));
  Serial.print(this->stepper.currentPosition());
  
  this->max_position_in_step = this->stepper.currentPosition();

  return r;
}



/*=============================================*/
/*               MOTOR METHODS                 */
/*=============================================*/
void Motor::end_switch_left_ISR(){
  if(this->pin_sensor_end_switch_left !=-1 )
    this->end_switch_left_state = digitalRead(this->pin_sensor_end_switch_left);
    if(DEBUG){
      Serial.print("inside LEFT ISR");
      Serial.println(this->end_switch_left_state);
    }
}
void Motor::end_switch_right_ISR(){
  if(this->pin_sensor_end_switch_right !=-1 )
    this->end_switch_right_state = digitalRead(this->pin_sensor_end_switch_right);
    if(DEBUG){
      Serial.print("inside RIGHT ISR");
      Serial.println(this->pin_sensor_end_switch_right);
    }
}


byte Motor::_is_destination_in_range(int destination){
  
  if(destination < 0 || destination > this->steps_in_complete_rotation) return false;
  else return true;
}



void Motor::set_state(byte state){
  if(DEBUG){
    Serial.print(F("Motor state: "));
    if(state == MOTOR_ON) Serial.println(F("ON"));
    else                  Serial.println(F("OFF"));
  }
  this->_motor_state = state; 
  digitalWrite(this->pin_mc_enable, state);
}


void Motor::_pulse_MC_driver(){
  digitalWrite(this->pin_mc_step,HIGH);
  delayMicroseconds(this->speed);
  digitalWrite(this->pin_mc_step,LOW);
  delayMicroseconds(this->speed);
  //this->last_step_time = millis();
}

void Motor::set_direction(byte dir){
  if(DEBUG){
    Serial.print(F("direction: "));
    Serial.println(dir);
  }
  digitalWrite(this->pin_mc_dir,dir);
  this->direction = dir;
}





