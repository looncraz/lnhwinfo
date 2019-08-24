#pragma once


#include "Globals.h"

/*
	Use HWPathMonitor directly... this is essentially private API.
*/
class HWPathMonitorItem {
	friend class HWPathMonitor;
	friend class HWSensorList;

	SString					name;
	SString					path;
	SString					value;

	time_ms_t					interval;
	time_point_t				lastRefresh;

	std::function<void(HWPathMonitorItem&)>		func;
};


/*
	Watch specific paths for changes
*/
class HWPathMonitor {
public:
	explicit					HWPathMonitor	();
								~HWPathMonitor	();

			void				Add(CString& name, CString& path, // sysfs
									uint32 milliseconds,
									std::function<void(HWPathMonitorItem&)>
									=[](HWPathMonitorItem&){});

			void				Remove	(CString& name);

	const	HWPathMonitor*		Get		(CString& name) const;
			HWPathMonitor*		Get		(CString& name);

			void				Begin	();
			void				Refresh	();
private:


	std::map<SString, HWPathMonitorItem> fItems;
			uint32				fShortestInterval;
};

extern HWPathMonitor* gPathMonitor;
