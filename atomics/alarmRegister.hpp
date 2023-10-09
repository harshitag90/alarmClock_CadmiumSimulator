#ifndef ALARM_REGISTER_HPP
#define ALARM_REGISTER_HPP

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

//Input and output ports
struct AlarmReg_defs {
   struct alarm_set : public in_port<Message_t> { };
   struct hours : public in_port<Message_t> { };
   struct minutes : public in_port<Message_t> { };
   struct alarm_time : public out_port<Message_t> { };
};

//The AlarmReg model
template<typename TIME>
class AlarmReg  {
public:
    // state definition
    struct state_type {
        bool active;
        bool passive;
        int alarmTime;
        int minute;
        int hour;
    };
    state_type state;

 
   //Ports definition
   using input_ports = tuple<typename AlarmReg_defs::alarm_set, typename AlarmReg_defs::hours, typename AlarmReg_defs::minutes>;
   using output_ports = tuple<typename AlarmReg_defs::alarm_time>;

   //Internal transition function
   void internal_transition() {
       state.passive = true;
   }

   //External transition function
   void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
       for (const auto &x : get_messages<typename AlarmReg_defs::alarm_set>(mbs)) {
           if (static_cast < int >(x.value1)) {
               state.active = true;
           } else {
               state.passive = true;
           }
       }

       for (const auto &x : get_messages<typename AlarmReg_defs::hours>(mbs)) {
           if (state.active == true) {
            state.hour = static_cast<int> (x.value1);
               if (state.hour == 23) {
                   state.hour = 0;
               } else {
                   state.hour = state.hour + 1;
               }
               state.alarmTime = state.hour * 100 + state.minute;
           }
       }

       for (const auto &x : get_messages<typename AlarmReg_defs::minutes>(mbs)) {
           if (state.active == true) {
            state.minute = static_cast<int> (x.value1);
               if (state.minute == 59) {
                   state.minute =0;
                   if (state.hour == 23) {
                       state.hour =0;
                   } else {
                       state.hour = state.hour + 1;
                   }
               } else {
                   state.minute = state.minute + 1;
               }
               state.alarmTime = state.hour * 100 + state.minute;
           }
       }
   }
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }
   //Output function
  typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bag;
    Message_t alarm_time_result;
    alarm_time_result.value1 = state.alarmTime;
       get_messages<typename AlarmReg_defs::alarm_time>(bag).push_back(alarm_time_result);
   return bag;
   }

    // time_advance function
    TIME time_advance() const {
        return TIME::infinity();
    }

   friend std::ostringstream& operator<<(std::ostringstream& os, const typename AlarmReg<TIME>::state_type& i) {
        os << "Alarm Time: " << i.alarmTime;
        return os;
    }
};
#endif // ALARM_REGISTER_HPP

 

 