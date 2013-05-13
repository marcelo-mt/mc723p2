#include<cache.h>

cache::cache (int size, int n, int wd, int w) {
	hits = 0;
	misses = 0;
	tag.resize(1 << n);
	valid.resize(1 << n, false);	// Blocks nao inicializados
	set < int > s;
	assoc4.resize(1 << n);
	bitsLinhas = n;
	ways = w;
	words = wd;
}

int cache::extraiData (int addr) {
	return addr;	// valor fora da tag
}

void cache::atualizaTag (int t, int addr) {
	misses++;
	if(assoc4[t].size() >= (1 << ways) ) {
		srand(time(NULL));
		int tmp = rand()%(1<<bitsLinhas);
		set <int>::iterator it = assoc4[t].begin();
		assoc4[t].erase(assoc4[t].begin());
		assoc4[t].insert(extraiData(addr));
	}
	else {
		assoc4[t].insert(extraiData(addr));
	}
	tag[t] = extraiData(addr);
}

void cache::access(int addr) {
	int mask = 1 << bitsLinhas;
	mask--;
	addr &= 0xFFFFFFFC;	// Alinhando endereco

	int desc = bitsLinhas + ways;
	int t = (addr >> desc);

	int mod = 1 << bitsLinhas;
	int index = t % mod;
	index = addr >> ways;
	index &= mod-1;

	if (!valid[index]) {
		valid[index] = true;
		atualizaTag(index, addr);		// Incrementa miss tambem
		return;
	}

	if (assoc4[index].find(extraiData(addr))!=assoc4[index].end()) {
		hits++;
	}
	else {
		atualizaTag(index, addr);
	}
}

void cache::detalhes (int option) {
	FILE *fp;
	fp = fopen("logCache","a");
	float hitRatio = (float)hits/(hits+misses);
	int tam = (1+32*(1 << ways)+(32-bitsLinhas))*(1 << bitsLinhas)/(8192);
	fprintf(fp, "%d, %d, %d, %d, %.2f%%, %d, %d\n", option, (1 << ways), (1 << bitsLinhas), tam, 100*hitRatio, hits, misses);
	fclose(fp);

}
