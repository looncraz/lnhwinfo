#pragma once

#include "Globals.h"
#include <unordered_map>


struct HWSensor {
	std::map<std::string, std::string>	values;		// "input" : "408"
};

using HWSensorMap = std::map<std::string, HWSensor>;


class HWController {

public:
								HWController	();
								HWController	(const std::string& path);
								~HWController	();

	const	std::string&		Name		() const;
	const	HWSensorMap&		Sensors		() const;

		// Refresh calls are ignored if not enough time has passed...
		// so frequent calls are expected.cd
			void				RefreshAll	();
			void				Refresh		(const std::string& byName);
			void				Refresh		(const std::string& byName,
												const std::string& oneValue);

			void				SetMaxUpdateInterval(time_ms_t);
			time_ms_t			MaxUpdateInterval	() const;
private:
			bool				_CanRefresh() const;
			void				_Refreshed();
			std::string			fPath;
			std::string			fName;
			HWSensorMap			fSensors;
			time_ms_t			fMaxUpdateInterval;
			time_point_t		fLastUpdateTime;
};


class HWControllerMonitor {
public:
	explicit					HWControllerMonitor	();
								~HWControllerMonitor	();

	const std::map<std::string, HWController>&
								Controllers() const;

			void				RefreshAll();

			void				ForEachValue(
				std::function<void(
						const std::string&, // controller (eg: amdgpu)
						const std::string&,	// sensor (eg: in0)
						const std::string&, // key (eg: in0_input)
						const std::string&  // value (eg: 464)
					)> ) const;

private:
		std::map<std::string, HWController>
								fControllers;
};


extern HWControllerMonitor* gControllers;
