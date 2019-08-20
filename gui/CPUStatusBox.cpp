#include <gtk/gtk.h>

#include "CPUStatusBox.h"
#include "CPUMonitor.h"

HWCPUStatusBox::HWCPUStatusBox()
	:
	Gtk::Box(Gtk::ORIENTATION_VERTICAL)
{
	gCPUMonitor->ForEachCore(
		[&](const HWCPUCore& core) {
			fWidgets.push_back(new HWCPUWidget(core));
		}
	);

	for (auto& widget : fWidgets)
		pack_start(*widget, Gtk::PACK_SHRINK, 5);

	show_all();
}


HWCPUStatusBox::~HWCPUStatusBox(){}
