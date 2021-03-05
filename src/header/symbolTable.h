#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

// Symbols == variables
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

struct Instruction{
    std::string simbolicOpcode;
    int opcode;
    int sizeInWords;
    // deve ser mudado para o tipo
    std::vector<int> arguments;
    
};
struct Directive {
    std::string simbolicOpcode;
    int opcode;
    int sizeInWords;
};
#endif