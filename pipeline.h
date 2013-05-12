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
		int acpc;

		void setBranch ();
};
