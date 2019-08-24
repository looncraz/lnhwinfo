#include <array>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <unistd.h>

#include <gtkmm.h>

#include "Globals.h"
#include "Utils.h"


namespace HWUtils {


rgb_color mix_color(rgb_color color1, rgb_color color2, uint8 amount)
{
	color1.red = (uint8)(((int16(color2.red) - int16(color1.red)) * amount)
		/ 255 + color1.red);
	color1.green = (uint8)(((int16(color2.green) - int16(color1.green))
		* amount) / 255 + color1.green);
	color1.blue = (uint8)(((int16(color2.blue) - int16(color1.blue)) * amount)
		/ 255 + color1.blue );
	color1.alpha = (uint8)(((int16(color2.alpha) - int16(color1.alpha))
		* amount) / 255 + color1.alpha );

	return color1;
}


// TODO: we need to get rid of this and just require running as root
SString		gRootPassword;

bool Run(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i) {
		SString str = argv[i];
		auto start = str.find("--rootpw=");
		if (start != SString::npos) {
			gRootPassword = str.substr(start + 9, str.size() - (start + 9));
		} else {
			fprintf(stderr, "Uknown option: %s\n", argv[i]);
		}
	}

	if (gRootPassword.size() == 0)
		gRootPassword = getpass("Root Password: ");

	return gRootPassword.size() != 0;
}

std::deque<uint64> ExtractAllIntegers(SString line) {
	std::deque<uint64> result;
	SString buf;
	while (line.size() > 0) {
		while (std::isdigit(line.back())) {
			buf.insert(0, 1, line.back());
			line.pop_back();
		}
		line.pop_back();
		if (buf.size() != 0)
			result.push_front(atoi(buf.c_str()));
		buf.resize(0);
	}
	return result;
}


int ExtractTrailingInteger(SString line) {
	SString buf;
	while (std::isdigit(line.back())) {
		buf.insert(0, 1, line.back());
		line.pop_back();
	}

	if (buf.size() == 0)
		return -1;

	return atoi(buf.c_str());
}


float ExtractTrailingFloat(SString line) {
	SString buf;
	while (std::isdigit(line.back()) || line.back() == '.') {
		buf.insert(0, 1, line.back());
		line.pop_back();
	}
	return atof(buf.c_str());
}


bool ReadFileAsInt(SString path, int32* out)
{
	if (path.size() == 0 || out == nullptr)
		return false;

	std::ifstream ifile;
	SString buf;

	ifile.open(path);
	if (!getline(ifile, buf)) {
		fprintf(stderr, "Can't open \"%s\"\n", path.c_str());
		ifile.close();
		return false;
	}

	// TODO: validate file contents as integer data?
	*out = atoi(buf.c_str());
	ifile.close();
	return true;
}



bool ReadFileAsString(SString path, SString* out)
{
	if (out == nullptr)
		return false;

	std::ifstream ifile;
	ifile.open(path);
	if (!std::getline(ifile, *out)) {
		fprintf(stderr, "Unable to read file: %s\n", path.c_str());
		ifile.close();
		return false;
	}
	ifile.close();
	return true;
}



SString ShellExec	(CString& command)
{
    std::array<char, 128> buffer;
    SString result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

SString ShellRootExec(CString& _command)
{
	if (gRootPassword.size() == 0)
		fprintf(stderr, "Root password is blank!\n");

	SString command = "sh -c \"echo \"" + gRootPassword
		+ "\" | sudo -S " + _command + "\"";

	return ShellExec(command);
}


std::vector<SString> SplitString(CString& s,
	CString& separator, unsigned limit)
{
   std::vector<SString> output;

    SString::size_type prev_pos = 0, pos = 0;

	unsigned count = 0;
	while(count < limit && (pos = s.find(separator, pos)) != SString::npos)
    {
		++count;
        SString substring( s.substr(prev_pos, pos-prev_pos) );

		if (substring.size() != 0 && substring != separator) {
			output.push_back(substring);
		}

        prev_pos = ++pos;
    }

	auto last = s.substr(prev_pos, pos-prev_pos);
	if (last.size() != 0 && last != separator)
		output.push_back(last); // Last word

    return output;
}


SString StripTrailingWhitespace(CString& str)
{
	SString result = str;

	for (int i = result.size() - 1; i > 0; --i) {
		if (result[i] == ' ' || result[i] == '\n' || result[i] == '\t') {
			result.pop_back();
		} else {
			break;
		}
	}
	return result;
}

SString StripTailUntilNumber(CString& str)
{
	SString result = str;

	for (int i = result.size() - 1; i > 0; --i) {
		if (!std::isdigit(result[i])) {
			result.pop_back();
		} else {
			break;
		}
	}
	return result;
}


SString StripLeadingWhitespace(CString& str)
{
	SString result = str;

	int i = 0;
	for (; i < result.size(); ++i) {
		if (result[i] == ' ' || result[i] == '\n' || result[i] == '\t')
			break;
	}

	result = result.substr(i, result.size() - i);
	return result;
}


SString StripLeadUntilNumber(CString& str)
{
	SString result = str;

	int i = 0;
	for (; i < result.size(); ++i) {
		if (std::isdigit(result[i]))
			break;
	}

	result = result.substr(i, result.size() - i);
	return result;
}


/*
	Shamelessly borrowed from xfce4-panel
*/


#define perr(X...) fprintf(stderr, "%s: ", __PRETTY_FUNCTION__); \
	fprintf(stderr, X); fprintf(stderr, "\n");


enum
{
  STRUT_LEFT = 0,
  STRUT_RIGHT,
  STRUT_TOP,
  STRUT_BOTTOM,
  STRUT_LEFT_START_Y,
  STRUT_LEFT_END_Y,
  STRUT_RIGHT_START_Y,
  STRUT_RIGHT_END_Y,
  STRUT_TOP_START_X,
  STRUT_TOP_END_X,
  STRUT_BOTTOM_START_X,
  STRUT_BOTTOM_END_X,
  N_STRUTS
};


bool ReserveScreenSpace	(Gtk::Window* window)
{
	if (!window) {
		perr("window is NULL");
		return false;
	}

	GdkWindow* gdkwin = gtk_widget_get_window (GTK_WIDGET (window->gobj()));

	if (!GDK_IS_WINDOW(gdkwin)) {
		perr("That ends that... fuck shit");
		return false;
	}

	auto cardinal_atom = gdk_atom_intern_static_string ("CARDINAL");
	auto net_wm_strut_partial_atom = gdk_atom_intern_static_string ("_NET_WM_STRUT_PARTIAL");
	auto net_wm_strut_atom = gdk_atom_intern_static_string ("_NET_WM_STRUT");

	if (cardinal_atom == 0 || net_wm_strut_partial_atom == 0
		|| net_wm_strut_atom == 0) {
		perr("cardinal_atom or net_wm_strut[_partial]_atom are zero!");
		return false;
	}

	if  (!gtk_widget_get_realized(GTK_WIDGET(window->gobj()))) {
		perr("Window not realized!");
		return false;
	}

	gulong struts[N_STRUTS] = { 0, };
	GdkRectangle rect;
	gdk_window_get_frame_extents(gdkwin, &rect);

	struts[STRUT_RIGHT]			= rect.width;
	struts[STRUT_RIGHT_START_Y] = 0;
	struts[STRUT_RIGHT_END_Y]	= window->get_screen()->get_height();

	gdk_property_change(gdkwin, net_wm_strut_partial_atom,
		cardinal_atom, 32, GDK_PROP_MODE_REPLACE,
		(guchar*) &struts, N_STRUTS);

	gdk_property_change(gdkwin, net_wm_strut_atom,
		cardinal_atom, 32, GDK_PROP_MODE_REPLACE,
		(guchar*) &struts, 4);

	return true;
}



}; // end namespace HWUtils
