#include "Module.h"
#include<cstdlib>
Module::Module(ifstream* file, int moduleNumber, GlobalState* gs) {
    startingAddress = gs->currentAddress;

    this->moduleNumber = moduleNumber;
    this->gs = gs;

    defListVector = new vector<Symbol*>();
    useListVector = new vector<string>();
    needsResolving = new vector<Code*>();
    codeVector = new vector<Code*>();

    parseDefList(file);
    parseUseList(file);
    parseCode(file);

    gs->currentAddress += size;

    checkDefListForErrors();
}

void Module::parseDefList(ifstream* file) {
    int numVars = atoi(parseString(file).c_str());

    for(int i = 0; i < numVars; i++) {
        string name = parseString(file);

        int relativeAddress = atoi(parseString(file).c_str());

        int absoluteAddress = relativeAddress + startingAddress;

        Symbol* symbol = new Symbol(name, absoluteAddress, moduleNumber);

        defListVector->push_back(symbol);

        //check to see if already in symbol table (error 1), otherwise add it
        pair< map<string, Symbol*>::iterator, bool > ret;
        ret = gs->symbols->insert( pair<string, Symbol*>(name, symbol) );
        if(!ret.second) {
            ostringstream error;
            error << "ERROR: symbol " << name << " already defined!";
            throw LinkerException(error.str());
        }
    }
}

void Module::parseUseList(ifstream* file) {
    int numVars = atoi(parseString(file).c_str());

    for(int i = 0; i < numVars; i++) {
        useListVector->push_back(parseString(file));
    }
}

void Module::parseCode(ifstream* file) {
    bool* useListCheck = new bool[useListVector->size()];
    for(int j = 0; j < useListVector->size(); j++) {
        useListCheck[j] = false;
    }

    size = atoi(parseString(file).c_str());

    for(int i = 0; i < size; i++) {
        string stype = parseString(file);

        char type = stype[0];

        string word = parseString(file);

        int address = atoi(word.substr(1).c_str());

        if(type == 'A' && address > 599) {
            ostringstream error;
            error << "ERROR: absolute address " << address << " exceeds the size of the machine!";
            throw LinkerException(error.str());
        }

        if(type == 'R') {
            if(address > size - 1) {
                ostringstream error;
                error << "ERROR: relative address " << address << " exceeds the size of this module!";
                throw LinkerException(error.str());
            }
            //we can relocate the relative addresses here so why not
            address += startingAddress;
        }

        ostringstream ss;
        ss << address;
        string addressStr = ss.str();
        while(addressStr.length() < 3) {
            addressStr.insert(0, "0");
        }
        word = word.substr(0, 1) + addressStr;

        Code* code = new Code(type, word);

        if(type == 'E') {
            if(address > useListVector->size() - 1) {
                ostringstream error;
                error << "ERROR: external address " << address << " is too large of an index into this module's use list!";
                throw LinkerException(error.str());
            }
            useListCheck[address] = true;
            needsResolving->push_back(code);
        }

        codeVector->push_back(code);
    }

    for(int i = 0; i < useListVector->size(); i++) {
        if(!useListCheck[i]) {
            cout << "WARNING: symbol " << (*useListVector)[i] << " appears in use list, but is not used!" << endl;
        }
    }
}

void Module::checkDefListForErrors() {
    vector<Symbol*>::iterator it;
    for(it = defListVector->begin(); it != defListVector->end(); it++) {
        //check if relativeAddress is larger than size (error 4)
        int relativeAddress = (*it)->address - startingAddress;
        if(relativeAddress > size) {
            ostringstream error;
            error << "ERROR: definition of symbol " << (*it)->name << " at address " << relativeAddress;
            error << " exceeds module " << moduleNumber << "'s size (" << size << ")!";
            throw LinkerException(error.str());
        }
    }
}

string Module::parseString(ifstream* s) {
    char c;
    string str = "";
    do {
        if(!s->good()) {
            throw LinkerException("ERROR: the input wasn't formatted correctly!");
        }
        c = s->get();
    } while(c == ' ' || c == '\n');

    while(c != ' ' && c != '\n') {
        str += c;

        if(!s->good()) {
            break;
        }

        c = s->get();
    }

    return str;
}

void Module::resolveExternalAddresses() {
    vector<Code*>::iterator it;

    for(it = needsResolving->begin(); it != needsResolving->end(); it++) {
        int index = atoi((*it)->word.substr(1).c_str());
        string name = (*useListVector)[index];

        SymbolTable::iterator symbolIt = gs->symbols->find(name);
        if(symbolIt == gs->symbols->end()) {
            ostringstream error;
            error << "ERROR: symbol " << name << " used at address " << (*it)->word << " is not defined!";
            throw LinkerException(error.str());
        }
        int resolvedAddress = symbolIt->second->address;
        ostringstream ss;
        ss << resolvedAddress;
        string addressStr = ss.str();
        while(addressStr.length() < 3) {
            addressStr.insert(0, "0");
        }
        (*it)->word = (*it)->word.substr(0, 1) + addressStr;
        symbolIt->second->used = true;
    }
}
