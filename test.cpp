#include "branchpredictor.h"

int main()
{
	int i;
	TwoBitPredictor tbp;

	for(i=0; i < TABLE_SIZE * 3; i++)
	{
		tbp.predict(i, i + 40);
	}

	cout << "Total Branches: " << tbp.branchesTaken + tbp.branchesNotTaken << endl;
	cout << "  Taken: " << tbp.branchesTaken << endl;
	cout << "  Not Taken: " << tbp.branchesNotTaken << endl;
	cout << "  Correct Predictions: " << tbp.correctPredictions << endl;
	cout << "  Miss Predictions: " << tbp.missPredictions << endl;

	return 0;
}