#pragma once

#include <gtkmm.h>

/*
	Required for generating and managing widgets to provide an GUI for
	any given sensor.

	Example:
	gSensors->AddHandler("amdgpu", "edge", new MySensorHandler());
*/
class HWSensorHandler {
public:
								HWSensorHandler		();
	virtual						~HWSensorHandler	();

	/*
		Initialize user interface which will be added to HWSensorBox.
	*/
	virtual	Gtk::Box&			CreateGUI() = 0;

	/*
		Hook function for when the sensor is updated.
		Contains `sensors` output (single line):

		edge:         +33.0°C  (crit = +100.0°C, hyst = -273.1°C)
	*/
	virtual void 				Updated(const std::string&) = 0;
};


class HWGenericSensorHandler : public HWSensorHandler {
public:
								HWGenericSensorHandler	();
								HWGenericSensorHandler	(const std::string&);
	virtual						~HWGenericSensorHandler	();

	virtual	Gtk::Box&			CreateGUI();
	virtual void 				Updated(const std::string&);
private:

			Gtk::Box			fBox;
			Gtk::Label			fName,
 								fData;

			std::string			fNameOverride;
};
