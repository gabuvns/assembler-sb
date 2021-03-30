# Custom Assembler 
Assembler created for "Basic Software" class at University of Brasilia.
There is also a simulator avaiable at [github](https://github.com/gabuvns/simulator-sb)

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
This program takes the name of the file to be assembled as an argument and optionally the name of the outputfile. If no output file name is defined will use default: "mybinfile.obj"

```
./assembler <inputFileName> <outputFileName>
./assembler inputFile.asm outputFile.obj
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
O programa recebe dois argumentos: o nome do arquivo a ser montado, e o nome do arquivo de saida. Caso nenhum nome de arquivo seja fornecido, o nome padrão será usado: "mybinfile.obj"
```
./assembler <inputFileName> <outputFileName>
./assembler inputFile.asm outputFile.obj
```

## Author

* **Carlos Gabriel Vilas Novas Soares** - [Github](https://github.com/gabuvns)
