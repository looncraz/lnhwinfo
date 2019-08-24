#pragma once

#include <gtkmm.h>
#include "LevelBar.h"

class HWUPSWidget : public Gtk::Box {
public:
	explicit					HWUPSWidget		();
								~HWUPSWidget	();

			void				Refresh();
private:
			Gtk::Label			fLabel;
			HWLevelBar			fBar;
};
