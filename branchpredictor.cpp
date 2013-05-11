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

#include "branchpredictor.h"

TwoBitPredictor::TwoBitPredictor ()
{
	int i;
	//constructor
	//printf("Constructing Branch Predictor...\n");
	branchesTaken = 0;
	branchesNotTaken = 0;
	correctPredictions = 0;
	missPredictions = 0;

	predictionBPBIndex = 0;
	prediction = FIRST_PREDICTION;
	branchTaken = true;

	branchPredictionBuffer.resize(TABLE_SIZE, NULL);
	//cout << "Table Size: " << branchPredictionBuffer.size() << endl;
	//for (i = 0; i < branchPredictionBuffer.size(); i++)
		//if (branchPredictionBuffer[i] == NULL)
		//	printf("\tbpb[%d] = NULL\n", i);
	//cout << "Constructed!" << endl;
}

TwoBitPredictor::~TwoBitPredictor ()
{
	int i;
	//printf("Destroing Branch Predictor\n");

	for(i = 0; i < TABLE_SIZE; i++)
	{
		if(branchPredictionBuffer[i] != NULL)
		{
			//printf("free bpb[%d] = (%d, %d)\n", i,
			//								    branchPredictionBuffer[i]->saturatingCounter,
			//								    branchPredictionBuffer[i]->count);
			free(branchPredictionBuffer[i]);
		}
	}

}

void TwoBitPredictor::predict(int ac_pc, int npc)
{
	/* get prediction for ac_pc using binary search
	   if id doesnt exist - insert new vector element
	   in order
	   check if branch taken or not (if npc = ac_pc + 4)
	   then not taken
	   compare with prediction
	   compute values
	   update prediction - salva o indice do elemento ateh aki
    */
	/* Getting prediction for branch */ 
	printf("starting prediction %d, %d\n", ac_pc, npc);
	predictionBPBIndex = (ac_pc % TABLE_SIZE);
	cout << "index: " << predictionBPBIndex;
	prediction = getPrediction(predictionBPBIndex);
	cout << " | prediction: " << prediction;
	branchTaken = checkIfBranchTaken(ac_pc, npc);
	/* Computing stats */
	if (branchTaken)
	{
		branchesTaken++;
		cout << " - TAKEN" << endl;
		if ((prediction == WEAK_TAKEN) || (prediction == STRONG_TAKEN))
			correctPredictions++;
		else
			missPredictions++;
	}
	else
	{
		branchesNotTaken++;
		cout << " - NOT TAKEN " << endl;
		if ((prediction == WEAK_N_TAKEN) || (prediction == STRONG_N_TAKEN))
			correctPredictions++;
		else
			missPredictions++;
	}
	//cout << branchTaken << endl;
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
	/* if receive imm (imediato) as parameter, instead
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
