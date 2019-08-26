#pragma once

#include "Globals.h"
#include "Provider.h"



class HWGPUMonitor : public HWProvider {
public:
	explicit					HWGPUMonitor	();
								~HWGPUMonitor	();

			void				Refresh	();

protected:
	/*
		'what' pattern:
		"ALL","USAGE","FAN","POWER"

		To specify a GPU:
		"ALL:0","ALL:1","ALL:[2]"...
	*/
	virtual	bool				StartWatching(HWProviderClient*, CString& what);

private:
			void				_IncludePath(CString&, CString&);
		SStringMap<SStringList>	fPaths;
};
