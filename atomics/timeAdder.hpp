#ifndef TIME_ADDER_HPP
#define TIME_ADDER_HPP

#include<cadmium/modeling/ports.hpp>
#include<cadmium/modeling/message_bag.hpp>
#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct TimeAdder_defs {
    struct time_hours : public in_port<Message_t> { };
    struct time_minutes : public in_port<Message_t> { };
    struct time_of_day : public out_port<int> { };
};

/*******************************************************************
* Model definition
********************************************************************/
template<typename TIME>
class TimeAdder {
public:
    // ports definition 
    using input_ports = tuple<typename TimeAdder_defs::time_hours, typename TimeAdder_defs::time_minutes>;
    using output_ports = tuple<typename TimeAdder_defs::time_of_day>;

    // state definition
    struct state_type{
        int hours;
        int minutes;
        int outTime;
        string phase;
    };
    state_type state;

    // default constructor
    TimeAdder() {
        state = {0,0,0};
    }

    // internal transition
    void internal_transition() {
        state.outTime = state.hours * 100 + state.minutes;
        state.phase = "passive";
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        if (get_messages<typename TimeAdder_defs::time_hours>(mbs).size() > 0) {
            state.hours = static_cast < int >(get_messages<typename TimeAdder_defs::time_hours>(mbs)[0].value1);
        }
        if (get_messages<typename TimeAdder_defs::time_minutes>(mbs).size() > 0) {
            state.minutes = static_cast < int >(get_messages<typename TimeAdder_defs::time_minutes>(mbs)[0].value1);
        }
    state.outTime = state.hours * 100 + state.minutes;
        state.hours = 0;
        state.minutes = 0;
        state.phase = "active";
    }

     void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }
    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bag;
        // Message_t time_output;
        // time_output.value1 = state.outTime; 
        get_messages<typename TimeAdder_defs::time_of_day>(bag).push_back(state.outTime);
        return bag;
    }

  // time_advance function
        TIME time_advance() const { 
            if (state.hours == 0 && state.minutes == 0 && state.phase=="passive" ) {
                return numeric_limits<TIME>::infinity();
            }
            return TIME();
        }

    friend ostringstream& operator<<(ostringstream& os, const typename TimeAdder<TIME>::state_type& i) {
        os << "outTime: " << i.outTime;
        os << "Phase: " << i.phase;
        return os;
    }
};


#endif // TIME_ADDER_HPP
