# Custom Assembler 
Assembler created for "Basic Software" class at University of Brasilia.
There is also a runtime simulator avaiable at [github](https://github.com/gabuvns/simulator-sb) so you can execute the code generated

## Getting Started
This assembler supports the instructions in the following table.
![List of instructions](./assets/assemblyInstructionTable.png)

## Prerequisites
Pretty simple, you only need: 

```
g++ (GCC)
gnu make (opt)
```
(Your version of g++ must support c++17, if your are using gcc 5.0 or newer, you should be good)

## Compiling

#### First option is to simply use make

```
make 
```

#### Or you can compile manually:

```
g++ -o assembler ./src/main.cpp ./src/assembler.cpp -std=gnu++1z
```



## Usage 
This program assembles the files which have been used as input. It is required at least one file.

```
./assembler <inputFileName> [OPT]<inputFileName2> ... [OPT]<inputFileNameN>
./assembler inputFile.asm inputFileName2 ... inputFileNameN
```
## Começando :brazil:
Este montador suporta as instruções disponíveis na tabela disponível no início do README.

## Pré-requisitos
Precisamos apenas de:

```
g++ (GCC)
gnu make (opt)
```
(A versão do GCC deve ter suporte para c++17, se sua versão é superior ao gcc 5.0, fique tranquilo.)

## Compilando

#### A primeira opção é simplesmente utilizar o comando make

```
make 
```

#### Ou então pode-se compilar manualmente

```
g++ -o assembler ./src/main.cpp ./src/assembler.cpp -std=gnu++1z
```

## Uso 
O programa realiza a montagem dos programas inseridos como input, sendo necessário pelo menos um arquivo.

```
./assembler <inputFileName> [OPT]<inputFileName2> ... [OPT]<inputFileNameN>
./assembler inputFile.asm inputFileName2 ... inputFileNameN
```

## Author

* **Carlos Gabriel Vilas Novas Soares** - [Github](https://github.com/gabuvns)
