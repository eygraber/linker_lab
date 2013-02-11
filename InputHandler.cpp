#include "InputHandler.h"
#include<iostream>
#include<fstream>

vector<Module*>* InputHandler::parseInput(char* path, GlobalState* gs) {
    ifstream inputFile(path);

    int moduleNumber = 1;

    vector<Module*>* modules = new vector<Module*>();

    while(!inputFile.eof()) {
        try {
            modules->push_back(new Module(&inputFile, moduleNumber++, gs));
        } catch(LinkerException e) {
            inputFile.close();
            throw e;
        }
    }

    inputFile.close();

    return modules;
}
