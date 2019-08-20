#pragma once

#include <gtkmm.h>
#include "SizeConstrainedBox.h"

class HWClockWidget : public SizeConstrainedBox {
public:
								HWClockWidget	();
								~HWClockWidget	();

			void 				Refresh();

private:
			Gtk::Label			fTime;
			Gtk::Label			fDay;
			Gtk::Label			fDate;
};
