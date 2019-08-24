#include "UPSWidget.h"
#include "Utils.h"


int on_ups_timer(void* _ups)
{
	auto ups = (HWUPSWidget*)_ups;
	ups->Refresh();
	return G_SOURCE_CONTINUE;
}

HWUPSWidget::HWUPSWidget()
	:
	Gtk::Box(Gtk::ORIENTATION_VERTICAL)
{
	fLabel.set_label("0.0 Watts");
	fBar.set_value(0.19);
	fBar.set_size_request(WINDOW_WIDTH, 5);

	pack_start(fLabel);
	pack_start(fBar);
	show_all();

	g_timeout_add(500, &on_ups_timer, this);
}


HWUPSWidget::~HWUPSWidget	()
{

}



void
HWUPSWidget::Refresh()
{
	auto result = HWUtils::ShellRootExec("pwrstat -status");

	auto lines = HWUtils::SplitString(result, "\n");

	for (auto& line : lines) {
		if (line.find("Load") != SString::npos) {

			auto numbers = HWUtils::ExtractAllIntegers(line);
			SString label = std::to_string(numbers[0]);
			label += " Watts";
			fLabel.set_label(label.c_str());
			fBar.set_value((double)numbers[1] / 100.0);
			break;
		}
	}
}
