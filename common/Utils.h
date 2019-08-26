#pragma once

#include <climits>
#include <string>
#include <utility>
#include <vector>
#include <gtkmm.h>

#include "Globals.h"


namespace HWUtils {

	rgb_color mix_color(rgb_color color1, rgb_color color2, uint8 amount);

	// for detecting "--rootpw=password"
	bool					Run(int argc, char** argv);

	std::deque<uint64>		ExtractAllIntegers		(SString);
	int						ExtractTrailingInteger	(SString);
	float					ExtractTrailingFloat	(SString);

	bool					ReadFileAsInt(SString path, int32* out);
	bool					ReadFileAsString(SString path,
								SString* out);


	SString					ShellExec	(CString& command);
	SString					ShellRootExec(CString& command);

	std::vector<SString>	SplitString	(CString& string, CString& delim,
								unsigned limit = UINT_MAX);

	SString					StripTrailingWhitespace(CString&);
	SString					StripTailUntilNumber(CString&);

	SString					StripLeadingWhitespace(CString&);
	SString					StripLeadUntilNumber(CString&);

	void					ReplaceAll(SString&, CString& from, CString& to);

	bool					ParsePaths(const SStringList&, SStringList* out,
								const SStringMap<SString>& subs = {});

	/*
		Modify X11 screen struts to prevent windows from being maximized
		and taking over a portion of the screen (an edge).
	*/
	bool					ReserveScreenSpace	(Gtk::Window* window);

	bool					UnreserveScreenSpace (Gtk::Window* window);


}; // end namespace HWUtils
