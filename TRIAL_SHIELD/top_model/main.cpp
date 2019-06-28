/**
* By: Ben Earle
* ARSLab - Carleton University
*
* Analog Input:
* This main file constructs the do simple line following project, using a Seed Bot Shield.
* Its purpose is to demonstrate how to use all of the port IO models in RT_ARM_MBED.
*/

#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_atomic.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/tuple_to_ostream.hpp>
#include <cadmium/logger/common_loggers.hpp>
#include <cadmium/io/iestream.hpp>


#include <NDTime.hpp>

#include <cadmium/real_time/arm_mbed/io/digitalInput.hpp>
#include <cadmium/real_time/arm_mbed/io/analogInput.hpp>
#include <cadmium/real_time/arm_mbed/io/pwmOutput.hpp>
#include <cadmium/real_time/arm_mbed/io/digitalOutput.hpp>

#include "../atomics/Alarm_Monitor.hpp"
#include "../atomics/Emergency_LED_Controller.hpp"
#include "../atomics/Fire_Alarm_Controller.hpp"
#include "../atomics/IR_Sensor_Controller.hpp"
#include "../atomics/Room_LED_Sensor_Controller.hpp"
#include "../atomics/Light_Sensor_Controller.hpp"
#include "../atomics/Fire_Sensor.hpp"

#ifdef RT_ARM_MBED
  #include "../mbed.h"
#else
  const char* A5  = "./inputs/A5_LIGHT_SENSOR.txt";
  const char* A4  = "./inputs/A4_TEMP_SENSOR.txt";
  const char* A0  = "./inputs/A1_IR_1.txt";
  const char* A3  = "./inputs/A3_IR_2.txt";
  const char* D12  = "./inputs/D12_FIRE_SWITCH.txt";

  const char* D2  = "./outputs/D2_E1L1.txt";
  const char* D3 = "./outputs/D3_E1L2.txt";
  const char* D4 = "./outputs/D4_E2L1.txt";
  const char* D5 = "./outputs/D5_E2L2.txt";
  const char* D6  = "./outputs/D6_R1L1.txt";
  const char* D7 = "./outputs/D7_R1L2.txt";
  const char* D8 = "./outputs/D8_R2L1.txt";
  const char* D11 = "./outputs/D11_R2L2.txt";
  const char* D13 = "./outputs/D13_ALARM.txt";


#endif

// SCARED OF THE DARK definition is for the analog sensor demo.
// If the analog sensor is not present then this definition should be commented out.
// #define SCARED_OF_THE_DARK

using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;


int main(int argc, char ** argv) {

  //This will end the main thread and create a new one with more stack.
  #ifdef RT_ARM_MBED

    //Logging is done over cout in RT_ARM_MBED
    struct oss_sink_provider{
      static std::ostream& sink(){
        return cout;
      }
    };
  #else
    // all simulation timing and I/O streams are ommited when running embedded

    auto start = hclock::now(); //to measure simulation execution time

    /*************** Loggers *******************/

    static std::ofstream out_data("Test_Shield_output.txt");
    struct oss_sink_provider{
      static std::ostream& sink(){
        return out_data;
      }
    };
  #endif

  using info=cadmium::logger::logger<cadmium::logger::logger_info, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using debug=cadmium::logger::logger<cadmium::logger::logger_debug, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using state=cadmium::logger::logger<cadmium::logger::logger_state, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using log_messages=cadmium::logger::logger<cadmium::logger::logger_messages, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using routing=cadmium::logger::logger<cadmium::logger::logger_message_routing, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using global_time=cadmium::logger::logger<cadmium::logger::logger_global_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using local_time=cadmium::logger::logger<cadmium::logger::logger_local_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using log_all=cadmium::logger::multilogger<info, debug, state, log_messages, routing, global_time, local_time>;
  using logger_top=cadmium::logger::multilogger<log_messages, global_time>;


/*******************************************/
/********************************************/
/*********** APPLICATION GENERATOR **********/
/********************************************/
  using AtomicModelPtr=std::shared_ptr<cadmium::dynamic::modeling::model>;
  using CoupledModelPtr=std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>>;

/********************************************/
/********** Initiating Atomics *******************/
/********************************************/

  AtomicModelPtr Alarm_Monitor = cadmium::dynamic::translate::make_dynamic_atomic_model<Alarm_Mon, TIME>("Alarm_Monitor");
  AtomicModelPtr Emergency_LED_Controller1 = cadmium::dynamic::translate::make_dynamic_atomic_model<Emer_LED_Con, TIME>("Emergency_LED_Controller1");
  AtomicModelPtr Emergency_LED_Controller2 = cadmium::dynamic::translate::make_dynamic_atomic_model<Emer_LED_Con, TIME>("Emergency_LED_Controller2");
  AtomicModelPtr Fire_Alarm_Controller = cadmium::dynamic::translate::make_dynamic_atomic_model<Fire_Alarm_Con, TIME>("Fire_Alarm_Controller");
  AtomicModelPtr IR_Sensor_Controller1 = cadmium::dynamic::translate::make_dynamic_atomic_model<IR_Sensor, TIME>("IR_Sensor_Controller1");
  AtomicModelPtr IR_Sensor_Controller2 = cadmium::dynamic::translate::make_dynamic_atomic_model<IR_Sensor, TIME>("IR_Sensor_Controller2");
  AtomicModelPtr Light_Sensor_Controller = cadmium::dynamic::translate::make_dynamic_atomic_model<Light_Sensor, TIME>("Light_Sensor_Controller");
  AtomicModelPtr Room_LED_Sensor_Controller1 = cadmium::dynamic::translate::make_dynamic_atomic_model<Room_LED_Con, TIME>("Room_LED_Sensor_Controller1");
  AtomicModelPtr Room_LED_Sensor_Controller2 = cadmium::dynamic::translate::make_dynamic_atomic_model<Room_LED_Con, TIME>("Room_LED_Sensor_Controller2");
  AtomicModelPtr Fire_Sensor = cadmium::dynamic::translate::make_dynamic_atomic_model<Fire_Sensor_Con, TIME>("Fire_Sensor");
 

/********************************************/
/****************** Input *******************/
/********************************************/
  AtomicModelPtr IR_1 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR_1" , A0);
  AtomicModelPtr IR_2 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR_2", A3);
  AtomicModelPtr lightSensor = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("lightSensor", A5);
  AtomicModelPtr TempSensor = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("TempSensor", A4);
  AtomicModelPtr FireSwitch = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("FireSwitch", D12);
  
/********************************************/
/***************** Output *******************/
/********************************************/
  AtomicModelPtr R1L1 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("R1L1", D6);
  AtomicModelPtr R2L1 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("R2L1", D8);
  AtomicModelPtr R1L2 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("R1L2", D7);
  AtomicModelPtr R2L2 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("R2L2", D11);
  AtomicModelPtr E1L1 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("E1L1", D2);
  AtomicModelPtr E2L1 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("E2L1", D4);
  AtomicModelPtr E1L2 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("E1L2", D3);
  AtomicModelPtr E2L2 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("E2L2", D5);
  AtomicModelPtr AlarmOut = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("AlarmOut", D13);


/********************************/
/*******EMERGENCY SYSTEM*********/
/********************************/
struct emerg_temp_in : public cadmium::in_port<float>{};
struct emerg_switch_in : public cadmium::in_port<bool>{};
struct emerg_alarm_out : public cadmium::out_port<bool>{};
struct emerg_E1L1_out : public cadmium::out_port<bool>{};
struct emerg_E1L2_out : public cadmium::out_port<bool>{};
struct emerg_E2L1_out : public cadmium::out_port<bool>{};
struct emerg_E2L2_out : public cadmium::out_port<bool>{};

  cadmium::dynamic::modeling::Ports iports_EMERG_SYS = {typeid(emerg_temp_in), typeid(emerg_switch_in)};
  cadmium::dynamic::modeling::Ports oports_EMERG_SYS = {typeid(emerg_alarm_out), typeid(emerg_E1L1_out), typeid(emerg_E1L2_out), typeid(emerg_E2L1_out), typeid(emerg_E2L2_out)};
  
  cadmium::dynamic::modeling::Models submodels_EMERG_SYS =  {Alarm_Monitor, Emergency_LED_Controller1, Emergency_LED_Controller2, Fire_Alarm_Controller, Fire_Sensor};
  
  cadmium::dynamic::modeling::EICs eics_EMERG_SYS = {

    cadmium::dynamic::translate::make_EIC<emerg_switch_in, Fire_Alarm_defs::inp>("Fire_Alarm_Controller"),
    cadmium::dynamic::translate::make_EIC<emerg_temp_in, Fire_Sen_defs::inp>("Fire_Sensor"),
  };
  cadmium::dynamic::modeling::EOCs eocs_EMERG_SYS = {
     cadmium::dynamic::translate::make_EOC<LED_defs::out_r, emerg_E1L1_out>("Emergency_LED_Controller1"),
     cadmium::dynamic::translate::make_EOC<LED_defs::out_g, emerg_E1L2_out>("Emergency_LED_Controller1"),
     cadmium::dynamic::translate::make_EOC<LED_defs::out_r, emerg_E2L1_out>("Emergency_LED_Controller2"),
     cadmium::dynamic::translate::make_EOC<LED_defs::out_g, emerg_E2L2_out>("Emergency_LED_Controller2"),
     cadmium::dynamic::translate::make_EOC<Alarm_defs::out, emerg_alarm_out>("Alarm_Monitor")
  };
  cadmium::dynamic::modeling::ICs ics_EMERG_SYS = {
     cadmium::dynamic::translate::make_IC<Fire_Sen_defs::out, Alarm_defs::inp1>("Fire_Sensor","Alarm_Monitor"),
     cadmium::dynamic::translate::make_IC<Fire_Alarm_defs::out, Alarm_defs::inp2>("Fire_Alarm_Controller","Alarm_Monitor"),
     cadmium::dynamic::translate::make_IC<Fire_Alarm_defs::out, LED_defs::inpa>("Fire_Alarm_Controller", "Emergency_LED_Controller2"),
     cadmium::dynamic::translate::make_IC<Alarm_defs::out, LED_defs::inpb>("Alarm_Monitor", "Emergency_LED_Controller2"),
     cadmium::dynamic::translate::make_IC<Fire_Sen_defs::out, LED_defs::inpa>("Fire_Sensor", "Emergency_LED_Controller1"),
     cadmium::dynamic::translate::make_IC<Alarm_defs::out, LED_defs::inpb>("Alarm_Monitor", "Emergency_LED_Controller1")  
  };
  CoupledModelPtr EMERG_SYS = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
   "EMERG_SYS",
   submodels_EMERG_SYS,
   iports_EMERG_SYS,
   oports_EMERG_SYS,
   eics_EMERG_SYS,
   eocs_EMERG_SYS,
   ics_EMERG_SYS
   );

///****************////

/********************************/
/*******ROOM LED CONTROLLER******/
/********************************/
struct room_IR_1 : public cadmium::in_port<bool>{};
struct room_IR_2 : public cadmium::in_port<bool>{};
struct room_LightSens : public cadmium::in_port<float>{};
struct room_alarm_out : public cadmium::in_port<bool>{};

struct room_R1L1_out : public cadmium::out_port<bool>{};
struct room_R1L2_out : public cadmium::out_port<bool>{};
struct room_R2L1_out : public cadmium::out_port<bool>{};
struct room_R2L2_out : public cadmium::out_port<bool>{};

  cadmium::dynamic::modeling::Ports iports_ROOM_LED = {typeid(room_IR_1), typeid(room_IR_2), typeid(room_alarm_out), typeid(room_LightSens)};
  cadmium::dynamic::modeling::Ports oports_ROOM_LED = {typeid(room_R1L1_out), typeid(room_R1L2_out), typeid(room_R2L1_out), typeid(room_R2L2_out)};
  
  cadmium::dynamic::modeling::Models submodels_ROOM_LED =  {IR_Sensor_Controller1, IR_Sensor_Controller2, Light_Sensor_Controller, Room_LED_Sensor_Controller1, Room_LED_Sensor_Controller2};
  
  cadmium::dynamic::modeling::EICs eics_ROOM_LED = {
     cadmium::dynamic::translate::make_EIC<room_IR_1, IR_Sen_defs::inp>("IR_Sensor_Controller1"),
     cadmium::dynamic::translate::make_EIC<room_IR_2, IR_Sen_defs::inp>("IR_Sensor_Controller2"),
     cadmium::dynamic::translate::make_EIC<room_LightSens, Light_Sen_defs::inp>("Light_Sensor_Controller"),
     cadmium::dynamic::translate::make_EIC<room_alarm_out, Room_LED_defs::inpa>("Room_LED_Sensor_Controller1"),
     cadmium::dynamic::translate::make_EIC<room_alarm_out, Room_LED_defs::inpa>("Room_LED_Sensor_Controller2")
     
  };
  cadmium::dynamic::modeling::EOCs eocs_ROOM_LED = {
     cadmium::dynamic::translate::make_EOC<Room_LED_defs::out_1, room_R1L1_out>("Room_LED_Sensor_Controller1"),
     cadmium::dynamic::translate::make_EOC<Room_LED_defs::out_2, room_R1L2_out>("Room_LED_Sensor_Controller1"),        
     cadmium::dynamic::translate::make_EOC<Room_LED_defs::out_1, room_R2L1_out>("Room_LED_Sensor_Controller2"),
     cadmium::dynamic::translate::make_EOC<Room_LED_defs::out_2, room_R2L2_out>("Room_LED_Sensor_Controller2")
   };
  cadmium::dynamic::modeling::ICs ics_ROOM_LED = {
     cadmium::dynamic::translate::make_IC<IR_Sen_defs::out, Room_LED_defs::inpc>("IR_Sensor_Controller1","Room_LED_Sensor_Controller1"),
     cadmium::dynamic::translate::make_IC<Light_Sen_defs::out, Room_LED_defs::inpb>("Light_Sensor_Controller", "Room_LED_Sensor_Controller1"),
     cadmium::dynamic::translate::make_IC<IR_Sen_defs::out, Room_LED_defs::inpc>("IR_Sensor_Controller2","Room_LED_Sensor_Controller2"),
     cadmium::dynamic::translate::make_IC<Light_Sen_defs::out, Room_LED_defs::inpb>("Light_Sensor_Controller", "Room_LED_Sensor_Controller2")    
     
  };
  CoupledModelPtr ROOM_LED = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
   "ROOM_LED",
   submodels_ROOM_LED,
   iports_ROOM_LED,
   oports_ROOM_LED,
   eics_ROOM_LED,
   eocs_ROOM_LED,
   ics_ROOM_LED
   );

///****************////


/*************************/
/*******TOP MODEL*********/
/*************************/
  cadmium::dynamic::modeling::Ports iports_TOP = {};
  cadmium::dynamic::modeling::Ports oports_TOP = {};
  
  cadmium::dynamic::modeling::Models submodels_TOP =  {ROOM_LED, EMERG_SYS, lightSensor, TempSensor, FireSwitch, IR_1, IR_2, AlarmOut, R1L1, R1L2, R2L1, R2L2, E1L1, E1L2, E2L1, E2L2};
  
  cadmium::dynamic::modeling::EICs eics_TOP = {};
  cadmium::dynamic::modeling::EOCs eocs_TOP = {};
  cadmium::dynamic::modeling::ICs ics_TOP = {
  
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, emerg_temp_in>("TempSensor", "EMERG_SYS"),
     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, emerg_switch_in>("FireSwitch", "EMERG_SYS"),
     cadmium::dynamic::translate::make_IC<emerg_alarm_out, digitalOutput_defs::in>("EMERG_SYS", "AlarmOut"),
     cadmium::dynamic::translate::make_IC<emerg_E1L1_out, digitalOutput_defs::in>("EMERG_SYS", "E1L1"),
     cadmium::dynamic::translate::make_IC<emerg_E1L2_out, digitalOutput_defs::in>("EMERG_SYS", "E1L2"),
     cadmium::dynamic::translate::make_IC<emerg_E2L1_out, digitalOutput_defs::in>("EMERG_SYS", "E2L1"),
     cadmium::dynamic::translate::make_IC<emerg_E2L2_out, digitalOutput_defs::in>("EMERG_SYS", "E2L2"),


     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, room_IR_1>("IR_1", "ROOM_LED"),
     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, room_IR_2>("IR_2", "ROOM_LED"),
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, room_LightSens>("lightSensor", "ROOM_LED"),

     cadmium::dynamic::translate::make_IC<room_R1L1_out, digitalOutput_defs::in>("ROOM_LED", "R1L1"),
     cadmium::dynamic::translate::make_IC<room_R1L2_out, digitalOutput_defs::in>("ROOM_LED", "R1L2"),
     cadmium::dynamic::translate::make_IC<room_R2L1_out, digitalOutput_defs::in>("ROOM_LED", "R2L1"),
     cadmium::dynamic::translate::make_IC<room_R2L2_out, digitalOutput_defs::in>("ROOM_LED", "R2L2"),

     cadmium::dynamic::translate::make_IC<emerg_alarm_out, room_alarm_out>("EMERG_SYS", "ROOM_LED")
  };
  CoupledModelPtr TOP = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
   "TOP",
   submodels_TOP,
   iports_TOP,
   oports_TOP,
   eics_TOP,
   eocs_TOP,
   ics_TOP
   );

///****************////

  #ifdef RT_ARM_MBED
    //Enable the motors:
    DigitalOut rightMotorEn(D9);
    DigitalOut leftMotorEn(D10);
    rightMotorEn = 0;
    leftMotorEn = 0;

    // AnalogIn pot(A4);
    // while(true)
    //   cout << "Pot: " << pot.read() << "\n";
  #endif


  // Logs are currently blocking opperations. It is recommended to turn them off when embedding your application.
  // They can be used for testing; however, keep in mind they will add extra delay to your model.
  cadmium::dynamic::engine::runner<NDTime, cadmium::logger::not_logger> r(TOP, {0});
  //cadmium::dynamic::engine::runner<NDTime, log_all> r(TOP, {0});
  r.run_until(NDTime("00:10:00:000"));

  #ifndef RT_ARM_MBED
    return 0;
  #endif
}
