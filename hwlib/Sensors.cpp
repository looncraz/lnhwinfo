#include <limits>
#include <thread>

#include "Sensors.h"
#include "Utils.h"
#include "Separator.h"
#include "ControllerMonitor.h"

int gSensorsContinue = G_SOURCE_CONTINUE;


int on_sensors_timer(void* _sensors) {
	auto sensors = (HWSensorList*)_sensors;
	if (gSensorsContinue == G_SOURCE_CONTINUE)
		sensors->Refresh();

	gControllers->RefreshAll();

	return gSensorsContinue;
}


HWSensorList::HWSensorList	()
	:
	fBox(Gtk::ORIENTATION_VERTICAL)
{
	gSensors = this;

	AddHandler("k10temp", "temp1",
		new HWTemperatureSensorHandler("Ryzen", 20.0, 95.0));
	AddHandler("nct6798", "temp7",
			new HWTemperatureSensorHandler("Chipset", 50.0, 80.0));
	AddHandler("corsair", "Temperature 0",
			new HWTemperatureSensorHandler("Coolant", 30.0, 42.0));
	AddHandler("corsair", "Temperature 1",
			new HWTemperatureSensorHandler("Rad In", 28.0, 38.0));
	AddHandler("corsair", "Temperature 3",
			new HWTemperatureSensorHandler("Power Supply", 40.0, 80.0));

	fBox.pack_start(*(new HWSeparator(10)), 0, 0, 0);

	AddPathHandler("GPU Usage", "/sys/class/drm/card0/device/gpu_busy_percent",
		250, new HWUsageSensorHandler("GPU Usage"));

	/*
		Video memory usage
	*/
	uint64 totalVRam = 0;
	std::string buf;
	if (HWUtils::ReadFileAsString(
			"/sys/class/drm/card0/device/mem_info_vram_total", &buf)) {

		totalVRam = std::stoll(buf);
		AddPathHandler("VRAM", "/sys/class/drm/card0/device/mem_info_vram_used",
			250, new HWUsageSensorHandler("VRAM Usage"),

			[totalVRam](HWPathMonitorItem& item){
				uint64 used = std::stoll(item.value);
				float percent = (float)used / (float)totalVRam;
				item.value = std::to_string((int)(percent * 100));
				item.value += " %";
			});
	}

	AddHandler("amdgpu", "power1", 		new HWGenericSensorHandler("Power",
		[](std::string& value) {
			value = value.substr(0, value.size() - 6);
			value += ".0 W";
		}));

	AddHandler("amdgpu", "in0", 		new HWGenericSensorHandler("Voltage",
		[](std::string& value) {
			if (value.size() == 3)
				value.resize(2);

			value = "0." + value + " V";
		}));

	AddHandler("amdgpu", "freq1", 		new HWGenericSensorHandler("Frequency",
		[](std::string& value) {
			value = value.substr(0, value.size() - 6);
			value += " MHz";
		}));

	AddHandler("amdgpu", "temp1",
 			new HWTemperatureSensorHandler("Edge", 20.0, 80.0));

	AddHandler("amdgpu", "temp2",
 			new HWTemperatureSensorHandler("Junction", 20.0, 110.0));

	AddHandler("amdgpu", "temp3",
 			new HWTemperatureSensorHandler("HBM", 20.0, 80.0));


	fBox.pack_start(*(new HWSeparator(10)), 0, 0, 0);

	AddHandler("corsair", "Fan 0",	new HWGenericSensorHandler("Rad Rear"));
	AddHandler("corsair", "Fan 1",	new HWGenericSensorHandler("Rad Mid"));
	AddHandler("corsair", "Fan 2",	new HWGenericSensorHandler("Rad Front"));
	AddHandler("corsair", "Fan 3",	new HWGenericSensorHandler("Front"));
	AddHandler("nct6798", "fan6",	new HWGenericSensorHandler("Chipset"));
	AddHandler("nct6798", "fan4",	new HWGenericSensorHandler("Pump"));
	AddHandler("nct6798", "fan5",	new HWGenericSensorHandler("Bottom"));
	AddHandler("corsair", "Fan 5",	new HWGenericSensorHandler("Power Supply"));

	gPathMonitor->Begin();
}


HWSensorList::~HWSensorList	()
{
	// TODO: removal hook for all sensors?
	gSensorsContinue = G_SOURCE_REMOVE;
}


void
HWSensorList::Begin(unsigned interval)
{
	g_timeout_add(interval, &on_sensors_timer, this);
}


void
HWSensorList::PathRefreshed	(const HWPathMonitorItem& item)
{
	if (fHandlers.find(item.name) != fHandlers.end()) {
		auto& pathMap = fHandlers[item.name];
		if (pathMap.find(item.path) != pathMap.end()) {
			std::string line = item.name + ":" + item.value;
			pathMap[item.path]->Updated(line);
		}
	}
}


void
HWSensorList::Refresh()
{

#if 1
	for (auto& [ctrlKey, controller] : gControllers->Controllers()) {
		if (fHandlers.find(ctrlKey) == fHandlers.end())
			continue;	// we have no one who cares about this controller!

		auto& ctrlHandlers = fHandlers[ctrlKey];
		for (auto& [sensKey, sensor] : controller.Sensors()) {

			if (ctrlHandlers.find(sensKey) == ctrlHandlers.end())
				continue; // no one cares about this sensor!

			//printf("Updated %s\n", sensKey.c_str());
			ctrlHandlers[sensKey]->Updated(sensor);
		}
	}

	_RefreshCorsair();
#else

	auto result = HWUtils::ShellExec("sensors");
	auto controllers = HWUtils::SplitString(result, "\n\n");

	for (auto& controllerData : controllers) {
		auto sensors = HWUtils::SplitString(controllerData, "\n");
		if (sensors.size() == 0)
			continue;

		for (auto& [ctrl, sensorMap] : fHandlers) {
			if (sensors[0].find(ctrl) != std::string::npos) {
				// iterate through handlers, find relevant line
				for (auto& [sens, hand] : sensorMap) {

					for (auto& line : sensors) {
						if (line.find(sens) != std::string::npos) {
							hand->Updated(line);
							break;
						}
					}
				}
			}
		}
	}

	_RefreshCorsair();
#endif
}


void
HWSensorList::_RefreshCorsair()
{
	auto result = HWUtils::ShellRootExec("loonOpenCorsairLink.elf --device 0");
	auto lines = HWUtils::SplitString(result, "\n");

	for (auto& line : lines) {
		for (auto& [controller, sensors] : fHandlers) {
			if (controller.find("corsair") != std::string::npos) {
				for (auto& [sensor, handler] : sensors) {
					if (line.find(sensor) != std::string::npos) {
						handler->Updated(line);
					}
				}
			}
		}

//		printf("line: %s\n", line.c_str());
	}
}


Gtk::Box&
HWSensorList::GetBox()
{
	return fBox;
}


bool
HWSensorList::AddHandler(const std::string& controller, const std::string& sensor,
	HWSensorHandler* handler )
{
	if (!handler || controller.size() == 0 || sensor.size() == 0) {
		fprintf(stderr, "%s: Bad argument!\n", __PRETTY_FUNCTION__);
		return false;
	}

	// ensure we only have one handler!
	if (	fHandlers.find(controller) != fHandlers.end()
		&& fHandlers[controller].find(sensor) != fHandlers[controller].end()) {

		fprintf(stderr, "Sensors: %s/%s - Handler already exists!\n",
 			controller.c_str(), sensor.c_str());

		return false;
	}

	fHandlers[controller][sensor] = handler;

	fBox.pack_start(handler->CreateGUI(), 0, 0, 0);
	fBox.show_all();
/*
	for (auto& [ctrl, sensorMap] : fHandlers) {
		printf("%s\n", ctrl.c_str());

		for (auto& [sens, hand] : sensorMap) {
			printf("\t%s = %p\n", sens.c_str(), hand);
		}
	}
*/
	return true;
}

bool
HWSensorList::AddPathHandler(const std::string& name, const std::string& path,
		uint32 interval, HWSensorHandler* handler,
 		std::function<void(HWPathMonitorItem&)> func)
{
	if (!AddHandler(name, path, handler))
		return false;

	gPathMonitor->Add(name, path, interval, func);
	return true;
}



HWSensorList* gSensors = nullptr;
