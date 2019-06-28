/**
* By: Ben Earle
* ARSLab - Carleton University
*
* Seeed Bot Driver:
* This model will do simple line following using a Seed Bot Shield.
* Its purpose is to demonstrate how to use all of the port IO models in RT_ARM_MBED.
*
* Note: The 'SCARED_OF_THE_DARK' macro will read from a Grove light sensor on 
*   analog port A5 and stop the car if the reading is less then 0.3.
* It must be defined here and in main if being used.
*/
#ifndef BOOST_SIMULATION_PDEVS_FIRE_SENSOR_HPP
#define BOOST_SIMULATION_PDEVS_FIRE_SENSOR_HPP

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
    struct Fire_Sen_defs {
        //Output ports
        struct out : public out_port<bool> { };
        
        //Input ports
        struct inp : public in_port<float> { };
  };

    template<typename TIME>
    class Fire_Sensor_Con {
        using defs=Fire_Sen_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            
            // default constructor
            Fire_Sensor_Con() noexcept{
              state.type = false;
              state.fire = false;
            }
            
            // state definition
            struct state_type{
              bool type;
              bool fire;
            }; 
            state_type state;
            // ports definition
            
            using input_ports=std::tuple<typename defs::inp>;
            using output_ports=std::tuple<typename defs::out>;

            // internal transition
            void internal_transition() {
               //state.fire = state.fire > 0.94 ? 1 : 0;

              state.type = false;
              //Do nothing... 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
               
               for(const auto &x : get_messages<typename defs::inp>(mbs))
                  state.fire = (x > 0.94);

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
              get_messages<typename defs::out>(bags).push_back(state.fire);
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
              if(state.type)
                return TIME("00:00:00");
              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Fire_Sensor_Con<TIME>::state_type& i) {
              os << "Current state: " << i.fire; 
              return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_FIRE_SENSOR_HPP