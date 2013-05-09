#include <iostream>

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
		int inst;
		int rs, rt, rd;
		int imm;
		int addr;
		int acpc;

		void setBranch ();
};
