#include "GlobalState.h"

void GlobalState::printSymbolTable() {
    SymbolTable::iterator it;

    cout << "Symbol Table" << endl;
    for(it = symbols->begin(); it != symbols->end(); it++) {
        cout << it->first << "=" << it->second->address << endl;
    }

    cout << endl;
}

void GlobalState::printNotUsedWarnings() {
    SymbolTable::iterator it;

    for(it = symbols->begin(); it != symbols->end(); it++) {
        if(!it->second->used) {
            cout << "WARNING: symbol " << it->second->name << " was defined in module ";
            cout << it->second->definingModuleNumber << ", but is never used!" << endl;
        }
    }
}

void GlobalState::printAddressList(vector<Module*>* modules) {
    int line = 0;
    vector<Module*>::iterator it;

    cout << "Memory Map" << endl;
    for(it = modules->begin(); it != modules->end(); it++) {
        (*it)->resolveExternalAddresses();
        vector<Code*>* codeVector = (*it)->getCodeVector();
        vector<Code*>::iterator codeIt;

        for(codeIt = codeVector->begin(); codeIt != codeVector->end(); codeIt++) {
            cout << line++ << ": " << (*codeIt)->word << endl;
        }
    }
}
