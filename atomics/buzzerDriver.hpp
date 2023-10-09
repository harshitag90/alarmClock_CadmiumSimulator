#ifndef BUZZER_DRIVER_HPP
#define BUZZER_DRIVER_HPP
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <string>
#include <assert.h>
#include <random>
#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct BuzDrv_defs {
   struct time_set : public in_port<Message_t> { };
   struct alarm_set : public in_port<Message_t> { };
   struct time_match : public in_port<int> { };
   struct alarm_on : public in_port<Message_t> { };
   struct snooze : public in_port<Message_t> { };
   struct buzzer_on : public out_port<bool> { };
};

template<typename TIME>
class BuzDrv {
public:
 
 struct state_type {
    //Time remaining for alarm
    TIME timeRemaining;
    //Time alarm is set to
    int alarmTime;
    //Armed or not
    bool armed;
    //Is the alarm set or not
    int alarmSet;
    //Is the time set or not
    int timeSet;
    //Is the snooze button pressed or not
    bool snooz;
    //Is the buzzer on or not
    bool buzzer;
    };
    state_type state;
  //Atomic model states
    enum BuzzDrvStates {passive, active};
    BuzzDrvStates current_state;

    //One hour time 
     TIME one_hour;
    //10 minutes time 
     TIME ten_minutes;

    //Input ports
    using input_ports = std::tuple<typename BuzDrv_defs::time_set, typename BuzDrv_defs::alarm_set, typename BuzDrv_defs::time_match, typename BuzDrv_defs::alarm_on, typename BuzDrv_defs::snooze>;
    //Output ports
    using output_ports = std::tuple<typename BuzDrv_defs::buzzer_on>;

    //BuzDrv constructor
    BuzDrv() {
        state.alarmTime = 0;
        state.alarmSet = 0;
        state.timeSet = 0;
    current_state=passive;
    one_hour = TIME("01:00:00:00");
    ten_minutes = TIME("00:10:00:00");
    }

   //Internal transition function
   void internal_transition() {
        if (!state.snooz && !state.buzzer) {
            // not snoozing and the buzzer is off. Either someone
            // turned off the alarm using the <alarm_on> port
            // or the buzzer reached its time limit of one
            // hour after the alarm time.
            current_state = passive;
            state.timeRemaining = TIME("00:00:00");
        } else if (state.buzzer) {
            // buzzer is on. Schedule an internal event to turn it off
            // when the time limit is reached.
            current_state = active;
            state.buzzer = false;
            state.timeRemaining = one_hour;
        } else {
            // buzzer is off and we are snoozing. Schedule an internal
            // event to turn the buzzer back on in 10 minutes.
            state.buzzer = true;
            current_state = passive;
            state.timeRemaining = ten_minutes;
        }
    }
   

   //External transition function
  //External transition function
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
            for(const auto &x : get_messages<typename BuzDrv_defs::time_set>(mbs)){
                state.timeSet = static_cast < int >(x.value1);
            }
            for(const auto &x : get_messages<typename BuzDrv_defs::alarm_set>(mbs)){
                state.alarmSet = static_cast < int >(x.value1);
            }
            for(const auto &x : get_messages<typename BuzDrv_defs::time_match>(mbs)){
                if(state.armed && (state.alarmSet==0) && (state.timeSet==0)){
                    state.alarmTime = x;
                    state.timeRemaining = one_hour;
                    state.buzzer = true;
                    state.snooz = false;
                    current_state = active;
                    state.timeRemaining = TIME("00:00:00");
                }
            }
            for(const auto &x : get_messages<typename BuzDrv_defs::snooze>(mbs)){
                if(current_state == active){
                    state.buzzer = false;
                    if(state.timeRemaining <= ten_minutes)
                    {
                        state.snooz = false;
                    }
                    else
                    {
                        state.snooz = true;
                        state.timeRemaining = ten_minutes;
                    }
                    current_state = passive;
                }
            }
            for(const auto &x : get_messages<typename BuzDrv_defs::alarm_on>(mbs)){
            if(static_cast < int >(x.value1)) {
                state.armed = true;
                state.buzzer = false;
                current_state = passive;
            } 
            else {
                state.armed = false;
                state.buzzer = false;
                state.snooz = false;
                if(current_state==active)
                current_state=passive;
                 else
                 current_state=passive;
            }
           
        }
        }

    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }

        // output function
        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bag;
            get_messages<typename BuzDrv_defs::buzzer_on>(bag).push_back(state.buzzer);
            return bag;
        }
 
        // time_advance function
        TIME time_advance() const { 
            if ( current_state==passive) {
                return numeric_limits<TIME>::infinity();
            }
            return TIME();
        }

        friend ostringstream& operator<<(ostringstream& os, const typename BuzDrv<TIME>::state_type& state) {
            os << "Alarm Time: " << state.alarmTime ;
            return os;
        }

};

#endif // BUZZER_DRIVER_HPP
