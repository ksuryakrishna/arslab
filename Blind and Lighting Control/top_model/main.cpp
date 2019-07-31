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

#include "../atomics/Illuminance.hpp"
#include "../atomics/Average.hpp"
#include "../atomics/Occupancy.hpp"
#include "../atomics/Sunlight Calc.hpp"

#ifdef RT_ARM_MBED
  #include "../mbed.h"
#else
  const char* sun_l  = "./inputs/sunlight_intensity.txt";
  const char* ang  = "./inputs/angle_of_incidence.txt";
  
  const char* IR_in1 = "./inputs/IR_in1.txt";
  const char* IR_in2 = "./inputs/IR_in2.txt";
  const char* IR_in3 = "./inputs/IR_in3.txt";
  const char* IR_in4 = "./inputs/IR_in4.txt";

  const char* dist_in1 = "./inputs/dist_in1.txt";
  const char* dist_in2 = "./inputs/dist_in2.txt";
  const char* dist_in3 = "./inputs/dist_in3.txt";
  const char* dist_in4 = "./inputs/dist_in4.txt";

  const char* illumin = "./outputs/Illumination.txt";
  const char* screen_height = "./outputs/screen.txt";

  const char* light_intensity1 = "./outputs/light_intensity1.txt";
  const char* light_intensity2 = "./outputs/light_intensity2.txt";
  const char* light_intensity3 = "./outputs/light_intensity3.txt";
  const char* light_intensity4 = "./outputs/light_intensity4.txt";


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

    static std::ofstream out_data("Test_Output.txt");
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
  AtomicModelPtr Light_Illum = cadmium::dynamic::translate::make_dynamic_atomic_model<Light_Sens, TIME>("Light_Illum");

  AtomicModelPtr Corner_input1 = cadmium::dynamic::translate::make_dynamic_atomic_model<Sun_Sens, TIME>("Corner_input1");
  AtomicModelPtr Corner_Illum1 = cadmium::dynamic::translate::make_dynamic_atomic_model<Occ_Sens, TIME>("Corner_Illum1");

  AtomicModelPtr Corner_input2 = cadmium::dynamic::translate::make_dynamic_atomic_model<Sun_Sens, TIME>("Corner_input2");
  AtomicModelPtr Corner_Illum2 = cadmium::dynamic::translate::make_dynamic_atomic_model<Occ_Sens, TIME>("Corner_Illum2");
  
  AtomicModelPtr Corner_input3 = cadmium::dynamic::translate::make_dynamic_atomic_model<Sun_Sens, TIME>("Corner_input3");
  AtomicModelPtr Corner_Illum3 = cadmium::dynamic::translate::make_dynamic_atomic_model<Occ_Sens, TIME>("Corner_Illum3");

  AtomicModelPtr Corner_input4 = cadmium::dynamic::translate::make_dynamic_atomic_model<Sun_Sens, TIME>("Corner_input4");
  AtomicModelPtr Corner_Illum4 = cadmium::dynamic::translate::make_dynamic_atomic_model<Occ_Sens, TIME>("Corner_Illum4");

/********************************************/
/****************** Input *******************/
/********************************************/
  AtomicModelPtr sun = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("sun" , sun_l);
  AtomicModelPtr angle = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("angle", ang);

  AtomicModelPtr dist1 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("dist1", dist_in1);
  AtomicModelPtr dist2 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("dist2", dist_in2);
  AtomicModelPtr dist3 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("dist3", dist_in3);
  AtomicModelPtr dist4 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("dist4", dist_in4);


  AtomicModelPtr IR1 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR1", IR_in1);
  AtomicModelPtr IR2 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR2", IR_in2);
  AtomicModelPtr IR3 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR3", IR_in3);
  AtomicModelPtr IR4 = cadmium::dynamic::translate::make_dynamic_atomic_model<DigitalInput, TIME>("IR4", IR_in4);
  
/********************************************/
/***************** Output *******************/
/********************************************/
  AtomicModelPtr illum = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("illum", illumin);
  AtomicModelPtr screen = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("screen", screen_height);

  AtomicModelPtr light1 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("light1", light_intensity1);
  AtomicModelPtr light2 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("light2", light_intensity2);
  AtomicModelPtr light3 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("light3", light_intensity3);
  AtomicModelPtr light4 = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogOutput, TIME>("light4", light_intensity4);

/*********************************/
/*******CORNER 1 COUPLING*********/
/*********************************/
struct sun_input_1 : public cadmium::in_port<float>{};
struct IR_1 : public cadmium::in_port<bool>{};
struct dist_1 : public cadmium::in_port<float>{};

struct light_out_1 : public cadmium::out_port<float>{};
struct occupy_1 : public cadmium::out_port<bool>{};

  cadmium::dynamic::modeling::Ports iports_CO_CO_1 = {typeid(sun_input_1), typeid(IR_1), typeid(dist_1)};
  cadmium::dynamic::modeling::Ports oports_CO_CO_1 = {typeid(light_out_1), typeid(occupy_1)};
  
  cadmium::dynamic::modeling::Models submodels_CO_CO_1 =  {Corner_Illum1, Corner_input1};
  
  cadmium::dynamic::modeling::EICs eics_CO_CO_1 = {

    cadmium::dynamic::translate::make_EIC<IR_1, Occ_Room::IR>("Corner_Illum1"),
    cadmium::dynamic::translate::make_EIC<dist_1, Sun_Room::dist>("Corner_input1"),
    cadmium::dynamic::translate::make_EIC<sun_input_1, Sun_Room::sun_calc>("Corner_input1")
  };
  cadmium::dynamic::modeling::EOCs eocs_CO_CO_1 = {
     cadmium::dynamic::translate::make_EOC<Occ_Room::light_intensity, light_out_1>("Corner_Illum1"),
     cadmium::dynamic::translate::make_EOC<Occ_Room::occupied, occupy_1>("Corner_Illum1")
  };
  cadmium::dynamic::modeling::ICs ics_CO_CO_1 = {
     cadmium::dynamic::translate::make_IC<Sun_Room::sunlight_intensity, Occ_Room::sun_intensity>("Corner_input1","Corner_Illum1")
  };
  CoupledModelPtr CO_CO_1 = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
   "CO_CO_1",
   submodels_CO_CO_1,
   iports_CO_CO_1,
   oports_CO_CO_1,
   eics_CO_CO_1,
   eocs_CO_CO_1,
   ics_CO_CO_1
   );

/*********************************/
/*******CORNER 2 COUPLING*********/
/*********************************/

struct sun_input_2 : public cadmium::in_port<float>{};
struct IR_2 : public cadmium::in_port<bool>{};
struct dist_2 : public cadmium::in_port<float>{};

struct light_out_2 : public cadmium::out_port<float>{};
struct occupy_2 : public cadmium::out_port<bool>{};

  cadmium::dynamic::modeling::Ports iports_CO_CO_2 = {typeid(sun_input_2), typeid(IR_2), typeid(dist_2)};
  cadmium::dynamic::modeling::Ports oports_CO_CO_2 = {typeid(light_out_2), typeid(occupy_2)};
  
  cadmium::dynamic::modeling::Models submodels_CO_CO_2 =  {Corner_Illum2, Corner_input2};
  
  cadmium::dynamic::modeling::EICs eics_CO_CO_2 = {

    cadmium::dynamic::translate::make_EIC<IR_2, Occ_Room::IR>("Corner_Illum2"),
    cadmium::dynamic::translate::make_EIC<dist_2, Sun_Room::dist>("Corner_input2"),
    cadmium::dynamic::translate::make_EIC<sun_input_2, Sun_Room::sun_calc>("Corner_input2")
  };
  cadmium::dynamic::modeling::EOCs eocs_CO_CO_2 = {
     cadmium::dynamic::translate::make_EOC<Occ_Room::light_intensity, light_out_2>("Corner_Illum2"),
     cadmium::dynamic::translate::make_EOC<Occ_Room::occupied, occupy_2>("Corner_Illum2")
  };
  cadmium::dynamic::modeling::ICs ics_CO_CO_2 = {
     cadmium::dynamic::translate::make_IC<Sun_Room::sunlight_intensity, Occ_Room::sun_intensity>("Corner_input2","Corner_Illum2")
  };
  CoupledModelPtr CO_CO_2 = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
   "CO_CO_2",
   submodels_CO_CO_2,
   iports_CO_CO_2,
   oports_CO_CO_2,
   eics_CO_CO_2,
   eocs_CO_CO_2,
   ics_CO_CO_2
   );

/*********************************/
/*******CORNER 3 COUPLING*********/
/*********************************/

struct sun_input_3 : public cadmium::in_port<float>{};
struct IR_3 : public cadmium::in_port<bool>{};
struct dist_3 : public cadmium::in_port<float>{};

struct light_out_3 : public cadmium::out_port<float>{};
struct occupy_3 : public cadmium::out_port<bool>{};

  cadmium::dynamic::modeling::Ports iports_CO_CO_3 = {typeid(sun_input_3), typeid(IR_3), typeid(dist_3)};
  cadmium::dynamic::modeling::Ports oports_CO_CO_3 = {typeid(light_out_3), typeid(occupy_3)};
  
  cadmium::dynamic::modeling::Models submodels_CO_CO_3 =  {Corner_Illum3, Corner_input3};
  
  cadmium::dynamic::modeling::EICs eics_CO_CO_3 = {

    cadmium::dynamic::translate::make_EIC<IR_3, Occ_Room::IR>("Corner_Illum3"),
    cadmium::dynamic::translate::make_EIC<dist_3, Sun_Room::dist>("Corner_input3"),
    cadmium::dynamic::translate::make_EIC<sun_input_3, Sun_Room::sun_calc>("Corner_input3")
  };
  cadmium::dynamic::modeling::EOCs eocs_CO_CO_3 = {
     cadmium::dynamic::translate::make_EOC<Occ_Room::light_intensity, light_out_3>("Corner_Illum3"),
     cadmium::dynamic::translate::make_EOC<Occ_Room::occupied, occupy_3>("Corner_Illum3")
  };
  cadmium::dynamic::modeling::ICs ics_CO_CO_3 = {
     cadmium::dynamic::translate::make_IC<Sun_Room::sunlight_intensity, Occ_Room::sun_intensity>("Corner_input3","Corner_Illum3")
  };
  CoupledModelPtr CO_CO_3 = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
   "CO_CO_3",
   submodels_CO_CO_3,
   iports_CO_CO_3,
   oports_CO_CO_3,
   eics_CO_CO_3,
   eocs_CO_CO_3,
   ics_CO_CO_3
   );

/*********************************/
/*******CORNER 4 COUPLING*********/
/*********************************/
struct sun_input_4 : public cadmium::in_port<float>{};
struct IR_4 : public cadmium::in_port<bool>{};
struct dist_4 : public cadmium::in_port<float>{};

struct light_out_4 : public cadmium::out_port<float>{};
struct occupy_4 : public cadmium::out_port<bool>{};

  cadmium::dynamic::modeling::Ports iports_CO_CO_4 = {typeid(sun_input_4), typeid(IR_4), typeid(dist_4)};
  cadmium::dynamic::modeling::Ports oports_CO_CO_4 = {typeid(light_out_4), typeid(occupy_4)};
  
  cadmium::dynamic::modeling::Models submodels_CO_CO_4 =  {Corner_Illum4, Corner_input4};
  
  cadmium::dynamic::modeling::EICs eics_CO_CO_4 = {

    cadmium::dynamic::translate::make_EIC<IR_4, Occ_Room::IR>("Corner_Illum4"),
    cadmium::dynamic::translate::make_EIC<dist_4, Sun_Room::dist>("Corner_input4"),
    cadmium::dynamic::translate::make_EIC<sun_input_4, Sun_Room::sun_calc>("Corner_input4")
  };
  cadmium::dynamic::modeling::EOCs eocs_CO_CO_4 = {
     cadmium::dynamic::translate::make_EOC<Occ_Room::light_intensity, light_out_4>("Corner_Illum4"),
     cadmium::dynamic::translate::make_EOC<Occ_Room::occupied, occupy_4>("Corner_Illum4")
  };
  cadmium::dynamic::modeling::ICs ics_CO_CO_4 = {
     cadmium::dynamic::translate::make_IC<Sun_Room::sunlight_intensity, Occ_Room::sun_intensity>("Corner_input4","Corner_Illum4")
  };
  CoupledModelPtr CO_CO_4 = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
   "CO_CO_4",
   submodels_CO_CO_4,
   iports_CO_CO_4,
   oports_CO_CO_4,
   eics_CO_CO_4,
   eocs_CO_CO_4,
   ics_CO_CO_4
   );

/**************************************/
/*******BLIND CONTROL COUPLING*********/
/**************************************/
struct sun_intensity : public cadmium::in_port<float>{};
struct angle_incidence : public cadmium::in_port<float>{};

struct occu_1 : public cadmium::in_port<bool>{};
struct light_corner_1 : public cadmium::in_port<float>{};

struct occu_2 : public cadmium::in_port<bool>{};
struct light_corner_2 : public cadmium::in_port<float>{};

struct occu_3 : public cadmium::in_port<bool>{};
struct light_corner_3 : public cadmium::in_port<float>{};

struct occu_4 : public cadmium::in_port<bool>{};
struct light_corner_4 : public cadmium::in_port<float>{};


struct blind_h : public cadmium::out_port<float>{};
struct sun_out : public cadmium::out_port<float>{};

  cadmium::dynamic::modeling::Ports iports_BLIND_CO = {typeid(sun_intensity), typeid(angle_incidence), typeid(occu_1), typeid(light_corner_1), typeid(occu_2), typeid(light_corner_2), typeid(occu_3), typeid(light_corner_3), typeid(occu_4), typeid(light_corner_4)};
  cadmium::dynamic::modeling::Ports oports_BLIND_CO = {typeid(sun_out), typeid(blind_h)};
  
  cadmium::dynamic::modeling::Models submodels_BLIND_CO =  {Blind_Input, Light_Illum};
  
  cadmium::dynamic::modeling::EICs eics_BLIND_CO = {

    cadmium::dynamic::translate::make_EIC<sun_intensity, Light_Room::sun_intensity>("Light_Illum"),
    cadmium::dynamic::translate::make_EIC<angle_incidence, Light_Room::AOI>("Light_Illum"),

    cadmium::dynamic::translate::make_EIC<occu_1, Avg_Room::occ1>("Blind_Input"),
    cadmium::dynamic::translate::make_EIC<light_corner_1, Avg_Room::Room1>("Blind_Input"),

    cadmium::dynamic::translate::make_EIC<occu_2, Avg_Room::occ2>("Blind_Input"),
    cadmium::dynamic::translate::make_EIC<light_corner_2, Avg_Room::Room2>("Blind_Input"),


    cadmium::dynamic::translate::make_EIC<occu_3, Avg_Room::occ3>("Blind_Input"),
    cadmium::dynamic::translate::make_EIC<light_corner_3, Avg_Room::Room3>("Blind_Input"),


    cadmium::dynamic::translate::make_EIC<occu_4, Avg_Room::occ4>("Blind_Input"),
    cadmium::dynamic::translate::make_EIC<light_corner_4, Avg_Room::Room4>("Blind_Input")

  };
  cadmium::dynamic::modeling::EOCs eocs_BLIND_CO = {
     cadmium::dynamic::translate::make_EOC<Light_Room::Illuminance, sun_out>("Light_Illum"),
     cadmium::dynamic::translate::make_EOC<Light_Room::Screen_A, blind_h>("Light_Illum")
  };
  cadmium::dynamic::modeling::ICs ics_BLIND_CO = {
     cadmium::dynamic::translate::make_IC<Avg_Room::light_intensity, Light_Room::light_intensity>("Blind_Input","Light_Illum"),
     cadmium::dynamic::translate::make_IC<Avg_Room::occupy, Light_Room::occup>("Blind_Input","Light_Illum")
  };
  CoupledModelPtr BLIND_CO = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
   "BLIND_CO",
   submodels_BLIND_CO,
   iports_BLIND_CO,
   oports_BLIND_CO,
   eics_BLIND_CO,
   eocs_BLIND_CO,
   ics_BLIND_CO
   );

/*************************/
/*******TOP MODEL*********/
/*************************/
  cadmium::dynamic::modeling::Ports iports_TOP = {};
  cadmium::dynamic::modeling::Ports oports_TOP = {};
  
  cadmium::dynamic::modeling::Models submodels_TOP =  {BLIND_CO, CO_CO_1, CO_CO_2, CO_CO_3, CO_CO_4, sun, angle, dist1, dist2, dist3, dist4, IR1, IR2, IR3, IR4, illum, screen, light1, light2, light3, light4};

  cadmium::dynamic::modeling::EICs eics_TOP = {};
  cadmium::dynamic::modeling::EOCs eocs_TOP = {};
  cadmium::dynamic::modeling::ICs ics_TOP = {
  
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, sun_intensity>("sun", "BLIND_CO"),
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, angle_incidence>("angle", "BLIND_CO"),
     cadmium::dynamic::translate::make_IC<sun_out, analogOutput_defs::in>("BLIND_CO", "illum"),
     cadmium::dynamic::translate::make_IC<blind_h, analogOutput_defs::in>("BLIND_CO", "screen"),

      cadmium::dynamic::translate::make_IC<digitalInput_defs::out, IR_1>("IR1", "CO_CO_1"),
      cadmium::dynamic::translate::make_IC<analogInput_defs::out, dist_1>("dist1", "CO_CO_1"),
      cadmium::dynamic::translate::make_IC<light_out_1, analogOutput_defs::in>("CO_CO_1", "light1"),
     
     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, IR_2>("IR2", "CO_CO_2"),
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, dist_2>("dist2", "CO_CO_2"),
     cadmium::dynamic::translate::make_IC<light_out_2, analogOutput_defs::in>("CO_CO_2", "light2"),
     
     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, IR_3>("IR3", "CO_CO_3"),
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, dist_3>("dist3", "CO_CO_3"),
      cadmium::dynamic::translate::make_IC<light_out_3, analogOutput_defs::in>("CO_CO_3", "light3"),
     
     cadmium::dynamic::translate::make_IC<digitalInput_defs::out, IR_4>("IR4", "CO_CO_4"),
     cadmium::dynamic::translate::make_IC<analogInput_defs::out, dist_4>("dist4", "CO_CO_4"),
     cadmium::dynamic::translate::make_IC<light_out_4, analogOutput_defs::in>("CO_CO_4", "light4"),

     cadmium::dynamic::translate::make_IC<sun_out, sun_input_1>("BLIND_CO", "CO_CO_1"),
     cadmium::dynamic::translate::make_IC<sun_out, sun_input_2>("BLIND_CO", "CO_CO_2"),
     cadmium::dynamic::translate::make_IC<sun_out, sun_input_3>("BLIND_CO", "CO_CO_3"),
     cadmium::dynamic::translate::make_IC<sun_out, sun_input_4>("BLIND_CO", "CO_CO_4"),

     cadmium::dynamic::translate::make_IC<occupy_1, occu_1>("CO_CO_1", "BLIND_CO"),
     cadmium::dynamic::translate::make_IC<light_out_1, light_corner_1>("CO_CO_1", "BLIND_CO"),
      
     cadmium::dynamic::translate::make_IC<occupy_2, occu_2>("CO_CO_2", "BLIND_CO"),
     cadmium::dynamic::translate::make_IC<light_out_2, light_corner_2>("CO_CO_2", "BLIND_CO"),
    
     cadmium::dynamic::translate::make_IC<occupy_3, occu_3>("CO_CO_3", "BLIND_CO"),
     cadmium::dynamic::translate::make_IC<light_out_3, light_corner_3>("CO_CO_3", "BLIND_CO"),
    
     cadmium::dynamic::translate::make_IC<occupy_4, occu_4>("CO_CO_4", "BLIND_CO"),
     cadmium::dynamic::translate::make_IC<light_out_4, light_corner_4>("CO_CO_4", "BLIND_CO")  

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
