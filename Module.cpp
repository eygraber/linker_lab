#include "Module.h"
#include<cstdlib>
Module::Module(ifstream* file, int moduleNumber, GlobalState* gs) {
    //GlobalState maintains a record of what the current
    //absolute address is. It is initially 0, and is
    //incremented by the size of each module in succession.
    //Thus, when a Module first reads it, it will
    //hold its absolute starting address
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

    //After parsing the code, we know what the size of this
    //module is. We can then increment GlobalState's
    //currentAddress by it, so that the next Module will
    //know its starting address
    gs->currentAddress += size;

    //if the current address will exceed the size of the machine
    if(gs->currentAddress > MACHINE_SIZE - 1) {
        ostringstream error;
        error << "ERROR: total size of all modules exceeds the size of the machine!";
        throw LinkerException(error.str());
    }

    //once we know the size of the module
    //we can make sure that none of the symbols
    //in the def list were addressed out of bounds
    //of the module
    checkDefListForErrors();
}

void Module::parseDefList(ifstream* file) {
    //gets the number of symbols defined
    int numVars = atoi(parseString(file).c_str());

    for(int i = 0; i < numVars; i++) {
        //gets the symbols name
        string name = parseString(file);

        //gets its address
        int relativeAddress = atoi(parseString(file).c_str());

        //we get the absolute address by adding this module's
        //starting address to the relative address we just parsed out
        int absoluteAddress = relativeAddress + startingAddress;

        //we create a new Symbol (defined in GlobalState.h)
        Symbol* symbol = new Symbol(name, absoluteAddress, moduleNumber);

        defListVector->push_back(symbol);

        //check to see if already in symbol table
        pair< map<string, Symbol*>::iterator, bool > ret;
        //gs->symbols is of type SymbolTable (defined in GlobalState.h)
        //which is a typedef of map<string, Symbol*>. We use the name of the
        //symbol as a key into the map, so that we can check if there are
        //multiple definitions
        ret = gs->symbols->insert( pair<string, Symbol*>(name, symbol) );
        //if ret->second is false, it means that the key already
        //existed in the map, and this symbol was multiply defined
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
        // keep a record of which symbols are in the use list
        //we use a vector and append to the end to preserve
        // the ordering of the list
        useListVector->push_back(parseString(file));
    }
}

void Module::parseCode(ifstream* file) {
    //useListCheck is an array corresponding to the
    //symbols in the use list. After encountering
    //an E address that corresponds to a symbol in the
    //use list, we set the respective index in useListCheck
    //to true. If any index is false at the end of parsing
    //the code, we issue a warning that the symbol
    //was in the use list, but was not actually used
    bool* useListCheck = new bool[useListVector->size()];
    for(int j = 0; j < useListVector->size(); j++) {
        useListCheck[j] = false;
    }

    size = atoi(parseString(file).c_str());

    for(int i = 0; i < size; i++) {
        string stype = parseString(file);

        char type = stype[0];

        string word = parseString(file);

        //the address is the last 3 digits of the word
        int address = atoi(word.substr(1).c_str());

        if(type == 'R') {
            //if the address is out of bounds
            //of the module
            if(address > size - 1) {
                ostringstream error;
                error << "ERROR: relative address " << address << " exceeds the size of this module!";
                throw LinkerException(error.str());
            }
            //we can relocate the relative addresses here so why not
            address += startingAddress;
        }

        //if the address type is Absolute or Relative and the address
        //is >= 600, it is out of bounds of the machine
        if(type == 'A' || type == 'R' && address > MACHINE_SIZE - 1) {
            ostringstream error;
            error << "ERROR: absolute address " << address << " exceeds the size of the machine!";
            throw LinkerException(error.str());
        }

        ostringstream ss;
        ss << address;
        //we convert the address to a string (itoa not always implemented)
        string addressStr = ss.str();
        //if the address is not in the 100+ range, we right-pad
        //it with zeroes
        while(addressStr.length() < 3) {
            addressStr.insert(0, "0");
        }
        //we replace the relative address with the relocated one
        word = word.substr(0, 1) + addressStr;

        //now that we have our type and word
        //we can insert it into the codeVector
        Code* code = new Code(type, word);

        if(type == 'E') {
            //if it is an External address and it references
            //an index that is out of bounds of the use list
            if(address > useListVector->size() - 1) {
                ostringstream error;
                error << "ERROR: external address " << address << " is too large of an index into this module's use list!";
                throw LinkerException(error.str());
            }
            //otherwise, notate that we used this symbol
            //and add it to the vector of addresses that need resolving
            useListCheck[address] = true;
            needsResolving->push_back(code);
        }

        codeVector->push_back(code);
    }

    for(int i = 0; i < useListVector->size(); i++) {
        //issue a warning for each symbol in the use
        //list that we didn't use
        if(!useListCheck[i]) {
            cout << "WARNING: symbol " << (*useListVector)[i] << " appears in use list, but is not used!" << endl;
        }
    }
}

void Module::checkDefListForErrors() {
    vector<Symbol*>::iterator it;
    for(it = defListVector->begin(); it != defListVector->end(); it++) {
        int relativeAddress = (*it)->address - startingAddress;
        //if the symbol in the def list has an address
        //that is greater than the size of the module
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
        //for External addresses the address is initially an index
        //into the use list
        int index = atoi((*it)->word.substr(1).c_str());
        //we get the name of the symbol from the use list
        //at the index stored in the code
        string name = (*useListVector)[index];

        //we get an iterator into the SymbolTable using the
        //name of the symbol from the use list as a key
        SymbolTable::iterator symbolIt = gs->symbols->find(name);
        //if it is not in the SymbolTable
        if(symbolIt == gs->symbols->end()) {
            ostringstream error;
            error << "ERROR: symbol " << name << " used at address " << (*it)->word << " is not defined!";
            throw LinkerException(error.str());
        }
        //we get the absolute address of the symbol from
        //the value returned from the SymbolTable
        int resolvedAddress = symbolIt->second->address;
        ostringstream ss;
        ss << resolvedAddress;
        //we convert the address to a string (itoa not always implemented)
        string addressStr = ss.str();
        //if the address is not in the 100+ range, we right-pad
        //it with zeroes
        while(addressStr.length() < 3) {
            addressStr.insert(0, "0");
        }
        //we replace the relative address with the resolved one
        (*it)->word = (*it)->word.substr(0, 1) + addressStr;
        //we notate that this symbol was used
        symbolIt->second->used = true;
    }
}
