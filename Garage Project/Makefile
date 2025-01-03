objects = main.o Control.o View.o Garage.o Entity.o RepairList.o Repair.o Mechanic.o Customer.o Car.o Tester.o
testobjects = test.o TestControl.o  View.o Garage.o Entity.o RepairList.o Repair.o Mechanic.o Customer.o Car.o Tester.o

all: a3 a3test

a3: $(objects)
	g++ -o a3 $(objects) 

a3test: $(testobjects) 
	g++ -o a3test $(testobjects) 

main.o: main.cc Control.h 
	g++ -c main.cc 

test.o: test.cc TestControl.h
	g++ -c test.cc

Control.o: Control.h Control.cc
	g++ -c Control.cc

TestControl.o: TestControl.cc TestControl.h
	g++ -c TestControl.cc
	
View.o: View.cc View.h
	g++ -c View.cc
	
Garage.o: Garage.cc Garage.h
	g++ -c Garage.cc

Entity.o: Entity.cc Entity.h
	g++ -c Entity.cc

Car.o: Car.cc Car.h
	g++ -c Car.cc

RepairList.o: RepairList.h RepairList.cc
	g++ -c RepairList.cc

Repair.o: Repair.h Repair.cc
	g++ -c Repair.cc

Mechanic.o: Mechanic.cc Mechanic.h Entity.h
	g++ -c Mechanic.cc 

Customer.o: Customer.cc Customer.h 
	g++ -c Customer.cc

Tester.o: Tester.cc Tester.h
	g++ -c Tester.cc
	

clean:
	rm -f a3 a3test *.o