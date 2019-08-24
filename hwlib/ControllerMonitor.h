#pragma once

#include "Globals.h"
#include <unordered_map>


struct HWSensor {
	std::map<SString, SString>	values;		// "input" : "408"
};

using HWSensorMap = std::map<SString, HWSensor>;


class HWController {

public:
								HWController	();
								HWController	(CString& path);
								~HWController	();

	const	SString&		Name		() const;
	const	HWSensorMap&		Sensors		() const;

		// Refresh calls are ignored if not enough time has passed...
		// so frequent calls are expected.cd
			void				RefreshAll	();
			void				Refresh		(CString& byName);
			void				Refresh		(CString& name, CString& value);

			void				SetMaxUpdateInterval(time_ms_t);
			time_ms_t			MaxUpdateInterval	() const;
private:
			bool				_CanRefresh() const;
			void				_Refreshed();
			SString				fPath;
			SString				fName;
			HWSensorMap			fSensors;
			time_ms_t			fMaxUpdateInterval;
			time_point_t		fLastUpdateTime;
};


class HWControllerMonitor {
public:
	explicit					HWControllerMonitor	();
								~HWControllerMonitor	();

	const SStringMap<HWController>& Controllers() const;

			void				RefreshAll();

			void				ForEachValue(
				std::function<void(
						CString&, // controller (eg: amdgpu)
						CString&,	// sensor (eg: in0)
						CString&, // key (eg: in0_input)
						CString&  // value (eg: 464)
					)> ) const;

private:
	SStringMap<HWController>	fControllers;
};


extern HWControllerMonitor* gControllers;
