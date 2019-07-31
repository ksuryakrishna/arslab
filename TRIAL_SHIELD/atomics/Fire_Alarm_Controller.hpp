/**
* By: Harish & Surya Krishna
* ARSLab - Carleton University
*/
#ifndef BOOST_SIMULATION_PDEVS_FIRE_ALARM_CONTROLLER_HPP
#define BOOST_SIMULATION_PDEVS_FIRE_ALARM_CONTROLLER_HPP

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
    struct Fire_Alarm_defs {
        //Output ports
        struct out : public out_port<bool> { };
        
        //Input ports
        struct inp : public in_port<bool> { };
  };

    template<typename TIME>
    class Fire_Alarm_Con {
        using defs=Fire_Alarm_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            
            // default constructor
            Fire_Alarm_Con() noexcept{
              state.type = false;
              state.alarm = false;
            }
            
            // state definition
            struct state_type{
              bool type;
              bool alarm;
            }; 
            state_type state;
            // ports definition
            
            using input_ports=std::tuple<typename defs::inp>;
            using output_ports=std::tuple<typename defs::out>;

            // internal transition
            void internal_transition() {
              if((state.alarm == 1) ? 1 : 0)
                  state.alarm = 1;
                else
                  state.alarm = 0;
              state.type = false;
              //Do nothing... 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
               
               for(const auto &x : get_messages<typename defs::inp>(mbs)){
                if((x == 1) ? 1 : 0)
                  state.alarm = 1;
                else
                  state.alarm = 0;

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
              get_messages<typename defs::out>(bags).push_back(state.alarm);
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
              if(state.type)
                return TIME("00:00:00");
              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Fire_Alarm_Con<TIME>::state_type& i) {
              os << "Current state: " << i.type; 
              return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_FIRE_ALARM_CONTROLLER_HPP