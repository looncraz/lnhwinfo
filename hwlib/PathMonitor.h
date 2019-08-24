#pragma once


#include "Globals.h"

/*
	Use HWPathMonitor directly... this is essentially private API.
*/
class HWPathMonitorItem {
	friend class HWPathMonitor;
	friend class HWSensorList;

	std::string					name;
	std::string					path;
	std::string					value;

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

			void				Add(const std::string& name,
									const std::string& path,	// to a file!
									uint32 milliseconds,
									std::function<void(HWPathMonitorItem&)>
									=[](HWPathMonitorItem&){});

			void				Remove	(const std::string& name);

	const	HWPathMonitor*		Get		(const std::string& name) const;
			HWPathMonitor*		Get		(const std::string& name);

			void				Begin	();
			void				Refresh	();
private:


	std::map<std::string, HWPathMonitorItem> fItems;
			uint32				fShortestInterval;
};

extern HWPathMonitor* gPathMonitor;
