// This file implements fine-grained dynamic cache

#include "mem/cache/tags/lru.hh"
#include "stdio.h"

class F_DYNALRU : public LRU{
private:
		unsigned L_assoc;
		unsigned H_assoc;

    public:
    F_DYNALRU( unsigned _numSets, unsigned _blkSize, unsigned _assoc,
            unsigned _hit_latency);
	
	virtual unsigned dec_size();
	virtual BlkType* get_evictBlk(unsigned tcid, unsigned index);
	
	virtual BlkType* accessBlock(Addr addr, int &lat, int master_id, uint64_t tid);
	virtual BlkType* accessBlock(Addr addr, int &lat, int context_src, uint64_t readLabel, uint64_t writeLabel){ return NULL;}
	virtual BlkType* findBlock(Addr addr, uint64_t tid);
	virtual BlkType* findBlock(Addr addr, uint64_t readLabel, uint64_t writeLabel){ return NULL;}
	virtual BlkType* findVictim(Addr addr, PacketList &writebacks, uint64_t tid);
	virtual void insertBlock(Addr addr, BlkType *blk, int master_id, uint64_t tid);
	virtual void invalidateBlk(BlkType *blk, uint64_t tid);
	
	protected:
    CacheSet **sets;
    virtual void init_sets();
    virtual CacheSet get_set( int setnum, uint64_t tid, Addr addr );
    // virtual void cleanupRefs();

    virtual int assoc_of_tc( int tcid );
};
