#include <pipeline.h>

// Construtor sem instrucao
fase::fase () {
	this->init = false;
	this->regWrite = false;
	this->memRead = false;
}

// Construtor com instrucao
fase::fase (int op) {
	cout << op;
	// Instrucoes de load
	if (op < 8 || op == 20) {
		this->regWrite = true;
		this->memRead = true;
		cout << "regWrite!" << endl;
	}

	// Instrucoes de store
	if (op >= 8 && op <= 12) {
		this->memRead = true;
		cout << "memRead!" << endl;
	}

	// Instrucoes de aritmetica e logica
	if ( (op >= 13 && op <= 14) || (op >= 21 && op <= 24) ||	// Add
		(op >= 17 && op <= 19) || (op >= 27 && op <= 30) ||	// Logicos
		(op >= 38 && op <= 45) || (op >= 32 && op <= 37)	// Mult/Div e Shifts
	   ) {
		this->regWrite = true;
		cout << "memRead!" << endl;
	}

	// Instrucoes de store
	if (op >= 8 && op <= 12) {
		this->memRead = true;
		cout << "memRead!" << endl;
	}
}
