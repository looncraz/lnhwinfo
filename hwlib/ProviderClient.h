#pragma once


#include "Globals.h"
#include "Message.h"


class HWProviderClient : public HWObject {
public:
								HWProviderClient	(CString& name);
	virtual						~HWProviderClient	();

			bool				IsWatching		(CString& who, CString& what);

			void				StartWatching	(CString& who, CString& what);
			void				StopWatching	(CString& who, CString& what);

	virtual	void				Updated	(CString& who, HWMessage&&) = 0;
		// the message is moved in Updated to avoid copies...

	const SStringMap<SStringList>&	Watching	() const;

private:
			SStringMap<SStringList>	fWatching;
};
