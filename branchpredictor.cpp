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
	branchpredictor.cpp
*/

#include <branchpredictor.h>

TwoBitPredictor::TwoBitPredictor ()
{
	// Constructor
	branchesTaken = 0;
	branchesNotTaken = 0;
	correctPredictions = 0;
	missPredictions = 0;

	predictionBPBIndex = 0;
	prediction = FIRST_PREDICTION;
	branchTaken = true;

	branchPredictionBuffer.resize(TABLE_SIZE, NULL);
}

TwoBitPredictor::~TwoBitPredictor ()
{
	int i;

	for(i = 0; i < TABLE_SIZE; i++)
	{
		if(branchPredictionBuffer[i] != NULL)
		{
			free(branchPredictionBuffer[i]);
		}
	}

}

void TwoBitPredictor::predict(int ac_pc, int npc)
{
	/* Get prediction for ac_pc
	   If it doesnt exist - insert new vector element
	   Check if branch taken or not (if npc = ac_pc + 4)
	   Compare with prediction
	   Compute values
	   Update prediction - salva o indice do elemento ateh aki
    */
	
	/* Getting prediction for branch */ 
	predictionBPBIndex = (ac_pc % TABLE_SIZE);
	prediction = getPrediction(predictionBPBIndex);
	branchTaken = checkIfBranchTaken(ac_pc, npc);
	
	/* Computing stats */
	if (branchTaken)
	{
		branchesTaken++;
		if ((prediction == WEAK_TAKEN) || (prediction == STRONG_TAKEN))
			correctPredictions++;
		else
			missPredictions++;
	}
	else
	{
		branchesNotTaken++;
		if ((prediction == WEAK_N_TAKEN) || (prediction == STRONG_N_TAKEN))
			correctPredictions++;
		else
			missPredictions++;
	}

	/* Updating prediction buffer */
	if (branchTaken)
	{
		if (prediction != STRONG_TAKEN)
			branchPredictionBuffer[predictionBPBIndex]->saturatingCounter++;
	}
	else
	{
		if (prediction != STRONG_N_TAKEN)
			branchPredictionBuffer[predictionBPBIndex]->saturatingCounter--;
	}
}

int TwoBitPredictor::getPrediction ( int bpbIndex )
{
	/* TODO: if receive imm (imediato) as parameter, instead
	   of a always/never taken policy, check if branch
	   is a backwards one and set to taken
	   or a forward one and set to not taken
	*/
	if (branchPredictionBuffer[bpbIndex] == NULL)
	{
		insertNewPrediction( bpbIndex, FIRST_PREDICTION );
		return FIRST_PREDICTION;
	}
	else
	{
		branchPredictionBuffer[bpbIndex]->count++;
		return branchPredictionBuffer[bpbIndex]->saturatingCounter;	
	}
}

void TwoBitPredictor::insertNewPrediction ( int index, int value )
{
	BPB * newBufferEntry = (BPB *)malloc(sizeof(BPB));

	newBufferEntry->saturatingCounter = FIRST_PREDICTION;
	newBufferEntry->count = 1;

	branchPredictionBuffer[index] = newBufferEntry;
}
	
bool TwoBitPredictor::checkIfBranchTaken ( int ac_pc, int npc )
{
	if (npc == ac_pc + 4)
		return false;
	else
		return true;
}
