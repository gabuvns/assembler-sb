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


// These trim functions and only them, are not mine and were originally avaiable at:
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring 
// Access: 8/03/2021
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
////////////////////////////////////////////////////////////////////////////////////


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
        cout << "Value: " << i.value <<endl;
        cout << "Line: " << i.line <<endl;
        cout << "LabelType: " << i.labelType <<endl;

    }
}
void printInstructionTable(){
    for(auto i : codeTable.instructionTable) {
        cout << "Instruction ===============\n";
        cout << "Name: " <<  i.simbolicOpcode<<endl;
        cout << "Opcode: " << i.opcode<<endl;
        cout << "Line: " << i.line <<endl;
        cout << "Parameters: " << i.numberOfParameters <<endl;
        for(auto j : i.parameters){
            cout << "Label param:" << j.label->name << " " << j.label->value << endl;
        }        

    }
}

// Error printing sections
void wrongNumberOfArguments(vector<string> codeLine){
    cout<< "Syntactical Error\n";
    cout <<"Wrong Number of Arguments at line: " << lineCounter << endl;
    printStringVector(codeLine);
    programError = 1;
}

void unknownLabelType(){
    cout<< "Syntactical Error\n";
    cout <<"Unknown type of label at line: " << lineCounter << endl;
    programError = 1;
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
// Detects which section of the code is currently beign read
void whichCodeSection(vector<string> readLine){
    if(readLine.front() == "SECTION"){
        sectionState = readLine.back() == "DATA" ? sectionData : sectionText;
    }
}

// Scanns the code for lexical erros and tokenizes it
vector<string> scanner(string readLine){
    lineCounter++;
    vector<string> codeLine;
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
                codeLine.push_back(currentWord);
            }
            currentWord.clear();
        
        }
        // Detects code comments
        else if(i==';') {
            if(!currentWord.empty()){
                codeLine.push_back(currentWord);
            }
            currentWord.clear(); 
            return codeLine;
        }

        else if(i==':'){
            if(!currentWord.empty()){
                currentWord+=i;
                codeLine.push_back(currentWord);
            }
            currentWord.clear();
        }
        // Checks if separate words
        else if(i == ' '){
            if(!currentWord.empty()){
                codeLine.push_back(currentWord);
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

    return codeLine;
}

Label * linkParameter(string paramName){
    for(auto &i : codeTable.labelTable){
        if(i.name == paramName){
            return &(i);
        }
    }
}

vector<Parameter> parseCopyParameters(vector<string> codeLine){
    vector<Parameter> auxVector;
    string parameters = codeLine.at(1);
    string paramName;
    Parameter auxParameter;
   
    for(auto &i : parameters){
        if(i==','){
            auxParameter.label = linkParameter(paramName);
            auxVector.push_back(auxParameter);
            paramName.clear();
        }
        else{
            paramName+=i;
        }
    }
    // Gets second Element
    auxParameter.label = linkParameter(paramName);
    auxVector.push_back(auxParameter);
    return auxVector;
}

vector<Parameter> addParametersToInstruction(vector<string> codeLine, Instruction auxInstruction){
    vector<Parameter> auxVector;

    if(auxInstruction.simbolicOpcode =="COPY"){
        // Verifica se argumento errado ex: COPY OLD_DATA,NEWDATA ABC
        if(codeLine.size() != 2){
            wrongNumberOfArguments(codeLine);
        }
        else{
            auxVector = parseCopyParameters(codeLine); 
            // Verifica se argumento errado ex: COPY OLD_DATA,
            if(auxVector.size() != 2){
                wrongNumberOfArguments(codeLine);
            }
            else{
                return auxVector; 

            }
        }
    }
    else if(auxInstruction.numberOfParameters != (codeLine.size() - 1)){
        Parameter auxParameter;
        // auxParameter.label.
    }
    else{
        cout <<"nao ta pronto\n";
        wrongNumberOfArguments(codeLine);
    }
    return auxVector;
}

// If found instruction return true;
int searchInstructionMap(vector<string> codeLine){
    auto searchedInstruction = InstructionsMap.find(codeLine.front());

    if(searchedInstruction !=  InstructionsMap.end()){   
        Instruction auxInstruction = searchedInstruction->second;
        auxInstruction.line = lineCounter;
        auxInstruction.programCounter = programCounter;
        auxInstruction.parameters = addParametersToInstruction(codeLine, auxInstruction);
        codeTable.instructionTable.push_back(auxInstruction);
        programCounter+=auxInstruction.sizeInWords;
        return 1;
    }

    else{
        return 0;
    }
}

// If found directive return true;
int searchDirectiveTable(vector<string> codeLine){

    return 1;
}
int classifyLine(vector<string> codeLine){
    if(searchInstructionMap(codeLine)){

    }
    else if(searchDirectiveTable(codeLine)){

    }
    else{
        // Error label not found
    }
    
    return 1;
}

// If found label on table returns true else false
int searchLabelTable(vector<string> codeLine ){
    bool found = false;
    if(codeLine.at(0) == "SECTION") return 0;
    if(codeLine.at(0).back() == ':')codeLine.at(0).pop_back();
    
    for(auto i : codeTable.labelTable){
        if(i.name == codeLine.at(0)){
            cout<< "Semantical/Parser Error\n";
            printStringVector(codeLine);
            cout << "Label declared previously at line: " << i.line <<  " Current Line: " << lineCounter <<endl;
            programError = 1;
            found = true;
        } 
    }
    
    return found;
}


////////////////////////////////////////////////////////////////////////////////////

// Scans data label
void parseDataLabel(vector<string> codeLine){
    
    if(codeLine.at(0).back() == ':')codeLine.at(0).pop_back();

    if(codeLine.size() == 1){
        wrongNumberOfArguments(codeLine);
    }

    else if (codeLine.at(1) == "CONST"){
        if(codeLine.size() == 3){
            programCounter++;
            codeTable.labelTable.push_back(Label(codeLine.at(0),  typeConst, stoi(codeLine.at(2)),
            lineCounter, programCounter));
        }
        else{
            wrongNumberOfArguments(codeLine);
        }
    }

    else if(codeLine.at(1) == "SPACE"){
        if(codeLine.size() ==2){
            programCounter++;
            codeTable.labelTable.push_back(Label(codeLine.at(0), typeSpace, 0, lineCounter, programCounter));
        }
        else{
            wrongNumberOfArguments(codeLine);
        }
    }
    else{
        unknownLabelType();
    }
    
}
void parser(vector<string> codeLine){
    if(sectionState == sectionData){
        if(!searchLabelTable(codeLine) && codeLine.front() !="SECTION"){
            if(codeLine.size() >4){
                wrongNumberOfArguments(codeLine);
            }
            else parseDataLabel(codeLine);
            
        }   
    }
    else if (sectionState == sectionText){
      if(classifyLine(codeLine)){

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
    // printLabelTable();

    // printProgram();
    return lineCounter;
}