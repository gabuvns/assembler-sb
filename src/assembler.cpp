#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "./header/assembler.h"
#include "./header/symbolTable.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;
using std::map;


enum SectionState {
    sectionData,
    sectionText
};
enum ProgramState {
    firstPassage ,
    secondPassage
};
ProgramState programState= firstPassage;
SectionState sectionState = sectionData;
bool programError = false;
vector<string> errorList;
vector<vector<string>> currentProgram;

int lineCounter = 0, acumulador = 0; 


// Estas funções de trim não são minhas e estão disponíveis em:
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring 
// Acesso: 8/03/2021
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}
// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}
// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}
// ======================================================================================


int isInvalidChar(char character, int indexCounter) {
    // Checks if symbol starts with number
    // Throw codes: 0->invalid char, 1->
    try{
        // Checks if symbol starts with number
        if(indexCounter == 0 && (int)character >=48 && (int)character <=57){
            throw(1);
        }
        // Checks for invalid char
        else if(!((int) character >= 65 && (int)character <= 90) && !((int)character >=48 && (int)character <=57)) {
            throw(2);
        }
    }
         
    
    catch(int errorCode){
        cout << "Scanner/Lexical error.\n" << errorCode <<endl;
        cout << "Invalid character: '" << character<< "' Line: " << lineCounter << " Column: " << indexCounter+1 <<endl;
        return 1;
    }
    
    return 0;
}

void whichCodeSection(vector<string> readLine){
    if(readLine.front() == "SECTION"){
        sectionState = readLine.back() == "DATA" ? sectionData : sectionText;
    }
}

vector<string> scanner(string readLine){
    lineCounter++;
    vector<string> currentPhrase;
    string currentWord;
    int indexCounter = 0;

    // Makes case insensitive
    for(auto &i:readLine) i = toupper(i);

    for(auto &i :readLine){
        // Checks if last element from line
        if(indexCounter == readLine.length()-1){
            currentWord+=i;
            trim(currentWord);
            if(!currentWord.empty()){
                currentPhrase.push_back(currentWord);
            }
            currentWord.clear();
        
        }
        // Detects code comments
        else if(i==';') {
            if(!currentWord.empty()){
                currentPhrase.push_back(currentWord);
            }
            currentWord.clear();
            return currentPhrase;
        }

        else if(i==':'){
            if(!currentWord.empty()){
                currentWord+=i;
                currentPhrase.push_back(currentWord);
            }
            currentWord.clear();
        }
        // Checks if separate words
        else if(i == ' '){
            if(!currentWord.empty()){
                currentPhrase.push_back(currentWord);
            }
                currentWord.clear();
            
        }
        // Check if char is invalid
        else if(i!='-' && i!=',' && isInvalidChar(i, indexCounter)){
            cout << "            " << readLine << endl;
            cout <<"            " ; for(int i =0; i < indexCounter; i++) cout <<" "; cout<< "^" <<endl;
            programError = true;
        }
        else currentWord +=i;
        
        indexCounter++;
    }

    return currentPhrase;

    // for(auto &i : currentPhrase) cout << "Palavra: " <<i << endl;
    // cout <<"=========================" <<endl;
}

Label parseLabel(vector<string> currentPhrase){
    string name = currentPhrase.at(0);
    // if(name.back() != ':') throw 2;
    LabelType labelType = currentPhrase.at(1) =="SPACE" ? typeSpace : typeConst;
    if(labelType == 0){
        int value = stoi(currentPhrase.at(2));
        cout <<"value:" << value <<endl;
        Label auxLabel(name, labelType, value);
        return auxLabel;
    }
    else if(labelType == 1){
        Label auxLabel(name, labelType, 0);
        return auxLabel;
    }

}

int searchLabelTable(){
    bool found = false;
    if(found){
        // error
        return 0;
    }
    else{
        // Found nothing and can add to table
        return 1;
    }
}
void parser(vector<string> currentPhrase){
    if(sectionState == sectionData){
        if(searchLabelTable() && currentPhrase.front() !="SECTION"){
            parseLabel(currentPhrase);
        }   
        
    }

}

void printProgram(){
    for(auto &i : currentProgram){
        for(auto j : i){
            cout  <<"{" << j <<  "}";
        }
        cout << endl <<"========================" <<endl;
    }
    cout << "Total Lines:" << lineCounter <<endl;

}

int analyzeCode(ifstream &inFile){
    while(!inFile.eof()){
        string readLine;
        getline(inFile,readLine, '\n');
        try{    
            vector<string> tokenizedLine = scanner(readLine);
            if(!tokenizedLine.empty()) currentProgram.push_back(tokenizedLine);
            whichCodeSection(tokenizedLine);

            parser(tokenizedLine);
            if(programError){
                throw 0;
            }            
        }
        catch (int error){
            cout << "Program ended with error.\n";
            return 0;
        }
        
    }
    printProgram();
    return lineCounter;
}