#include "mem/cache/tags/f_dynalru.hh"
#include "base/intmath.hh"
#include "debug/CacheRepl.hh"
#include "mem/cache/tags/cacheset.hh"
#include "mem/cache/tags/lru.hh"
#include "mem/cache/base.hh"
#include "sim/core.hh"
#include "mem/cache/blk.hh"
#include <typeinfo>

F_DYNALRU::F_DYNALRU( unsigned _numSets,
        unsigned _blkSize,
        unsigned _assoc,
        unsigned _hit_latency)
    : LRU(_numSets, _blkSize, _assoc, _hit_latency )
{
	L_assoc = _assoc/2;
	H_assoc = _assoc - L_assoc;
	init_sets();
}

// label: 0->L, 1->H
CacheSet
F_DYNALRU::get_set( int setnum, uint64_t label, Addr addr ){
    CacheSet s = sets[label][setnum];
    return s;
}

int
F_DYNALRU::assoc_of_tc( int tcid ){
    if (tcid == 0) return L_assoc;
	else return H_assoc;
}

void
F_DYNALRU::init_sets(){
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

unsigned 
F_DYNALRU::dec_size(){
	for( unsigned i = 0; i < numSets; i++){
		// Search backwards to evict the LRU first
		for( unsigned j = L_assoc - 1; j >= 0; j--){
			BlkType *tempBlk = sets[0][i].blks[j];
			// Found a cacheline in Low partition that hasn't been used, should evict
			if (tempBlk->threadID == 0 && tempBlk->isTouched == false && tempBlk->isValid()){
				// assign 1000 to threadID for later eviction
				sets[0][i].blks[j]->threadID = 1000;
				break;
			}
		}
	}

	return numSets;
}

F_DYNALRU::BlkType*
F_DYNALRU::get_evictBlk(unsigned index){
	for(unsigned i = 0; i < L_assoc; i++){
		BlkType *tempBlk = sets[0][index].blks[i];
		if(tempBlk->threadID == 1000){
			tempBlk->threadID = 0;
			return tempBlk;
		}
	}
	return NULL;
}

F_DYNALRU::BlkType*
F_DYNALRU::accessBlock(Addr addr, int &lat, int master_id, uint64_t tid)
{
    Addr tag = extractTag(addr);
    unsigned set = extractSet(addr);
    // BlkType *blk = sets[set].findBlk(tag);
    BlkType *blk = get_set(set,tid,addr).findBlk(tag);
    lat = hitLatency;
    if (blk != 0 ) {
        // move this block to head of the MRU list
        get_set(set,tid,addr).moveToHead(blk);
        DPRINTF(CacheRepl, "set %x: moving blk %x to MRU\n",
                set, regenerateBlkAddr(tag, set));
        if (blk->whenReady > curTick()
            && blk->whenReady - curTick() > hitLatency) {
            lat = blk->whenReady - curTick();
        }
        blk->refCount += 1;
		// set the used bit
		blk->isTouched = 1;
    }

    return blk;
}


F_DYNALRU::BlkType*
F_DYNALRU::findBlock(Addr addr, uint64_t tid)
{
    Addr tag = extractTag(addr);
    unsigned set = extractSet(addr);
    BlkType *blk = get_set(set,tid,addr).findBlk(tag);
    return blk;
}

F_DYNALRU::BlkType*
F_DYNALRU::findVictim(Addr addr, PacketList &writebacks, uint64_t tid)
{
    unsigned set = extractSet(addr);
    // grab a replacement candidate
    BlkType *blk = get_set(set,tid,addr).blks[assoc_of_tc(tid)-1];

    if (blk->isValid()) {
        DPRINTF(CacheRepl, "set %x: selecting blk %x for replacement\n",
                set, regenerateBlkAddr(blk->tag, set));
    }
    return blk;
}

void
F_DYNALRU::insertBlock(Addr addr, BlkType *blk, int master_id, uint64_t tid)
{
    if (!blk->isTouched) {
        tagsInUse++;
        blk->isTouched = true;
        if (!warmedUp && tagsInUse.value() >= warmupBound) {
            warmedUp = true;
            warmupCycle = curTick();
        }
    }

    // If we're replacing a block that was previously valid update
    // stats for it. This can't be done in findBlock() because a
    // found block might not actually be replaced there if the
    // coherence protocol says it can't be.
    if (blk->isValid()) {
        replacements[0]++;
        totalRefs += blk->refCount;
        ++sampledRefs;
        blk->refCount = 0;

        // deal with evicted block
        assert(blk->srcMasterId < cache->system->maxMasters());
        occupancies[blk->srcMasterId]--;
    }

    // Set tag for new block.  Caller is responsible for setting status.
    blk->tag = extractTag(addr);

    // deal with what we are bringing in
    assert(master_id < cache->system->maxMasters());
    occupancies[master_id]++;
    blk->srcMasterId = master_id;

    unsigned set = extractSet(addr);
    get_set(set,tid,addr).moveToHead(blk);
}

void
F_DYNALRU::invalidateBlk(BlkType *blk, uint64_t tid)
{
    if (blk) {
        if (blk->isValid()) {
            tagsInUse--;
            assert(blk->srcMasterId < cache->system->maxMasters());
            occupancies[blk->srcMasterId]--;
            blk->srcMasterId = Request::invldMasterId;
            unsigned set = blk->set;
            get_set(set,tid,blk->set).moveToTail(blk);
        }
        blk->status = 0;
        blk->isTouched = false;
        blk->clearLoadLocks();

        // should be evicted before valid blocks
        //unsigned set = blk->set;
        //sets[set].moveToTail(blk);
    }
}
