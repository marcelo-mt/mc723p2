#include<vector>
#include<math.h>

using namespace std;

class cache {
	public:
	int misses, hits;
	int bitsBlocos;

	vector < int > tag;
	vector < bool > valid;

	cache (int, int);		// Construtor == Inicializacao da cache
	cache (int, int, int);	// Construtor set/full associative

	void access (int);
	void atualizaTag(int, int);
	int extraiData(int);
};

