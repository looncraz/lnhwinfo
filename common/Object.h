#pragma once


#include "Globals.h"


class HWObject {
public:
								HWObject	(CString& name);
	virtual						~HWObject	();

			CString&			Name		() const;
private:
			SString				fName;
};
