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
	
	uint64_t array_avg(uint64_t* array, int count)
	{
		assert(count != 0);
		uint64_t sum = 0;
		for(int i = 0; i < count; i++){
			sum += array[i];
		}
		
		return sum/count;
	}
	
	void update_history(uint64_t* array, int count, uint64_t curr_misses)
	{
		assert(count != 0);
		for(int i = 0; i < count-1; i++){
			array[i] = array[i+1];
		}
		array[count-1] = curr_misses;
	}
	
	void dec_size();
	
	EventWrapper<C_DynamicCache<TagStore>, &C_DynamicCache<TagStore>::adjustPartition> adjustEvent;
	
private:
	// Time interval to change partition size (ticks)
	uint64_t interval;
	// Thresholds for changing partition size
	float th_inc, th_dec;
	// Window size
	uint64_t window_size;
	// Moving average
	uint64_t *miss_history;
	// Explore flag and stable flag
	bool explore_phase, explore_inc, explore_dec, stable_phase;
	// Stable length
	uint64_t stable_length, stable_counter;
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
