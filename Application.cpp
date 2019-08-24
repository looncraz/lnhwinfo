#include "Application.h"
#include "CPUMonitor.h"
#include "MainWindow.h"
#include "ControllerMonitor.h"
#include "Sensors.h"


#include <thread>

HWApplication* g_hwapp = NULL;


HWApplication::HWApplication	()
	:
	Gtk::Application("org.looncraz.hwinfo.dock"),
	fMainWindow	(NULL)
{
	g_hwapp = this;
}


HWApplication::~HWApplication()
{
	g_hwapp = NULL;
}


void
HWApplication::on_activate()
{
	new HWCPUMonitor();
	new HWPathMonitor();
	new HWControllerMonitor();
	new HWSensorList();	// must be before HWMainWindow

	fMainWindow = new HWMainWindow();
	add_window(*fMainWindow);
	fMainWindow->show();
}
