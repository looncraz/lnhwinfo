#include "LevelBar.h"


HWLevelBar::HWLevelBar ()
	:
	Gtk::DrawingArea(),
 	fValue(0.0f),
	fRed(0.1),
 	fGreen(0.4),
	fBlue(1.0),
	fAlpha(0.7)
{
}

HWLevelBar::~HWLevelBar	()
{

}

void
HWLevelBar::set_value(float value)
{
	fValue = value;
	queue_draw();
}


void
HWLevelBar::set_bar_color(float r, float g, float b, float a)
{
	fRed = r;
	fGreen = g;
	fBlue = b;
	fAlpha = a;
	queue_draw();
}


bool
HWLevelBar::on_draw(const ::Cairo::RefPtr<::Cairo::Context >& cr)
{
	float width = (get_width() * fValue) - 1;

	cr->set_line_width(get_height());

	float midH = (float)get_height() / 2.0f;

	cr->set_source_rgba(0, 0, 0, 1);
	cr->move_to(0, midH);
	cr->line_to(get_width(), midH);
	cr->stroke();

	cr->set_line_width(get_height() - 2);
	cr->set_source_rgba(0.2, 0.2, 0.2, 1);
	cr->move_to(1, midH);
	cr->line_to(get_width() - 1, midH);
	cr->stroke();

	if (fValue > 0.0f) {
		cr->set_line_width(get_height() - 2);
		cr->set_source_rgba(fRed, fGreen, fBlue, fAlpha);
		cr->move_to(1, midH);
		cr->line_to(width, midH);
		cr->stroke();
	}
	return true;
}
