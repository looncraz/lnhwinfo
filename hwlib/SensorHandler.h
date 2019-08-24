#pragma once

#include <gtkmm.h>
#include "ControllerMonitor.h"
#include "LevelBar.h"
#include "Separator.h"

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
	virtual	void				Updated(const HWSensor&) = 0;
};


class HWGenericSensorHandler : public HWSensorHandler {
public:
								HWGenericSensorHandler	();
								HWGenericSensorHandler	(const std::string&,
									std::function<void(std::string&)>
										= [](std::string&){});
	virtual						~HWGenericSensorHandler	();

	virtual	Gtk::Box&			CreateGUI();
	virtual void 				Updated(const std::string&);
	virtual	void				Updated(const HWSensor&);
private:

			Gtk::Box			fBox;
			Gtk::Label			fName,
 								fData;

			std::string			fNameOverride;
			std::function<void(std::string&)>	fFunc;
};


class HWTemperatureSensorHandler : public HWSensorHandler {
public:
								HWTemperatureSensorHandler	();
								HWTemperatureSensorHandler	(const std::string&,
									float min = 0.0f, float max = 115.0f);
	virtual						~HWTemperatureSensorHandler	();

	virtual	Gtk::Box&			CreateGUI();
	virtual void 				Updated(const std::string&);
	virtual	void				Updated(const HWSensor&);
private:

			Gtk::Box			fOuterBox, fBox;
			Gtk::Label			fName,
 								fData,
								fUnit;

			HWLevelBar			fLevel;
			HWSeparator			fSep;

			std::string			fNameOverride;
			float				fMinTemp, fMaxTemp;
};


class HWUsageSensorHandler : public HWSensorHandler {
public:
								HWUsageSensorHandler	();
								HWUsageSensorHandler	(const std::string&);
	virtual						~HWUsageSensorHandler	();

	virtual	Gtk::Box&			CreateGUI();
	virtual void 				Updated(const std::string&);
	virtual	void				Updated(const HWSensor&);
private:

			Gtk::Box			fOuterBox, fBox;
			Gtk::Label			fName,
 								fData,
								fUnit;

			HWLevelBar			fLevel;
			HWSeparator			fSep;

			std::string			fNameOverride;
};
