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
			bool				AddHandler	(	CString& controller,
												CString& sensor,
												HWSensorHandler* );

			bool				AddPathHandler	(CString& name,
												CString& path,
												uint32 milliseconds,
												HWSensorHandler*,
									std::function<void(HWPathMonitorItem&)>
									= [](HWPathMonitorItem&){});

			// use a specific path
			bool				AddHandler	(	CString& path,
												HWSensorHandler* );

			void				PathRefreshed	(const HWPathMonitorItem&);

private:
	std::map<SString,		// controller
		std::map<SString,	// sensor
			HWSensorHandler*>>	fHandlers;

			void				_RefreshCorsair();

			void				_Invoke(CString& controller,
									CString& sensor);

			Gtk::Box			fBox;
};


extern HWSensorList* gSensors;
