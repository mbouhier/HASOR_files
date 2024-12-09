/*=============================================*/
/*     METHODS DECLARATION  CAMERA SHUTTER     */
/*=============================================*/
#include <multiCameraIrControl.h>

//For pinout of various brand of camera, check http://www.doc-diy.net/photo/remote_pinout/


struct Relay_State{
  unsigned long on_delay; 
  unsigned long on_time;
  byte is_ON;
};


struct Camera{
  Nikon nikon;
  Canon canon;
  CanonWLDC100 canonWLDC100;
  Pentax pentax;
  Olympus olympus;
  Minolta minolta;
  Sony sony;
} camera = {
  Nikon(PIN_SHUTTER_IR_LED),
  Canon(PIN_SHUTTER_IR_LED),
  CanonWLDC100(PIN_SHUTTER_IR_LED),
  Pentax(PIN_SHUTTER_IR_LED),
  Olympus(PIN_SHUTTER_IR_LED),
  Minolta(PIN_SHUTTER_IR_LED),
  Sony(PIN_SHUTTER_IR_LED)
};



class Shutter{
  public:
    void init(ZenithParameters);
    void shutter_now();
    void shutter_now_IR();
    void shutter_now_Relay();
    void loop();
    void setShutterType(byte);
    void setShutterDuration(long);
    void setIRSignalID(byte);
    
    Relay_State relay = {0,0,0};
    
  private:
    void _setRelayON();
    void _setRelayOFF();
    byte _shutter_type;
    byte _IR_signal_id;
    long _shutter_duration;
    
} shutter;



void Shutter::init(ZenithParameters p){
  Serial.println(F("Initialisation: camera shutter"));
  pinMode(PIN_SHUTTER_RELAY, OUTPUT);
  pinMode(PIN_FOCUS_RELAY, OUTPUT);

  this->_setRelayOFF();

  //if(parameters){
  this->setShutterDuration(p.shutter_duration);
  this->setShutterType(p.shutter_type);
  //}
  
}


void Shutter::_setRelayOFF(){
    digitalWrite(PIN_SHUTTER_RELAY, RELAY_OPEN);
    digitalWrite(PIN_FOCUS_RELAY, RELAY_OPEN);
    this->relay.is_ON = false;
}
void Shutter::_setRelayON(){
    digitalWrite(PIN_SHUTTER_RELAY, RELAY_CLOSED);
    digitalWrite(PIN_FOCUS_RELAY, RELAY_CLOSED);
    this->relay.is_ON = true;
    this->relay.on_time = millis();
}

void Shutter::setShutterType(byte type){
  this->_shutter_type = type;
}

void Shutter::setShutterDuration(long shutter_duration){
  this->_shutter_duration = shutter_duration;
}

void Shutter::setIRSignalID(byte IR_signal_id){
  this->_IR_signal_id = IR_signal_id;
}


    
void Shutter::shutter_now(){
  Serial.println(F("shutter camera"));

  if(this->_shutter_type == SHUTTER_TYPE_RELAY) 
    this->shutter_now_Relay();
  else if(this->_shutter_type == SHUTTER_TYPE_IR) 
    this->shutter_now_IR();
}



void Shutter::shutter_now_IR(){
  Serial.print(F("IR triggered (type "));
  Serial.print(this->_IR_signal_id);
  Serial.println(F(")"));

  switch(this->_IR_signal_id){
    case IR_NIKON_ID:        {camera.nikon.shutterNow();break;}
    case IR_CANON_ID:        {camera.canon.shutterNow();break;}
    case IR_CANONWLDC100_ID: {camera.canonWLDC100.shutterNow();break;}
    case IR_PENTAX_ID:       {camera.pentax.shutterNow();break;}
    case IR_OLYMPUS_ID:      {camera.olympus.shutterNow();break;}
    case IR_MINOLTA_ID:      {camera.minolta.shutterNow();break;}
    case IR_SONY_ID:         {camera.sony.shutterNow();break;}
    case IR_CUSTOM_ID:{
      Serial.println(F("Sending Custom IR"));
      break;
    }
    default: break;;
  }
}

void Shutter::shutter_now_Relay(){
    Serial.println(F("relay triggered"));
    //Methode bloquante, faire avec un timer?
    this->_setRelayON();
}

void Shutter::loop(){

    long ellapsed_relay_ON = millis() - this->relay.on_time;


    
    if (this->relay.is_ON and ellapsed_relay_ON >= this->_shutter_duration){
    
      char print_buffer[38];
      sprintf(print_buffer, "Switching off relay after %ld ms", ellapsed_relay_ON);
      Serial.println(print_buffer);
      this->_setRelayOFF();
    }
}


