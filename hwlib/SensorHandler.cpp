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
	fBox.pack_start(fName, 0, 0, 4);
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
