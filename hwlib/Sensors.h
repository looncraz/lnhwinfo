#pragma once

#include <functional>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "HWTypes.h"
#include "PathMonitor.h"
#include "SensorHandler.h"


class HWSensorList {
public:
	explicit					HWSensorList	();
								~HWSensorList	();

			void				Begin			(unsigned milliseconds = 1000);
			void				Refresh			();

			Gtk::Box&			GetBox			();

			// only one handler per sensor!
			bool				AddHandler	(	const std::string& controller,
												const std::string& sensor,
												HWSensorHandler* );

			bool				AddPathHandler	(const std::string& name,
												const std::string& path,
												uint32 milliseconds,
												HWSensorHandler*,
									std::function<void(HWPathMonitorItem&)>
									= [](HWPathMonitorItem&){});

			// use a specific path
			bool				AddHandler	(	const std::string& path,
												HWSensorHandler* );

			void				PathRefreshed	(const HWPathMonitorItem&);

private:
	std::map<std::string,		// controller
		std::map<std::string,	// sensor
			HWSensorHandler*>>	fHandlers;

			void 				_RefreshCorsair();

			void 				_Invoke(const std::string& controller,
									const std::string& sensor);

			Gtk::Box			fBox;
};


extern HWSensorList* gSensors;
