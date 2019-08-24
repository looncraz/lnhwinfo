#pragma once

#include <gtkmm.h>
#include "ClockWidget.h"
#include "CPUStatusBox.h"
#include "UPSWidget.h"
#include "SizeConstrainedBox.h"

class HWMainWindow : public Gtk::Window {
public:
								HWMainWindow	();
	virtual						~HWMainWindow	();


private:

	virtual	void				on_show			()		override;
	virtual void				on_size_allocate(Gtk::Allocation&)	override;
	virtual void				on_realize() override;

	virtual	bool				on_button_click(GdkEventButton*);

			SizeConstrainedBox	fBox;
			HWClockWidget		fClock;
			HWCPUStatusBox		fCPUStatus;
			HWUPSWidget			fUPSWidget;
};
