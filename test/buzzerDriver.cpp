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
#include "../atomics/buzzerDriver.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

#define INPUT_FILEPATH1 "../input_data/buzzerDriver_input_test_time_set.txt"
#define INPUT_FILEPATH2 "../input_data/buzzerDriver_input_test_alarm_set.txt"
#define INPUT_FILEPATH3 "../input_data/buzzerDriver_input_test_time_match.txt"
#define INPUT_FILEPATH4 "../input_data/buzzerDriver_input_test_alarm_on.txt"
#define INPUT_FILEPATH5 "../input_data/buzzerDriver_input_test_snooze.txt"
#define OUTPUT_FILEPATH "../simulation_results/buzzerDriver_output_messages.txt"

using TIME = NDTime;

/***** Define input port for coupled models *****/
  struct time_set : public in_port<Message_t> { };
   struct alarm_set : public in_port<Message_t> { };
   struct time_match : public in_port<int> { };
   struct alarm_on : public in_port<Message_t> { };
   struct snooze : public in_port<Message_t> { };
     
/***** Define output ports for coupled model *****/
  struct buzzer_on : public out_port<bool> { };

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

    /****** Input Reader for Time Set atomic model instantiation *******************/
    const char * i_input_data_time_set = INPUT_FILEPATH1;
    shared_ptr<dynamic::modeling::model> input_reader_time_set;
    input_reader_time_set = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t, TIME, const char*>("input_reader_time_set", move(i_input_data_time_set));

    /****** Input Reader for Alarm Set atomic model instantiation *******************/
    const char * i_input_data_alarm_set = INPUT_FILEPATH2;
    shared_ptr<dynamic::modeling::model> input_reader_alarm_set;
    input_reader_alarm_set = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t, TIME, const char*>("input_reader_alarm_set", move(i_input_data_alarm_set));

    /****** Input Reader for Time Match atomic model instantiation *******************/
    const char * i_input_data_time_match = INPUT_FILEPATH3;
    shared_ptr<dynamic::modeling::model> input_reader_time_match;
    input_reader_time_match = dynamic::translate::make_dynamic_atomic_model<InputReader_int, TIME, const char*>("input_reader_time_match", move(i_input_data_time_match));

 /****** Input Reader for Alarm on atomic model instantiation *******************/
    const char * i_input_data_alarm_on = INPUT_FILEPATH4;
    shared_ptr<dynamic::modeling::model> input_reader_alarm_on;
    input_reader_alarm_on = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t, TIME, const char*>("input_reader_alarm_on", move(i_input_data_alarm_on));

 /****** Input Reader for Snooze atomic model instantiation *******************/
    const char * i_input_data_snooze = INPUT_FILEPATH5;
    shared_ptr<dynamic::modeling::model> input_reader_snooze;
    input_reader_snooze = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t, TIME, const char*>("input_reader_snooze", move(i_input_data_snooze));

    /****** Buzzer Driver atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> buzzerDriver1;
    buzzerDriver1 = dynamic::translate::make_dynamic_atomic_model<BuzDrv, TIME>("buzzerDriver1");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(buzzer_on)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader_time_set, input_reader_alarm_set, input_reader_time_match,input_reader_alarm_on,input_reader_snooze, buzzerDriver1};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<BuzDrv_defs::buzzer_on, buzzer_on>("buzzerDriver1")
    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,BuzDrv_defs::time_set>("input_reader_time_set","buzzerDriver1"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,BuzDrv_defs::alarm_set>("input_reader_alarm_set","buzzerDriver1"),
        dynamic::translate::make_IC<iestream_input_defs<int>::out,BuzDrv_defs::time_match>("input_reader_time_match","buzzerDriver1"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,BuzDrv_defs::alarm_on>("input_reader_alarm_on","buzzerDriver1"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,BuzDrv_defs::snooze>("input_reader_snooze","buzzerDriver1"),

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
    static ofstream out_state("../simulation_results/buzzerDriver_output_state.txt");
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

    cout<<"Buzzer Driver Model Run Successful."<<"\n";
    cout<<"To see the Output, go to results folder."<<"\n";
	return 0;
}