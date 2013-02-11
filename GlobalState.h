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

struct Symbol {
    Symbol(string name, int address, int definingModuleNumber) : name(name), address(address), definingModuleNumber(definingModuleNumber), used(false) {}
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
        SymbolTable* symbols;
        int currentAddress;

        void printSymbolTable();

        void printNotUsedWarnings();

        void printAddressList(vector<Module*>* modules);
};

#endif
