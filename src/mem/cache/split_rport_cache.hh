#include "mem/cache/base.hh"
#include "mem/cache/cache.hh"
#include "params/BaseCache.hh"
#include "stdio.h"

typedef BaseCacheParams Params;
template <class TagStore>
class SplitRPortCache : public SplitMSHRCache<TagStore>
{
    uint64_t *blocked_array;
	public:
    SplitRPortCache( const Params *p, TagStore *tags );

    virtual void setBlocked(BaseCache::BlockedCause cause, int threadID)
    {
        uint8_t flag = 1 << cause;
        if (blocked_array[threadID] == 0) {
            this->blocked_causes[cause]++;
            this->blockedCycle = curTick();
            this->cpuSidePort->setBlocked(threadID);
        }
        blocked_array[threadID] |= flag;
    }
	
    virtual void clearBlocked(BaseCache::BlockedCause cause, int threadID)
    {
        uint8_t flag = 1 << cause;
        blocked_array[threadID] &= ~flag;
        if (blocked_array[threadID] == 0) {
            this->blocked_cycles[cause] += curTick() - this->blockedCycle;
            this->cpuSidePort->clearBlocked(threadID);
        }
    }
	
	protected:
    //virtual bool isSplitRPort() {return true;}
    class SRCpuSidePort : public SplitMSHRCache<TagStore>::CpuSidePort{
        SlavePacketQueue** respQueues;
		int num_tcs;
		bool *blocked_array;

        virtual bool recvTimingReq(PacketPtr pkt)
		{
		    if (!pkt->memInhibitAsserted() && blocked_array[pkt->threadID]) {
		        DPRINTF(Cache,"Scheduling a retry while blocked\n");
		        this->mustSendRetry = true;
		        return false;
		    }

		    this->cache->tracePrinter->addTrace( pkt , "recvTimingReq" );
		    this->cache->timingAccess(pkt);
		    return true;
		}
		
		virtual void schedTimingResp(PacketPtr pkt, Tick when, int threadID)
        { 
            this->respQueues[threadID]->schedSendTiming(pkt, when);
        }

        virtual void recvRetry() { 
            for( int i=0; i < num_tcs; i++) recvRetry(i);
        }
		
		virtual void recvRetry(int threadID) { 
            this->respQueues[threadID]->retry();
        }
        
		virtual void setBlocked(int threadID) {
			assert(!blocked_array[threadID]);
			blocked_array[threadID] = true;
		}
		
		virtual void clearBlocked(int threadID) {
		    assert(blocked_array[threadID]);
		    blocked_array[threadID] = false;
		    if (this->mustSendRetry) {
		        this->mustSendRetry = false;
		        // @TODO: need to find a better time (next bus cycle?)
		        this->owner.schedule(this->sendRetryEvent, curTick() + 1);
		    }
		}
		
        public:
        SRCpuSidePort(const std::string &_name, Cache<TagStore> *_cache,
                    const std::string &_label) :
        SplitMSHRCache<TagStore>::CpuSidePort( _name, _cache, _label ){
			num_tcs = _cache->params->num_tcs;
			blocked_array = new bool[num_tcs];
			for( int i=0; i < num_tcs; i++)
				blocked_array[i] = false;
	        this->respQueues = new SlavePacketQueue*[_cache->params->num_tcs];
            for( int i=0; i < (_cache->params->num_tcs); i++ ){
                this->respQueues[i] = new SlavePacketQueue(*_cache, *this, "SlavePacketQueue", i);
            }
        }
    };
};
