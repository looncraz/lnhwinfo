#include "ControllerMonitor.h"
#include "Globals.h"
#include "Utils.h"


HWController::HWController	()
	:
	fMaxUpdateInterval(std::chrono::milliseconds(50)),
	fLastUpdateTime(std::chrono::system_clock::now() - fMaxUpdateInterval)
{
}


HWController::HWController	(CString& path)
	:
	fPath(path),
	fMaxUpdateInterval(std::chrono::milliseconds(50)),
	fLastUpdateTime(std::chrono::system_clock::now() - fMaxUpdateInterval)
{
	//printf("%s\n", path.c_str());

	SString name;

	for (auto& p : fs::directory_iterator(path)) {
		name = p.path().string().substr(path.size() + 1,
				p.path().string().size() - path.size());

		if (name == "name") {
			// Our name!
			HWUtils::ReadFileAsString(p.path().string(), &fName);
//			printf("\tController: %s\n", fName.c_str());
			continue;
		}

		auto end = name.find("_");
		if (end == SString::npos)
			continue;


		SString subName = name.substr(0, end);
		//printf("\t%s\n", subName.c_str());

		auto& sensor = fSensors[subName];
		SString value;
		HWUtils::ReadFileAsString(p.path().string(), &value);

		sensor.values[name] = value;
	}
//puts("---");
/*
	for (auto& [key, sensor] : fSensors) {
		printf("\t%s\n", key.c_str());
		for (auto& [vkey, value] : sensor.values) {
			printf("\t\t%-20s = %s\n", vkey.c_str(), value.c_str());
		}
	}
*/
}


HWController::~HWController	()
{

}


CString&
HWController::Name () const
{
	return fName;
}

const HWSensorMap&
HWController::Sensors	() const
{
	return fSensors;
}

		// Refresh calls are ignored if not enough time has passed...
		// so frequent calls are expected.cd
void
HWController::RefreshAll()
{
	if (!_CanRefresh())
		return;
	_Refreshed();

	for (auto& [key, sensor] : fSensors) {
		for (auto& [key, value] : sensor.values) {
			HWUtils::ReadFileAsString(fPath + "/" + key, &value);
		}
	}
}


void
HWController::Refresh(CString& byName)
{
	if (!_CanRefresh())
		return;
	_Refreshed();

}


void
HWController::Refresh(CString& byName, CString& oneValue)
{
	if (!_CanRefresh())
		return;
	_Refreshed();

}

void
HWController::SetMaxUpdateInterval(time_ms_t interval)
{
	fMaxUpdateInterval = interval;
}

time_ms_t
HWController::MaxUpdateInterval	() const
{
	return fMaxUpdateInterval;
}


bool
HWController::_CanRefresh() const
{
	return (std::chrono::system_clock::now() - fLastUpdateTime)
			>= fMaxUpdateInterval;
}


void
HWController::_Refreshed()
{
	fLastUpdateTime = std::chrono::system_clock::now();
}



//#pragma mark HWControllerMonitor -------------------------------------------------------


HWControllerMonitor* gControllers = nullptr;


HWControllerMonitor::HWControllerMonitor	()
{
	try {
	for (auto& dir : fs::directory_iterator("/sys/class/hwmon")) {
		SString path = dir.path();
		SString name;
		if (HWUtils::ReadFileAsString(path + "/name", &name))
			fControllers[name] = HWController(path);
	}
	} catch(...) {
		fprintf(stderr, "Holy fuck, stupid filesystem API is a bit immature\n");
	}

	gControllers = this;

/*
	ForEachValue([](CString& a, CString& b,
			CString& c, CString& d){
		printf("%-10s %-10s %-30s = %s\n", a.c_str(), b.c_str(), c.c_str(), d.c_str());
	});
*/
}

HWControllerMonitor::~HWControllerMonitor	()
{}


const SStringMap<HWController>&
HWControllerMonitor::Controllers() const
{
	return fControllers;
}

void
HWControllerMonitor::RefreshAll()
{
	for (auto& [name, controller] : fControllers) {
		controller.RefreshAll();
	}
}


void
HWControllerMonitor::ForEachValue(
	std::function<void(
			CString&, // controller (eg: amdgpu)
			CString&,	// sensor (eg: in0)
			CString&, // key (eg: in0_input)
			CString&  // value (eg: 464)
		)> func) const
{
	for (auto& [controllerName, controller] : Controllers()) {
		for (auto& [sensorName, sensor] : controller.Sensors()) {
			for (auto& [key, value] : sensor.values) {
				func(controllerName, sensorName, key, value);
			}
		}
	}
}
