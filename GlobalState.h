#ifndef GLOBALSTATE_H
#define GLOBALSTATE_H

#include<iostream>
#include<stdexcept>
#include<map>
#include<vector>
#include "Module.h"
using namespace std;

class LinkerException : public runtime_error {
    public:
        LinkerException(string msg) : runtime_error(msg) {}
};

//a struct that holds values for a symbol
//used for values in the SymbolTable and definition lists
struct Symbol {
    Symbol(string name, int address, int definingModuleNumber) :
        name(name), address(address), definingModuleNumber(definingModuleNumber), used(false) {}
    string name;
    int address;
    int definingModuleNumber;
    bool used;
};

typedef map<string, Symbol*> SymbolTable;

//forward declaration
class Module;

class GlobalState {
    public:
        GlobalState() : symbols(new SymbolTable), currentAddress(0) {}
        //holds all of the symbols in the module's definition lists
        SymbolTable* symbols;

        //holds the current absolute address that the linker is up to
        //initially 0
        int currentAddress;

        void printSymbolTable();

        void printNotUsedWarnings();

        void printAddressList(vector<Module*>* modules);
        
        ~GlobalState() {
            delete symbols;
        }
};

#endif
