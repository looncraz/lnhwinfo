#pragma once


#include "Globals.h"
#include "Message.h"


class HWObject {
public:
								HWObject	(CString& name);
	virtual						~HWObject	();

			void				LockObject		();
			void				UnlockObject	();

	virtual	void				MessageReceived	(const HWMessage&);

			CString&			Name		() const;
private:
		std::recursive_mutex	fObjectMutex;
			SString				fName;
};
