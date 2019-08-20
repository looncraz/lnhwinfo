#include "Separator.h"


HWSeparator::HWSeparator(int hreq)
{
	set_size_request(-1, hreq);
}

HWSeparator::~HWSeparator	()
{

}

bool
HWSeparator::on_draw(const ::Cairo::RefPtr<::Cairo::Context >& cr)
{
	cr->set_source_rgba(1,1,1,0.05);
	cr->rectangle(0, 0, get_width(), get_height());
	cr->fill();

	cr->set_line_width(1);

	cr->set_source_rgba(1,1,1,0.25);
	cr->move_to(0, 0);
	cr->line_to(get_width(), 0);
	cr->stroke();

	cr->set_source_rgba(0,0,0,1);
	cr->move_to(0, get_height());
	cr->line_to(get_width(), get_height());
	cr->stroke();

	return true;
}
