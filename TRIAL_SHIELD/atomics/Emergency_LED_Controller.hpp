/**
* By: Harish & Surya Krishna
* ARSLab - Carleton University
*/
#ifndef BOOST_SIMULATION_PDEVS_EMERGENCY_LED_HPP
#define BOOST_SIMULATION_PDEVS_EMERGENCY_LED_HPP

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
    struct LED_defs {
        //Output ports
        struct out_g : public out_port<bool> { };
        struct out_r : public out_port<bool> { };
        
        //Input ports
        struct inpa : public in_port<bool> { };
        struct inpb : public in_port<bool> { };
  };

    template<typename TIME>
    class Emer_LED_Con {
        using defs=LED_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            
            // default constructor
            Emer_LED_Con() noexcept{
              state.type = false;
              state.input_a = false;
              state.input_b = false;
              state.output_green = false;
              state.output_red = false; 
            }
            
            // state definition
            struct state_type{
              bool type;
              bool input_a;
              bool input_b;
              bool output_green;
              bool output_red;
            }; 
            state_type state;
            // ports definition
            
            using input_ports=std::tuple<typename defs::inpa, typename defs::inpb>;
            using output_ports=std::tuple<typename defs::out_g, typename defs::out_r>;

            // internal transition
            void internal_transition() {
              
              
              state.type = false;
              //Do nothing... 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
               
              for(const auto &x : get_messages<typename defs::inpa>(mbs))
                state.input_a = (x == 1) ? 1 : 0;
                    
              for(const auto &x : get_messages<typename defs::inpb>(mbs))
                state.input_b = (x == 1) ? 1 : 0;
                  
              if(state.input_a){                          
                state.output_red = 1;
                state.output_green = 0;
                }
              else if(state.input_b){                    
                state.output_green = 1;
                state.output_red = 0;
                }
              else{                    
                state.output_green = 0;
                state.output_red = 0;
                }

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
              
              get_messages<typename defs::out_g>(bags).push_back(state.output_green);
              get_messages<typename defs::out_r>(bags).push_back(state.output_red);
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
              if(state.type)
                return TIME("00:00:00");
              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Emer_LED_Con<TIME>::state_type& i) {
              os << "Current state: " << i.type; 
              return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_EMERGENCY_LED_HPP