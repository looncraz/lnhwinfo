#include "Object.h"


HWObject::HWObject	(CString& name)
	:
	fName(name)
{}


HWObject::~HWObject	()
{}


CString&
HWObject::Name() const
{
	return fName;
}
