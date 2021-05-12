/*
Made by Carlos Gabriel Vilas Novas Soares
Most recent code and hopefully bug free at:
https://github.com/gabuvns/assembler-sb
Find the at: github.com/gabuvns/
*/

#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <sstream> 
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
    sectionModule,
    sectionData,
    sectionText
};
string _outputFileName = "mybinfile.obj";
int wasSectionDataReadFirst = 0;
SectionState sectionState = sectionText;
CodeTable codeTable;
int programError = 0;
vector<string> errorList;
vector<vector<string>> currentProgram;
int lineCounter = 0, acumulador = 0, programCounter = 0; 
int  foundEnd = 0,   foundBegin= 0, sectionDataSize = 0, pubExCounter=0;
int sectionTextSize = 0;
int textSizeDifference = 0;



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
    s.erase(std::remove(s.begin(), s.end(), '\t'), s.end());
    ltrim(s);
    rtrim(s);
    
}
////////////////////////////////////////////////////////////////////////////////////

int countWords(std::string x) {
    int wordsTotal = 0;      
    char prev = ' ';
    for(unsigned int i = 0; i < x.size(); i++) {
        if(x[i] != ' ' && prev == ' ') wordsTotal++;
        prev = x[i]; 
    }
    return wordsTotal;
}

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

string  boolVectorToString(vector<int> vetor){
    string aux = "";
    for(auto i : vetor){
        aux+=std::to_string(i);
    } 
    return aux;
}

void printSymbolTable(){
    for(auto i : codeTable.symbolTable) {
        cout << "Symbol ===============\n";
        cout << "Name: " << i.name <<endl;
        cout << "Value: " << i.value <<endl;
        cout << "Line: " << i.line <<endl;
        cout << "ProgramCounter: " << i.programCounter <<endl;
        cout << "SymbolType: " << i.symbolType <<endl;
    }
    cout <<endl;
}

void printDefinitionTable(){
    for(auto i : codeTable.defTable) {
        if(i.symbolType==typePublic){
            cout << "Definition table===============\n";
            cout << "Name: " << i.name <<endl;
            cout << "Line: " << i.line <<endl;            
            cout << "Value: " << i.value <<endl;
            cout << "ProgramCounter: " << i.programCounter << endl;
        }
    }
    cout <<endl;
}

void printUseTable(){
    for(auto i : codeTable.useTable) {
        if(i.symbolType==typeExtern){
            cout << "Use table ===============\n";
            cout << "Name: " << i.name <<endl;
            cout << "ProgramCounter: " << i.programCounter << endl;
            cout << "Line: " << i.line <<endl;            
            cout << "Uses:" <<" ";
            for(auto j : i.uses) cout << j << " ";
            cout << endl;            
        }
    }
    cout <<endl;
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

    cout <<endl;

}
void printLabelMap(){
    for (auto const& [key, val] : LabelMap){
        cout << "Label ===============\n";
        std::cout << "Nome: "<<key<<endl  
                << "Valor: " << val.instruction.simbolicOpcode 
                << std::endl;
    }

    cout <<endl;
}
/////////////////////////////////////////////////////////
// Error sections
void errorWrongNumberOfArguments(vector<string> codeLine){
    cout<< "Syntactical Error\n";
    cout <<"Wrong Number of Arguments at line: " << lineCounter << endl;
    printStringVector(codeLine);
    programError = 1;
    cout << endl;
}

void errorUnknownSymbolType(vector<string> codeLine){
    cout<< "Syntactical Error\n";   
    cout <<"Unknown type of symbol directive at line: " << lineCounter << endl;
    printStringVector(codeLine);
    programError = 1;
    cout << endl;
}

void errorSymbolNotDeclared(string codeLine){
    cout<< "Semantical Error\n";
    cout << "Symbol not declared previously: " << codeLine <<endl;
    programError = 1;
    cout << endl;

}

void errorInvalidChar(vector<string> codeLine, char invalidChar){
    cout << "Scanner/Lexical error.\n"  <<endl;
    cout << "Invalid character: '" << invalidChar << '\''<< endl<< "At line: " << lineCounter <<endl;
    programError = 1;
    cout << endl;

}

void errorInstructionDoesNotExist(vector<string> codeLine){
    cout<< "Syntactical Error\n";
    cout <<"Instruction does not exist at line: " << lineCounter << endl;
    printStringVector(codeLine);
    programError = 1;
    cout << endl;

}


void errorSymbolAlreadyDeclared(vector<string> codeLine, int line){
    cout<< "Semantical/Parser Error\n";
    printStringVector(codeLine);
    cout << "Symbol declared previously at line: " << line <<  " Current Line: " << lineCounter <<endl;
    programError = 1;
    cout << endl;

}

void errorLabelAlreadyDeclared(vector<string> codeLine){
    cout<< "Semantical/Parser Error\n";
    printStringVector(codeLine);
    cout << "Label already declared previously " << "Current Line: " << lineCounter <<endl;
    programError = 1;
    cout << endl;
}

void wrongSymbolDeclaration(vector<string> codeLine){
    cout<< "Syntactical Error\n";
    printStringVector(codeLine);
    cout << "Symbol wasn't declared the correct way:" << "Current Line: " << lineCounter <<endl;
    cout <<"Declare as <<NAME>: <DIRECTIVE>>" <<endl;
    programError = 1;
    cout << endl;
}
// If betype == 0 begin not found, if 1 end not found
void errorBeginNotFound(vector<string> codeLine, int betype){
    string auxStr = betype ? "END" : "BEGIN";
    cout << "Error Module" <<endl;
    printStringVector(codeLine);
    cout << auxStr << " not found. Current Line:" << lineCounter <<endl <<endl;
    programError=1;
}
void errorEndNotFound(int foundbegin){
        cout << "Error Module" <<endl;
    if(foundBegin){
        cout << "END not found. Current Line:" << lineCounter <<endl <<endl;    
    }
    else{
        cout << "BEGIN not found. Current Line:" << lineCounter <<endl <<endl;    

        
    }
    programError=1;
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
        if(sectionState==sectionData) sectionDataSize++;
        else if(sectionState==sectionText) sectionTextSize++;
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
        else if(i=='	') {
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
            programError = 1;
        }
        else currentWord +=i;
        
        indexCounter++;
    }
    if(sectionText == sectionModule){
        if(lineCounter == 2 && codeLine.at(1) == "DATA"){
            wasSectionDataReadFirst = 1;
        }
    }
    else{
        if(lineCounter == 2 && codeLine.at(1) == "DATA"){
            wasSectionDataReadFirst = 1;
        } 
    }
    return codeLine;
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
        for(auto &i : codeTable.useTable){
            if(codeLine.size() > 1){
                if(i.name == codeLine.at(1) && i.symbolType==typeExtern){
                        
                    i.uses.push_back(programCounter-sectionDataSize);
                } 
            }
        }
        return 1;
    }

    else{
        errorInstructionDoesNotExist(codeLine);
        programError = 1;
        return 0;
    }
}

// If found directive return true;
int searchDirectiveTable(vector<string> codeLine){

    return 1;
}
// If does not find label adds to the table
void searchLabelMap(vector<string> codeLine, int isBegin){
    auto searchedLabel = LabelMap.find(codeLine.front());
    // Label not yet found
    if(isBegin){
        Label auxLabel;
        if(codeLine.at(0).back() == ':') codeLine.at(0).pop_back();
        auxLabel.name = codeLine.at(0);
        auxLabel.line = lineCounter;
        auxLabel.programCounter = 0;
        LabelMap.insert({codeLine.at(0), auxLabel});
        
    }
    else if(searchedLabel == LabelMap.end()){
        Label auxLabel;
        if(codeLine.at(0).back() == ':') codeLine.at(0).pop_back();
        auxLabel.name = codeLine.at(0);
        auxLabel.line = lineCounter;
        auxLabel.programCounter = programCounter - sectionDataSize + 1;
        if(auxLabel.programCounter < 0) auxLabel.programCounter = 0;
 
        // Search for instruction
        vector<string> auxVector = codeLine;
        auxVector.erase(auxVector.begin());
        // printStringVector(codeLine);
        auto searchedInstruction = InstructionsMap.find(auxVector.front());
        // If found instruction
        if(searchedInstruction !=  InstructionsMap.end() || codeLine.at(0) == "STOP"){   
            Instruction auxInstruction = searchedInstruction->second;
            auxInstruction.line = lineCounter;
            auxInstruction.programCounter = programCounter;
            auxInstruction.parameters = addParametersToInstruction(auxVector, auxInstruction);
            programCounter+=auxInstruction.sizeInWords;
            codeTable.instructionTable.push_back(auxInstruction);
            auxLabel.instruction = auxInstruction;
            LabelMap.insert({codeLine.at(0), auxLabel});
        }
        // Didn't find instruction
        else{
            errorInstructionDoesNotExist(codeLine);
        }
    }
    //It can look counter-ituitive, but searchedElement can be different (meaning no element found on map) 
    //But filled with memory 
    else if(LabelMap.size() != 1 && searchedLabel != LabelMap.end()){
        errorLabelAlreadyDeclared(codeLine);
    }
    else if(LabelMap.size() == 1 && searchedLabel == LabelMap.end()){
        // errorLabelAlreadyDeclared(codeLine);
    }
}
int classifyLine(vector<string> codeLine){
    
    // Assumes 0 for instruction, 1 for symbol
    int typeOfLine = codeLine.at(0).back() == ':' ? 1 : 0;
    if(codeLine.size() == 1){
        if(codeLine.at(0) == "END") foundEnd = 1;
        if(codeLine.at(0) == "STOP") searchInstructionMap(codeLine);
    }
    else if(typeOfLine == 1){
        searchLabelMap(codeLine,0);
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
        if(i.name == codeLine.at(0) && i.symbolType!=typePublic && i.symbolType!=typeSpace){
            errorSymbolAlreadyDeclared(codeLine, i.line);
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
        wrongSymbolDeclaration(codeLine);
    }

    else if (codeLine.at(1) == "CONST"){
        textSizeDifference++;
        if(codeLine.size() == 3){
            for(auto &i : codeTable.defTable){
                if(i.symbolType == typePublic && codeLine.at(0) == i.name ){
                    i.value = programCounter + 1;
                }
            }
            codeTable.symbolTable.push_back(Symbol(codeLine.at(0),  typeConst, stoi(codeLine.at(2)),
            lineCounter, programCounter));
            programCounter++;
        }
        else{
            errorWrongNumberOfArguments(codeLine);
        }
    }

    else if(codeLine.at(1) == "SPACE"){
        textSizeDifference++;
        if(codeLine.size() ==2){
            // first we have to check if it alredy was declared as a public constant, so that we can
            // update our definition table
            // 10000 is an arbitrary value to differentiate space in obj
            codeTable.symbolTable.push_back(Symbol(codeLine.at(0), typeSpace, 0, lineCounter, programCounter));
            programCounter++;
        }
        else{
            errorWrongNumberOfArguments(codeLine);
        }
    }
    else if(codeLine.at(0) == "PUBLIC"){
            codeTable.defTable.push_back(Symbol(codeLine.at(1),typePublic,0,lineCounter, programCounter));
            programCounter++;
            pubExCounter++;
    }
    else if(codeLine.at(1) == "EXTERN"){
            codeTable.useTable.push_back(Symbol(codeLine.at(0), typeExtern, 0, lineCounter, programCounter));
            programCounter++;
            pubExCounter++;
    }
    else{
        errorUnknownSymbolType(codeLine);
    }
    
}
void parser(vector<string> codeLine){
    if(sectionState == sectionModule){
        if(codeLine.size() == 2){
            if(codeLine.at(1) !="BEGIN"){
                errorBeginNotFound(codeLine, 0);
            }
            else{
                foundBegin = 1;
                searchLabelMap(codeLine, 1);
                
            }
        }
        else{
            errorWrongNumberOfArguments(codeLine);
            errorBeginNotFound(codeLine, 0);
        }
        
    }
    else if(sectionState == sectionData){
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
int firstPassage(ifstream &inFile){
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
                // throw 0;
            }            
        }
        catch (int error){
            cout << "Program ended with error.\n";
            return 0;
        }
        
    }
    // printSymbolTable();
    // printInstructionTable();
    // printLabelMap();
    // printProgram();
    return lineCounter;
}

vector<Symbol> parameterLinking(vector<string> parameters){
    vector<Symbol> auxSymbol;
    for(auto &i : parameters){
        bool foundSymbol =false;
        for(auto &j : codeTable.symbolTable){
            // link sutff in def table
            if(i == j.name){
                for(auto &k : codeTable.defTable){
                    if(k.name == i  && (k.symbolType==typeConst || k.symbolType==typePublic)){
                        k.value = j.programCounter + sectionTextSize + 1;
                    }
                }
                
                foundSymbol = true;
                auxSymbol.push_back(j);
            }
        }
        
        if(!foundSymbol){
            for(auto j : codeTable.useTable){
                if(i == j.name){
                    foundSymbol = true;
                    auxSymbol.push_back(j);
                }
            }
            
            if(!foundSymbol) errorSymbolNotDeclared(i);
            // search useTable
            
        } 
    }
    return auxSymbol;
}

int labelUsedExtern(string paramName){
    for(auto i : codeTable.useTable){
        if(i.name == paramName) return 1;
        
    }
    return 0;
}
int labelLinking(string paramName){
    int pc = 0;
    if(labelUsedExtern(paramName)){
        return 0;
    }
    if(wasSectionDataReadFirst){
        pc -= codeTable.symbolTable.size();
    }
    for(auto const&[nome, valor] : LabelMap){
        if(paramName == nome){
            pc = valor.instruction.programCounter - sectionDataSize;
            pc++;
        }
    }
    
    return pc;
}
vector<int> composeRelocationBits(int totalWords) {
    vector<int> auxBool;
    
    for(auto &j : codeTable.instructionTable){
        auxBool.push_back(0);
        for(int k = 0; k < j.numberOfParameters;k++){
            auxBool.push_back(1);
        }
    }
    int relBitsTextSize = auxBool.size();
    for(int i =0; i < totalWords - relBitsTextSize; i++){
        auxBool.push_back(0);
    }

    return auxBool;
}

void  assembleToObject(string codeName){
    std::stringstream objCodeStr;
    std::ofstream outputFile;
    outputFile.open(_outputFileName);
    // Iterates trough instruction list
    int pcDifference = codeTable.instructionTable.back().programCounter - codeTable.symbolTable.back().programCounter; 
    for(auto &i : codeTable.instructionTable){
        // cout << i.opcode << " ";
        objCodeStr << i.opcode << " ";
        // Gathers Symbol
        // Check if is Jump
        if(i.opcode >= 5 && i.opcode<=8){
            // cout << labelLinking(i.parameters.at(0)) << " ";
            objCodeStr << labelLinking(i.parameters.at(0)) << " ";            
        }
        else{
            if(wasSectionDataReadFirst){
                i.linkedParameters = parameterLinking(i.parameters);
                for(auto &j : i.linkedParameters){
                    if(j.symbolType == typeExtern){
                        objCodeStr <<j.value<<" ";
                    }
                    else{
                        if(j.symbolType==typeConst){
                            objCodeStr <<j.programCounter +sectionTextSize +1<<" ";
                            
                        }
                        else if(j.symbolType==typeSpace){
                            objCodeStr <<j.programCounter +sectionTextSize +1<<" ";
                            
                        }
                        else{
                        objCodeStr <<j.programCounter +sectionTextSize<<endl;
                        }
                        
                    }
                } 
            }
            else{
                i.linkedParameters = parameterLinking(i.parameters);
                
                for(auto &j : i.linkedParameters){
                    // cout << j.programCounter <<" ";
                    objCodeStr << j.programCounter <<" ";
                } 

            }
        }
    }
    // PrintSymbol
    for(auto &i : codeTable.symbolTable){
        if(i.symbolType == typeSpace){
            // cout << 0 << " "; 
            objCodeStr << 0 << " ";    
        
        }
        else if(i.symbolType!=typePublic && i.symbolType!=typeExtern){
            // cout << i.value << " ";
            objCodeStr << i.value << " ";
        }
    } 
    
    // Header prog. name
    string auxStr = objCodeStr.str();
    outputFile << "H: " << codeName <<endl;
    //Header Code Size 
    int totalWords = countWords(auxStr);    
    
    outputFile << "H: " << totalWords <<endl;
    // Header relocation bits
    vector<int> relocationBits= composeRelocationBits(totalWords);
    string relocationBitsString = boolVectorToString(relocationBits);
    outputFile << "R: " << relocationBitsString <<endl;
    
    
    // Add definition table to header, which is an indicator of which line our public variables are declared
    for(auto i : codeTable.defTable){
        if(i.symbolType == typePublic){
            int printedAlredy =0;
            // value here is a pointer to the line in which it is declared
            outputFile << "D: ";
            for (auto const& [key, val] : LabelMap){
                if(val.name == i.name) {
                    outputFile << i.name << " " << val.programCounter  << endl;
                    printedAlredy=1;
                    break;                
                }
            }
            if(!printedAlredy){
                outputFile << i.name << " " <<  i.value-1 <<" " << endl;
            }
        }
    }
    
    // We now must add our use table, which is an indicator of where extern variables are used 
    for(auto i : codeTable.useTable){
        if(i.symbolType == typeExtern){
            // value here is a pointer to the line in which it is declared
            for(auto j : i.uses){
                outputFile << "U: ";
                outputFile << i.name << " " <<  j <<"+ " <<endl;
                
            }
        }
    }
    
    outputFile << "T: " << objCodeStr.str() <<endl; 
    // Header data  
    outputFile.close();

}

// Clears the memory for every program
void resetMemory(){
    wasSectionDataReadFirst = 0;
    sectionState = sectionText;
    
    codeTable.instructionTable.clear();
    codeTable.instructionTable.shrink_to_fit();
    
    codeTable.symbolTable.clear();
    codeTable.symbolTable.shrink_to_fit();
    
    codeTable.useTable.clear();
    codeTable.useTable.shrink_to_fit();

    codeTable.defTable.clear();  
    codeTable.defTable.shrink_to_fit();
    
    programError = 0;
    
    errorList.clear();
    errorList.shrink_to_fit();
    
    currentProgram.clear();
    currentProgram.shrink_to_fit();
    sectionDataSize = 0;
    sectionTextSize = 0;
    lineCounter = 0;
    acumulador = 0; 
    textSizeDifference = 0;
    programCounter = 0; 
    
    LabelMap.clear();
    
}

void analyzeCode(ifstream &inFile, string outputFileName, int argNum){
    string rawFileName = outputFileName.substr(0, outputFileName.find(".asm"));
    if(!outputFileName.empty()){
        _outputFileName = rawFileName + ".obj"; 
    }
    sectionState = sectionModule;   
    
    firstPassage(inFile);
    if(foundBegin == 0 || foundEnd == 0 ) errorEndNotFound(foundBegin);
    assembleToObject(rawFileName);
    if(!programError){
        cout << "\nWritten to file: "<< _outputFileName <<endl;
    }
    else{
        cout << "Program ended with errors\nCreated obj file contains errors and should not be used\n";
    }
    // printUseTable();
    // printDefinitionTable();
    // printInstructionTable();
    resetMemory();
} 
