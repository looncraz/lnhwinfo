#include "ProviderClient.h"
#include "ProviderPrivate.h"
#include "Provider.h"


HWProviderClient::HWProviderClient	(CString& name)
	:
	HWObject(name)
{}


HWProviderClient::~HWProviderClient	()
{}


bool
HWProviderClient::IsWatching(CString& who, CString& what)
{
	if (fWatching.find(who) != fWatching.end()
		&& ContainerHasItem(fWatching[who], what))
		return true;

	return false;
}


void
HWProviderClient::StartWatching	(CString& who, CString& what)
{
	if (!IsWatching(who, what)) {
		gProviderManager->StartWatching(this, who, what);
		fWatching[who].push_back(what);
	}
}


void
HWProviderClient::StopWatching	(CString& who, CString& what)
{
	if (fWatching.find(who) != fWatching.end()) {
		auto iter = ContainerFindItem(fWatching[who], what);
		if (iter != fWatching[who].end()) {
			gProviderManager->StopWatching(this, who, what);

			fWatching[who].erase(iter);
		}
	}
}


const SStringMap<SStringList>&
HWProviderClient::Watching	() const
{
	return fWatching;
}
