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

DYNALRU::BlkType* 
DYNALRU::findBlock(Addr addr, uint64_t readLabel, uint64_t writeLabel) {
    Addr tag = extractTag(addr);
    unsigned set = extractSet(addr);
	
	// (L, L)
	if (readLabel == 0 && writeLabel == 0)
	{
		BlkType *blk_L = get_set(set, 0, addr).findBlk(tag);
		BlkType *blk_H = get_set(set, 1, addr).findBlk(tag);
		
		// cache hit
		if (blk_L != NULL)
		{
			return blk_L;
		}
		// fake miss
		else if (blk_L == NULL && blk_H != NULL)
		{
			return NULL;
		}
		// real miss
		else
		{
			return NULL;
		}
	}
	// (H, H)
	else if (readLabel == 1 && writeLabel == 1)
	{
		BlkType *blk_L = get_set(set, 0, addr).findBlk(tag);
		BlkType *blk_H = get_set(set, 1, addr).findBlk(tag);
		
		if (blk_L != NULL)
		{
			return blk_L;
		}
		else
		{
			return blk_H;
		}
	}
	// (L, H)
	else if (readLabel == 0 && writeLabel == 1)
	{
		BlkType *blk_L = get_set(set, 0, addr).findBlk(tag);
		BlkType *blk_H = get_set(set, 1, addr).findBlk(tag);
		
		// cache hit
		if (blk_L != NULL)
		{
			return blk_L;
		}
		// fake miss
		else if (blk_L == NULL && blk_H != NULL)
		{
			return NULL;
		}
		// real miss
		else
		{
			return NULL;
		}
	}
	// (H, L)
	else
	{
		BlkType *blk_L = get_set(set, 0, addr).findBlk(tag);
		BlkType *blk_H = get_set(set, 1, addr).findBlk(tag);
		
		if (blk_L != NULL)
		{
			return blk_L;
		}
		else
		{
			return blk_H;
		}		
	}
}

uint64_t
DYNALRU::locateBlock(Addr addr, uint64_t readLabel, uint64_t writeLabel)
{
    Addr tag = extractTag(addr);
    unsigned set = extractSet(addr);
	
	if (readLabel == 1)
	{
		BlkType *blk_L = get_set(set, 0, addr).findBlk(tag);
		
		if (blk_L != NULL)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

DYNALRU::BlkType*
DYNALRU::accessBlock(Addr addr, int &lat, int master_id, uint64_t readLabel, uint64_t writeLabel)
{
    unsigned set = extractSet(addr);
    // BlkType *blk = sets[set].findBlk(tag);
    BlkType *blk = findBlock(addr, readLabel, writeLabel);
    lat = hitLatency;
    if (blk != 0 ) {
        // move this block to head of the MRU list
		uint64_t locBlk = locateBlock(addr, readLabel, writeLabel);
        get_set(set,locBlk,addr).moveToHead(blk);
        if (blk->whenReady > curTick()
            && blk->whenReady - curTick() > hitLatency) {
            lat = blk->whenReady - curTick();
        }
        blk->refCount += 1;
    }

    return blk;
}
