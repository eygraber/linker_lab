linker : main.o Module.o GlobalState.o InputHandler.o
	g++ -o linker main.o Module.o GlobalState.o InputHandler.o
		 
main.o : main.cpp InputHandler.h Module.h GlobalState.h
	g++ -c main.cpp
Module.o : Module.cpp Module.h GlobalState.h
	g++ -c Module.cpp
GlobalState.o : GlobalState.cpp GlobalState.h Module.h
	g++ -c GlobalState.cpp
InputHandler.o : InputHandler.cpp InputHandler.h
	g++ -c InputHandler.cpp
clean : 
	rm linker main.o Module.o GlobalState.o InputHandler.o