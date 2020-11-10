################################################################################################################################################
###        In this Makefile made by Negi3939, you can use as                                                               	                     ###
###        $ make                     # You can get the executable file which written in TARGET.                                             ###
###        $ make target=hoge         # You can get the executable file which written in hoge.                                               ###
###        $ make clean               # The executable file which written in TARGET will removed.                                            ###
###        $ make clean target=hoge   # The executable file which written in hoge will removed.                                              ###
################################################################################################################################################

TARGET=$(MAKECMDGOALS)
ifeq ($(MAKECMDGOALS),)
	TARGET=comclient
endif
ifeq ($(MAKECMDGOALS),clean)
	TARGET=comclient
endif


DIRX = /usr/X11R6/lib

CXXFLAGS = -I /home/jsk/eigen-2.0.17/ -I /home/taka/eigenlib -fpermissive
LDFLAGS	 = -L "$(DIRX)" -lm -pthread -std=c++11

GCCVERSION = $(shell g++ --version | grep ^g++)
ifeq "$(GCCVERSION)" "g++ (GCC) 3.3.5 (Debian 1:3.3.5-13)"
	CXXFLAGS += -DGCC3p3
endif

ifeq ($(TARGET),seq)
	SOURCE_MAIN = sequence.cpp
	CXXFLAGS += -DSEQUENCE_IS_MAIN
endif

ifeq ($(TARGET),comserver)
	SOURCE_MAIN = server.cpp
	SOURCE_SUB = socketcom.cpp sequence.cpp mymath.cpp
endif

ifeq ($(TARGET),comclient)
	SOURCE_MAIN = client.cpp
	SOURCE_SUB = socketcom.cpp sequence.cpp mymath.cpp
endif

ifeq ($(TARGET),sock)
sock.out:
	g++ -o sequence.o sequence.cpp -c $(CXXFLAGS) -w
	g++ -o socketcom.o socketcom.cpp -c $(CXXFLAGS) -w
	g++ -o server.o server.cpp -c $(CXXFLAGS) -w
	g++ -o client.o client.cpp -c $(CXXFLAGS) -w
	g++ -o sever.out sequence.o socketcom.o server.o $(LDFLAGS) -w
	g++ -o client.out sequence.o socketcom.o client.o $(LDFLAGS) -w
endif

ifeq ($(TARGET),hoge)
	SOURCE_MAIN = hoge.cpp
	SOURCE_SUB = hogesub1.cpp hogesub2.cpp
	CXXFLAGS += -Dhogehoge_MAIN
endif

SUBOBJ = $(SOURCE_SUB:%.cpp=%.o)
MAINOBJ = $(SOURCE_MAIN:%.cpp=%.o)

PROGRAM = $(SOURCE_MAIN:%.cpp=%.out)
PROGRAM += $(SUBOBJ) $(MAINOBJ)

comclient: $(PROGRAM)
comserver: $(PROGRAM)
seq: $(PROGRAM)

%.out: %.o $(SUBOBJ)
	g++ -o $@ $^ $(LDFLAGS) -w
%.o : %.cpp
	g++ -o $@ $< -c $(CXXFLAGS) -w
clean:
	rm -f *.o *.so $(PROGRAM)
