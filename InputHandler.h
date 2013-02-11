#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "Module.h"
#include "GlobalState.h"
#include<vector>

using namespace std;

class InputHandler {
    public:
        static vector<Module*>* parseInput(char* path, GlobalState* gs);
};

#endif
