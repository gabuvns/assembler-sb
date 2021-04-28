/*
Author: Carlos Gabriel Vilas Novas Soares
*/
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
    if(argc >= 2){
        for(int i =1; i < argc; i++){
            string fileName;
            string outputFileName;

            //Check initial input arguments 
        
            fileName = argv[i];
            // OpenFile
            ifstream inFile(fileName);
            if(!inFile.is_open()){
                cout << "Error opening file." << endl;
                cout <<"Tried to open: '" << fileName << "'." << endl;
            }      
            analyzeCode(inFile, fileName);

            inFile.close();     
        }
            
    }
    
    else{
        cout << "Invalid number of arguments." << endl;
        return 1;
    }

    return 0;
}