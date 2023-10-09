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
#include "../atomics/minRegister.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct time_set : public in_port<Message_t> {};
struct minutes : public in_port<Message_t> {};
   
/***** Define output ports for coupled model *****/
 struct time_minutes : public out_port<Message_t> {};
 struct wrap_around : public out_port<Message_t> {};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Message_t : public iestream_input<Message_t,T> {
    public:
        InputReader_Message_t () = default;
        InputReader_Message_t (const char* file_path) : iestream_input<Message_t,T>(file_path) {}
};

int main(){


    /****** Input Reader atomic models instantiation *******************/
    const char * i_input_data_time_set = "../input_data/minRegister_input_test_time_set.txt";
    shared_ptr<dynamic::modeling::model> input_reader_time_set = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t, TIME, const char* >("input_reader_time_set" , move(i_input_data_time_set));

    const char * i_input_data_minutes= "../input_data/minRegister_input_test_minutes.txt";
    shared_ptr<dynamic::modeling::model> input_reader_minutes = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t , TIME, const char* >("input_reader_minutes" , move(i_input_data_minutes));


    /****** minRegister atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> minRegister1 = dynamic::translate::make_dynamic_atomic_model<MinReg, TIME>("minRegister1");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(time_minutes),typeid(wrap_around)};
    dynamic::modeling::Models submodels_TOP = {input_reader_time_set, input_reader_minutes, minRegister1};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<minRegister_defs::time_minutes,time_minutes>("minRegister1"),
        dynamic::translate::make_EOC<minRegister_defs::wrap_around,wrap_around>("minRegister1"),
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,minRegister_defs::time_set>("input_reader_time_set","minRegister1"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,minRegister_defs::minutes>("input_reader_minutes","minRegister1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/minRegister_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/minRegister_test_output_state.txt");
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

     cout<<"Minutes Register Model Run Successful."<<"\n";
     cout<<"To see the Output, go to simulation_results folder."<<"\n";
    return 0;
}
