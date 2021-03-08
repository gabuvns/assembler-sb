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
    int value = 0;
    Label(string name, LabelType labelType, int value){
        this->name= name;
        this->labelType = labelType;
        if(this->labelType == typeSpace){
            this->value = value;
        }
    }
};
struct Parameter{
    string label;
    int value;
    Parameter(string label, int value){
        this->label = label;
        this->value = value;
    }
};

struct Instruction{
    string simbolicOpcode;
    int opcode;
    int sizeInWords;
    int numberOfParameters;
    vector<Parameter> parameters;
    
    Instruction(string simbolicOpcode, int opcode, int sizeInWords, int numberOfParametes, vector<Parameter> parameters){
        this->simbolicOpcode = simbolicOpcode;
        this->opcode = opcode;
        this->sizeInWords = sizeInWords;
        this->numberOfParameters = numberOfParameters;  
        this->parameters = parameters;

    }
};
struct Directive {
    string simbolicOpcode;
    int opcode;
    int sizeInWords;
    Directive(string simbolicOpcode, int opcode, int sizeInWords) {
        this->simbolicOpcode = simbolicOpcode;
        this->opcode = opcode;
        this->sizeInWords = sizeInWords;
    }
};

map<string, Label> labelTable;
#endif