#pragma once

#include <gtkmm.h>


class HWMainWindow;


class HWApplication : public Gtk::Application {
public:
								HWApplication	();
	virtual						~HWApplication	();

protected:
	virtual	void				on_activate		() override;

private:
			HWMainWindow*		fMainWindow;

};


extern HWApplication* g_hwapp;