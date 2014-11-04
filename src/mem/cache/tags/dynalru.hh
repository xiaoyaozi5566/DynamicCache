// This file implements dynamic cache

#include "mem/cache/tags/lru.hh"
#include "stdio.h"

class DYNALRU : public LRU{
private:
		unsigned L_assoc;
		unsigned H_assoc;

    public:
    DYNALRU( unsigned _numSets, unsigned _blkSize, unsigned _assoc,
            unsigned _hit_latency);
	
	protected:
    CacheSet **sets;
    virtual void init_sets();
    virtual CacheSet get_set( int setnum, uint64_t tid, Addr addr );
    // virtual void cleanupRefs();

    virtual int assoc_of_tc( int tcid );
	
	virtual void inc_size();
	virtual unsigned dec_size();
	virtual BlkType* check_dirty(unsigned index);
};
