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
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#define STRONG_N_TAKEN		0
#define WEAK_N_TAKEN		1
#define WEAK_TAKEN 			2
#define STRONG_TAKEN 		3
#define FIRST_PREDICTION	WEAK_N_TAKEN

#define TABLE_SIZE			10000000

using namespace std;

typedef struct _branchPredictionBuffer
{
	int saturatingCounter;
	int count;

} BPB;

class TwoBitPredictor {

	public:
		/* Pubilc Data Members (attributes) */ 
		int branchesTaken;
		int branchesNotTaken;
		int correctPredictions;
		int missPredictions; 
		/* Public Member Functions (methods) */
		TwoBitPredictor ();
		~TwoBitPredictor ();
		void predict (int ac_pc, int npc);

	private:
		/* Private Data Members (attributes) */ 
		int predictionBPBIndex;
		int prediction;
		bool branchTaken;

		vector < BPB * > branchPredictionBuffer;
		
		/* Private Member Functions (methods) */
		int getPrediction ( int bpbIndex );
		void insertNewPrediction ( int index, int value );
		bool checkIfBranchTaken ( int ac_pc, int npc );

};
