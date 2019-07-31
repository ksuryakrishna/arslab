/*
* By: Harish & Surya Krishna
* ARSLab - Carleton University
*/
#ifndef BOOST_SIMULATION_PDEVS_LIGHT_INTENSITY_HPP
#define BOOST_SIMULATION_PDEVS_LIGHT_INTENSITY_HPP

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
    struct Light_Room {
        //Output ports
        struct Illuminance : public out_port<float> { };
        struct Screen_A : public out_port<float> { };
        
        //Input ports
        struct sun_intensity : public in_port<float> { };
        struct AOI : public in_port<float> { };
        struct occup : public in_port<bool> { };
        struct light_intensity : public in_port<float> { };

  };

    template<typename TIME>
    class Light_Sens {
        using defs=Light_Room; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            
            // default constructor
            Light_Sens() noexcept{
              state.sun_in = 0.0;
              state.light_in = 0.0;
              state.ang = 0.0;
              state.occ = 0;

              state.illum = 0.0;
              state.scr_a = 0.0;
                     
            }
            
            // state definition
            struct state_type{
              bool type;

              bool occ;
              float sun_in;
              float light_in;
              float ang;
              


              float illum;
           	  float scr_a;
            }; 
            state_type state;

            // ports definition
            
            using input_ports=std::tuple<typename defs::sun_intensity, typename defs::AOI, typename defs::light_intensity, typename defs::occup >;
            using output_ports=std::tuple<typename defs::Illuminance, typename defs::Screen_A>;

            // internal transition
            void internal_transition() {
               state.type = false;
              //Do nothing... 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
              
              float temp;
              float const_dist = 1;
              float const_dist1 = 0;
              float ill_me;
              float perc = 0;
              float dist = 2.75;

              for(const auto &x : get_messages<typename defs::sun_intensity>(mbs))
                    state.sun_in = x;
              for(const auto &x : get_messages<typename defs::AOI>(mbs))
                    state.ang = x;
              for(const auto &x : get_messages<typename defs::light_intensity>(mbs))
                    state.light_in = x;
              for(const auto &x : get_messages<typename defs::occup>(mbs))
                    state.occ = x;
              
            	temp = abs(state.sun_in * (cos(state.ang)));
            	temp = 0.67 * temp;
       
              ill_me = 0.2*(temp/5.0625 + temp/6.25 + temp/7.5625 + temp/9 + temp/10.5625);
              
              if(state.light_in < 3*ill_me)
                ill_me = 0.5 * (ill_me + state.light_in);
              else ill_me = state.light_in;

              if(ill_me > 300){
              	perc = 100*(ill_me - 300)/300; 
              	if(perc>100) perc = 100;
              }

              if(!state.occ)
                perc = 100;
              
              temp = temp*(1-perc/100);
             

              state.illum = (temp) * const_dist * const_dist / pow((dist - const_dist1),2);  

              state.scr_a = perc;
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
              get_messages<typename defs::Illuminance>(bags).push_back(state.illum);
			        get_messages<typename defs::Screen_A>(bags).push_back(state.scr_a);
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
               if(state.type)
                return TIME("00:00:01");
              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Light_Sens<TIME>::state_type& i) {
              os << "Current state: " << i.illum; 
              return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_LIGHT_INTENSITY_HPP