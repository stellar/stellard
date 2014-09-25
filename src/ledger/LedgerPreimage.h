#ifndef __LEDGERPREIMAGE__
#define __LEDGERPREIMAGE__

#include <boost/shared_ptr.hpp>
#include "ripple/types/api/base_uint.h"
#include "ripple_app/misc/SerializedLedger.h"

/*
This is the form of the ledger that is hashed to create the ledger id.
*/
using namespace ripple;

namespace stellar
{
	class LedgerPreimage
	{
	public:
		typedef boost::shared_ptr<LedgerPreimage> pointer;

		virtual void addEntry(uint256& newHash, SLE::pointer newEntry)=0;
		virtual void updateEntry(uint256& oldHash, uint256& newHash, SLE::pointer updatedEntry)=0;
		virtual void deleteEntry(uint256& hash)=0;
		virtual void closeLedger()=0;  // need to call after all the tx have been applied to save that last versions of the ledger entries into the buckets

		virtual uint256 getHash()=0;
	};
}

#endif


