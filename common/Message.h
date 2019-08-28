#pragma once

#include "Globals.h"


class HWMessage {
public:
								HWMessage	(CString& what = "");
								HWMessage	(const HWMessage&);
								HWMessage	(HWMessage&&);
								~HWMessage	();

			void				PrintToStream	() const;

			bool				HasName		(CString&) const;

			void				MakeEmpty	();
			bool				IsEmpty		() const;
			int					Count		() const;
			void				Remove		(CString&);

			SString				Flatten		() const;
			bool				Unflatten	(CString&);

			bool				Find		(CString&, bool&) const;
			bool				Find		(CString&, uint64&) const;
			bool				Find		(CString&, int64&) const;
			bool				Find		(CString&, double&) const;
			bool				Find		(CString&, SString&) const;
			bool				Find		(CString&, void**, int* len) const;
				// we allocate the data, but you own it!

			// fails if name exists
			bool				Add			(CString&, bool);
			bool				Add			(CString&, uint64);
			bool				Add			(CString&, int64);
			bool				Add			(CString&, double);
			bool				Add			(CString&, CString&);
			bool				Add			(CString&, const char*);
			bool				Add			(CString&, const void*, int len);

			// creates name if it doesn't exist
			bool				Set			(CString&, bool);
			bool				Set			(CString&, uint64);
			bool				Set			(CString&, int64);
			bool				Set			(CString&, double);
			bool				Set			(CString&, CString&);
			bool				Set			(CString&, const char*);
			bool				Set			(CString&, const void*, int len);

			// Data is stored in second string
			void				ForEach	(
									std::function<void(CString&, CString&)>&
								) const;

	SString						What;

			HWMessage&			operator = (const HWMessage& other);
			HWMessage&			operator = (HWMessage&& move);
			bool				operator == (const HWMessage& other) const;
			bool				operator != (const HWMessage& other) const;
private:
	class HWMData;
	std::unordered_map<SString, HWMData*> 	fData;
};
