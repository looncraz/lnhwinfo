#pragma once

#include <climits>
#include <string>
#include <utility>
#include <vector>
#include <gtkmm.h>

#include "Globals.h"


namespace HWUtils {

	#ifndef uint64
	#define uint64 unsigned long long
	#endif

	// for detecting "--rootpw=password"
	void						Run(int argc, char** argv);

	std::deque<uint64> 			ExtractAllIntegers		(std::string);
	int 						ExtractTrailingInteger	(std::string);
	float						ExtractTrailingFloat	(std::string);

	std::string					ShellExec	(const std::string& command);
	std::string					ShellRootExec(const std::string& command);

	std::vector<std::string>	SplitString	(const std::string& string,
									const std::string& delim, unsigned limit = UINT_MAX);

	std::string					StripTrailingWhitespace(const std::string&);

	/*
		Modify X11 screen struts to prevent windows from being maximized
		and taking over a portion of the screen (an edge).
	*/
	bool 						ReserveScreenSpace	(Gtk::Window* window);

	bool 						UnreserveScreenSpace (Gtk::Window* window);


}; // end namespace HWUtils
