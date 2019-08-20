#include <limits>
#include <thread>

#include "Sensors.h"
#include "Utils.h"
#include "Separator.h"


int gSensorsContinue = G_SOURCE_CONTINUE;


int on_sensors_timer(void* _sensors) {
	auto sensors = (HWSensorList*)_sensors;
	if (gSensorsContinue == G_SOURCE_CONTINUE)
		sensors->Refresh();

	return gSensorsContinue;
}


HWSensorList::HWSensorList	()
	:
	fBox(Gtk::ORIENTATION_VERTICAL)
{
	gSensors = this;

	AddHandler("amdgpu", "power", 		new HWGenericSensorHandler("GPU Power"));
	AddHandler("amdgpu", "vdd", 		new HWGenericSensorHandler("GPU Voltage"));

	fBox.pack_start(*(new HWSeparator(10)), 0, 0, 0);

	AddHandler("k10temp", "die",
 			new HWTemperatureSensorHandler("Ryzen", 20.0, 95.0));

	AddHandler("amdgpu", "edge",
 			new HWTemperatureSensorHandler("GPU", 20.0, 80.0));

	AddHandler("amdgpu", "junction",
 			new HWTemperatureSensorHandler("GPU Junction", 20.0, 110.0));

	AddHandler("amdgpu", "mem",
 			new HWTemperatureSensorHandler("GPU HBM", 20.0, 80.0));

	fBox.pack_start(*(new HWSeparator(10)), 0, 0, 0);

	AddHandler("nct6798", "MASTER 1",
			new HWTemperatureSensorHandler("Chipset", 50.0, 80.0));
	AddHandler("corsair", "Temperature 0",
			new HWTemperatureSensorHandler("Coolant", 30.0, 42.0));
	AddHandler("corsair", "Temperature 1",
			new HWTemperatureSensorHandler("Rad In", 30.0, 35.0));
	AddHandler("corsair", "Temperature 3",
			new HWTemperatureSensorHandler("Power Supply", 40.0, 80.0));

	fBox.pack_start(*(new HWSeparator(10)), 0, 0, 0);

	AddHandler("corsair", "Fan 0",	new HWGenericSensorHandler("Rad Rear"));
	AddHandler("corsair", "Fan 1",	new HWGenericSensorHandler("Rad Mid"));
	AddHandler("corsair", "Fan 2",	new HWGenericSensorHandler("Rad Front"));
	AddHandler("corsair", "Fan 3",	new HWGenericSensorHandler("Front"));
	AddHandler("nct6798", "fan6",	new HWGenericSensorHandler("Chipset"));
	AddHandler("nct6798", "fan4",	new HWGenericSensorHandler("Pump"));
	AddHandler("nct6798", "fan5",	new HWGenericSensorHandler("Bottom"));
	AddHandler("corsair", "Fan 5",	new HWGenericSensorHandler("Power Supply"));

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
HWSensorList::Refresh()
{

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
/*
void 				RemoveHandler(	const std::string& controller,
									const std::string& sensor);

void 				RemoveHandler(	HWSensorHandler*);
*/


HWSensorList* gSensors = nullptr;
