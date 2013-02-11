#ifndef MODULE_H
#define MODULE_H

#include "GlobalState.h"
#include<fstream>
#include<sstream>
#include<vector>
using namespace std;

struct Code {
    Code(char type, string word) : type(type), word(word) {}
    char type;
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
        int size;
        int startingAddress;

        int moduleNumber;

        GlobalState* gs;

        vector<Symbol*>* defListVector;
        vector<string>* useListVector;
        vector<Code*>* codeVector;
        vector<Code*>* needsResolving;

        void parseDefList(ifstream* file);

        void parseUseList(ifstream* file);

        void parseCode(ifstream* file);

        void checkDefListForErrors();

        string parseString(ifstream* s);
};

#endif
