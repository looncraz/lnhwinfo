#pragma once


#include "Globals.h"

class HWProvider;
class HWProviderClient;


class HWProviderPrivate {
public:
		void				Refresh();

private:
	friend class HWProvider;
	friend class HWProviderClient;

	explicit				HWProviderPrivate();
							~HWProviderPrivate();

		void				Register(HWProvider* provider);
		void				Unregister(HWProvider* provider);

		void				StartWatching(HWProviderClient*, CString&, CString&);
		void				StopWatching(HWProviderClient*, CString&, CString&);

		HWProvider*			FindProvider(CString&);
private:
	std::deque<HWProvider*>	fProviders;

		SStringMap<std::map<HWProviderClient*, SStringList>>
							fPendingClients;
};

extern HWProviderPrivate* gProviderManager;
