#include "CPUMonitor.h"
#include "Utils.h"


/*
struct HWCPUCore {
	int							coreID;
	std::map<HWHistory<>>		usage; 		// [thread, history]
	std::list<int>				threads;	// which threads belong to this core
	HWHistory<>					frequency;
	HWHistory<bool>				sleep;		// TODO!
	HWHistory<>					voltage; 	// TODO!
};
*/

int on_cores_timer(void*)
{
	if (gCPUMonitor != nullptr)
		gCPUMonitor->Refresh();
	else
		return G_SOURCE_REMOVE;

	return G_SOURCE_CONTINUE;
}


HWCPUMonitor* gCPUMonitor = NULL;

HWCPUMonitor::HWCPUMonitor	()
	:
	fOnSecond(false),
	fThreadCount(0),
	fThreadsPerCore(0)
{
	gCPUMonitor = this;
	auto lines = HWUtils::SplitString(HWUtils::ShellExec("cat /proc/cpuinfo |"
				" grep \"processor\\|core id\""), "\n");
	/*
		lines data:
			[0] processor\t   : #
			[1] core id\t     : #
			...

		convert # to int, build two integer lists linking
		threads to cores, build fCores mapping.
	*/

	std::vector<int>	cores, threads;

	for (auto& line : lines) {
		int index = line.find("processor");
		if (index == 0) {
			fThreadCount++;
			threads.push_back(HWUtils::ExtractTrailingInteger(line));
		} else {
			cores.push_back(HWUtils::ExtractTrailingInteger(line));
		}
	}

	for (int i = 0; i < cores.size(); ++i) {
		fCores[cores[i]].threads.push_back(threads[i]);
		fCores[cores[i]].coreID = cores[i];
		fCores[cores[i]].scheduler = HWSCHED_UNKNOWN;
	}


	for (auto& pair : fCores) {
		printf("Core: %i, threads: ", pair.first);

		for (auto thd : pair.second.threads) {
			pair.second.usage[thd].push(0.0);
			printf("%i ", thd);
		}

		printf("\n");
	}

	fThreadsPerCore = fThreadCount / fCores.size();

	// LAST!
	g_timeout_add_full(G_PRIORITY_HIGH, 250, &on_cores_timer, this, nullptr);
}


HWCPUMonitor::~HWCPUMonitor	()
{
	gCPUMonitor = nullptr;
}


void
HWCPUMonitor::Refresh()
{
	if (!fOnSecond) {
		fFirst = HWUtils::SplitString(
				HWUtils::ShellExec("cat /proc/stat | grep cpu | grep -v \"cpu \""),
			"\n");

		fOnSecond = true;
		return;
	}
	fOnSecond = false;
	fSecond = HWUtils::SplitString(
			HWUtils::ShellExec("cat /proc/stat | grep cpu | grep -v \"cpu \""),
		"\n");

	std::deque<float> usage; // per "cpu" / logical process / CPU thread

	/*
		Iterate through cpu# and get percentages.
		/proc/stat should never violate the enumumeration, so we make thread
		assumption that the cpus are listed in order to save CPU time.
	*/
	for (int cpu = 0; cpu < fThreadCount; ++cpu) {
		usage.push_back(
				_CalculateUsage(
					HWUtils::ExtractAllIntegers(fFirst[cpu]),
					HWUtils::ExtractAllIntegers(fSecond[cpu])
				)
			);
	}

	for (auto& [coreID, core] : fCores) {
		float avgUsage = 0.0;
		int useCount = 0;

		for (auto& thd : core.threads) {
			core.usage[thd].push(usage[thd]);
			avgUsage += core.usage[thd].running_average();
			++useCount;
		}

		avgUsage /= (float)useCount;

		if (avgUsage > 40) {
			_SetSched(core, HWSCHED_PERFORMANCE);
		} else if (avgUsage > 25) {
			_SetSched(core, HWSCHED_SCHEDUTIL);
		} else if (avgUsage < 10) {
			_SetSched(core, HWSCHED_POWERSAVE);
		}
		_RefreshFrequency();

		_Invoke(coreID);
	}
}


void
HWCPUMonitor::ForEachCore(const std::function<void(const HWCPUCore&)>& func) const
{
	for (auto& core : fCores) {
		func(core.second);
	}
}


void
HWCPUMonitor::ForCore(int core, std::function<void(const HWCPUCore&)>& func) const
{
	func(fCores.at(core));
}


void
HWCPUMonitor::AddListener(int core, std::function<void(const HWCPUCore&)> func)
{
	fListeners[core].push_back(func);
}


void
HWCPUMonitor::_SetSched(HWCPUCore& core, HWScheduler sched)
{
	if (core.scheduler == sched)
		return;

	core.scheduler = sched;

	for (auto& thd : core.threads) {
		std::string cmd = "cpufreq-set -c ";
		cmd += std::to_string(thd);
		cmd += " -g ";

		std::string mode;

		switch (sched) {
			case HWSCHED_POWERSAVE  : mode = "powersave";   break;
			case HWSCHED_SCHEDUTIL  : mode = "schedutil";   break;
			case HWSCHED_PERFORMANCE: mode = "performance"; break;
		}

		cmd += mode;

		HWUtils::ShellRootExec(cmd);
	}
}


void
HWCPUMonitor::_Invoke(int core)
{
	for (auto & func : fListeners[core]) {
		func(fCores[core]);
	}
}


float
HWCPUMonitor::_CalculateUsage(const std::deque<uint64>& first,
	 	const std::deque<uint64>& second)
{
	uint64	firstTotalJiffies = 0,
			firstTotalWorkies = 0,
			secondTotalJiffies = 0,
			secondTotalWorkies = 0;

	for (auto& i : first) { firstTotalJiffies += i; }
	for (auto& i : second) { secondTotalJiffies += i; }

	for (int i = 0; i < 3; ++i) {
		firstTotalWorkies += first[i];
		secondTotalWorkies += second[i];
	}

	uint64 jiffies = secondTotalJiffies - firstTotalJiffies;
	uint64 workies = secondTotalWorkies - firstTotalWorkies;

	if (jiffies == 0)
		return 0.0f;

	return 100.0 * ((double)workies / (double)jiffies);
}


void
HWCPUMonitor::_RefreshFrequency()
{
	auto lines = HWUtils::SplitString(HWUtils::ShellExec("cat /proc/cpuinfo |"
		"grep \"processor\\|cpu MHz\""), "\n");

	if (lines.size() < 2) {
		fprintf(stderr, "Fatal error reading /proc/cpuinfo!\n");
		return;
	}

	int processor = -1;

	for (auto& line : lines) {
		if (line.find("processor") == 0) {
			processor = HWUtils::ExtractTrailingInteger(line);
			continue;
		}

		// find core for processor, update frequency
		for (auto& [coreID, core] : fCores) {
			for (auto& thread : core.threads) {
				if (thread == processor) {
					float useMax = 0.0;

					/*
						Estimate active cycles when in power saving mode
						since Ryzen power and clock gates in this mode with
						extreme aggression.

						This roughly mimicks Ryzen Master's behavior, though
						we don't probe cc6 states... because I don't know how
						on Ryzen.
					*/
					if (core.scheduler != HWSCHED_POWERSAVE) {
						useMax = 100.0;
					} else {
						for (auto thd : core.threads) {
							if (core.usage[thd].back() > useMax)
								useMax = core.usage[thd].back();
							if (core.usage[thd].running_average() > useMax)
								useMax = core.usage[thd].running_average();
						}
					}

					core.frequency.push(
						(useMax / 100.0) * HWUtils::ExtractTrailingFloat(line));
					break;
				}
			}
		}
	}
}
