/**
* Harshita Ghushe
* 101289614
* Carleton University
*/

//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Messages structures
#include "../data_structures/message.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/timeComparator.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

#define INPUT_FILEPATH1 "../input_data/timeComparator_input_test_alarm_time.txt"
#define INPUT_FILEPATH2 "../input_data/timeComparator_input_test_time_of_day.txt"
#define OUTPUT_FILEPATH "../simulation_results/timeComparator_test_output_messages.txt"

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct alarm_time : public in_port<Message_t> { };
struct time_of_day : public in_port<int> { };
  

/***** Define output ports for coupled model *****/

  struct time_match : public out_port<int> { };

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Message_t : public iestream_input<Message_t,T> {
    public:
        InputReader_Message_t () = default;
        InputReader_Message_t (const char* file_path) : iestream_input<Message_t,T>(file_path) {}
};
template<typename T>
class InputReader_int : public iestream_input<int,T> {
    public:
        InputReader_int () = default;
        InputReader_int (const char* file_path) : iestream_input<int,T>(file_path) {}
};

int main(){

    /****** Input Reader for Alarm Time atomic model instantiation *******************/
    const char * i_input_data_alarm_time = INPUT_FILEPATH1;
    shared_ptr<dynamic::modeling::model> input_reader_alarm_time;
    input_reader_alarm_time = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t, TIME, const char*>("input_reader_alarm_time", move(i_input_data_alarm_time));

    /****** Input Reader for Time of Day atomic model instantiation *******************/
    const char * i_input_data_time_of_day = INPUT_FILEPATH2;
    shared_ptr<dynamic::modeling::model> input_reader_time_of_day;
    input_reader_time_of_day = dynamic::translate::make_dynamic_atomic_model<InputReader_int, TIME, const char*>("input_reader_time_of_day", move(i_input_data_time_of_day));

   
    /****** Time Comparator atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> timeComparator1;
    timeComparator1 = dynamic::translate::make_dynamic_atomic_model<TimeComp, TIME>("timeComparator1");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(time_match)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader_alarm_time, input_reader_time_of_day, timeComparator1};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<TimeComp_defs::time_match, time_match>("timeComparator1")
    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,TimeComp_defs::alarm_time>("input_reader_alarm_time","timeComparator1"),
        dynamic::translate::make_IC<iestream_input_defs<int>::out,TimeComp_defs::time_of_day>("input_reader_time_of_day","timeComparator1"),

    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages(OUTPUT_FILEPATH);
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/timeComparator_test_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/ 
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("08:00:00:000"));

    cout<<"Time Comparator Model Run Successful."<<"\n";
    cout<<"To see the Output, go to simulation_results folder."<<"\n";
	return 0;
}