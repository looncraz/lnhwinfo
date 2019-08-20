#include "LevelBar.h"


HWLevelBar::HWLevelBar ()
	:
	Gtk::DrawingArea()
{
	fMinColor = (rgb_color){26, 102, 255, 179};
	set_min_color(fMinColor);
	set_mid_color(fMinColor);
	set_max_color(fMinColor);
	set_value(0.0f);
}

HWLevelBar::~HWLevelBar	()
{

}

void
HWLevelBar::set_value(float value)
{
	fValue = value;

	if (value < fMid)
		set_bar_color(fMinColor);
	else if (value < fMax)
		set_bar_color(fMidColor);
	else
		set_bar_color(fMaxColor);
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

void
HWLevelBar::set_bar_color(const rgb_color& color)
{
	set_bar_color(color.rf(), color.gf(), color.bf(), color.af());
}


void
HWLevelBar::set_min_color(const rgb_color& color, float min)
{
	fMinColor = color;
	fMin = min;

	if (fValue < fMid)
		queue_draw();
}


void
HWLevelBar::set_mid_color(const rgb_color& color, float mid)
{
	fMidColor = color;
	fMid = mid;
	if (fValue > mid && fValue < fMax)
		queue_draw();
}


void
HWLevelBar::set_max_color(const rgb_color& color, float max)
{
	fMaxColor = color;
	fMax = max;

	if (fValue > max)
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
