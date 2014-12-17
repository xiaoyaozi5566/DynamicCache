#include "mem/cache/tags/c_dynalru.hh"
#include "base/intmath.hh"
#include "debug/CacheRepl.hh"
#include "mem/cache/tags/cacheset.hh"
#include "mem/cache/tags/lru.hh"
#include "mem/cache/base.hh"
#include "sim/core.hh"
#include "mem/cache/blk.hh"
#include <typeinfo>

C_DYNALRU::C_DYNALRU( unsigned _numSets,
        unsigned _blkSize,
        unsigned _assoc,
        unsigned _hit_latency,
		unsigned _L_assoc,
		unsigned _H_min)
    : LRU(_numSets, _blkSize, _assoc, _hit_latency )
{
	L_assoc = _L_assoc;
	H_min = _H_min;
	H_assoc = _assoc - L_assoc;
	init_sets();
}

// label: 0->L, 1->H
CacheSet
C_DYNALRU::get_set( int setnum, uint64_t label, Addr addr ){
    CacheSet s = sets[label][setnum];
    return s;
}

int
C_DYNALRU::assoc_of_tc( int tcid ){
    if (tcid == 0) return L_assoc;
	else return H_assoc;
}

void
C_DYNALRU::init_sets(){
    sets = new CacheSet*[2];
    for( int i=0; i< 2; i++ ){ sets[i] = new CacheSet[numSets]; }

    numBlocks = numSets * assoc;
    blks = new BlkType[numBlocks];
    dataBlks = new uint8_t[numBlocks * blkSize];

    unsigned blkIndex = 0;
    for( unsigned tc=0; tc< 2; tc++ ){
        for( unsigned i = 0; i< numSets; i++ ){
            int tc_assoc = assoc_of_tc(tc);
            sets[tc][i].assoc = tc_assoc;
            sets[tc][i].blks  = new BlkType*[tc_assoc];
            for( unsigned j = 0; j<tc_assoc; j++ ){
                BlkType *blk = &blks[blkIndex];
                blk->data = &dataBlks[blkSize*blkIndex];
                ++blkIndex;

                blk->status = 0;
                blk->tag = j;
                blk->whenReady = 0;
                blk->isTouched = false;
                blk->size = blkSize;
                blk->set = i;
				// initialize the threadID
				blk->threadID = tc;
                sets[tc][i].blks[j]=blk;
            }
        }
    }
}

// increase the size of Low partition
unsigned
C_DYNALRU::inc_size(){
	if(H_assoc == H_min) return 0;
	
	L_assoc += 1;
	H_assoc -= 1;
	
	fprintf(stderr, "L_assoc = %2d H_assoc = %2d\n", L_assoc, H_assoc);
	
	for( unsigned i = 0; i < numSets; i++){
		// increase the size of L partition
		sets[0][i].assoc = L_assoc;
		CacheBlk **tempBlks = sets[0][i].blks;
		sets[0][i].blks = new BlkType*[L_assoc];
		// copy the original cache blocks
		for( unsigned j = 0; j<L_assoc-1; j++){
			sets[0][i].blks[j] = tempBlks[j];
		}
		// append the new blk from High partition
		sets[0][i].blks[L_assoc-1] = sets[1][i].blks[H_assoc];
		// mark as dirty to hide the "dirty" bit
		// sets[0][i].blks[L_assoc-1]->status |= BlkDirty;
		
		// decrease the size of H partition
		sets[1][i].assoc = H_assoc;
		tempBlks = sets[1][i].blks;
		sets[1][i].blks = new BlkType*[H_assoc];
		// remove one block from High partition
		for( unsigned j = 0; j<H_assoc; j++){
			sets[1][i].blks[j] = tempBlks[j];
		}
	}
	return numSets;
}

unsigned 
C_DYNALRU::dec_size(){
	if(L_assoc == 1) return 0;
	
	L_assoc -= 1;
	H_assoc += 1;
	
	fprintf(stderr, "L_assoc = %2d H_assoc = %2d\n", L_assoc, H_assoc);
	
	for( unsigned i = 0; i < numSets; i++){
		// increase the size of H partition
		sets[1][i].assoc = H_assoc;
		CacheBlk **tempBlks = sets[1][i].blks;
		sets[1][i].blks = new BlkType*[H_assoc];
		// copy the original blocks
		for( unsigned j = 0; j<H_assoc-1; j++){
			sets[1][i].blks[j] = tempBlks[j];
		}
		// append the new blk from Low partition
		sets[1][i].blks[H_assoc-1] = sets[0][i].blks[L_assoc];
		
		// decrease the size of L partition
		sets[0][i].assoc = L_assoc;
		tempBlks = sets[0][i].blks;
		sets[0][i].blks = new BlkType*[L_assoc];
		// remove one block from Low partition
		for( unsigned j = 0; j<L_assoc; j++){
			sets[0][i].blks[j] = tempBlks[j];
		}
	}
	return numSets;
}

C_DYNALRU::BlkType*
C_DYNALRU::get_evictBlk(unsigned tcid, unsigned index){
	// increase the low partition
	if(tcid == 0)
		return sets[0][index].blks[L_assoc-1];
	// decrease the low partition
	else
		return sets[1][index].blks[H_assoc-1];
}
