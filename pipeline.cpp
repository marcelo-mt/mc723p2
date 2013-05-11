#include <pipeline.h>

// Construtor sem instrucao
fase::fase () {
	this->init = false;
	this->regWrite = false;
	this->memRead = false;
	this->branch = false;
}

// Construtor com instrucao
fase::fase (int op, int ac_pc) {
	this->inst = op;
	this->acpc = ac_pc;
	this->branch = false;
	
	// Instrucoes de load
	if (	(op < 8 || op == 20) ||
		(op >= 27 && op <= 30)		// logico com regs
	   ) {
		this->regWrite = true;
		this->memRead = true;
//		cout << "regWrite e memRead!" << endl;
	}
	//
	// Instrucoes de store
	if (op >= 8 && op <= 12) {
//		this->memRead = true;
//		cout << "memWrite!" << endl;
	}

	// Instrucoes de aritmetica e logica
	if ( (op >= 13 && op <= 14) || (op >= 21 && op <= 24) ||	// Add
		(op >= 17 && op <= 19) ||	// Logicos
		(op >= 38 && op <= 45) || (op >= 32 && op <= 37)	// Mult/Div e Shifts
	   ) {
		this->regWrite = true;
//		cout << "memRead!" << endl;
	}


	// Instrucoes de branch
	if ( (op >= 50 && op <= 57) 		// Branch
//		|| (op >= 46 && op <= 49)		// Jump
	   ) {
		this->branch = true;
//		cout << "BRANCH!!!" << endl;
	}
}

void fase::setBranch () {
	this->branch = true;
//	cout << "branch!! " << this->inst << endl;
}
