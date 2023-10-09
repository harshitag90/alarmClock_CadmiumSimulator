#include <math.h> 
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "message.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const Message_t& msg) {
  
	os << msg.value1 << " " << msg.value2<< " " << msg.value3<< " " << msg.value4<< " " << msg.value5<< " " << msg.value6;
	return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, Message_t& msg) {
 
	is >> msg.value1;
	is >> msg.value2;
		is >> msg.value3;
			is >> msg.value4;
				is >> msg.value5;
					is >> msg.value6;
	return is;
}
