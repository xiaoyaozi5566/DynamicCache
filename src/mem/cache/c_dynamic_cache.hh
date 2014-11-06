#include "mem/cache/base.hh"
#include "mem/cache/blk.hh"
#include "mem/cache/cache.hh"
#include "params/BaseCache.hh"
#include "stdio.h"

typedef BaseCacheParams Params;
template <class TagStore>
class C_DynamicCache : public SplitRPortCache<TagStore>
{
    public:
    C_DynamicCache( const Params *p, TagStore *tags );
    /** Define the type of cache block to use. */
    typedef typename TagStore::BlkType BlkType;
    /** A typedef for a list of BlkType pointers. */
    typedef typename TagStore::BlkList BlkList;
	
protected:
	virtual void incMissCount(PacketPtr pkt)
	{
		if(pkt->threadID == 0) this->missCounter++;
	}
	
	void adjustPartition();
	
	EventWrapper<C_DynamicCache<TagStore>, &C_DynamicCache<TagStore>::adjustPartition> adjustEvent;
	
private:
	// Time interval to change partition size (ticks)
	uint64_t interval;
	// Thresholds for changing partition size
	uint64_t th_inc, th_dec;
	// protected:
	//     virtual bool access(PacketPtr pkt, BlkType *&blk,
	//                 int &lat, PacketList &writebacks);
	//
	// virtual bool timingAccess(PacketPtr pkt);
	//
	// virtual void handleResponse(PacketPtr pkt);
	//
	//     virtual BlkType *handleFill(PacketPtr pkt, BlkType *blk,
	//                         PacketList &writebacks);
};
