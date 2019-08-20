#include "SensorHandler.h"

#include "Utils.h"

/*
	This file represents the highest level of code complexity ever encountered
	by man or machine.

	Good thing I am neither.
*/

HWSensorHandler::HWSensorHandler(){}
HWSensorHandler::~HWSensorHandler(){}


#pragma mark HWGenericSensorHandler --------------


HWGenericSensorHandler::HWGenericSensorHandler	()
	:
	fBox(Gtk::ORIENTATION_HORIZONTAL),
	fName("Sensor"),
	fData("Data *C")
{
}

HWGenericSensorHandler::HWGenericSensorHandler	(const std::string& over)
	:
	fBox(Gtk::ORIENTATION_HORIZONTAL),
	fName(over),
	fData("Data *C"),
	fNameOverride(over)
{
}


HWGenericSensorHandler::~HWGenericSensorHandler	()
{
}


Gtk::Box&
HWGenericSensorHandler::CreateGUI()
{
	fName.set_xalign(0.0f);
	fBox.pack_start(fName, Gtk::PACK_EXPAND_WIDGET);
	fBox.pack_end(fData, 0, 0, 4);
	fBox.show_all();
	fData.set_use_markup(true);
	return fBox;
}

void
HWGenericSensorHandler::Updated(const std::string& line)
{
	auto split = HWUtils::SplitString(line, ":");

	if (split.size() < 2) {
		fprintf(stderr, "HWGenericSensorHandler: Unable to parse:\n\t\"%s\"\n", line.c_str());
		return;
	}

	if (fNameOverride.size() == 0)
		fName.set_label(HWUtils::StripTrailingWhitespace(split[0].c_str()));

	split = HWUtils::SplitString(split[1], "(");
	std::string data = "<span size=\"small\">"
 		+ HWUtils::StripTrailingWhitespace(split[0]) + "</span>";
	fData.set_label(data);
}


#pragma mark HWTemperatureSensorHandler --------------


HWTemperatureSensorHandler::HWTemperatureSensorHandler	()
	:
	fOuterBox(Gtk::ORIENTATION_VERTICAL),
	fBox(Gtk::ORIENTATION_HORIZONTAL),
	fName("Sensor"),
	fData("Data *C"),
	fMinTemp(0.0),
	fMaxTemp(115.0)
{
}

HWTemperatureSensorHandler::HWTemperatureSensorHandler	(const std::string& over,
		float min, float max)
	:
	fOuterBox(Gtk::ORIENTATION_VERTICAL),
	fBox(Gtk::ORIENTATION_HORIZONTAL),
	fName(over),
	fData("Data *C"),
	fNameOverride(over),
	fMinTemp(min),
	fMaxTemp(max)
{
}


HWTemperatureSensorHandler::~HWTemperatureSensorHandler	()
{
}


Gtk::Box&
HWTemperatureSensorHandler::CreateGUI()
{
	fUnit.set_use_markup(true);
	fUnit.set_label("<span size=\"small\" color=\"#ffcaca\"> °C </span>");
	fUnit.set_size_request(10, 5);
	fData.set_use_markup(true);

	fLevel.set_size_request(WINDOW_WIDTH, 3);

	fLevel.set_min_color((rgb_color){26, 102, 255, 179});
	fLevel.set_mid_color((rgb_color){255, 200, 55, 179});
	fLevel.set_max_color((rgb_color){200, 50, 50, 179});

	fName.set_xalign(0.0f);
	fBox.pack_start(fName, Gtk::PACK_EXPAND_WIDGET);
	fBox.pack_start(fData, 0, 0, 4);
	fBox.pack_end(fUnit, 0, 0, 0);
	fBox.show_all();

	fOuterBox.pack_start(fBox, Gtk::PACK_EXPAND_WIDGET);
	fOuterBox.pack_start(fLevel);
	fOuterBox.pack_end(fSep);

	fOuterBox.show_all();

	return fOuterBox;
}

void
HWTemperatureSensorHandler::Updated(const std::string& line)
{
	auto split = HWUtils::SplitString(line, ":");

	if (split.size() < 2) {
		fprintf(stderr, "HWTemperatureSensorHandler: Unable to parse:\n\t\"%s\"\n", line.c_str());
		return;
	}

	if (fNameOverride.size() == 0)
		fName.set_label(HWUtils::StripTrailingWhitespace(split[0].c_str()));

	split = HWUtils::SplitString(split[1], "(");
	auto str =	HWUtils::StripLeadUntilNumber(HWUtils::StripTailUntilNumber(split[0]));
	auto val = HWUtils::ExtractTrailingFloat(str);
	str = std::to_string(val);
	if (val < 100)
		str.resize(4);
	else
		str.resize(5);

	val = (val - fMinTemp) / (fMaxTemp - fMinTemp);
	fLevel.set_value(val);

	std::string data = "<span size=\"small\">" + str + "</span>";
	fData.set_label(data);
}
