#include "GPUMonitor.h"


// TODO: be more compliant... check DRM version and all that shit.
std::map<SString, SStringList> GPU_PathMap = {
	{"USAGE",	{	"/sys/class/drm/card[GPU]/device/*_busy_percent"}},
	{"TEMPS",	{	"/sys/class/drm/card[GPU]/device/temp*"}},
	{"VRAM",	{	"/sys/class/drm/card[GPU]/device/mem_info_*"}},
	{"POWER", 	{	"/sys/class/drm/card[GPU]/device/hwmon/*/power*"}},
	{"CLOCKS", 	{	"/sys/class/drm/card[GPU]/device/hwmon/*/freq*"}},
	{"FAN", 	{	"/sys/class/drm/card[GPU]/device/hwmon/*/fan*",
					"/sys/class/drm/card[GPU]/device/hwmon/*/pwm*"	}},
};


HWGPUMonitor::HWGPUMonitor	()
	:
	HWProvider("GPU")
{
}


HWGPUMonitor::~HWGPUMonitor	()
{
}


void
HWGPUMonitor::Refresh	()
{
	// cache all values at once:
	SStringMap<SStringMap<SString>>	cache;

	for (auto& [what, paths] : fPaths) {
		
	}
}


bool
HWGPUMonitor::StartWatching(HWProviderClient* client, CString& _what)
{
	if (!HWProvider::StartWatching(client, _what))
		return false;

	// convert 'what' into relevant paths
	// don't worry about the potential for bloat in fPaths.
	SStringList paths;
	SStringMap<SString> subs;
	SString what = _what;
	std::transform(what.begin(), what.end(), what.begin(), ::toupper);

	auto pos = what.find(":");
	if (pos == SString::npos) {
		subs["[GPU]"] = "0";
	} else {
		subs["[GPU]"] = what.substr(pos, what.size() - pos);
	}

	bool all = what.find("ALL") != SString::npos,
		result = false;

	for (auto& [watch, pathTemplates] : GPU_PathMap) {
		if (all || what.find(watch) != SString::npos) {
			HWUtils::ParsePaths(pathTemplates, &paths, subs);
			result = true;
			for (auto& path : paths)
				_IncludePath(watch, path);

			paths.clear();
		}
	}

	return result;
}


void
HWGPUMonitor::_IncludePath(CString& watch, CString& path)
{
	bool included = false;
	for (auto& value : fPaths[watch]) {
		if (value == path) {
			included = true;
			break;
		}
	}

	if (!included)
		fPaths[watch].push_back(path);
}
