#include "Object.h"
#include "Core.h"


HWObject::HWObject	(CString& name)
	:
	fName(name)
{
	gCore->Register(this);
}


HWObject::~HWObject	()
{
	gCore->Unregister(this);
}


void
HWObject::LockObject()
{
	fObjectMutex.lock();
}


void
HWObject::UnlockObject()
{
	fObjectMutex.unlock();
}


void
HWObject::MessageReceived	(const HWMessage&)
{

}


CString&
HWObject::Name() const
{
	return fName;
}
