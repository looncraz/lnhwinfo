#include "Application.h"
#include "Globals.h"
#include "MainWindow.h"
#include "Sensors.h"
#include "Utils.h"
#include "Separator.h"

HWMainWindow::HWMainWindow	()
	:
	Gtk::Window(Gtk::WindowType::WINDOW_TOPLEVEL),
	fBox(Gtk::ORIENTATION_VERTICAL, -1, WINDOW_WIDTH)
{
	set_type_hint(Gdk::WINDOW_TYPE_HINT_DOCK);

	set_gravity(Gdk::GRAVITY_NORTH_WEST);
	move(Gdk::Screen::get_default()->get_width() - WINDOW_WIDTH, 0);

	fBox.pack_start(fClock, false, false, 2);
	fBox.pack_start(*(new HWSeparator()), false, false, 0);
	fBox.pack_start(fCPUStatus, false, false, 2);
	fBox.pack_start(*(new HWSeparator()), false, false, 0);
	fBox.pack_start(fUPSWidget, false, false, 2);
	fBox.pack_start(*(new HWSeparator()), false, false, 0);
	fBox.pack_start(gSensors->GetBox(), false, false, 2);
	fBox.pack_start(*(new HWSeparator()), false, false, 0);
	fBox.show_all();
	add(fBox);
}


HWMainWindow::~HWMainWindow	()
{
}



void
HWMainWindow::on_size_allocate	(Gtk::Allocation& alloc)
{
	alloc.set_width(WINDOW_WIDTH);
	Gtk::Window::on_size_allocate(alloc);
}


void
HWMainWindow::on_show	()
{
	gSensors->Begin();
	Gtk::Window::on_show();
}

void
HWMainWindow::on_realize()
{
	Gtk::Window::on_realize();
	HWUtils::ReserveScreenSpace(this);
}
