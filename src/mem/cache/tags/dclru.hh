// This file implements dirty cache

#include "mem/cache/tags/lru.hh"

class DCLRU : public LRU{
    private:

    public:
    DCLRU( unsigned _numSets, unsigned _blkSize, unsigned _assoc,
            unsigned _hit_latency);

    protected:
    CacheSet **sets;
    virtual void init_sets();
    virtual CacheSet get_set( int setnum, uint64_t tid, Addr addr );
    // virtual void cleanupRefs();

    virtual int assoc_of_tc( int tcid );
};
