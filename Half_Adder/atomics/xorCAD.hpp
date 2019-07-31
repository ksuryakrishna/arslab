/**
* By: Harish & Surya Krishna
* ARSLab - Carleton University
*/

#ifndef HALF_ADDER_OR_CAD_HPP
#define HALF_ADDER_OR_CAD_HPP


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

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
    struct xorCAD_defs{
        struct outx : public out_port<Message_t> {
        };
        struct in1: public in_port<Message_t>{
        };
        struct in2 : public in_port<Message_t> {
        };
    };
   
    template<typename TIME>
    class xorCAD{
        using defs=xorCAD_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            TIME   passingTime;
            // default constructor
            xorCAD() noexcept{
              passingTime  = TIME("00:00:01");
              state.passing     = false;
              state.input1 =  false;
              state.input2 = false;
              state.outputx = ((state.input1 && (!state.input2))||(state.input2 && (!state.input1)));
            }
            
            // state definition
            struct state_type{
              bool passing;
              bool input1;
              bool input2;
              bool outputx;
            }; 
            state_type state;

            // ports definition
            using input_ports=std::tuple<typename defs::in1, typename defs::in2>;
            using output_ports=std::tuple<typename defs::outx>;

            // internal transition
            void internal_transition() {
              state.outputx = ((state.input1 && (!state.input2))||(state.input2 && (!state.input1)));
              state.passing = false; 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
    
              for(const auto &x : get_messages<typename defs::in1>(mbs)){
                state.input1 = (x.value == 1) ? 1 : 0;
                state.passing = true;
              } 

              for(const auto &x : get_messages<typename defs::in2>(mbs)){
                state.input2 = (x.value == 1) ? 1 : 0;
                state.passing = true;
              }   
                           
            }

            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                internal_transition();
                external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
              typename make_message_bags<output_ports>::type bags;
              Message_t out;              
              out.value = state.outputx ? 1 : 0 ;
              get_messages<typename defs::outx>(bags).push_back(out); 
              return bags;

            }

            // time_advance function
            TIME time_advance() const {  
              TIME next_internal;
              if (state.passing) {
                next_internal = passingTime;
              }else {
                next_internal = std::numeric_limits<TIME>::infinity();
              }    
              return next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename xorCAD<TIME>::state_type& i) {
                os << "Output: " << (i.outputx ? 1 : 0); 
            return os;
            }
        };     
  

#endif // HALF_ADDER_OR_CAD_HPP