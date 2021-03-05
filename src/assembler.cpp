#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include "./header/assembler.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;
int lineCounter = 0;

int isInvalidChar(char character, int indexCounter) {
    // Checks if symbol starts with number
    // Throw codes: 0->invalid char, 1->
    try{
        if(indexCounter == 0 && (int)character >=48 && (int)character <=57){
            throw(1);
        }
        else{
            return 0;
        } 
    }
    catch(int errorCode){
        cout << "Scanner/Lexical error.\n";
        cout << "Invalid character: '" << character<< "' Line: " << lineCounter << " Column: " << indexCounter+1 <<endl;
        return 1;
    }
    
    return 0;
}
int tokenizeLine(string readLine){
    lineCounter++;

    vector<string> currentPhrase;
    string currentWord;
    // Makes case insensitive
    for(auto &i:readLine) i = toupper(i);
    
    int indexCounter = 0;
    for(auto &i :readLine){
        
        // Checks if endline
        if(indexCounter == readLine.length()-1){
            currentWord+=i;
            currentPhrase.push_back(currentWord);
            currentWord.clear();
        
        }
        // Detects code comments
        else if(i==';') {
            currentPhrase.push_back(currentWord);
            currentWord.clear();
            return 1;
            
        }
        // Checks if separate words
        else if(i==':'|| i == ' '){
            
            if(currentWord.length() > 0){
                currentPhrase.push_back(currentWord);
                currentWord.clear();
            }
            
        }
        // Check if char is invalid
        else if(isInvalidChar(i, indexCounter)){
            cout << "            " << readLine << endl;
            cout <<"            " ; for(int i =0; i < indexCounter; i++) cout <<" "; cout<< "^" <<endl;
            return 0;
        }
        else currentWord +=i;
        
        indexCounter++;
    }

    for(auto &i : currentPhrase) cout << "Palavra: " <<i << endl;
    cout <<"=========================" <<endl;
    return 1;
}

int analyzeCode(ifstream &inFile){
    while(!inFile.eof()){
        string readLine;
        getline(inFile,readLine);
        if(!tokenizeLine(readLine)){
            cout << "Program ended with error.\n";
            return 0;
        }
    }
    cout << "Total Lines:" << lineCounter <<endl;
}