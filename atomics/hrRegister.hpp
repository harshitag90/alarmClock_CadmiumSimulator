#ifndef HRREG_HPP
#define HRREG_HPP
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
struct HrReg_defs {
    struct time_set : public in_port<Message_t> {};
    struct hours : public in_port<Message_t> {};
    struct wrap_around : public in_port<Message_t> {};
    struct time_hours : public out_port<Message_t> {};
};

template<typename TIME>
class HrReg{
public:
    // default constructor
    HrReg() noexcept {
        state.passive = true;
    }

     // ports definition
        using input_ports=std::tuple<typename HrReg_defs::time_set, typename HrReg_defs::hours,typename HrReg_defs::wrap_around>;
        using output_ports=std::tuple<typename HrReg_defs::time_hours>;
        

    // internal transition function
    void internal_transition() noexcept {
        state.passive = true;
    }

    // external transition function
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) noexcept {
        for(const auto &x : get_messages<typename HrReg_defs::time_set>(mbs)) {
            if(static_cast < int > (x.value1)) {
                state.active = true;
            } else {
                state.passive = true;
            }
        }
        for(const auto &x : get_messages<typename HrReg_defs::hours>(mbs)) {
            // Only increment the hour if the model is in the 
	           // active phase.
            if(state.active == true) {
                state.hour = static_cast<int>(x.value1);
                if(state.hour == 23) {
                    state.hour = 0;
                } else {
                    ++state.hour;
                }
                // Activate the output function immediately
		        // so the new hour can be seen.
                state.active = true;
            }
        }
        for(const auto &x : get_messages<typename HrReg_defs::wrap_around>(mbs)) {
            state.hour = static_cast<int> (x.value1);
            if(state.passive == true) {
                if(state.hour == 23) {
                    state.hour = 0;
                } else {
                    ++state.hour;
                }
                state.passive = true;
            }
        }
    }
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }

    // output function
    typename make_message_bags<output_ports>::type output() const noexcept {
        typename make_message_bags<output_ports>::type bags;
         Message_t result_matched;
        result_matched.value1 = state.hour;
        get_messages<typename HrReg_defs::time_hours>(bags).push_back(result_matched);
        return bags;
    }

    // time_advance function
    TIME time_advance() const noexcept {
        return std::numeric_limits<TIME>::infinity();
    }

public:
    // state variables
  
    struct state_type { 
        bool passive;
        bool active;
        int hour;
        } ;

       state_type state;

    friend ostringstream& operator<<(ostringstream& os, const typename HrReg<TIME>::state_type& i) {
            os << "Hours Registered: " << i.hour;
            return os;
        }
};

#endif // HRREG_HPP
