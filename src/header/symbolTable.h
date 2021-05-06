#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

// Symbols == variables
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;
using std::map;

enum SymbolType {
    typeBegin,
    typeEnd,
    typePublic,
    typeExtern,
    typeConst,
    typeSpace
};

struct Symbol{
    string name;
    SymbolType symbolType;
    int line = 0;
    int programCounter;
    int value = 0;
    Symbol(string name, SymbolType symbolType, int value, int line, int programCounter){
        this->line = line;
        this->programCounter = programCounter;
        this->name= name;
        this->symbolType = symbolType;
        if(this->symbolType == typeConst){
            this->value = value;
        }
    }
    Symbol() {}
};


struct Instruction{
    string simbolicOpcode;
    int opcode = 0;
    int line = 0;
    int programCounter = 0;
    int sizeInWords = 0;
    int numberOfParameters = 0; 
    vector<string> parameters;
    vector<Symbol> linkedParameters;
    
    Instruction(string simbolicOpcode, int opcode, int sizeInWords, int numberOfParameters){
        this->simbolicOpcode = simbolicOpcode;
        this->opcode = opcode;
        this->sizeInWords = sizeInWords;
        this->numberOfParameters = numberOfParameters;  
    }
    Instruction(){}
};
struct Label{
    string name;
    int line;
    Instruction instruction;
    Label(){}

};

struct Directive {
    string simbolicOpcode;
    SymbolType symbolType;
    int value;
    int line;
    int sizeInWords;
    int programCounter;
    Directive(string simbolicOpcode,SymbolType symbolType, int value, int lineCounter, int programCounter) {
        this->simbolicOpcode = simbolicOpcode;
        this->symbolType = symbolType;
        this->value = value;
        this->line = lineCounter;
        this->programCounter = programCounter;
        this->sizeInWords = sizeInWords;
    }
};

// const map<string, Directive> DirectivesTable {
//     {"SPACE", Directive("SPACE",typeSpace, 1, 1)},  
//     {"CONST", Directive("CONST",typeConst, 1, 1)},
//     {"BEGIN", Directive("BEGIN",typeBegin, 0, 1)},
//     {"END",   Directive("END",  typeEnd 0, 1)},  
//     {"PUBLIC",Directive("PUBLIC",typePublic 0, 1)},  
//     {"EXTERN",Directive("EXTERN",typeExtern 0, 1)},  
    
// };

const map<string, Instruction> InstructionsMap{
    // {"ADD", Instruction(simboliocOpcode, opcode, sizeInWords, numberOfParameters)}     
    {"ADD", Instruction("ADD"   , 1, 2, 1)},     
    {"SUB", Instruction("SUB"   , 2, 2, 1)},
    {"MUL", Instruction("MUL"  , 3, 2, 1)},
    {"DIV", Instruction("DIV"   , 4, 2, 1)},
    {"JMP", Instruction("JMP"   , 5, 2, 1)},
    {"JMPN", Instruction("JMPN" , 6, 2, 1)},
    {"JMPP", Instruction("JMPP" , 7, 2, 1)},
    {"JMPZ", Instruction("JMPZ" , 8, 2, 1)},
    {"COPY", Instruction("COPY"   , 9, 3, 2)},
    {"LOAD", Instruction("LOAD"   , 10, 2, 1)},
    {"STORE", Instruction("STORE" , 11, 2, 1)},
    {"INPUT", Instruction("INPUT"       , 12, 2, 1)},
    {"OUTPUT", Instruction("OUTPUT"     , 13, 2, 1)},
    {"STOP", Instruction("STOP", 14, 1, 0)},
};
map<string, Label> LabelMap;
// Use table 

// Definition table
struct CodeTable{
    vector<Instruction> instructionTable;
    vector<Directive> useTable;
    vector<Directive> defTable;
    vector<Symbol> symbolTable;
};

#endif