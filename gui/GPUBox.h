#pragma once

#include <gtkmm.h>

#include "Globals.h"
#include "ProviderClient.h"


class HWGPUBox : public HWProviderClient, Gtk::DrawingArea {
public:

								HWGPUBox	(int gpu = 0);
								HWGPUBox	(int gpu,
 												CString& label,
												CString& startValue,
												CString& unit);
	virtual						~HWGPUBox	();

			void				Set	(CString&, CString&, CString&);

	virtual	void				Updated	(CString&, CString&, CString&);

private:
			int					fCPU;
			HWPoint				fLabelStart, fValueStart, fUnitStart;
			SString				fLabel, fValue, fUnit;
};
