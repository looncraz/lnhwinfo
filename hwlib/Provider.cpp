#include "Provider.h"
#include "ProviderClient.h"
#include "ProviderPrivate.h"


#include <gtkmm.h>


HWProvider::HWProvider	(CString& name)
	:
	HWObject	(name),
	fOnline		(false)

{
	fInterval.store(std::chrono::milliseconds(2000));

	if (gProviderManager == nullptr)
		new HWProviderPrivate();

	gProviderManager->Register(this);
}


HWProvider::~HWProvider	()
{
	gProviderManager->Unregister(this);
	SetOnline(false);
}


bool
HWProvider::IsOnline() const
{
	return fOnline.load();
}


void
HWProvider::PrintToStream(FILE* f) const
{
	#define PRINT(X...) fprintf(f, X);

	PRINT("HWProvider \"%s\" {\n", Name().c_str());

	for (auto& [what, clients] : fClientCache) {
		PRINT("\t%s - %lu watchers\n", what.c_str(), clients.size());
	}

	PRINT("}\n");
	#undef PRINT
}


void
HWProvider::SetInterval	(time_ms_t interval)
{
	fInterval.store(std::chrono::milliseconds(interval));
}


time_ms_t
HWProvider::Interval	() const
{
	return fInterval.load();
}


/*
	Provider interface
*/


void
HWProvider::SetOnline	(bool online)
{
	/*
		This is usually only called once.
	*/

	if (fOnline.load() ==  online) {
		fprintf(stderr, "HWProvider (%s) already %s!",
			Name().c_str(), online ? "online" : "offline");
		return;
	}

	fOnline.store(online);

	if (online) {
		std::lock_guard<std::mutex> _(fMutex);
		fThread = std::thread();

		fThread.detach();
	}
	// TODO: should we wait for the thread to join()?
	// I'm thinking it won't matter... even if the thread is still alive when we
 	// start another thread due to a rapid SetOnline() call for whatever reason
	// (online, offline, online... perhaps in response to some system event)
	// the dangling thread will quietly exit and not access anything without
	// using an atomic operation or grabbing the mutex.
}


void
HWProvider::Update	(CString& what, HWMessage&& message)
{	// This gets called from Refresh() with fMutex held.
	fPendingUpdates[what] = std::move(message);
}


const SStringMap<std::deque<HWProviderClient*>>&
HWProvider::ClientCache() const
{
	std::lock_guard<std::mutex> _(fMutex);
	return fClientCache;
}



void
HWProvider::AddClient	(HWProviderClient* client)
{
	std::lock_guard<std::mutex> _(fMutex);
}


void
HWProvider::RemoveClient(HWProviderClient* client)
{
	std::lock_guard<std::mutex> _(fMutex);

	auto cliter = ContainerFindItem(fClients, client);
	if (cliter == fClients.end())
		return;
	fClients.erase(cliter);

	SStringList whats;

	for (auto& [what, clients] : fClientCache) {
		// "clients" is a std::deque
		auto iter = ContainerFindItem(clients, client);
		if (clients.end() == iter)
			continue;	// we ain't got it!

		fClientCache[what].erase(iter);
		whats.push_back(what);
	}

	// remove mapped deque if it's empty after removing the client
	for (auto& what : whats)
		if (fClientCache[what].size() == 0)
			fClientCache.erase(fClientCache.find(what));
}


bool
HWProvider::StartWatching(HWProviderClient* client, CString& what)
{
	if (client == nullptr) return false;

	std::lock_guard<std::mutex> _(fMutex);

	if (fClientCache.find(what) != fClientCache.end()
		&& ContainerHasItem(fClientCache[what], client)) {
		fprintf(stderr, "Client is already listening to %s\n",
			what.c_str());
		return false;
	}

	fClientCache[what].push_back(client);
	return true;
}


void
HWProvider::StartWatching(HWProviderClient* client, const SStringList& whats)
{
	if (client == nullptr) return;

	std::lock_guard<std::mutex> _(fMutex);

	for (auto& what : whats) {
		if (fClientCache.find(what) != fClientCache.end()
			&& ContainerHasItem(fClientCache[what], client))
			continue;
		fClientCache[what].push_back(client);
	}
}


void
HWProvider::StopWatching(HWProviderClient* client, CString& what)
{
	std::lock_guard<std::mutex> _(fMutex);
	auto cliter = ContainerFindItem(fClientCache[what], client);

	if (cliter == fClientCache[what].end())
		return;	// already not watching... are you drunk?

	fClientCache[what].erase(cliter);

	if (fClientCache.size() == 0)
		fClientCache.erase(fClientCache.find(what));
}


/*
	Called from application thread!
*/
void
HWProvider::_NotifyClients()
{
	std::lock_guard<std::mutex> _(fMutex);

	for (auto& [what, value] : fPendingUpdates) {
		if (fClientCache.find(what) == fClientCache.end())
			continue; // no watchers!  Cycle waster!

		for (auto& client : fClientCache[what]) {
			client->Updated(Name(), std::move(value));
		}
	}
}
