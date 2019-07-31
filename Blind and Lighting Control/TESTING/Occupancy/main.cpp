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

#include "../../atomics/Occupancy.hpp"

#ifdef RT_ARM_MBED
  #include "../mbed.h"
#else
 
  const char* sun_inp = "./inputs/sun_inp.txt";
  const char* IR = "./inputs/IR.txt";
 
  const char* occupy = "./outputs/occupy.txt";
  const char* light_out = "./outputs/light_out.txt";

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

  AtomicModelPtr Occupied = cadmium::dynamic::translate::make_dynamic_atomic_model<Occ_Sens, TIME>("Occupied");
  
/********************************************/
/****************** Input *******************/
/********************************************/
  
  AtomicModelPtr sun = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("sun", sun_inp);
  AtomicModelPtr occup = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("occup", IR);

/********************************************/
/***************** Output *******************/
/********************************************/
  AtomicModelPtr occ = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("occ", occupy);
  AtomicModelPtr light = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("light", light_out);

/*************************/
/*******TOP MODEL*********/
/*************************/
  cadmium::dynamic::modeling::Ports iports_TOP = {};
  cadmium::dynamic::modeling::Ports oports_TOP = {};
  
  cadmium::dynamic::modeling::Models submodels_TOP =  {light, sun, occ, occup, Occupied};

  cadmium::dynamic::modeling::EICs eics_TOP = {};
  cadmium::dynamic::modeling::EOCs eocs_TOP = {};
  cadmium::dynamic::modeling::ICs ics_TOP = {
  
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, Occ_Room::sun_intensity>("sun", "Occupied"),
     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, Occ_Room::IR>("occup", "Occupied"),

     cadmium::dynamic::translate::make_IC<Occ_Room::light_intensity, analogOutput_defs::in>("Occupied", "light"),
     cadmium::dynamic::translate::make_IC<Occ_Room::occupied, digitalOutput_defs::in>("Occupied", "occ"),  

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
