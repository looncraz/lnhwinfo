#pragma once


#include <gtkmm.h>


class HWLevelBar : public Gtk::DrawingArea {
public:
								HWLevelBar	();
	virtual						~HWLevelBar	();

			void 				set_value(float);
			void 				set_bar_color(float, float, float, float);

	virtual	bool 				on_draw(const ::Cairo::RefPtr<
									::Cairo::Context >& cr) override;

private:
			float				fValue, fRed, fGreen, fBlue, fAlpha;
};
