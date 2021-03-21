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

// Print stiff sections
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

void printSymbolTable(){
    for(auto i : codeTable.symbolTable) {
        cout << "Symbol ===============\n";
        cout << "Name: " << i.name <<endl;
        cout << "Value: " << i.value <<endl;
        cout << "Line: " << i.line <<endl;
        cout << "SymbolType: " << i.symbolType <<endl;

    }
}
void printInstructionTable(){
    for(auto i : codeTable.instructionTable) {
        cout << "Instruction ===============\n";
        cout << "Name: " <<  i.simbolicOpcode<<endl;
        cout << "Opcode: " << i.opcode<<endl;
        cout << "Line: " << i.line <<endl;
        cout << "ProgramCounter: " <<i.programCounter <<endl;
        cout << "Parameters: " << i.numberOfParameters <<endl;
        for(auto j : i.parameters){
            cout << "Symbol param:" << j << endl;
        }        

    }
}
/////////////////////////////////////////////////////////
// Error sections
void errorWrongNumberOfArguments(vector<string> codeLine){
    cout<< "Syntactical Error\n";
    cout <<"Wrong Number of Arguments at line: " << lineCounter << endl;
    printStringVector(codeLine);
    programError = 1;
}

void errorUnknownSymbolType(vector<string> codeLine){
    cout<< "Syntactical Error\n";
    cout <<"Unknown type of symbol at line: " << lineCounter << endl;
    printStringVector(codeLine);
    programError = 1;
}

void errorSymbolNotDeclared(vector<string> codeLine){
    cout<< "Syntactical Error\n";
    cout <<"Symbol Not Declared: " << lineCounter << endl;
    printStringVector(codeLine);
    programError = 1;
}

void errorInvalidChar(vector<string> codeLine, char invalidChar){
    cout << "Scanner/Lexical error.\n"  <<endl;
    cout << "Invalid character: '" << invalidChar << '\''<< endl<< "At line: " << lineCounter <<endl;
    programError = 1;
}

void errorInstructionNotExist(vector<string> codeLine){
    cout<< "Syntactical Error\n";
    cout <<"Unknown type of symbol at line: " << lineCounter << endl;
    printStringVector(codeLine);
    programError = 1;
}

void errorSymbolAlreadyDeclared(vector<string> codeLine, string line){
    cout<< "Semantical/Parser Error\n";
    printStringVector(codeLine);
    cout << "Symbol declared previously at line: " << line <<  " Current Line: " << lineCounter <<endl;
    programError = 1;
}

void errorSymbolAlredyDeclared(vector<string> codeLine, Symbol searchedSymbol){
    cout<< "Semantical/Parser Error\n";
    printStringVector(codeLine);
    cout << "Symbol declared previously at line: " << searchedSymbol.name <<  " Current Line: " << lineCounter <<endl;
    programError = 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

Symbol * linkParameter(string paramName){
    for(auto &i : codeTable.symbolTable){
        if(i.name == paramName){
            cout << "LINK PARAM: " << i.name <<endl;
            return &(i);
        }
    }

}

vector<string> parseCopyParameters(vector<string> codeLine){
    vector<string> auxVector;
    string parameters = codeLine.at(1);
    string paramName;
    int index = 0;
    for(auto &i : parameters){
        if(i==',' ){
            auxVector.push_back(paramName);
            paramName.clear();
        }
        else if(i == parameters.back() && index == parameters.size()-1){
            paramName+=i;
            auxVector.push_back(paramName);
            paramName.clear();
        }
        else{
            paramName+=i;
        }
        index++;
    }
   
    if(auxVector.size() == 1){
        errorWrongNumberOfArguments(codeLine);
    }
    // Checks for more than two arguments
    else if((auxVector.at(0).size() + auxVector.at(1).size()+1) != parameters.size()){
        // Checks for copy a,b,
        if(parameters.back() == ','){
            errorInvalidChar(codeLine, ',');

        }
        else{
            errorWrongNumberOfArguments(codeLine);
        }
    }
    else if(auxVector.at(0)==auxVector.at(1)){
        cout<<"erro3\n";
        errorWrongNumberOfArguments(codeLine);
    }


    return auxVector;
}

vector<string> addParametersToInstruction(vector<string> codeLine, Instruction auxInstruction){
    vector<string> auxVector;

    if(auxInstruction.simbolicOpcode =="COPY"){
        // Verifica se argumento errado ex: COPY OLD_DATA,NEWDATA ABC
        if(codeLine.size() != 2){
            errorWrongNumberOfArguments(codeLine);
        }
        else{
            auxVector = parseCopyParameters(codeLine);             
            return auxVector; 
        }
    }
    else if(auxInstruction.simbolicOpcode =="STOP"){
        // handleStop
    }
    else if(auxInstruction.numberOfParameters == (codeLine.size() - 1)){
        string auxParameter;
        auxParameter = codeLine.at(1);
        auxVector.push_back(auxParameter);
    }
    else{
        errorWrongNumberOfArguments(codeLine);
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
        cout <<"Instruction not found\n" << lineCounter;
        // errorUnknownSymbolType
        programError = 1;
        return 0;
    }
}

// If found directive return true;
int searchDirectiveTable(vector<string> codeLine){

    return 1;
}
// If does not find label adds to the table
void searchLabelMap(vector<string> codeLine){
    auto searchedLabel = LabelMap.find(codeLine.front());
    if(searchedLabel == LabelMap.end()){
        Label auxLabel;
        if(codeLine.at(0).back() == ':') codeLine.at(0).pop_back();
        auxLabel.name = codeLine.at(0);
        auxLabel.line = lineCounter;

        // auxLabel.instruction;
        programCounter+=2;
        
        // LabelMap.insert(codeLine.at(0), )
    }
    else{
        cout <<"Label found, throw error" << endl;
        cout << "line counter: " << searchedLabel->second.name <<endl;
    }
}
int classifyLine(vector<string> codeLine){
    // Assumes 0 for instruction, 1 for symbol
    int typeOfLine = codeLine.at(0).back() == ':' ? 1 : 0;
    
    if(typeOfLine == 1){
        // searchLabelMap(codeLine);
    }
    else if(typeOfLine == 0){
        searchInstructionMap(codeLine);
    }
    else if(searchDirectiveTable(codeLine)){

    }
    else{
        // Error symbol not found
    }
    
    return 1;
}

// If found symbol on table returns true else false
int searchSymbolTable(vector<string> codeLine ){
    bool found = false;
    if(codeLine.at(0) == "SECTION") return 0;
    if(codeLine.at(0).back() == ':')codeLine.at(0).pop_back();
    
    for(auto i : codeTable.symbolTable){
        if(i.name == codeLine.at(0)){
            errorSymbolAlreadyDeclared(codeLine, i.name);
            found = true;
        } 
    }
    
    return found;
}


////////////////////////////////////////////////////////////////////////////////////

// Scans data symbol
void parseDataSymbol(vector<string> codeLine){
    
    if(codeLine.at(0).back() == ':')codeLine.at(0).pop_back();

    if(codeLine.size() == 1){
        errorWrongNumberOfArguments(codeLine);
    }

    else if (codeLine.at(1) == "CONST"){
        if(codeLine.size() == 3){
            programCounter++;
            codeTable.symbolTable.push_back(Symbol(codeLine.at(0),  typeConst, stoi(codeLine.at(2)),
            lineCounter, programCounter));
        }
        else{
            errorWrongNumberOfArguments(codeLine);
        }
    }

    else if(codeLine.at(1) == "SPACE"){
        if(codeLine.size() ==2){
            programCounter++;
            codeTable.symbolTable.push_back(Symbol(codeLine.at(0), typeSpace, 0, lineCounter, programCounter));
        }
        else{
            errorWrongNumberOfArguments(codeLine);
        }
    }
    else{
        errorUnknownSymbolType(codeLine);
    }
    
}
void parser(vector<string> codeLine){
    if(sectionState == sectionData){
        if(!searchSymbolTable(codeLine) && codeLine.front() !="SECTION"){
            if(codeLine.size() >4){
                errorWrongNumberOfArguments(codeLine);
            }
            else parseDataSymbol(codeLine);
            
        }   
    }
    else if (sectionState == sectionText){
      if(codeLine.front() != "SECTION"){
            classifyLine(codeLine);
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
    // printSymbolTable();
    printInstructionTable();
    // printProgram();
    return lineCounter;
}