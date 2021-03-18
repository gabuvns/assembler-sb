#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <vector>

#include "./header/assembler.h"
using std::cout;
using std::endl;
using std::ifstream;
using std::string;


int main(int argc, char **argv) {
    string fileName;
    //Check initial input arguments 
    if(argc >= 2){
        fileName = argv[1];
    }
    else{
        cout << "Invalid number of arguments." << endl;
        return 0;
    }

    // OpenFile
    ifstream inFile(fileName);
    if(!inFile.is_open()){
        cout << "Error opening file." << endl;
        cout <<"Tried to open: '" << fileName << "'." << endl;
    }       
    
    analyzeCode(inFile);

    inFile.close();
    return 0;
}