#ifndef DISP_DRV_HPP
#define DISP_DRV_HPP

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

// Port definition
struct DispDrv_defs {
    struct alarm_time : public in_port<Message_t> { };
    struct time_of_day : public in_port<int> { };
    struct alarm_set : public in_port<Message_t> { };
    struct displayed_time : public out_port<Message_t> { };
};

// Model definition
template<typename TIME>
class DispDrv {
public:

 // ports definition
    using input_ports = tuple<typename DispDrv_defs::alarm_time, typename DispDrv_defs::time_of_day, typename DispDrv_defs::alarm_set>;
    using output_ports = tuple<typename DispDrv_defs::displayed_time>;


    // state definition
    struct state_type {
        bool active;
        bool passive;
        bool current;
        int alarmTime;
        int timeOfDay;
    };
    state_type state;

    DispDrv() {
        // state initialization
        state.active = false;
        state.passive = true;
        state.current = state.passive;
}
    
   
   

    // internal transition
    void internal_transition() {
        state.current = state.current;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        for(const auto &x : get_messages<typename DispDrv_defs::alarm_time>(mbs)) {
            state.alarmTime =static_cast < int >(x.value1) ;
            if (state.current == state.active)
                state.current = state.active;
        }

        for(const auto &x : get_messages<typename DispDrv_defs::time_of_day>(mbs)) {
            state.timeOfDay = x;
            if (state.current == state.passive)
                state.current = state.passive;
        }

        for(const auto &x : get_messages<typename DispDrv_defs::alarm_set>(mbs)) {
            if (static_cast < int >(x.value1))
                state.current = state.active;
            else
                state.current = state.passive;

        }
    }
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }

    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bag;
        int time_to_display = state.current == state.active ? state.alarmTime : state.timeOfDay;
        Message_t display_time;
        display_time.value1 = time_to_display;
        get_messages<typename DispDrv_defs::displayed_time>(bag).push_back(display_time);
        return bag;
    }

    // time_advance function
    TIME time_advance() const {
        return TIME::infinity();
    }

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename DispDrv<TIME>::state_type& i) {
        os << "current state: " << (i.current ? "active" : "passive") << " alarmTime: " << i.alarmTime << " timeOfDay: " << i.timeOfDay;
        return os;
    }

public:
    // port definition
    typename DispDrv_defs::alarm_time alarm_time;
    typename DispDrv_defs::time_of_day time_of_day;
    typename DispDrv_defs::alarm_set alarm_set;
    typename DispDrv_defs::displayed_time displayed_time;

};

#endif // TIME_COMP_HPP
