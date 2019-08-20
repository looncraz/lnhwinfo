#include "SizeConstrainedBox.h"


SizeConstrainedBox::SizeConstrainedBox	(Gtk::Orientation orient,
							int minWidth, int maxWidth,
							int minHeight, int maxHeight)
	:
	Gtk::Box(orient)
{
	SetSizeConstraints(minWidth, maxWidth, minHeight, maxHeight);
}

SizeConstrainedBox::~SizeConstrainedBox	()
{}


int
SizeConstrainedBox::MinWidth () const
{
	return fMinWidth;
}

int
SizeConstrainedBox::MaxWidth () const
{
	return fMaxWidth;
}


int
SizeConstrainedBox::MinHeight () const
{
	return fMinHeight;
}

int
SizeConstrainedBox::MaxHeight () const
{
	return fMaxHeight;
}


void
SizeConstrainedBox::SetSizeConstraints(int minWidth, int maxWidth,
	int minHeight, int maxHeight)
{
	fMinWidth = minWidth;
	fMaxWidth = maxWidth;
	fMinHeight = minHeight;
	fMaxHeight = maxHeight;

	Gtk::Allocation alloc;

	if (fMinWidth != -1 && alloc.get_width() < fMinWidth)
		alloc.set_width(fMinWidth);
	else if (fMaxWidth != -1 && alloc.get_width() > fMaxWidth)
		alloc.set_width(fMaxWidth);

	if (fMinHeight != -1 && alloc.get_height() < fMinHeight)
		alloc.set_height(fMinHeight);
	else if (fMaxHeight != -1 && alloc.get_height() > fMaxHeight)
		alloc.set_height(fMaxHeight);

	size_allocate(alloc);
}


void
SizeConstrainedBox::on_size_allocate(Gtk::Allocation& alloc)
{
//	puts(__PRETTY_FUNCTION__);

//	printf("%i\n", alloc.get_width());

	if (fMinWidth != -1 && alloc.get_width() < fMinWidth)
		alloc.set_width(fMinWidth);
	else if (fMaxWidth != -1 && alloc.get_width() > fMaxWidth)
		alloc.set_width(fMaxWidth);

	if (fMinHeight != -1 && alloc.get_height() < fMinHeight)
		alloc.set_height(fMinHeight);
	else if (fMaxHeight != -1 && alloc.get_height() > fMaxHeight)
		alloc.set_height(fMaxHeight);
	set_allocation(alloc);
	Gtk::Box::on_size_allocate(alloc);
}
