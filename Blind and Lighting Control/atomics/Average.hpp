/*
* By: Harish & Surya Krishna
* ARSLab - Carleton University
*/
#ifndef BOOST_SIMULATION_PDEVS_AVERAGE_HPP
#define BOOST_SIMULATION_PDEVS_AVERAGE_HPP

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
    struct Avg_Room {
        
        //Output ports
        struct light_intensity : public out_port<float> { };
        struct occupy : public out_port<bool> { };
        
        //Input ports
        struct Room1 : public in_port<float> { };
        struct Room2 : public in_port<float> { };
        struct Room3 : public in_port<float> { };
        struct Room4 : public in_port<float> { };
        struct occ1 : public in_port<bool> { };
        struct occ2 : public in_port<bool> { };
        struct occ3 : public in_port<bool> { };
        struct occ4 : public in_port<bool> { };

  };

    template<typename TIME>
    class Avg_Sens {
        using defs=Avg_Room; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            // default constructor
            Avg_Sens() noexcept{
       
              state.r_1 = 0.0;
              state.r_2 = 0.0;
              state.r_3 = 0.0;
              state.r_4 = 0.0;

              state.occ_1 = 0;
              state.occ_2 = 0;
              state.occ_3 = 0;
              state.occ_4 = 0;

              state.light = 0.0;    
              state.occ = 0;      
                     
            }
            
            // state definition
            
            struct state_type{
              bool type;

              float r_1;
              float r_2;
              float r_3;
              float r_4;

              bool occ_1;
              bool occ_2;
              bool occ_3;
              bool occ_4;
              
              bool occ;
              float light;
            }; 
            state_type state;
            
            // ports definition
            
            using input_ports=std::tuple<typename defs::Room1, typename defs::Room2, typename defs::Room3, typename defs::Room4, typename defs::occ1, typename defs::occ2, typename defs::occ3, typename defs::occ4>;
            using output_ports=std::tuple<typename defs::light_intensity, typename defs::occupy>;

            // internal transition
            void internal_transition() {
               state.type = false;
              //Do nothing... 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 

             
              for(const auto &x : get_messages<typename defs::Room1>(mbs))
                    state.r_1 = x;
              for(const auto &x : get_messages<typename defs::Room2>(mbs))
                    state.r_2 = x;
              for(const auto &x : get_messages<typename defs::Room3>(mbs))
                    state.r_3 = x;
              for(const auto &x : get_messages<typename defs::Room4>(mbs))
                    state.r_4 = x;

              for(const auto &x : get_messages<typename defs::occ1>(mbs))
                    state.occ_1 = x;
              for(const auto &x : get_messages<typename defs::occ2>(mbs))
                    state.occ_2 = x;
              for(const auto &x : get_messages<typename defs::occ3>(mbs))
                    state.occ_3 = x;
              for(const auto &x : get_messages<typename defs::occ4>(mbs))
                    state.occ_4 = x;

             if(state.occ_1 || state.occ_2 || state.occ_3 || state.occ_4)   
             	state.occ = 1;    

            state.light = 0.25 * (state.r_1 + state.r_2 + state.r_3 + state.r_4);

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
              get_messages<typename defs::light_intensity>(bags).push_back(state.light);
			  get_messages<typename defs::occupy>(bags).push_back(state.occ);
			     
              return bags;
            }


            // time_advance function
            TIME time_advance() const { 
               if(state.type)
                return TIME("00:00:01");
              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Avg_Sens<TIME>::state_type& i) {
              os << "Current state: " << i.light; 
              return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_AVERAGE_HPP