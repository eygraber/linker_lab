#ifndef MODULE_H
#define MODULE_H

#include "GlobalState.h"
#include<fstream>
#include<sstream>
#include<vector>
using namespace std;

//a struct used to hold information from
//the code section of the module
struct Code {
    Code(char type, string word) : type(type), word(word) {}
    //type of address (R, E, A, I)
    char type;
    //the opcode (not used) followed by the 3 digit address
    string word;
};

//forward declarations
class GlobalState;
struct Symbol;

class Module {
    public:
        Module(ifstream* file, int moduleNumber, GlobalState* gs);

        void resolveExternalAddresses();

        vector<Code*>* getCodeVector() {
            return codeVector;
        }
    private:
        //this size of the module
        int size;
        //the absolute starting address of the module
        int startingAddress;

        int moduleNumber;

        GlobalState* gs;

        vector<Symbol*>* defListVector;
        vector<string>* useListVector;
        vector<Code*>* codeVector;
        //holds addresses of type E that need to
        //be resolved in the second pass
        vector<Code*>* needsResolving;

        void parseDefList(ifstream* file);

        void parseUseList(ifstream* file);

        void parseCode(ifstream* file);

        void checkDefListForErrors();

        //parseString traverses the file stream
        //and parses out a string until whitespace,
        //a newline, or eof is encountered
        string parseString(ifstream* s);
};

#endif
