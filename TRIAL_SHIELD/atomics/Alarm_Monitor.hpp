/**
* By: Harish & Surya Krishna
* ARSLab - Carleton University
*/

#ifndef BOOST_SIMULATION_PDEVS_ALARM_MONITOR_HPP
#define BOOST_SIMULATION_PDEVS_ALARM_MONITOR_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>


using namespace cadmium;
using namespace std;

//Port definition
    struct Alarm_defs {
        //Output ports
        struct out : public out_port<bool> { };
        
        //Input ports
        struct inp1 : public in_port<bool> { };
        struct inp2 : public in_port<bool> { };
  };

    template<typename TIME>
    class Alarm_Mon {
        using defs=Alarm_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            
            // default constructor
            Alarm_Mon() noexcept{
              state.type = false;
              state.input1 = false;
              state.input2 = false;
              state.output = state.input1 || state.input2; 
            }
            
            // state definition
            struct state_type{
              bool type;
              bool input1;
              bool input2;
              bool output;
            }; 
            state_type state;
            // ports definition
            
            using input_ports=std::tuple<typename defs::inp1, typename defs::inp2>;
            using output_ports=std::tuple<typename defs::out>;

            // internal transition
            void internal_transition() {
              state.output = state.input1 || state.input2;
              state.type = false;
              //Do nothing... 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
               
              for(const auto &x : get_messages<typename defs::inp1>(mbs))
                if(state.input1 = (x == 1) ? 1 : 0)
                    state.input1 = 1;
              for(const auto &x : get_messages<typename defs::inp2>(mbs))
                if(state.input2 = (x == 1) ? 1 : 0)
                    state.input2 = 1;
             state.output = state.input1 || state.input2;
             state.type = true;
            }

            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              internal_transition();
              external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
              typename make_message_bags<output_ports>::type bags;
              get_messages<typename defs::out>(bags).push_back(state.output);
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
              if(state.type)
                return TIME("00:00:00");
              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Alarm_Mon<TIME>::state_type& i) {
              os << "Current state: " << i.type; 
              return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_ALARM_MONITOR_HPP