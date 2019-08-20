#pragma once

#include <gtkmm.h>


class HWSeparator : public Gtk::Separator {
public:
								HWSeparator		(int hreq = 2);
	virtual						~HWSeparator	();

	virtual bool				on_draw(const ::Cairo::RefPtr<
									::Cairo::Context >& cr) override;

};
