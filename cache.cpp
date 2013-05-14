#include<cache.h>

cache::cache (int size, int n, int wd, int w) {
	hits = 0;
	misses = 0;
	tag.resize(1 << n);
	valid.resize(1 << n, false);	// Blocks nao inicializados
	//cout << "vai" << endl;
	set < int > s;
	assoc4.resize(1 << n);
	bitsLinhas = n;
	ways = w;
	words = wd;
}

int cache::extraiData (int addr) {
	//return ((1 << bitsLinhas) - 1) & addr;	// valor fora da tag
	return addr;	// valor fora da tag
}

void cache::atualizaTag (int t, int addr) {
	misses++;
	//cout << "!!!!MISS!!!!" << endl;
	//cout << "Tamanho do set[" << t << "]: " << assoc4[t].size() << endl;
	if(assoc4[t].size() >= (1 << ways) ) {
		srand(time(NULL));
		int tmp = rand()%(1<<ways);
//		cout << "Candidato a ser removido " << tmp << " found: " << *(assoc4[t].lower_bound(tmp)) << endl;
		//assoc4[t].erase(assoc4[t].find(tmp));
		set <int>::iterator it = assoc4[t].begin();
		for (int aux=0; aux < tmp; aux++)
			it++;
		assoc4[t].erase(it);
		//assoc4[t].erase(assoc4[t].begin());
		assoc4[t].insert(extraiData(addr));
		//cout << " del " << *it << endl;
	}
	else {
		assoc4[t].insert(extraiData(addr));
		//cout << " ins " << t << ": " << extraiData(addr) << endl;
	}
	/*
	*/
	tag[t] = extraiData(addr);
}

void cache::access(int addr) {
	//cout << endl << "----------" << endl;
	//cout << "Bits de linhas " << bitsLinhas << " #words = " << words << " ways " << (1 << ways) << " nBlocos= " << (1 << bitsLinhas) << endl;
	//cout << "Tentando accessar end: " << hex << uppercase << addr << "..." << endl;

	int mask = 1 << bitsLinhas;
	mask--;
	//cout << "Mascara de bits: " << mask << endl;

	/*
	   mask ^= t;
	   t &= mask;
	   //cout << "Tag0: " << t << endl;
	   */
	addr &= 0xFFFFFFFC;	// Alinhando endereco
	//cout << "Alinhamento: " << addr << endl;

	int desc = bitsLinhas + ways;
	int t = (addr >> desc);
	//cout << "Tag: " << t << endl;

	int mod = 1 << bitsLinhas;
	int index = t % mod;
	index = addr >> ways;
	index &= mod-1;

	//cout << "Indice: " << index << endl;

	if (!valid[index]) {
		//cout << "Not initialized..." << endl;
		valid[index] = true;
		atualizaTag(index, addr);		// Incrementa miss tambem
		//cout << dec;
		return;
	}

		//cout << "initialized..." << endl;
	/*
	   if (tag[t]==extraiData(addr)) {
	   hits++;
	   }
	   */
	if (assoc4[index].find(extraiData(addr))!=assoc4[index].end()) {
		hits++;
	}
	else {
		atualizaTag(index, addr);
	}
	//cout << dec;
}

void cache::detalhes (int option) {
	FILE *fp;
	fp = fopen("logCache","a");
	float hitRatio = (float)hits/(hits+misses);
	int tam = (1+32*(1 << ways)+(32-bitsLinhas))*(1 << bitsLinhas)/(8192);
//	int t = tam ? tam : (1+32*(1 << ways)+(32-bitsLinhas))*(1 << bitsLinhas);
	fprintf(fp, "%d, %d, %d, %d, %.2f%%, %d, %d\n", option, (1 << ways), (1 << bitsLinhas), tam, 100*hitRatio, hits, misses);
	fclose(fp);

	/*
	cout << "CACHE------------------";
	if (option)
		cout << " Memoria de Dados ---------------" << endl;
	else
		cout << " Memoria de Instrucoes ---------------" << endl;
	cout << "Cache de " << (1 << ways) << " ways e " << (1 << bitsLinhas) << " linhas." << endl;
	cout << "Tamanho: " << (1+32*(1 << ways)+(32-bitsLinhas))*(1 << bitsLinhas)/(8192) << "KB" << endl;
	cout << "Misses: " << misses << " Hits: " << hits << endl;
	cout << "Hit Ratio: " << 100*( (float)hits/(hits+misses) ) << "%" << endl;
	cout << endl;
	*/
}
