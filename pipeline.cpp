/*
  	MC723 - 2013s1 - 	Laboratorio de Projeto
                            	de
    	               Sistemas Computacionais
	Grupo 16:
  	
  	73805	Victor de Oliveira Carmona
	94139	Marcelo Mingatos de Toledo
	105008	Guilherme Alcarde Gallo

	Prof. Rodolfo

	P2: Modelagem de processador em alto nivel
	14/05/2013
	pipeline.cpp
*/

#include <pipeline.h>

// Construtor sem instrucao
fase::fase () {
	this->init = false;
	this->regWrite = false;
	this->memRead = false;
	this->branch = false;
	this->half = false;
	this->mem = false;
}

// Construtor com instrucao
fase::fase (int op, int ac_pc) {
	this->inst = op;
	this->acpc = ac_pc;
	this->branch = false;
	
	// Instrucoes de load
	if ((op < 8 || op == 20) || 
		(op >= 27 && op <= 30) // logico com regs
		){
		this->mem = true;
		this->regWrite = true;
		this->memRead = true;
		
		// Instrucoes desalinhadas
		int halfOps[] = {3,4,9};
		set < int > hOps (halfOps,halfOps+3);
		// Verificando se e desalinhada ou nao
		if (hOps.find(op)!=hOps.end()) {
			this->half = true;
		}
	}

	// Instrucoes de store
	if (op >= 8 && op <= 12) {
		this->mem = true;
	}

	// Instrucao Set on Less Than
	if (op == 15 || op == 16 || op == 25 || op == 26) {
		this->regWrite = true;
	}
	
	// Instrucoes de aritmetica e logica
	if ( (op >= 13 && op <= 14) || (op >= 21 && op <= 24) ||	// Add
		(op >= 17 && op <= 19) ||	// Logicos
		(op >= 38 && op <= 45) || (op >= 32 && op <= 37)	// Mult/Div e Shifts
	   ) {
		this->regWrite = true;
	}

	// Instrucoes de branch
	if (op >= 50 && op <= 57) {
		this->branch = true;
	}


	// Instrucoes de Jump
	if  (op >= 46 && op <= 49) {
		this->mem = true;		// Jump
	}
}

void fase::setBranch () {
	this->branch = true;
//	cout << "branch!! " << this->inst << endl;
}
