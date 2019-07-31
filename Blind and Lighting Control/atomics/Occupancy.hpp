/*
* By: Harish & Surya Krishna
* ARSLab - Carleton University
*/
#ifndef BOOST_SIMULATION_PDEVS_OCCUPANCY_HPP
#define BOOST_SIMULATION_PDEVS_OCCUPANCY_HPP

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
    struct Occ_Room {
        //Output ports
        struct occupied : public out_port<bool> { };
        struct light_intensity : public out_port<float> { };
               
        
        //Input ports
        struct sun_intensity : public in_port<float> { };
        struct IR : public in_port<bool> { };
      };

    template<typename TIME>
    class Occ_Sens {
        using defs=Occ_Room; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            
            // default constructor
            Occ_Sens() noexcept{
              state.sun_in = 0.0;
              state.ir_in = 0;
              
              state.occ = 0;
              state.light_in = 0.0;             
                     
            }
            
            // state definition
            struct state_type{
              bool type;

              bool occ;
              float light_in;
              float d_o;

              float sun_in;
              bool ir_in;
              float d_i;
            }; 
            state_type state;
           
            // ports definition
            
            using input_ports=std::tuple<typename defs::sun_intensity, typename defs::IR>;
            using output_ports=std::tuple<typename defs::occupied, typename defs::light_intensity>;

            // internal transition
            void internal_transition() {
               state.type = false;
              //Do nothing... 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 

              float bright = 600;
             
              for(const auto &x : get_messages<typename defs::sun_intensity>(mbs))
                    state.sun_in = x;
              for(const auto &x : get_messages<typename defs::IR>(mbs))
                    state.ir_in = x;
                           
              state.occ = !state.ir_in;

              if(state.occ){
                if(state.sun_in < 600)
                state.light_in = bright - state.sun_in;

              }

              else state.light_in = 0;
              
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
              get_messages<typename defs::occupied>(bags).push_back(state.occ);
			        get_messages<typename defs::light_intensity>(bags).push_back(state.light_in);
              
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
               if(state.type)
                return TIME("00:00:01");
              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Occ_Sens<TIME>::state_type& i) {
              os << "Current state: " << i.light_in; 
              return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_OCCUPANCY_HPP