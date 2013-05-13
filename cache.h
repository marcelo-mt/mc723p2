#include<vector>
#include<math.h>
#include<set>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>

using namespace std;

class cache {
	public:
	int misses, hits;
	int bitsLinhas;
	int ways;				// numero de ways
	int words;

	vector < int > tag;
	vector < set < int > > assoc4;
	vector < bool > valid;

	cache (int, int);		// Construtor == Inicializacao da cache
	cache (int, int, int, int);	// Construtor set/full associative

	void access (int);
	void atualizaTag(int, int);
	int extraiData(int);
	void detalhes (int);
};

