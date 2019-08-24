#include "PathMonitor.h"
#include "Sensors.h"
#include <gtkmm.h>



HWPathMonitor* gPathMonitor = nullptr;


int on_path_monitor_timer(void*)
{
	gPathMonitor->Refresh();
	return G_SOURCE_CONTINUE;
}


HWPathMonitor::HWPathMonitor	()
	:
	fShortestInterval(2000)
{
	gPathMonitor = this;
}


HWPathMonitor::~HWPathMonitor	(){}


void
HWPathMonitor::Add(const std::string& name, const std::string& path,
	uint32 milliseconds, std::function<void(HWPathMonitorItem&)> func)
{
	if (milliseconds < fShortestInterval)
		fShortestInterval = milliseconds;

	time_ms_t ms = std::chrono::milliseconds(milliseconds);
	time_point_t last = std::chrono::system_clock::now() - ms;

	auto& item = fItems[name];
	item.name = name;
	item.path = path;
	item.interval = ms;
	item.lastRefresh = last;
	item.func = func;

}


void
HWPathMonitor::Begin	()
{
	g_timeout_add(fShortestInterval, &on_path_monitor_timer, nullptr);
}


void
HWPathMonitor::Refresh()
{
	auto now = std::chrono::system_clock::now();
	for (auto& [name, item] : fItems) {
		if (now - item.lastRefresh >= item.interval) {
			if (HWUtils::ReadFileAsString(item.path, &item.value)) {
				item.func(item);
				gSensors->PathRefreshed(item);
				item.lastRefresh = now;
			}
		}
	}
}
