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
#include "../atomics/alarmRegister.hpp"
#include "../atomics/buzzerDriver.hpp"
#include "../atomics/displayDriver.hpp"
#include "../atomics/hrRegister.hpp"
#include "../atomics/minRegister.hpp"
#include "../atomics/timeAdder.hpp"
#include "../atomics/timeComparator.hpp"
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs

//C++ headers 
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>


using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct time_hours : public in_port<Message_t> { };
struct time_minutes : public in_port<Message_t> { };
struct alarm_time : public in_port<Message_t> { };
struct e_alarm_time : public in_port<Message_t> { };
struct time_of_day : public in_port<int> { };
struct time_set : public in_port<Message_t> {};
struct e_time_set : public in_port<Message_t> {};
 struct hours : public in_port<Message_t> {};
 struct e_hours : public in_port<Message_t> {};
 struct minutes : public in_port<Message_t> { };
 struct e_minutes : public in_port<Message_t> { };
 struct wrap_around : public in_port<Message_t> {};
 struct alarm_set : public in_port<Message_t> {};
 struct e_alarm_set : public in_port<Message_t> {};
 struct time_match : public in_port<int> { };
 struct alarm_on : public in_port<Message_t> { };
 struct e_alarm_on : public in_port<Message_t> { };
 struct snooze : public in_port<Message_t> { };
 struct e_snooze : public in_port<Message_t> { };

/***** Define output ports for coupled model *****/
struct Time_of_day : public out_port<int> {};
struct Time_match : public out_port<int> {};
struct Alarm_time : public out_port<Message_t> {};
struct Time_minutes : public out_port<Message_t> {};
struct Wrap_around : public out_port<Message_t> {};
struct Time_hours : public out_port<Message_t> {};
struct displayed_time : public out_port<Message_t> {};
struct buzzer_on : public out_port<bool> {};
struct e_buzzer_on : public out_port<bool> {};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<Message_t,T> {
public:
    InputReader_Int() = default;
    InputReader_Int(const char* file_path) : iestream_input<Message_t,T>(file_path) {}
};


int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1; 
    }
    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader" , move(i_input));

     /****** Time Comparator atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> timeComparator1;
    timeComparator1 = dynamic::translate::make_dynamic_atomic_model<TimeComp, TIME>("timeComparator1");

    /****** Time Adder atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> timeAdder1;
    timeAdder1 = dynamic::translate::make_dynamic_atomic_model<TimeAdder, TIME>("timeAdder1");

   /****** Buzzer Driver atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> buzzerDriver1;
    buzzerDriver1 = dynamic::translate::make_dynamic_atomic_model<BuzDrv, TIME>("buzzerDriver1");

   /****** minRegister atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> minRegister1 = dynamic::translate::make_dynamic_atomic_model<MinReg, TIME>("minRegister1");

   /****** HrReg atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> hourRegister1;
    hourRegister1 = dynamic::translate::make_dynamic_atomic_model<HrReg, TIME>("hourRegister1");
 
   /****** DispDrv atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> displayDriver1;
    displayDriver1 = dynamic::translate::make_dynamic_atomic_model<DispDrv, TIME>("displayDriver1");

   /****** AlarmReg atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> alarmRegister1;
    alarmRegister1 = dynamic::translate::make_dynamic_atomic_model<AlarmReg, TIME>("alarmRegister1");

    /*******TIME REGISTER COUPLED MODEL********/
    dynamic::modeling::Ports iports_timeRegister = {typeid(minutes),typeid(time_set),typeid(hours)};
    dynamic::modeling::Ports oports_timeRegister = {typeid(Time_of_day)};
    dynamic::modeling::Models submodels_timeRegister = {minRegister1, hourRegister1,timeAdder1};
    dynamic::modeling::EICs eics_timeRegister = {
        dynamic::translate::make_EIC<minutes, minRegister_defs::minutes>("minRegister1"),
        dynamic::translate::make_EIC<time_set, minRegister_defs::time_set>("minRegister1"),
        dynamic::translate::make_EIC<hours, HrReg_defs::hours>("hourRegister1"),
        dynamic::translate::make_EIC<time_set, HrReg_defs::time_set>("hourRegister1"),
    };
    dynamic::modeling::EOCs eocs_timeRegister = {
         dynamic::translate::make_EOC<TimeAdder_defs::time_of_day,Time_of_day>("timeAdder1"),
    };
    dynamic::modeling::ICs ics_timeRegister = {
         dynamic::translate::make_IC<minRegister_defs::wrap_around, HrReg_defs::wrap_around>("minRegister1", "hourRegister1"),
            dynamic::translate::make_IC<minRegister_defs::time_minutes, TimeAdder_defs::time_minutes>("minRegister1","timeAdder1"),
            dynamic::translate::make_IC<HrReg_defs::time_hours, TimeAdder_defs::time_hours>("hourRegister1","timeAdder1"),
                   
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TIMEREGISTER;
    TIMEREGISTER = make_shared<dynamic::modeling::coupled<TIME>>(
        "TIMEREGISTER", submodels_timeRegister, iports_timeRegister, oports_timeRegister, eics_timeRegister, eocs_timeRegister, ics_timeRegister 
    );

    /*******ALARM CONTROLLER COUPLED MODEL********/
    dynamic::modeling::Ports iports_alarmController = {typeid(alarm_set),typeid(time_of_day),typeid(alarm_on),typeid(snooze),typeid(hours),typeid(minutes),typeid(time_set) };
    dynamic::modeling::Ports oports_alarmController = {typeid(Alarm_time),typeid(buzzer_on)};
    dynamic::modeling::Models submodels_alarmController = {alarmRegister1, timeComparator1,buzzerDriver1};
    dynamic::modeling::EICs eics_alarmController = {
        dynamic::translate::make_EIC<alarm_set, AlarmReg_defs::alarm_set>("alarmRegister1"),
        dynamic::translate::make_EIC<hours, AlarmReg_defs::hours>("alarmRegister1"),
        dynamic::translate::make_EIC<minutes, AlarmReg_defs::minutes>("alarmRegister1"),
        dynamic::translate::make_EIC<time_of_day, TimeComp_defs::time_of_day>("timeComparator1"),
         dynamic::translate::make_EIC<alarm_on, BuzDrv_defs::alarm_on>("buzzerDriver1"),
        dynamic::translate::make_EIC<snooze, BuzDrv_defs::snooze>("buzzerDriver1"),
        dynamic::translate::make_EIC<time_set, BuzDrv_defs::time_set>("buzzerDriver1"),
    };
    dynamic::modeling::EOCs eocs_alarmController = {
         dynamic::translate::make_EOC<AlarmReg_defs::alarm_time,Alarm_time>("alarmRegister1"),
         dynamic::translate::make_EOC<BuzDrv_defs::buzzer_on,buzzer_on>("buzzerDriver1"),
    };
    dynamic::modeling::ICs ics_alarmController = {
         dynamic::translate::make_IC<TimeComp_defs::time_match, BuzDrv_defs::time_match>("timeComparator1", "buzzerDriver1"),
         dynamic::translate::make_IC<AlarmReg_defs::alarm_time, TimeComp_defs::alarm_time>("alarmRegister1", "timeComparator1"),
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> ALARMCONTROLLER;
    ALARMCONTROLLER = make_shared<dynamic::modeling::coupled<TIME>>(
        "ALARMCONTROLLER", submodels_alarmController, iports_alarmController, oports_alarmController, eics_alarmController, eocs_alarmController, ics_alarmController 
    );

   /*******ALARM CLOCK COUPLED MODEL********/
    dynamic::modeling::Ports iports_ALARMCLOCK = {typeid(e_minutes),typeid(e_hours),typeid(e_alarm_set),typeid(e_time_set),typeid(e_alarm_on),typeid(e_snooze)};
    dynamic::modeling::Ports oports_ALARMCLOCK = {typeid(displayed_time),typeid(e_buzzer_on)};
    dynamic::modeling::Models submodels_ALARMCLOCK = {TIMEREGISTER, ALARMCONTROLLER, displayDriver1};
    dynamic::modeling::EICs eics_ALARMCLOCK = {
       dynamic::translate::make_EIC<e_alarm_set, alarm_set>("ALARMCONTROLLER"),
       dynamic::translate::make_EIC<e_hours, hours>("ALARMCONTROLLER"),
       dynamic::translate::make_EIC<e_hours, hours>("TIMEREGISTER"),
       dynamic::translate::make_EIC<e_minutes, minutes>("ALARMCONTROLLER"),
       dynamic::translate::make_EIC<e_minutes, minutes>("TIMEREGISTER"),
        dynamic::translate::make_EIC<e_time_set, time_set>("ALARMCONTROLLER"),
        dynamic::translate::make_EIC<e_time_set, time_set>("TIMEREGISTER"),
        dynamic::translate::make_EIC<e_alarm_on, alarm_on>("ALARMCONTROLLER"),
        dynamic::translate::make_EIC<e_snooze, snooze>("ALARMCONTROLLER"),
    };
    dynamic::modeling::EOCs eocs_ALARMCLOCK = {
        dynamic::translate::make_EOC<DispDrv_defs::displayed_time,displayed_time>("displayDriver1"),
        dynamic::translate::make_EOC<buzzer_on,e_buzzer_on>("ALARMCONTROLLER"),
    };
    dynamic::modeling::ICs ics_ALARMCLOCK = {
        dynamic::translate::make_IC<Time_of_day, DispDrv_defs::time_of_day>("TIMEREGISTER","displayDriver1"),
        dynamic::translate::make_IC<Time_of_day, time_of_day>("TIMEREGISTER","ALARMCONTROLLER"),
        dynamic::translate::make_IC<Alarm_time, DispDrv_defs::alarm_time>("ALARMCONTROLLER","displayDriver1"),
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> ALARMCLOCK;
    ALARMCLOCK = make_shared<dynamic::modeling::coupled<TIME>>(
        "ALARMCLOCK", submodels_ALARMCLOCK, iports_ALARMCLOCK, oports_ALARMCLOCK, eics_ALARMCLOCK, eocs_ALARMCLOCK, ics_ALARMCLOCK 
    );

    /*******TOP COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(displayed_time),(typeid(e_buzzer_on))};
    dynamic::modeling::Models submodels_TOP = {input_reader, ALARMCLOCK};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<displayed_time,displayed_time>("ALARMCLOCK"),
        dynamic::translate::make_EOC<e_buzzer_on,e_buzzer_on>("ALARMCLOCK"),
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out, e_time_set>("input_reader","ALARMCLOCK"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out, e_alarm_set>("input_reader","ALARMCLOCK"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out, e_hours>("input_reader","ALARMCLOCK"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out, e_minutes>("input_reader","ALARMCLOCK"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out, e_alarm_on>("input_reader","ALARMCLOCK"),
        dynamic::translate::make_IC<iestream_input_defs<Message_t>::out, e_snooze>("input_reader","ALARMCLOCK"),
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/ALARM_CLOCK_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/ALARM_CLOCK_output_state.txt");
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
    r.run_until_passivate();


    cout<<"ALARM CLOCK Model Run Successful."<<"\n";
    cout<<"To see the Output, go to results folder."<<"\n";


    return 0;
}
