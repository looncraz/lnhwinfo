#include "ControllerMonitor.h"
#include "Utils.h"

#if __cplusplus >= 201402L
#	include <filesystem>
namespace fs = std::filesystem;
#elif __cplusplus > 199711L
#	include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#	error "Unsupported C++ version! Set C++11 or newer!"
#endif


/*
struct HWSensor {
	std::string							name;		// eg "in0"
	std::map<std::string, std::string>	values;		// "input" : "408"
};
*/
HWController::HWController	()
	:
	fMaxUpdateInterval(std::chrono::milliseconds(50)),
	fLastUpdateTime(std::chrono::system_clock::now() - fMaxUpdateInterval)
{
}


HWController::HWController	(const std::string& path)
	:
	fPath(path),
	fMaxUpdateInterval(std::chrono::milliseconds(50)),
	fLastUpdateTime(std::chrono::system_clock::now() - fMaxUpdateInterval)
{
	//printf("%s\n", path.c_str());

	std::string name;

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
		if (end == std::string::npos)
			continue;


		std::string subName = name.substr(0, end);
		//printf("\t%s\n", subName.c_str());

		auto& sensor = fSensors[subName];
		std::string value;
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


const std::string&
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
HWController::Refresh(const std::string& byName)
{
	if (!_CanRefresh())
		return;
	_Refreshed();

}


void
HWController::Refresh(const std::string& byName, const std::string& oneValue)
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
		std::string path = dir.path();
		std::string name;
		if (HWUtils::ReadFileAsString(path + "/name", &name))
			fControllers[name] = HWController(path);
	}
	} catch(...) {
		fprintf(stderr, "Holy fuck, stupid filesystem API is a bit immature\n");
	}

	gControllers = this;

/*
	ForEachValue([](const std::string& a, const std::string& b,
			const std::string& c, const std::string& d){
		printf("%-10s %-10s %-30s = %s\n", a.c_str(), b.c_str(), c.c_str(), d.c_str());
	});
*/
}

HWControllerMonitor::~HWControllerMonitor	()
{}


const std::map<std::string, HWController>&
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
			const std::string&, // controller (eg: amdgpu)
			const std::string&,	// sensor (eg: in0)
			const std::string&, // key (eg: in0_input)
			const std::string&  // value (eg: 464)
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
