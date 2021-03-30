assembler: ./src/main.cpp  ./src/assembler.cpp
	g++ -o assembler ./src/main.cpp ./src/assembler.cpp -std=gnu++1z