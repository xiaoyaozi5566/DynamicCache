#include "mem/cache/base.hh"
#include "mem/cache/blk.hh"
#include "mem/cache/cache.hh"
#include "params/BaseCache.hh"
#include "stdio.h"

typedef BaseCacheParams Params;
template <class TagStore>
class DirtyCache : public SplitRPortCache<TagStore>
{
    public:
    DirtyCache( const Params *p, TagStore *tags );
    /** Define the type of cache block to use. */
    typedef typename TagStore::BlkType BlkType;
    /** A typedef for a list of BlkType pointers. */
    typedef typename TagStore::BlkList BlkList;
	
	protected:
    virtual bool access(PacketPtr pkt, BlkType *&blk,
                int &lat, PacketList &writebacks);
	
	virtual bool timingAccess(PacketPtr pkt);
	
	virtual void handleResponse(PacketPtr pkt);
	
    virtual BlkType *handleFill(PacketPtr pkt, BlkType *blk,
                        PacketList &writebacks);
};
