/*
* By: Harish & Surya Krishna
* ARSLab - Carleton University
*/
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium.h>
#include <cadmium/io/iestream.hpp>

#include <NDTime.hpp>

#include <cadmium/real_time/arm_mbed//io/analogInput.hpp>
#include <cadmium/real_time/arm_mbed//io/analogOutput.hpp>
#include <cadmium/real_time/arm_mbed//io/digitalInput.hpp>
#include <cadmium/real_time/arm_mbed//io/digitalOutput.hpp>

#include "../../atomics/Sunlight Calc.hpp"

#ifdef RT_ARM_MBED
  #include "../mbed.h"
#else
 
  const char* sun_inp = "./inputs/sun_inp.txt";
  const char* dist = "./inputs/dist.txt";
 
  const char* sun_out = "./outputs/light_out.txt";

#endif

using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;


int main(int argc, char ** argv) {

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

    static std::ofstream out_data("Test_output.txt");
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

  AtomicModelPtr Sunlight = cadmium::dynamic::translate::make_dynamic_atomic_model<Sun_Sens, TIME>("Sunlight");
  
/********************************************/
/****************** Input *******************/
/********************************************/
  
  AtomicModelPtr sun_i = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("sun_i", sun_inp);
  AtomicModelPtr distance = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("distance", dist);

/********************************************/
/***************** Output *******************/
/********************************************/
  AtomicModelPtr sun_o = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("sun_o", sun_out);

/*************************/
/*******TOP MODEL*********/
/*************************/
  cadmium::dynamic::modeling::Ports iports_TOP = {};
  cadmium::dynamic::modeling::Ports oports_TOP = {};
  
  cadmium::dynamic::modeling::Models submodels_TOP =  {sun_o, sun_i, distance, Sunlight};

  cadmium::dynamic::modeling::EICs eics_TOP = {};
  cadmium::dynamic::modeling::EOCs eocs_TOP = {};
  cadmium::dynamic::modeling::ICs ics_TOP = {
  
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, Sun_Room::sun_calc>("sun_i", "Sunlight"),
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, Sun_Room::dist>("distance", "Sunlight"),

     cadmium::dynamic::translate::make_IC<Sun_Room::sunlight_intensity, analogOutput_defs::in>("Sunlight", "sun_o")
     
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

cadmium::dynamic::engine::runner<NDTime, cadmium::logger::not_logger> r(TOP, {0});
r.run_until(NDTime("00:10:00:000"));

  #ifndef RT_ARM_MBED
    return 0;
  #endif
}
