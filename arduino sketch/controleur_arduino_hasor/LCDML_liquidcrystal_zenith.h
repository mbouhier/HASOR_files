// ============================================================                                                            
// Example:     LCDML: display liquid crystal                         
// ============================================================
// Autor:       Nils Feldkämper
// Last update: 08.01.2017
// License:     MIT                                     
// ============================================================ 
// Descripton:  
// This example shows you, how to use this lib with LiquidCrystal
// lib. The LCD object have to create in this tab. In "LCDML_DISO"
// you can edit the layout of the menu. (content, cursor, scrollbar)
//
// When you rewrite this function, you can use every other LCD 
// or graphic LCD Lib with this menu.
// ============================================================ 

  // include libs
  #include <LiquidCrystal.h>
  #include <LCDMenuLib.h>
  
  // lib config
  #define _LCDML_DISP_cfg_button_press_time          200    // button press time in ms
  #define _LCDML_DISP_cfg_scrollbar                  1      // enable a scrollbar
  #define _LCDML_DISP_cfg_cursor                     0x7E   // cursor Symbol 

// ********************************************************************* 
// LCDML TYPE SELECT
// *********************************************************************
  // settings for lcd 
  #define _LCDML_DISP_cols             20
  #define _LCDML_DISP_rows             4  

  // lcd object
  // liquid crystal needs (rs, e, dat4, dat5, dat6, dat7)
  //LiquidCrystal lcd(16,17,23,25,27,29);
  LiquidCrystal lcd(16,17,23,19,27,29);
  
  const uint8_t scroll_bar[5][8] = {
    {B10001, B10001, B10001, B10001, B10001, B10001, B10001, B10001}, // scrollbar top
    {B11111, B11111, B10001, B10001, B10001, B10001, B10001, B10001}, // scroll state 1 
    {B10001, B10001, B11111, B11111, B10001, B10001, B10001, B10001}, // scroll state 2
    {B10001, B10001, B10001, B10001, B11111, B11111, B10001, B10001}, // scroll state 3
    {B10001, B10001, B10001, B10001, B10001, B10001, B11111, B11111}  // scrollbar bottom
  }; 

// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
  // create menu
  // menu element count - last element id
  // this value must be the same as the last menu element
  #define _LCDML_DISP_cnt    42
  
  // LCDML_root        => layer 0 
  // LCDML_root_X      => layer 1 
  // LCDML_root_X_X    => layer 2 
  // LCDML_root_X_X_X  => layer 3 
  // LCDML_root_... 	 => layer ... 
  
  // LCDMenuLib_add(id, group, prev_layer_element, new_element_num, lang_char_array, callback_function)
  LCDML_DISP_init(_LCDML_DISP_cnt);

  LCDML_DISP_add      (0  , _LCDML_G1  , LCDML_root        , 1  , "START"              , LCDML_FUNC_launchRtiProcess);
  LCDML_DISP_add      (1  , _LCDML_G1  , LCDML_root        , 2  , "Settings"           , LCDML_FUNC);
  LCDML_DISP_add      (2  , _LCDML_G1  , LCDML_root        , 3  , "Infos"              , LCDML_FUNC_displayInfos);

  LCDML_DISP_add      (3  , _LCDML_G1  , LCDML_root_2      , 1  , "..."                , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (4  , _LCDML_G1  , LCDML_root_2      , 2  , "Motor"              , LCDML_FUNC);
  LCDML_DISP_add      (5  , _LCDML_G1  , LCDML_root_2      , 3  , "Photo trigger"      , LCDML_FUNC);
  LCDML_DISP_add      (6  , _LCDML_G1  , LCDML_root_2      , 4  , "Acquisition"        , LCDML_FUNC);
  LCDML_DISP_add      (7  , _LCDML_G1  , LCDML_root_2      , 5  , "Leds"               , LCDML_FUNC);
  LCDML_DISP_add      (8  , _LCDML_G1  , LCDML_root_2      , 6  , "Tests"              , LCDML_FUNC);
  LCDML_DISP_add      (9  , _LCDML_G1  , LCDML_root_2      , 7  , "Load"               , LCDML_FUNC_loadParameters);
  LCDML_DISP_add      (10 , _LCDML_G1  , LCDML_root_2     , 8   , "Save"               , LCDML_FUNC_saveParameters);


  LCDML_DISP_add      (11  , _LCDML_G1  , LCDML_root_2_4   , 1  , "..."                , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (12  , _LCDML_G1  , LCDML_root_2_4   , 2  , "Steps count"        , LCDML_FUNC_setPolarSteps);
  LCDML_DISP_add      (13  , _LCDML_G1  , LCDML_root_2_4   , 3  , "Min position"       , LCDML_FUNC_setPolarMin);
  LCDML_DISP_add      (14  , _LCDML_G1  , LCDML_root_2_4   , 4  , "Max position"       , LCDML_FUNC_setPolarMax);

  
  LCDML_DISP_add      (15  , _LCDML_G1  , LCDML_root_2_6   , 1  , "..."                , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (16  , _LCDML_G1  , LCDML_root_2_6   , 2  , "Motor"              , LCDML_FUNC_MotorTest);
  LCDML_DISP_add      (17  , _LCDML_G1  , LCDML_root_2_6   , 3  , "Shutter"            , LCDML_FUNC);
  //LCDML_DISP_add      (18  , _LCDML_G1  , LCDML_root_2_6   , 4  , "TO_RE_USE"          , LCDML_FUNC);
  LCDML_DISP_add      (18  , _LCDML_G1  , LCDML_root_2_6   , 4  , "Leds"               , LCDML_FUNC);


  LCDML_DISP_add      (19  , _LCDML_G1  , LCDML_root_2_5   , 1  , "..."                  , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (20  , _LCDML_G1  , LCDML_root_2_5   , 2  , "Modules Count"        , LCDML_FUNC_setModulesCount);
  LCDML_DISP_add      (21  , _LCDML_G1  , LCDML_root_2_5   , 3  , "Leds by module"       , LCDML_FUNC_setLedsByModuleCount);
  LCDML_DISP_add      (22  , _LCDML_G1  , LCDML_root_2_5   , 4  , "Intensity ratio"      , LCDML_FUNC_setLedsFading);
  LCDML_DISP_add      (23  , _LCDML_G1  , LCDML_root_2_5   , 5  , "Max leds intensity"   , LCDML_FUNC_setLedsMaxIntensity);
  LCDML_DISP_add      (24  , _LCDML_G1  , LCDML_root_2_5   , 6  , "LedOn duration"       , LCDML_FUNC_setLedOnDuration);

  
  LCDML_DISP_add      (25  , _LCDML_G1  , LCDML_root_2_6_4   , 1  , "..."                , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (26  , _LCDML_G1  , LCDML_root_2_6_4   , 2  , "Launch test"        , LCDML_FUNC_LedsTest);

  LCDML_DISP_add      (27  , _LCDML_G1  , LCDML_root_2_6_3   , 1  , "..."                , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (28  , _LCDML_G1  , LCDML_root_2_6_3   , 2  , "Trigger now"        , LCDML_FUNC_ShutterTest);
//  LCDML_DISP_add      (16  , _LCDML_G1  , LCDML_root_2_6_3   , 1  , "..."                , LCDML_FUNC_backInMenu);
//  LCDML_DISP_add      (17  , _LCDML_G1  , LCDML_root_2_6_3   , 2  , "Trigger now"        , LCDML_FUNC_IRTest);
  
//  LCDML_DISP_add      (29  , _LCDML_G1  , LCDML_root_2_6_4   , 1  , "..."                , LCDML_FUNC_backInMenu);
//  LCDML_DISP_add      (30  , _LCDML_G1  , LCDML_root_2_6_4   , 2  , "Trigger now"        , LCDML_FUNC_RelayTest);
  
  
  LCDML_DISP_add      (29  , _LCDML_G1  , LCDML_root_2_3     , 1  , "..."              , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (30  , _LCDML_G1  , LCDML_root_2_3    , 2  , "Shutter type"      , LCDML_FUNC_setShutterType);
  LCDML_DISP_add      (31  , _LCDML_G1  , LCDML_root_2_3    , 3  , "IR Signal"         , LCDML_FUNC);
  LCDML_DISP_add      (32  , _LCDML_G1  , LCDML_root_2_3    , 4  , "Shutter Duration"  , LCDML_FUNC_setShutterDuration);
  LCDML_DISP_add      (33  , _LCDML_G1  , LCDML_root_2_3    , 5  , "Snaps Delay"       , LCDML_FUNC_setSnapsDelay);
  
  LCDML_DISP_add      (34  , _LCDML_G1  , LCDML_root_2_3_3    , 1  , "..."             , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (35  , _LCDML_G1  , LCDML_root_2_3_3    , 2  , "Predefined"      , LCDML_FUNC_setIRBrand);
  LCDML_DISP_add      (36  , _LCDML_G1  , LCDML_root_2_3_3    , 3  , "Record IR"       , LCDML_FUNC);

  LCDML_DISP_add      (37  , _LCDML_G1  , LCDML_root_2_2    , 1  , "..."                 , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (38  , _LCDML_G1  , LCDML_root_2_2    , 2  , "Stab. delay"         , LCDML_FUNC_setStabDelay);
  LCDML_DISP_add      (39  , _LCDML_G1  , LCDML_root_2_2    , 3  , "Motor speed"         , LCDML_FUNC_setMotorSpeed);
  LCDML_DISP_add      (40  , _LCDML_G1  , LCDML_root_2_2    , 4  , "Calibrate"           , LCDML_FUNC);

  LCDML_DISP_add      (41  , _LCDML_G1  , LCDML_root_2_2_4   , 1  , "..."                , LCDML_FUNC_backInMenu);
  LCDML_DISP_add      (42  , _LCDML_G1  , LCDML_root_2_2_4   , 2  , "Calibrate now"      , LCDML_FUNC_processMotorCalibration);
  
  
  LCDML_DISP_createMenu(_LCDML_DISP_cnt);




// ********************************************************************* 
// LCDML BACKEND (core of the menu, do not change here anything yet)
// ********************************************************************* 
  // define backend function  
  #define _LCDML_BACK_cnt    1  // last backend function id
  
  LCDML_BACK_init(_LCDML_BACK_cnt);
  LCDML_BACK_new_timebased_dynamic (0  , ( 20UL )         , _LCDML_start  , LCDML_BACKEND_control);
  LCDML_BACK_new_timebased_dynamic (1  , ( 1000UL )       , _LCDML_stop   , LCDML_BACKEND_menu);
  LCDML_BACK_create();


// *********************************************************************
// SETUP
// *********************************************************************
  void zenith_LCDML_setup()// void setup() en standalone
  {  
    // serial init; only be needed if serial control is used 
//    while(!Serial);                    // wait until serial ready
//    Serial.begin(9600);                // start serial    
//    Serial.println(F(_LCDML_VERSION)); // only for examples
    
    // LCD Begin
    lcd.begin(_LCDML_DISP_cols,_LCDML_DISP_rows);  
    // set special chars for scrollbar
    lcd.createChar(0, (uint8_t*)scroll_bar[0]);
    lcd.createChar(1, (uint8_t*)scroll_bar[1]);
    lcd.createChar(2, (uint8_t*)scroll_bar[2]);
    lcd.createChar(3, (uint8_t*)scroll_bar[3]);
    lcd.createChar(4, (uint8_t*)scroll_bar[4]);
  
    // Enable all items with _LCDML_G1
    LCDML_DISP_groupEnable(_LCDML_G1); // enable group 1
  
    // LCDMenu Setup
    LCDML_setup(_LCDML_BACK_cnt);  
  }

// *********************************************************************
// LOOP
// *********************************************************************
  void zenith_LCDML_loop() // void loop() en standalone
  { 
    // this function must called here, do not delete it
    LCDML_run(_LCDML_priority); 
  }




// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
# if(_LCDML_DISP_rows > _LCDML_DISP_cfg_max_rows)
# error change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib.h
# endif
# if(_LCDML_DISP_cols > _LCDML_DISP_cfg_max_string_length)
# error change value of _LCDML_DISP_cfg_max_string_length in LCDMenuLib.h
# endif
