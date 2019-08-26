#include "ProviderPrivate.h"
#include "Provider.h"
#include "ProviderClient.h"


int gProviderManagerContinue = G_SOURCE_CONTINUE;
HWProviderPrivate* gProviderManager = nullptr;


int hwpriv_ontimer(void*)
{
	if (gProviderManagerContinue == G_SOURCE_CONTINUE)
		gProviderManager->Refresh();

	return gProviderManagerContinue;
}


HWProviderPrivate::HWProviderPrivate()
{
	gProviderManager = this;
	g_timeout_add(MAX_UPDATE_RATE, &hwpriv_ontimer, this);
}


HWProviderPrivate::~HWProviderPrivate()
{
		gProviderManagerContinue = G_SOURCE_REMOVE;
		gProviderManager = nullptr;
}


void
HWProviderPrivate::Register(HWProvider* provider)
{
	fProviders.push_back(provider);

	// check if provider has any pending clients!
	auto whoiter = fPendingClients.find(provider->Name());
	if (whoiter != fPendingClients.end()) {
		// well, well, well... look who is late to the party!

		for (auto& [client, whats] : fPendingClients[provider->Name()])
			provider->StartWatching(client, whats);

		fPendingClients.erase(whoiter);
	}
}


void
HWProviderPrivate::Unregister(HWProvider* provider)
{
	auto iter = ContainerFindItem(fProviders, provider);
	if (iter != fProviders.end())
		fProviders.erase(iter);
}


void
HWProviderPrivate::StartWatching(HWProviderClient* client, CString& who,
	CString& what)
{
	auto provider = FindProvider(who);
	if (provider) {
		provider->StartWatching(client, what);
		return;
	}

	printf("Provider: \"%s\" is late to the party!\n", who.c_str());
	// SStringMap<std::map<HWProviderClient*, SStringList>>

	fPendingClients[who][client].push_back(what);
}


void
HWProviderPrivate::StopWatching(HWProviderClient* client, CString& who,
	CString& what)
{
	auto provider = FindProvider(who);
	if (provider)
		provider->StopWatching(client, what);
}


HWProvider*
HWProviderPrivate::FindProvider(CString& who)
{
	for (auto provider : fProviders) {
		if (provider->Name() == who)
			return provider;
	}

	return nullptr;
}



void
HWProviderPrivate::Refresh()
{
	for (auto provider : fProviders)
		provider->_NotifyClients();
}
