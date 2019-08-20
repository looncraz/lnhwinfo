#include "Separator.h"


HWSeparator::HWSeparator()
{

}

HWSeparator::~HWSeparator	()
{

}

bool
HWSeparator::on_draw(const ::Cairo::RefPtr<::Cairo::Context >& cr)
{
	auto res = Gtk::Separator::on_draw(cr);

	//cr->set_line_width(2);
	cr->set_dash(std::vector<double>{2.0, 3.0}, 1.0);

	cr->set_source_rgba(0,0,0, 0.5);
	cr->move_to(0, 0);
	cr->line_to(get_width(), 0);
	cr->stroke();

	return res;
}
