#include <thread>

#include "ClockWidget.h"
#include "Globals.h"
#include "Utils.h"

int gContinue = G_SOURCE_CONTINUE;


int on_timer(void* _widget) {
	auto widget = (HWClockWidget*)_widget;
	if (gContinue == G_SOURCE_CONTINUE)
		widget->Refresh();

	return gContinue;
}


HWClockWidget::HWClockWidget()
	:
	SizeConstrainedBox(Gtk::ORIENTATION_VERTICAL, -1, WINDOW_WIDTH),
	fTime("<span size='xx-large'>...</span>"),
	fDay("..."),
	fDate("...")
{
	fTime.set_use_markup(true);
	pack_start(fTime, Gtk::PACK_SHRINK);
	pack_start(fDay, Gtk::PACK_SHRINK);
	pack_start(fDate, Gtk::PACK_SHRINK);
	show_all();
	Refresh();
	g_timeout_add(500, &on_timer, this);
}

HWClockWidget::~HWClockWidget	()
{
	gContinue = G_SOURCE_REMOVE;
}


void
HWClockWidget::Refresh()
{
	auto day = HWUtils::ShellExec("date +%A");
	auto date = HWUtils::ShellExec("date +\"%B %d\"");

	auto time = HWUtils::ShellExec("date +%H:%M:%S");
	time.insert(0, "<span size='xx-large'>");
	time += "</span>";
	fTime.set_label(time.c_str());
	fDay.set_label(day.c_str());
	fDate.set_label(date.c_str());
}
