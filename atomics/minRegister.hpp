#ifndef __MINREGISTER_H
#define __MINREGISTER_H

#include<cadmium/modeling/ports.hpp>
#include<cadmium/modeling/message_bag.hpp>
#include <limits>
#include <ctime>
#include <assert.h>
#include <string>
#include <random>
#include "../data_structures/message.hpp"
#include <chrono>


using namespace cadmium;
using namespace std;

//Port definition
struct minRegister_defs {
    struct time_set : public in_port<Message_t> {};
    struct minutes : public in_port<Message_t> {};
    struct time_minutes : public out_port<Message_t> {};
    struct wrap_around : public out_port<Message_t> {};
};
template<typename TIME>
//Model description
class MinReg {
public:
    //Constructor
    MinReg() noexcept {
        state.wrap = false;
        state.active = false;
    }
     struct state_type {
        int minute;
        mutable bool wrap;
        bool active;

        
    }; 
    state_type state;
       
    //Inputs
    using input_ports = tuple<typename minRegister_defs::time_set, typename minRegister_defs::minutes>;
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        for(const auto &x : get_messages<typename minRegister_defs::time_set>(mbs)) {
        if (static_cast < int >(x.value1)) {
            // <time_set> was pressed
            state.active = true;
            state.wrap = false;
        } 
        // Only increment the minute if both <time_set> and <minutes>
	   // are pressed.  I.e <minutes> is disregarded if we are
	   // in the passive state.
        for(const auto &x : get_messages<typename minRegister_defs::minutes>(mbs)) {
            if(state.active == true) {
                state.minute = static_cast<int> (x.value1);
                if(state.minute == 59) {
                    state.minute = 0;
                } else {
                    ++state.minute;
                }
                // Activate the output function immediately
		        // so the new minute can be seen.
                state.active = true;
            }
        }
       
    }
    }

    //Internal
    void internal_transition() {
        if (state.active ==false) {
            // increment the minute register if the model is in the passive state.
            // Check for wrap around.
            state.minute++;
            state.minute = state.minute % 60;
            if (state.minute == 0) {
                state.wrap = true;
            }
        }
    }

     void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }

    //Output
    using output_ports = tuple<typename minRegister_defs::time_minutes, typename minRegister_defs::wrap_around>;
    typename make_message_bags<output_ports>::type output() const{
        typename make_message_bags<output_ports>::type bags;
        Message_t time_Register;
        time_Register.value1 = state.minute;
        get_messages<typename minRegister_defs::time_minutes>(bags).push_back(time_Register);
        if (state.wrap) {
             Message_t wrapAroundValue;
        wrapAroundValue.value1 =static_cast<int>(state.wrap);
            // indicate the minute value1 wrapped around
            get_messages<typename minRegister_defs::wrap_around>(bags).push_back(wrapAroundValue);
            state.wrap = false;
        }
        return bags;
    }

    //Time advance
    TIME time_advance() const {
        return TIME::infinity();
    }

    friend ostringstream& operator<<(ostringstream& os, const typename MinReg::state_type& i) {
        os << "minute: " << i.minute << " wrap: " << i.wrap << " active: " << i.active;
        return os;
    }
    
   
};
#endif
