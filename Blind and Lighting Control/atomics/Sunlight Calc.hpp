/*
* By: Harish & Surya Krishna
* ARSLab - Carleton University
*/
#ifndef BOOST_SIMULATION_PDEVS_SUNLIGHT_CALCULATION_HPP
#define BOOST_SIMULATION_PDEVS_SUNLIGHT_CALCULATION_HPP

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
    struct Sun_Room {
        //Output ports
        struct sunlight_intensity : public out_port<float> { };
        
        //Input ports
        struct dist : public in_port<float> { };
        struct sun_calc : public in_port<float> { };        
  };

    template<typename TIME>
    class Sun_Sens {
        using defs=Sun_Room; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            
            // default constructor
            Sun_Sens() noexcept{
              state.d = 0.0;
              state.sun = 0.0;
              state.light = 0.0;             
                     
            }
            
            // state definition
            struct state_type{
              bool type;

              float d;
              float sun;
              float light;
            }; 
            state_type state;
            // ports definition
            
            using input_ports=std::tuple<typename defs::dist, typename defs::sun_calc>;
            using output_ports=std::tuple<typename defs::sunlight_intensity>;

            // internal transition
            void internal_transition() {
               state.type = false;
              //Do nothing... 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
             
              for(const auto &x : get_messages<typename defs::dist>(mbs))
                    state.d = x;
              for(const auto &x : get_messages<typename defs::sun_calc>(mbs))
                    state.sun = x;

            state.light = state.sun * (2.75 * 2.75) / (state.d * state.d);
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
              get_messages<typename defs::sunlight_intensity>(bags).push_back(state.light);
			        
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
               if(state.type)
                return TIME("00:00:01");
              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Sun_Sens<TIME>::state_type& i) {
              os << "Current state: " << i.light; 
              return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_SUNLIGHT_CALCULATION_HPP