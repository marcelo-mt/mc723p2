#include<cache.h>

cache::cache (int size, int n) {
	hits = 0;
	misses = 0;
	tag.resize(1 << size);
	valid.resize(1 << size,false);	// Blocks nao inicializados
	bitsBlocos = n;
}


int cache::extraiData (int addr) {
	return ((1 << bitsBlocos) - 1) & addr;	// valor fora da tag
}

void cache::atualizaTag (int t, int addr) {
	tag[t] = extraiData(addr);
}

void cache::access(int addr) {
	addr &= 0xFFF8;		// Alinhando endereco

	int t = addr;
	int mask = 1 << bitsBlocos;
	mask--;
	mask ^= t;
	t &= mask;

	if (!valid[t]) {
		misses++;
		valid[t] = true;
		atualizaTag(t, addr);
		return;
	}

	if (tag[t]==extraiData(addr)) {
		hits++;
	}
	else {
		misses++;
		atualizaTag(t, addr);
	}
}
