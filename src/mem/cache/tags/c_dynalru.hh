// This file implements dynamic cache

#include "mem/cache/tags/lru.hh"
#include "stdio.h"

class C_DYNALRU : public LRU{
private:
		unsigned L_assoc;
		unsigned H_assoc;
		unsigned assoc;
		unsigned H_min;

    public:
    C_DYNALRU( unsigned _numSets, unsigned _blkSize, unsigned _assoc,
            unsigned _hit_latency, unsigned _L_assoc, unsigned _H_min);
	
	void reset_umon();
	unsigned curr_L_assoc();
	unsigned lookup_umon(int index);
	virtual unsigned inc_size();
	virtual unsigned dec_size();
	virtual BlkType* get_evictBlk(unsigned tcid, unsigned index);
	
	virtual BlkType* accessBlock(Addr addr, int &lat, int master_id, uint64_t tid);
	virtual BlkType* accessBlock(Addr addr, int &lat, int context_src, uint64_t readLabel, uint64_t writeLabel){ return NULL;}

	virtual void invalidateBlk(BlkType *blk, uint64_t tid);
	
	protected:
    CacheSet **sets;
	CacheSet *umon_set;
	BlkType *umon_blks;
	unsigned *umon_counters;
    virtual void init_sets();
    virtual CacheSet get_set( int setnum, uint64_t tid, Addr addr );
    // virtual void cleanupRefs();

    virtual int assoc_of_tc( int tcid );
};
