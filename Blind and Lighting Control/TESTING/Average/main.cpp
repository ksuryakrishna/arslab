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

#include "../../atomics/Average.hpp"

#ifdef RT_ARM_MBED
  #include "../mbed.h"
#else
 
  const char* occ_1 = "./inputs/occ_1.txt";
  const char* occ_2 = "./inputs/occ_2.txt";
  const char* occ_3 = "./inputs/occ_3.txt";
  const char* occ_4 = "./inputs/occ_4.txt";

  const char* light_in1 = "./inputs/light_in1.txt";
  const char* light_in2 = "./inputs/light_in2.txt";
  const char* light_in3 = "./inputs/light_in3.txt";
  const char* light_in4 = "./inputs/light_in4.txt";

  const char* occupied = "./outputs/occupied.txt";
  const char* light_avg = "./outputs/light_avg.txt";

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

  AtomicModelPtr Blind_Input = cadmium::dynamic::translate::make_dynamic_atomic_model<Avg_Sens, TIME>("Blind_Input");
  
/********************************************/
/****************** Input *******************/
/********************************************/
  
  AtomicModelPtr light1 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("light1", light_in1);
  AtomicModelPtr light2 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("light2", light_in2);
  AtomicModelPtr light3 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("light3", light_in3);
  AtomicModelPtr light4 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("light4", light_in4);


  AtomicModelPtr IR1 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR1", occ_1);
  AtomicModelPtr IR2 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR2", occ_2);
  AtomicModelPtr IR3 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR3", occ_3);
  AtomicModelPtr IR4 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR4", occ_4);
  
/********************************************/
/***************** Output *******************/
/********************************************/
  AtomicModelPtr occupy = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalOutput, TIME>("occupy", occupied);
  AtomicModelPtr light_out = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("light_out", light_avg);

/*************************/
/*******TOP MODEL*********/
/*************************/
  cadmium::dynamic::modeling::Ports iports_TOP = {};
  cadmium::dynamic::modeling::Ports oports_TOP = {};
  
  cadmium::dynamic::modeling::Models submodels_TOP =  {Blind_Input, light1, light2, light3, light4, IR1, IR2, IR3, IR4, light_out, occupy};

  cadmium::dynamic::modeling::EICs eics_TOP = {};
  cadmium::dynamic::modeling::EOCs eocs_TOP = {};
  cadmium::dynamic::modeling::ICs ics_TOP = {
  
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, Avg_Room::Room1>("light1", "Blind_Input"),
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, Avg_Room::Room2>("light2", "Blind_Input"),
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, Avg_Room::Room3>("light3", "Blind_Input"),
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, Avg_Room::Room4>("light4", "Blind_Input"),

     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, Avg_Room::occ1>("IR1", "Blind_Input"),
     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, Avg_Room::occ2>("IR2", "Blind_Input"),
     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, Avg_Room::occ3>("IR3", "Blind_Input"),
     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, Avg_Room::occ4>("IR4", "Blind_Input"),

     cadmium::dynamic::translate::make_IC<Avg_Room::light_intensity, analogOutput_defs::in>("Blind_Input", "light_out"),
     cadmium::dynamic::translate::make_IC<Avg_Room::occupy, digitalOutput_defs::in>("Blind_Input", "occupy"),  

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
