#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

// Symbols == variables
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;
using std::map;

enum LabelType {
    typeConst,
    typeSpace
};
struct Label{
    string name;
    LabelType labelType;
    int line = 0;
    int programCounter;
    int value = 0;
    Label(string name, LabelType labelType, int value, int line, int programCounter){
        this->line = line;
        this->programCounter = programCounter;
        this->name= name;
        this->labelType = labelType;
        if(this->labelType == typeSpace){
            this->value = value;
        }
    }
    Label() {}
};
struct Parameter{
    Label *label;
    int value = 0;
    Parameter() {}
};

struct Instruction{
    string simbolicOpcode;
    int line = 0;
    int programCounter = 0;
    int opcode = 0;
    int sizeInWords = 0;
    int numberOfParameters = 0; 
    vector<string> parameters;
    
    Instruction(string simbolicOpcode, int opcode, int sizeInWords, int numberOfParameters){
        this->simbolicOpcode = simbolicOpcode;
        this->opcode = opcode;
        this->sizeInWords = sizeInWords;
        this->numberOfParameters = numberOfParameters;  
    }
};
struct Directive {
    string simbolicOpcode;
    int value;
    int line;
    int sizeInWords;
    int programCounter;
    Directive(string simbolicOpcode, int value, int sizeInWords) {
        this->simbolicOpcode = simbolicOpcode;
        this->value = value;
        this->sizeInWords = sizeInWords;
    }
};

const map<string, Directive> DirectivesTable {
    {"SPACE", Directive("SPACE", 1, 1)},  
    {"CONST", Directive("CONST", 1, 1)}   
};

const map<string, Instruction> InstructionsMap{
    // {"ADD", Instruction(simboliocOpcode, opcode, sizeInWords, numberOfParameters)}     
    {"ADD", Instruction("ADD"   , 1, 2, 1)},     
    {"SUB", Instruction("SUB"   , 2, 2, 1)},
    {"MUL", Instruction("MULT"  , 3, 2, 1)},
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

map<string, Label> labelTable;

struct CodeTable{
    vector<Instruction> instructionTable;
    // vector<Directive> dataTable;
    vector<Label> labelTable;
};

#endif