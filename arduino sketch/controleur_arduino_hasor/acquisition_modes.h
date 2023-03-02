// ============================================================                                                            
// Zenith - Open RTI: Acquisition modes                         
// ============================================================
// Autor:       Mickael BOUHIER
// Last update: 10.04.2017
// License:     MIT                                     
// ============================================================ 
// Descripton:  
// This file contains the typical sequences of acquistion
// 
// ============================================================ 


// ============================================================ 
//               ACQUISITION STATES  
// ============================================================ 
#define AS_GOTO_POSITION            0 
#define AS_POSITIONING              1
#define AS_CHECK_POSITION           2
#define AS_WAIT_FOR_STABILIZATION   3
#define AS_START_SEQUENCE           4
#define AS_CHECK_SEQUENCE_STATE     5
#define AS_SNAP_PHOTO               6
#define AS_CHECK_PHOTO_DELAY        7
#define AS_CHECK_ACQUISITION_STATE  8
#define AS_CHECK_PHOTO_DELAY        9
#define AS_ACQUISITION_STATE        10
#define AS_END_ACQUISITION          11
#define AS_ABORT_ACQUISITION        12
#define AS_ACQUISITION_SUCCESS      13
#define AS_ACQUISITION_ABORTED      13
#define AS_ACQUISITION_ERROR        14
#define AS_ACQUISITION_WAIT         15


#define SS_START    0
#define SS_RUNNING  1
#define SS_FINISHED 2
// ============================================================ 
//               METHODS DECLARATION      
// ============================================================ 



// ============================================================ 
//               VIARIABLES DECLARATION      
// ============================================================ 

class Acquisition{
  public:
    byte is_running = false;
    byte reset      = false;//Stop required by user
    int  progress   = 0; // 0 to 1000
    int  state      = AS_ACQUISITION_WAIT; //AS_GOTO_POSITION, AS_POSITIONING etc...
    int  previous_step = 0; 
    int  getCurrentStep();
    float getProgress();
    float getSequenceProgress();
    bool isSequenceRunning();
    

    void loop();
    void run();
    void stop();

  private:
    void _startSnapSequence();
    void _snapPhoto();
    void _gotoNextPosition();
    int  _sequence_state     = 0;
    int  _azimuthal_counter  = 0;
    int  _polar_counter      = 0; 
    unsigned long _last_sequence_time = 0;
    unsigned long _last_snap_time     = millis();
    
} acquisition;
  


void Acquisition::run(){
  //Call once to launch the process
  Serial.println("Starting acquisition");

  
  if(CALIBRATE_ON_STARTUP){
    byte r = motor.calibrate();
    if(r != MOTOR_CALIBRATION_OK){
      this->stop();
    }
    else{
      this->state = AS_WAIT_FOR_STABILIZATION;
    }
  }
  
  
  motor.moveToRelativePosition(0); //RAZ
  this->_polar_counter = 0;
  this->_azimuthal_counter = 0;
  
  //this->reset = false;
  this->state = AS_GOTO_POSITION;
}

//User ask for end of acquisition
void Acquisition::stop(){
  Serial.println("Stopping acquisition");

  if(this->state != AS_ACQUISITION_SUCCESS){
    this->state = AS_ABORT_ACQUISITION;
    return;
  }
  return;
}



void Acquisition::loop(){

    int state = this->state;

    
    switch(state){

      case AS_ACQUISITION_ERROR:
        Serial.println(F("Acquisition Error"));
        this->state = AS_END_ACQUISITION;
        break;
        
      case AS_ABORT_ACQUISITION:
        Serial.println(F("Aborting acquisition"));
        motor.moveToRelativePosition(0); //RAZ
        this->state = AS_ACQUISITION_ABORTED;
        break;
        
      case AS_GOTO_POSITION:
        Serial.println(F("Going to next position"));
        this->_gotoNextPosition();
        break;
        
      case AS_POSITIONING:
        if(motor.destination_reached){
          this->state = AS_WAIT_FOR_STABILIZATION;
          Serial.println(F("Positioning done, waiting for stabilization"));
        }
        break;
  
      case AS_WAIT_FOR_STABILIZATION:
        if( (millis() - motor.end_time) > params.stabilization_delay){
          Serial.println(F("Stabilization done, starting sequence"));
          this->state = AS_START_SEQUENCE;
        }
        break;
        
      case AS_START_SEQUENCE:
        this->_startSnapSequence();
        break;
        
      case AS_CHECK_SEQUENCE_STATE:
        if(this->_sequence_state == SS_FINISHED){
          Serial.println(F("Sequence done, checking acquisition state"));
          this->state = AS_CHECK_ACQUISITION_STATE;
        }
        else
        {
          Serial.println(F("Sequence not finished"));
          this->state = AS_SNAP_PHOTO;
        }

        break;
  
      case AS_SNAP_PHOTO:
        this->_snapPhoto();
        this->state = AS_CHECK_PHOTO_DELAY;
        break;
        
      case AS_CHECK_PHOTO_DELAY:

        if( (millis() - this->_last_snap_time) > (params.led_on_duration + params.snaps_delay)){
            Serial.println(F("Snaps delay ellapsed"));
            this->state = AS_CHECK_SEQUENCE_STATE;
        }
        break;
        
      case AS_CHECK_ACQUISITION_STATE:
        //TODO faire le check ici ou dans _gotoNextPosition ?
        this->state = AS_GOTO_POSITION;
        
        break;
  
      case AS_END_ACQUISITION:
        break;

      break;
    }
    
  }

int Acquisition::getCurrentStep(){
  return this->_polar_counter;
}

float Acquisition::getProgress(){
  
  int current_step = this->_polar_counter;
  int current_sequence_step = this->_azimuthal_counter;
  
  if(this->state == AS_END_ACQUISITION) 
    return 1.0;
    
  float progress;
  
  progress = (current_step - 1 + this->getSequenceProgress()) /params.steps_count;
  
  return progress;
}

float Acquisition::getSequenceProgress(){

  int max_steps_in_sequence  = LED_COUNT_IN_MODULE*LED_MODULES_COUNT;

  if(this->_sequence_state == SS_FINISHED) 
    return 1.0;
  else 
    return 1.0*this->_azimuthal_counter / max_steps_in_sequence;

}

void Acquisition::_gotoNextPosition(){

  float start_position = params.min_polar_position;
  float end_position   = params.max_polar_position;
  int position_count   = params.steps_count - 1;

  float polar_increment = (end_position - start_position) / position_count;
  
  float next_position = start_position + this->_polar_counter*polar_increment;

  if(next_position > end_position){
      Serial.println("last position reached");
      this->state = AS_END_ACQUISITION;
  }
  else{
      Serial.print("Goto next position:");
      Serial.println(next_position);
      motor.moveToRelativePosition(next_position);
      this->_polar_counter++;
      this->state = AS_POSITIONING;
  }
  
}

void Acquisition::_startSnapSequence(){
  Serial.print(F("Starting led sequence - step"));
  Serial.println(this->state);

  //TODO: mettre ailleur ?
  motor.set_speed(long(MOTOR_MAX_SPEED*params.motor_speeds[params.preset_id]));
  
  this->_sequence_state = SS_START;
  this->state = AS_SNAP_PHOTO;
}

bool Acquisition::isSequenceRunning(){
  if(this->_sequence_state != SS_FINISHED) return true;
  else                                     return false;
}


void Acquisition::_snapPhoto(){

    int max_steps_in_sequence    = LED_COUNT_IN_MODULE*LED_MODULES_COUNT;
    int module_to_switch_on      = this->_azimuthal_counter;
    int module_part_to_switch_on = 0;

    Serial.println("snapping photo");
    switch_on_led(module_to_switch_on, 0, params.led_on_duration);
    shutter.shutter_now();
    
    this->_azimuthal_counter++;
    this->_last_snap_time = millis();

    if(this->_azimuthal_counter == max_steps_in_sequence){
      this->_sequence_state = SS_FINISHED;
      this->_azimuthal_counter = 0;
    }
   

}










