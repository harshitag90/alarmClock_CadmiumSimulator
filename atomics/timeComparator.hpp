#ifndef TIME_COMP_HPP
#define TIME_COMP_HPP

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
struct TimeComp_defs {
    struct alarm_time : public in_port<Message_t> { };
    struct time_of_day : public in_port<int> { };
    struct time_match : public out_port<int> { };
};

//Model definition
template<typename TIME>
class TimeComp{
    public:
       
        // state definition
        struct state_type{
            int alarmTime;
            int timeOfDay;
            int outputvalue1;
            string phase;
        }; 
        state_type state;
        
        // ports definition
        using input_ports=std::tuple<typename TimeComp_defs::alarm_time, typename TimeComp_defs::time_of_day>;
        using output_ports=std::tuple<typename TimeComp_defs::time_match>;
        
        // internal transition function
        void internal_transition() {
            state.phase = "passive";
        }

        // external transition function
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            if(get_messages<typename TimeComp_defs::alarm_time>(mbs).size()>1 or get_messages<typename TimeComp_defs::time_of_day>(mbs).size()>1){
                state.phase = "error";
                state.outputvalue1 = numeric_limits<int>::infinity();
                state.alarmTime = numeric_limits<int>::infinity();
                state.timeOfDay = numeric_limits<int>::infinity();
            }else{
                if(get_messages<typename TimeComp_defs::alarm_time>(mbs).size()==1){
                    state.alarmTime = static_cast < int > ((get_messages<typename TimeComp_defs::alarm_time>(mbs)[0]).value1);
                }
                if(get_messages<typename TimeComp_defs::time_of_day>(mbs).size()==1){
                    state.timeOfDay = get_messages<typename TimeComp_defs::time_of_day>(mbs)[0];
                }
                if(state.timeOfDay == state.alarmTime){
                    state.outputvalue1 = state.alarmTime;
                    state.phase = "active";
                }else{
                    state.outputvalue1 = numeric_limits<int>::infinity();
                    state.phase = "passive";
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
        //     Message_t result_matched;
        // result_matched.value1 = state.outputvalue1; 
            get_messages<typename TimeComp_defs::time_match>(bag).push_back(state.outputvalue1);
            return bag;
        }
 
        // time_advance function
        TIME time_advance() const { 
            if (state.phase == "passive") {
                return numeric_limits<TIME>::infinity();
            }
            return TIME();
        }

        friend ostringstream& operator<<(ostringstream& os, const typename TimeComp<TIME>::state_type& i) {
            os << "Alarm Time: " << i.alarmTime ;
            os << "Time of Day: " << i.timeOfDay ;
            os << "Output value1: " << i.outputvalue1;
            os << "Phase: " << i.phase;
            return os;
        }
};

#endif // TIME_COMP_HPP
