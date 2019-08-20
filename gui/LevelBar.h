#pragma once


#include <gtkmm.h>
#include "Globals.h"

class HWLevelBar : public Gtk::DrawingArea {
public:
								HWLevelBar	();
	virtual						~HWLevelBar	();

			void 				set_value(float);
			void 				set_bar_color(float, float, float, float);
			void 				set_bar_color(const rgb_color&);

	// will change bar color if reaching mid/max.  Min is anything below mid.
	// By default, these are all the same color (blue);
			void				set_min_color(const rgb_color&, float min = 0.0);
			void				set_mid_color(const rgb_color&, float mid = 0.5);
			void				set_max_color(const rgb_color&, float max = 1.0);

	virtual	bool 				on_draw(const ::Cairo::RefPtr<
									::Cairo::Context >& cr) override;

private:
			float				fValue, fRed, fGreen, fBlue, fAlpha;
			rgb_color			fMinColor, fMidColor, fMaxColor;
			float				fMin, fMid, fMax;
			bool				fCustomColors;
};
