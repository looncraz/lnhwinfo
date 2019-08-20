#pragma once

#include <deque>
#include <functional>
#include <map>

#include "HWTypes.h"


enum HWScheduler {
	HWSCHED_POWERSAVE = 0,
	HWSCHED_SCHEDUTIL,
	HWSCHED_PERFORMANCE
};


struct HWCPUCore {
	int							coreID;
	std::map<int, HWHistory<>>	usage; 		// [thread, history]
	std::list<int>				threads;	// which threads belong to this core
	HWHistory<>					frequency;
	HWHistory<bool>				sleep;		// TODO!
	HWHistory<>					voltage; 	// TODO!
	HWScheduler					scheduler;
};


class HWCPUMonitor {
public:
	explicit					HWCPUMonitor	();
								~HWCPUMonitor	();

			void				Refresh			();

			void				AddListener		(int core,
									std::function<void(const HWCPUCore&)>);

			void 				ForEachCore(const std::function<void(const HWCPUCore&)>&) const;
			void 				ForCore(int core, std::function<void(const HWCPUCore&)>&) const;
private:
			void				_Invoke(int core);
			void 				_RefreshFrequency();
			float				_CalculateUsage(const std::deque<uint64>&,
									const std::deque<uint64>&);

			void 				_SetSched(HWCPUCore&, HWScheduler);

	std::map<int, std::vector<std::function<void(const HWCPUCore&)>>>
								fListeners;
	std::vector<std::string>	fFirst, fSecond;
			bool				fOnSecond;

	std::map<int, HWCPUCore>	fCores;

			int					fThreadCount,
								fThreadsPerCore;
};

extern HWCPUMonitor*	gCPUMonitor;
