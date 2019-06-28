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

#include "../vendor/NDTime.hpp"
#include "../vendor/iestream.hpp"

#include "../data_structures/message.hpp"

#include "../atomics/andCAD.hpp"
#include "../atomics/xorCAD.hpp"




using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;


/***** SETING INPUT PORTS FOR COUPLEDs *****/
struct inp_1 : public cadmium::in_port<Message_t>{};
struct inp_2 : public cadmium::in_port<Message_t>{};
/***** SETING OUTPUT PORTS FOR COUPLEDs *****/
struct sum_out : public cadmium::out_port<Message_t>{};
struct carry_out : public cadmium::out_port<Message_t>{};

/********************************************/
/****** APPLICATION GENERATOR *******************/
/********************************************/
template<typename T>
class ApplicationGen : public iestream_input<Message_t,T> {
public:
  ApplicationGen() = default;
  ApplicationGen(const char* file_path) : iestream_input<Message_t,T>(file_path) {}
};


int main(int argc, char ** argv) {

  if (argc < 2) {
   cout << "you are using this program with wrong parameters. The program should be invoked as follows:";
   cout << argv[0] << " path to the input file " << endl;
   return 1; 
  }

  auto start = hclock::now(); //to measure simulation execution time

/*************** Loggers *******************/
  static std::ofstream out_data("half_adder_output.txt");
    struct oss_sink_provider{
        static std::ostream& sink(){          
            return out_data;
        }
    };

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
/****** APPLICATION GENERATOR *******************/
/********************************************/
string input_data_control_one = argv[1];
const char * i_input_data_control_one = input_data_control_one.c_str();

std::shared_ptr<cadmium::dynamic::modeling::model> generator_con_one = cadmium::dynamic::translate::make_dynamic_atomic_model<ApplicationGen, TIME, const char* >("generator_con_one" , std::move(i_input_data_control_one));

string input_data_control_two = argv[2];
const char * i_input_data_control_two = input_data_control_two.c_str();

std::shared_ptr<cadmium::dynamic::modeling::model> generator_con_two = cadmium::dynamic::translate::make_dynamic_atomic_model<ApplicationGen, TIME, const char* >("generator_con_two" , std::move(i_input_data_control_two));

/********************************************/
/****** and *******************/
/********************************************/

std::shared_ptr<cadmium::dynamic::modeling::model> andCAD1 = cadmium::dynamic::translate::make_dynamic_atomic_model<andCAD, TIME>("andCAD1");

/********************************************/
/****** xor *******************/
/********************************************/

std::shared_ptr<cadmium::dynamic::modeling::model> xorCAD1 = cadmium::dynamic::translate::make_dynamic_atomic_model<xorCAD, TIME>("xorCAD1");


/************************/
/*******Half Adder********/
/************************/
cadmium::dynamic::modeling::Ports iports_Half_Adder = {typeid(inp_1), typeid(inp_2)};
cadmium::dynamic::modeling::Ports oports_Half_Adder = {typeid(sum_out),typeid(carry_out)};
cadmium::dynamic::modeling::Models submodels_Half_Adder = {andCAD1, xorCAD1};
cadmium::dynamic::modeling::EICs eics_Half_Adder = {
  cadmium::dynamic::translate::make_EIC<inp_1, andCAD_defs::in1>("andCAD1"), 
  cadmium::dynamic::translate::make_EIC<inp_1, xorCAD_defs::in1>("xorCAD1"), 
  cadmium::dynamic::translate::make_EIC<inp_2, andCAD_defs::in2>("andCAD1"), 
  cadmium::dynamic::translate::make_EIC<inp_2, xorCAD_defs::in2>("xorCAD1")
};

cadmium::dynamic::modeling::EOCs eocs_Half_Adder = {
  cadmium::dynamic::translate::make_EOC<andCAD_defs::outa, sum_out>("andCAD1"),
  cadmium::dynamic::translate::make_EOC<xorCAD_defs::outx, carry_out>("xorCAD1")
};
cadmium::dynamic::modeling::ICs ics_Half_Adder = {};

std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> Half_Adder = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
 "Half_Adder", 
 submodels_Half_Adder, 
 iports_Half_Adder, 
 oports_Half_Adder, 
 eics_Half_Adder, 
 eocs_Half_Adder, 
 ics_Half_Adder
  );


/************************/
/*******TOP MODEL********/
/************************/
cadmium::dynamic::modeling::Ports iports_TOP = {};
cadmium::dynamic::modeling::Ports oports_TOP = {typeid(sum_out),typeid(carry_out)};
cadmium::dynamic::modeling::Models submodels_TOP = {generator_con_one, generator_con_two, Half_Adder};
cadmium::dynamic::modeling::EICs eics_TOP = {};
cadmium::dynamic::modeling::EOCs eocs_TOP = {
  cadmium::dynamic::translate::make_EOC<sum_out, sum_out>("Half_Adder"),
  cadmium::dynamic::translate::make_EOC<carry_out,carry_out>("Half_Adder")
};
cadmium::dynamic::modeling::ICs ics_TOP = {
  cadmium::dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,inp_1>("generator_con_one","Half_Adder"),
  cadmium::dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,inp_2>("generator_con_two","Half_Adder")
};
std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
 "TOP", 
 submodels_TOP, 
 iports_TOP, 
 oports_TOP, 
 eics_TOP, 
 eocs_TOP, 
 ics_TOP 
  );

///****************////

    auto elapsed1 = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Model Created. Elapsed time: " << elapsed1 << "sec" << endl;
    
    cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    elapsed1 = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Runner Created. Elapsed time: " << elapsed1 << "sec" << endl;

    cout << "Simulation starts" << endl;

    r.run_until(NDTime("04:00:00:000"));
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Simulation took:" << elapsed << "sec" << endl;
    return 0;
}