#include "mem/cache/tags/dynalru.hh"
#include "base/intmath.hh"
#include "debug/CacheRepl.hh"
#include "mem/cache/tags/cacheset.hh"
#include "mem/cache/tags/lru.hh"
#include "mem/cache/base.hh"
#include "sim/core.hh"
#include "mem/cache/blk.hh"
#include <typeinfo>

DYNALRU::DYNALRU( unsigned _numSets,
        unsigned _blkSize,
        unsigned _assoc,
        unsigned _hit_latency)
    : LRU(_numSets, _blkSize, _assoc, _hit_latency )
{
    init_sets();
}

// label: 0->L, 1->H
CacheSet
DYNALRU::get_set( int setnum, uint64_t label, Addr addr ){
    CacheSet s = sets[label][setnum];
    return s;
}

int
DYNALRU::assoc_of_tc( int tcid ){
    int a = assoc / 2;
    if(tcid < (assoc%2)) a++;
    return a;
}

void
DYNALRU::init_sets(){
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
                sets[tc][i].blks[j]=blk;
            }
        }
    }
}
