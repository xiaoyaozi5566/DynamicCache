// This file implements dirty cache

#include "mem/cache/tags/lru.hh"

class DYNALRU : public LRU{
    private:

    public:
    DYNALRU( unsigned _numSets, unsigned _blkSize, unsigned _assoc,
            unsigned _hit_latency);

	using LRU::findBlock;
	virtual BlkType* findBlock(Addr addr, uint64_t readLabel, uint64_t writeLabel) override;
	
	using LRU::accessBlock;
	virtual BlkType* accessBlock(Addr addr, int &lat, int master_id, uint64_t readLabel, uint64_t writeLabel) override;
	
	virtual uint64_t locateBlock(Addr addr, uint64_t readLabel, uint64_t writeLabel);
	
	protected:
    CacheSet **sets;
    virtual void init_sets();
    virtual CacheSet get_set( int setnum, uint64_t tid, Addr addr );
    // virtual void cleanupRefs();

    virtual int assoc_of_tc( int tcid );
};
