#pragma once


#include <gtkmm.h>


class SizeConstrainedBox : public Gtk::Box {
public:
								SizeConstrainedBox	(	Gtk::Orientation,
														int minWidth = -1,
														int maxWidth = -1,
														int minHeight = -1,
														int maxHeight = -1 );
	virtual						~SizeConstrainedBox	();

			int					MinWidth () const;
			int					MaxWidth () const;
			int					MinHeight() const;
			int					MaxHeight()	const;

			void				SetSizeConstraints	(	int minWidth = -1,
														int maxWidth = -1,
														int minHeight = -1,
														int maxHeight = -1 );
	virtual	void				on_size_allocate(Gtk::Allocation&) override;


private:
			int					fMinWidth,	fMaxWidth,
								fMinHeight,	fMaxHeight;
};
