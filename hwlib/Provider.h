#pragma once

#include "Globals.h"
#include "Message.h"

class HWProviderClient;

/*
	Provides system data as it comes available.
*/
class HWProvider : HWObject {
public:
									// name must be unique!
								HWProvider	(CString& name);
	virtual						~HWProvider	();

			bool				IsOnline() const;
	virtual	void				PrintToStream(FILE* = stdout) const;

			// How often Refresh() gets called!
			void				SetInterval	(time_ms_t);
			time_ms_t			Interval	() const;

	/*
		Provider interface
	*/
protected:
			void				SetOnline	(bool);
			/*
				This MUST be called before the provider will begin...
				and absolutely must be called prior to leaving the
				constructor!
			*/

			void				Update	(CString& what, const HWMessage&);

	virtual	void				Refresh	() = 0;
		/*
			Called from within a dedicated thread.
			Proper threading safety must be observed!

			Client calls are made from the application thread using
			g_timeout_add().
		*/

	const SStringMap<std::deque<HWProviderClient*>>&
								ClientCache() const;

	/*
		Provider client interface (essentially private)
	*/
protected:
	friend class HWProviderClient;
	virtual	void				AddClient	(HWProviderClient*);
	virtual	void				RemoveClient(HWProviderClient*);

	virtual	bool				StartWatching(HWProviderClient*, CString& what);
	virtual	void				StopWatching(HWProviderClient*, CString& what);

private:
	friend class HWProviderPrivate;
			void				_NotifyClients();
	virtual	void				StartWatching(HWProviderClient*,
 									const SStringList&);
private:
			std::atomic_bool		fOnline;

			std::atomic<time_ms_t>	fInterval;
			time_point_t			fLastUpdate;

			std::thread				fThread;

	mutable	std::mutex				fMutex;
	std::deque<HWProviderClient*>	fClients;

	/*
		Remember which client wants what value and make a list of pending
		updates that will be serviced from the Gtk::Application thread.
	*/
	SStringMap<std::deque<HWProviderClient*>>
									fClientCache;

	SStringMap<HWMessage>			fPendingUpdates;
};
