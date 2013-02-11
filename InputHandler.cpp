#include "InputHandler.h"
#include<iostream>
#include<fstream>

vector<Module*>* InputHandler::parseInput(char* path, GlobalState* gs) {
    ifstream inputFile(path);

    //this is how we keep track of the order
    //in which the modules were processed
    int moduleNumber = 1;

    vector<Module*>* modules = new vector<Module*>();

    while(!inputFile.eof()) {
        try {
            //the file stream is passed to the module
            //it will parse it until it hits the end of the module
            //or the eof
            modules->push_back(new Module(&inputFile, moduleNumber++, gs));
        } catch(LinkerException e) {
            inputFile.close();
            throw e;
        }
    }

    inputFile.close();

    return modules;
}
