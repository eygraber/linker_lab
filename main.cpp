#include<iostream>
#include<fstream>
#include "GlobalState.h"
#include "InputHandler.h"
#include "Module.h"

using namespace std;

int main(int argc, char** argv) {
    ofstream out("output.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());
    for(int i = 1; i < argc; i++) {
        cout << "--------For File " << argv[i] << "---------" << endl;
        GlobalState* gs = new GlobalState();

            try {
                InputHandler ih;
                vector<Module*>* modules = ih.parseInput(argv[i], gs);

                gs->printSymbolTable();
                gs->printAddressList(modules);
                gs->printNotUsedWarnings();
            } catch(LinkerException e) {
                cout << e.what() << endl;
            }
        cout << endl;
    }

    cout.rdbuf(coutbuf);

    return 0;
}
