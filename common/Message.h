#pragma once

#include "Globals.h"


class HWMessage {
public:
								HWMessage	(CString& what = "");
								~HWMessage	();

			void				PrintToStream	() const;

			bool				HasName		(CString&) const;

			void				MakeEmpty	();
			void				Remove		(CString&);

			SString				Flatten		() const;
			bool				Unflatten	(CString&);

			bool				FindBool	(CString&, bool&) const;
			bool				FindInteger	(CString&, uint64&) const;
			bool				FindInteger	(CString&, int64&) const;
			bool				FindFloat	(CString&, double&) const;
			bool				FindString	(CString&, SString&) const;
			bool				FindData	(CString&, void**, int* len) const;
				// we allocate the data, but you own it!

			// fails if name exists
			bool				AddBool		(CString&, bool);
			bool				AddInteger	(CString&, uint64);
			bool				AddInteger	(CString&, int64);
			bool				AddFloat	(CString&, double);
			bool				AddString	(CString&, CString&);
			bool				AddData		(CString&, const void*, int len);

			// creates name if it doesn't exist
			bool				SetBool		(CString&, bool);
			bool				SetInteger	(CString&, uint64);
			bool				SetInteger	(CString&, int64);
			bool				SetFloat	(CString&, double);
			bool				SetString	(CString&, CString&);
			bool				SetData		(CString&, const void*, int len);

			// Data is stored in second string
			void				ForEach	(
									std::function<void(CString&, CString&)>&
								) const;

	SString						What;

private:
	class HWMData;
	std::unordered_map<SString, HWMData*> 	fData;
};
