#ifndef BOOST_SIMULATION_MESSAGE_HPP
#define BOOST_SIMULATION_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

/*******************************************/
/**************** Message_t ****************/
/*******************************************/
struct Message_t{
 
	Message_t(){}
  	Message_t(int i_value1,int i_value2, int i_value3,int i_value4, int i_value5,int i_value6)
  	:value1(i_value1), value2(i_value2),value3(i_value3),value4(i_value4),value5(i_value5),value6(i_value6){}
	
  	int value1; 
	int value2; 
	int value3; 
	int value4; 
	int value5; 
	int value6; 

};

istream& operator>> (istream& is, Message_t& msg);

ostream& operator<<(ostream& os, const Message_t& msg);


#endif // BOOST_SIMULATION_MESSAGE_HPP