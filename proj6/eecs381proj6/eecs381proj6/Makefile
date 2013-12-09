CC = g++
LD = g++

CFLAGS = -c -g -pedantic -std=c++11 -Wall
LFLAGS = -g

OBJS = p6_main.o Model.o View.o Controller.o
OBJS += MapView.o LocalMapView.o FullMapView.o ListView.o HealthView.o AmountsView.o AttackView.o
OBJS += Sim_object.o Structure.o Moving_object.o AgentComponent.o
OBJS += Farm.o Town_Hall.o
OBJS += Peasant.o Warrior.o Soldier.o Archer.o Magician.o
OBJS += AgentComponent_factory.o Structure_factory.o
OBJS += Geometry.o Utility.o
PROG = p6exe

default: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) -o $(PROG)

tests: $(PROG)
	make test file=fight
	make test file=fight_noshow
	make test file=name_validity
	make test file=new_features
	make test file=states
	make test file=views
	make test file=workviolence
	make test file=workviolence_noshow
test: $(PROG)
	./p6exe < samples/$(file)_in.txt > junk.out
	diff junk.out samples/$(file)_out.txt

p6_main.o: p6_main.cpp Model.h Controller.h
	$(CC) $(CFLAGS) p6_main.cpp

Model.o: Model.cpp Model.h View.h Sim_object.h Structure.h AgentComponent.h AgentComponent_factory.h Structure_factory.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Model.cpp

View.o: View.cpp View.h Geometry.h Utility.h
	$(CC) $(CFLAGS) View.cpp

MapView.o: MapView.cpp MapView.h View.h Geometry.h Utility.h
	$(CC) $(CFLAGS) MapView.cpp
  
AttackView.o: AttackView.cpp View.h Geometry.h Utility.h
	$(CC) $(CFLAGS) AttackView.cpp

FullMapView.o: FullMapView.cpp MapView.h View.h Geometry.h Utility.h
	$(CC) $(CFLAGS) FullMapView.cpp

LocalMapView.o: LocalMapView.cpp MapView.h View.h Geometry.h Utility.h
	$(CC) $(CFLAGS) LocalMapView.cpp

ListView.o: ListView.cpp ListView.h View.h Geometry.h Utility.h
	$(CC) $(CFLAGS) ListView.cpp
  
HealthView.o: HealthView.cpp ListView.h View.h Geometry.h Utility.h
	$(CC) $(CFLAGS) HealthView.cpp
  
AmountsView.o: AmountsView.cpp ListView.h View.h Geometry.h Utility.h
	$(CC) $(CFLAGS) AmountsView.cpp

Controller.o: Controller.cpp Controller.h Model.h View.h MapView.h Sim_object.h Structure.h AgentComponent.h AgentComponent_factory.h Structure_factory.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Controller.cpp

Sim_object.o: Sim_object.cpp Sim_object.h Geometry.h
	$(CC) $(CFLAGS) Sim_object.cpp

Structure.o: Structure.cpp Structure.h Sim_object.h Geometry.h
	$(CC) $(CFLAGS) Structure.cpp

Farm.o: Farm.cpp Farm.h Structure.h Sim_object.h Geometry.h
	$(CC) $(CFLAGS) Farm.cpp

Town_Hall.o: Town_Hall.cpp Town_Hall.h Structure.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Town_Hall.cpp

AgentComponent.o: AgentComponent.cpp AgentComponent.h Moving_object.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) AgentComponent.cpp

Peasant.o: Peasant.cpp Peasant.h AgentComponent.h Moving_object.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Peasant.cpp

Warrior.o: Warrior.cpp Warrior.h AgentComponent.h Moving_object.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Warrior.cpp
    
Soldier.o: Soldier.cpp Warrior.h AgentComponent.h Moving_object.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Soldier.cpp
    
Archer.o: Archer.cpp Warrior.h AgentComponent.h Moving_object.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Archer.cpp
    
Magician.o: Magician.cpp Warrior.h AgentComponent.h Moving_object.h Sim_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Magician.cpp

Moving_object.o: Moving_object.cpp Moving_object.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Moving_object.cpp

AgentComponent_factory.o: AgentComponent_factory.cpp AgentComponent_factory.h Geometry.h Utility.h
	$(CC) $(CFLAGS) AgentComponent_factory.cpp

Structure_factory.o: Structure_factory.cpp Structure_factory.h Geometry.h Utility.h
	$(CC) $(CFLAGS) Structure_factory.cpp

Geometry.o: Geometry.cpp Geometry.h
	$(CC) $(CFLAGS) Geometry.cpp

Utility.o: Utility.cpp Utility.h
	$(CC) $(CFLAGS) Utility.cpp

submit: $(PROG)
	submit381 6 Makefile *.h *.cpp *.txt
  
clean:
	rm -f *.o
real_clean:
	rm -f $(PROG)
	rm -f *.o
