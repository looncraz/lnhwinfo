#pragma once

#include <gtkmm.h>

#include "CPUMonitor.h"
#include "LevelBar.h"
#include "SizeConstrainedBox.h"


class HWCPUWidget : public SizeConstrainedBox {
public:
								HWCPUWidget		(const HWCPUCore& core);
	virtual						~HWCPUWidget	();

private:
			void				_Refresh		(const HWCPUCore& core);
			SizeConstrainedBox	fThreadBox;
			int					fCoreID;
		std::map<int, HWLevelBar*>
								fUsageBars;
			Gtk::Label			fFrequencyLabel;
//			Gtk::Image*			fStatusImage; // TODO: demonstrate core status (sleep, boost, etc.)
};
