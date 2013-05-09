#include <iostream>

#define STRONG_N_TAKEN		0
#define WEAK_N_TAKEN		1
#define WEAK_TAKEN 			2
#define STRONG_TAKEN 		3

using namespace std;

typedef struct _branch_prediction_buffer
{
	long int address;
	int saturating_counter = WEAK_TAKEN;

} BPB;

class TwoBitPredictor {
	
	vector < BPB > branch_prediction_buffer;
	
	dint last_prediction_index = -1;


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
