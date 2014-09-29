#include "TrustSetTx.h"
#include "ledger/TrustLine.h"
#include "ledger/LedgerMaster.h"

namespace stellar
{
	TER TrustSetTx::doApply()
	{
		TrustLine txTrustLine;
		TER terResult = txTrustLine.fromTx(mSigningAccount, this);
		if(terResult == tesSUCCESS)
		{
			// SANITY anything else to check here?

			bool modSigner = false;

			// Pull this trust line from SQL
			// change this side of the line
			// save the line back to SQL
			TrustLine existingLine;
			if(existingLine.loadFromDB(txTrustLine.getIndex()))
			{ // modify existing trustline
				bool lowerOwnerCount = false;
				bool raiseOwnerCount = false;
				if(mSigningAccount.mAccountID == existingLine.mLowAccount)
				{	// we are moding the lowAccount
					if(existingLine.mLowLimit > 0 && txTrustLine.mLowLimit == 0 && existingLine.mBalance >= 0) lowerOwnerCount = true;
					else if(existingLine.mLowLimit == 0 && txTrustLine.mLowLimit > 0 && existingLine.mBalance <= 0) raiseOwnerCount = true;
					existingLine.mLowLimit = txTrustLine.mLowLimit;
					existingLine.mHighAuthSet = txTrustLine.mHighAuthSet;

				} else
				{	// we are moding the highAccount
					if(existingLine.mHighLimit > 0 && txTrustLine.mHighLimit == 0 && existingLine.mBalance <= 0) lowerOwnerCount = true;
					else if(existingLine.mHighLimit == 0 && txTrustLine.mHighLimit > 0 && existingLine.mBalance >= 0) raiseOwnerCount = true;
					existingLine.mHighLimit = txTrustLine.mHighLimit;
					existingLine.mLowAuthSet = txTrustLine.mLowAuthSet;
				}

				if(lowerOwnerCount)
				{
					mSigningAccount.mOwnerCount--;
				} else if(raiseOwnerCount)
				{
					// make sure the account has enough reserve
					terResult = mSigningAccount.tryToIncreaseOwnerCount();
					if(terResult != tesSUCCESS) return(terResult);
				}
				//check if line should be deleted
				if(existingLine.mLowLimit == 0 && existingLine.mHighLimit == 0 && existingLine.mBalance == 0)
				{	// delete line
					existingLine.storeDelete();
				} else
				{
					existingLine.storeChange();
				}

				modSigner = lowerOwnerCount | raiseOwnerCount;

			} else
			{  // this trust line doesn't exist yet

				// make sure the account has enough reserve
				terResult = mSigningAccount.tryToIncreaseOwnerCount();
				if(terResult != tesSUCCESS) return(terResult);

				modSigner = true;

				existingLine.storeAdd();
			}

			if(modSigner)
			{
				mSigningAccount.storeChange();
			}
		} else
		{	// some error building trustline from tx
			return(terResult);
		}

		return terResult;
	}
}
