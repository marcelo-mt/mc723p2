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
	branchpredictor.h
*/

#include <iostream>
#include <set>

using namespace std;

class fase {
	public:
		// Construtores
		fase ();	// Inicializacao do pipeline
		fase (int,int);	// Fase valida

		bool init;
		bool regWrite;
		bool memRead;
		bool branch;
		bool half;
		bool mem; 		// Para a cache
		int inst;
		int rs, rt, rd;
		int imm;
		int addr;
		int memAddr;		// Para a cache
		int acpc;

		void setBranch ();
};
