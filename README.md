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
## Output
Object file is outputed as follows:
For the input code:
```
MOD_A: BEGIN
SECTION DATA
    MOD_B: EXTERN
	PUBLIC FAT
	public ONE
	PUBLIC N
	N: SPACE
	ONE: CONST 1
SECTION TEXT
	INPUT N
	LOAD N
	FAT: SUB ONE
	JMPZ FIM
	JMP MOD_B
	FIM: OUTPUT N
	STOP
END
```
Obj file output:
```
H: codeName
H: 14 (codeSize)
R: 00000000010000 (RelocationBits)
D: FAT 4+ (Label position)
D: ONE 1  (data + value in the case of CONST)
D: N 100001 (If SPACE then 10000 + dataPosition in output text)
U: MOD_B 9+ (Use of extern variables)
T: 12 13 10 13 2 14 8 14 5 0 13 13 0 1 

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
## Output
Objeto como saida:
Para o seguinte código:
```
MOD_A: BEGIN
SECTION DATA
    MOD_B: EXTERN
	PUBLIC FAT
	public ONE
	PUBLIC N
	N: SPACE
	ONE: CONST 1
SECTION TEXT
	INPUT N
	LOAD N
	FAT: SUB ONE
	JMPZ FIM
	JMP MOD_B
	FIM: OUTPUT N
	STOP
END
```
Obj file output:
```
H: codeName
H: 14 (tamanho do código)
R: 00000000010000 (Bits de realocação)
D: FAT 4+ (Posição da label)
D: ONE 1  (dado + valor no caso de variaveis const)
D: N 100001 (caso space then 10000 + dataPosition in output text)
U: MOD_B 9+ (Use of extern variables)
T: 12 13 10 13 2 14 8 14 5 0 13 13 0 1 

```
## Author

* **Carlos Gabriel Vilas Novas Soares** - [Github](https://github.com/gabuvns)
