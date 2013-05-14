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
	cache.h
*/
#include <vector>
#include <math.h>
#include <set>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

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

