#pragma once

#include <gtkmm.h>

#include "CPUWidget.h"


class HWCPUStatusBox : public Gtk::Box {
public:
								HWCPUStatusBox	();
								~HWCPUStatusBox	();

private:
	std::deque<HWCPUWidget*>	fWidgets;
};
