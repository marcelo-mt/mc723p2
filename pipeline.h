#include <iostream>

using namespace std;

class fase {
	public:
		fase ();
		fase (int);
		bool init;
		bool regWrite;
		bool memRead;
		int inst;
		int rs, rt, rd;
		int imm;
		int addr;

};
