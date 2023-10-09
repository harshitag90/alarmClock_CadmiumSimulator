CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
message.o: data_structures/message.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/message.cpp -o build/message.o

main.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/main.cpp -o build/main.o

timeComparator.o: test/timeComparator.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/timeComparator.cpp -o build/timeComparator.o
	
timeAdder.o: test/timeAdder.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/timeAdder.cpp -o build/timeAdder.o
	
hoursRegister.o: test/hoursRegister.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/hoursRegister.cpp -o build/hoursRegister.o
	
minRegister.o: test/minRegister.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/minRegister.cpp -o build/minRegister.o
	
displayDriver.o: test/displayDriver.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/displayDriver.cpp -o build/displayDriver.o
	
buzzerDriver.o: test/buzzerDriver.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/buzzerDriver.cpp -o build/buzzerDriver.o
	
alarmRegister.o: test/alarmRegister.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/alarmRegister.cpp -o build/alarmRegister.o
		
tests: message.o timeComparator.o timeAdder.o minRegister.o hoursRegister.o buzzerDriver.o alarmRegister.o displayDriver.o
	$(CC) -g -o bin/TIME_COMPARATOR build/timeComparator.o build/message.o
	$(CC) -g -o bin/TIME_ADDER build/timeAdder.o build/message.o
	$(CC) -g -o bin/MINUTES_REGISTER build/minRegister.o build/message.o
	$(CC) -g -o bin/HOURS_REGISTER build/hoursRegister.o build/message.o
	$(CC) -g -o bin/BUZZER_DRIVER build/buzzerDriver.o build/message.o
	$(CC) -g -o bin/ALARM_REGISTER build/alarmRegister.o build/message.o
	$(CC) -g -o bin/DISPLAY_DRIVER build/displayDriver.o build/message.o

#TARGET TO COMPILE ONLY IOBS SIMULATOR
simulator: main.o message.o 
	$(CC) -g -o bin/ALARMCLOCK build/main.o build/message.o 
	
#TARGET TO COMPILE EVERYTHING (ALARM Clock + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*