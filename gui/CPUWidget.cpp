#include "CPUWidget.h"

#include "CPUMonitor.h"

#include "Globals.h"

Gdk::RGBA rgba;

HWCPUWidget::HWCPUWidget(const HWCPUCore& core)
	:
	SizeConstrainedBox(Gtk::ORIENTATION_HORIZONTAL, -1, WINDOW_WIDTH),
	fThreadBox(Gtk::ORIENTATION_VERTICAL, -1, WINDOW_WIDTH / 1.7f),
	fCoreID(core.coreID)
{
	fThreadBox.set_homogeneous();
	fThreadBox.set_spacing(4);

	auto css = Gtk::CssProvider::create();
	css->load_from_data("CPUProgBar {color: red} ");


	rgba.set_rgba(0.2, 0.2, 0.2, 1);

	for (auto& pair : core.usage) {
		auto bar = new HWLevelBar();
		bar->set_value(pair.second.back());
		bar->override_background_color(rgba);
		bar->set_margin_right(2);
		bar->set_name("CPUProgBar");
		fThreadBox.pack_start(*bar, false, false, 0);
		bar->set_size_request(100, 5);
		fUsageBars[pair.first] = bar;
	}

	pack_start(fThreadBox, Gtk::PACK_SHRINK, 2);
	fFrequencyLabel.set_label(" MHz");
	fFrequencyLabel.set_use_markup(true);
	fFrequencyLabel.set_padding(4, 0);
	pack_end(fFrequencyLabel, Gtk::PACK_SHRINK, 0);

	show_all();

	gCPUMonitor->AddListener(fCoreID, [&](const HWCPUCore& core) {
		_Refresh(core);
	});


}


HWCPUWidget::~HWCPUWidget()
{

}


void
HWCPUWidget::_Refresh(const HWCPUCore& core)
{
	auto freq = core.frequency.back();
	std::string label;

	if (freq > 1000) {
		std::string str = std::to_string(freq / 1000.0);
		str.resize(4);
		label += str;
	} else {
		label += "<span color=\"#cacaca\">";
		if (freq < 1 && freq > 0.25)
			freq = 1;
		std::string str = std::to_string((int)freq);
		label += str;
	}

	if (freq > 1000)
		label += " GHz";
	else
		label += " MHz</span>";

	if (freq < 0.25 && core.scheduler == HWSCHED_POWERSAVE)
		label = "<span color=\"#a8a8a8\">sleep</span>";

	fFrequencyLabel.set_label(label.c_str());

	float r = 0.1, g = 0.4, b = 1.0, a = 0.75;
	switch (core.scheduler) {
		case HWSCHED_POWERSAVE:
			r = 0.1; g = 0.7; b = 0.6; a = 0.75;
			break;
		case HWSCHED_PERFORMANCE:
			r = 0.6; g = 0.2; b = 0.4; a = 0.75;
			break;
	}

	for (auto& [thread, history] : core.usage) {
		double fraction = (history.back() / 100.00);

		fUsageBars[thread]->set_bar_color(r, g, b, a);
		fUsageBars[thread]->set_value(fraction);
	}
}
