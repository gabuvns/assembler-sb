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
SectionState sectionState = sectionText;
CodeTable codeTable;
bool programError = false;
vector<string> errorList;
vector<vector<string>> currentProgram;
int lineCounter = 0, acumulador = 0, programCounter = 0; 


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

void printProgram(){
    for(auto &i : currentProgram){
        for(auto j : i){
            cout  <<"{" << j <<  "}";
        }
        cout << endl <<"========================" <<endl;
    }
    cout << "Total Lines:" << lineCounter <<endl;

}

void printStringVector(vector<string> vetor){
    for(auto i : vetor) cout << i <<" ";
    cout << endl;
}

void printLabelTable(){
    for(auto i : codeTable.labelTable) {
        cout << "Label ===============\n";
        cout << "Name: " << i.name <<endl;
        cout << "value: " << i.value <<endl;
        cout << "line: " << i.line <<endl;
        cout << "LabelType: " << i.labelType <<endl;

    }
}
int isInvalidChar(char character, int indexCounter) {
    // Checks if symbol starts with number
    // Throw codes: 0->invalid char, 1->
    try{
        // Checks if symbol starts with number
        if(indexCounter == 0 && (int)character >=48 && (int)character <=57){
            throw(1);
        }
        // Checks for invalid char
        else if(!((int) character >= 65 && (int)character <= 90) && !((int)character >=48 && (int)character <=57)){
            if(character != '_'){
                throw(2);

            }
        }
    } 
    
    catch(int errorCode){
        cout << "Scanner/Lexical error.\n"  <<endl;
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

int classifyLine(vector<string> vetor){
    auto instructionName = InstructionsTable.find(vetor.front());
    if(instructionName != InstructionsTable.end()){

        // cout <<"Achei: " << instructionName->first <<endl;
    }
    return 1;
}

int searchLabelTable(vector<string> currentPhrase ){
    bool found = false;
    if(currentPhrase.at(0) == "SECTION") return 0;
    if(currentPhrase.at(0).back() == ':')currentPhrase.at(0).pop_back();
    
    for(auto i : codeTable.labelTable){
        if(i.name == currentPhrase.at(0)){
            cout<< "Semantical/Parser Error\n";
            printStringVector(currentPhrase);
            cout << "Label declared previously at line: " << i.line <<  " Current Line: " << lineCounter <<endl;
            programError = 1;
            found = true;
        } 
    }
    
    return !found;
}

void wrongNumberOfArguments(vector<string> currentPhrase){
    cout<< "Syntactical Error\n";
    cout <<"Wrong Number of Arguments at line: " << lineCounter << endl;
    programError = 1;
}
void unknownLabelType(){
    cout<< "Syntactical Error\n";
    cout <<"Unknown type of label at line: " << lineCounter << endl;
    programError = 1;
}

void parseLabel(vector<string> currentPhrase){
    
    if(currentPhrase.at(0).back() == ':')currentPhrase.at(0).pop_back();

    if(currentPhrase.size() == 1){
        wrongNumberOfArguments(currentPhrase);
    }

    else if (currentPhrase.at(1) == "CONST"){
        if(currentPhrase.size() == 3){
            codeTable.labelTable.push_back(Label(currentPhrase.at(0),  typeConst, stoi(currentPhrase.at(2)),
            lineCounter, programCounter));
        }
        else{
            wrongNumberOfArguments(currentPhrase);
        }
    }

    else if(currentPhrase.at(1) == "SPACE"){
        if(currentPhrase.size() ==2){
            codeTable.labelTable.push_back(Label(currentPhrase.at(0), typeSpace, 0, lineCounter, programCounter));
        }
        else{
            wrongNumberOfArguments(currentPhrase);
        }
    }
    else{
        unknownLabelType();
    }
    
}
void parser(vector<string> currentPhrase){
    if(sectionState == sectionData){
        if(searchLabelTable(currentPhrase) && currentPhrase.front() !="SECTION"){
            if(currentPhrase.size() >4){
                wrongNumberOfArguments(currentPhrase);
            }
            else parseLabel(currentPhrase);
            
        }   
    }
    else if (sectionState == sectionText){
      if(classifyLine(currentPhrase)){

      }
    }
}

int analyzeCode(ifstream &inFile){
    while(!inFile.eof()){
        string readLine;
        getline(inFile,readLine, '\n');
        try{    
            vector<string> tokenizedLine = scanner(readLine);
            if(!tokenizedLine.empty()) {
                currentProgram.push_back(tokenizedLine);
                whichCodeSection(tokenizedLine);
            }

            if(!tokenizedLine.empty()) parser(tokenizedLine);
            if(programError){
                throw 0;
            }            
        }
        catch (int error){
            cout << "Program ended with error.\n";
            return 0;
        }
        
    }
    printLabelTable();

    // printProgram();
    return lineCounter;
}