#include<iostream>
#include<fstream>
#include "GlobalState.h"
#include "InputHandler.h"
#include "Module.h"

using namespace std;

int main(int argc, char** argv) {
    //temporarily sets stdout to ./output.txt
    ofstream out("output.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());
    //for all of the filenames passed in
    for(int i = 1; i < argc; i++) {
        cout << "--------For File " << argv[i] << "---------" << endl;
        //create a new GlobalState for each file
        GlobalState* gs = new GlobalState();

            try {
                InputHandler ih;
                vector<Module*>* modules = ih.parseInput(argv[i], gs);

                gs->printSymbolTable();
                gs->printAddressList(modules);
                gs->printNotUsedWarnings();
            } catch(LinkerException e) {
                //prints the error
                cout << e.what() << endl;
            }

        delete gs;

        cout << endl;
    }

    //sets stdout back
    cout.rdbuf(coutbuf);

    return 0;
}
