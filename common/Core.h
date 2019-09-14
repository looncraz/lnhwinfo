#pragma once

#include <condition_variable>

#include "Globals.h"
#include "Object.h"


class HWCore {
public:
	explicit					HWCore		();
								~HWCore	();

			void				Register	(HWObject*);
			void				Unregister	(HWObject*);

			int					CountObjects() const;
			void				ForEachObject(std::function<void(HWObject*)>);
			void				ForEachObject(std::function<void(const HWObject*)>) const;

			void				Sync();

		/*
			One or more threads may be used to broadcast the message to
			objects.  The same message may be received by multiple objects
 			concurrently without copies.
		*/
			void				BroadcastMessage(HWMessage&&);
			void				BroadcastMessage(const HWMessage&);

			void				PostMessage(CString& recipient, HWMessage&&);
			void				PostMessage(CString& recipient,
									const HWMessage&);

		/*
			Blocks until each object has returned from their MessageReceived()
 			hook for the given message.
		*/
			void				SyncBroadcastMessage(HWMessage&&);
			void				SyncBroadcastMessage(const HWMessage&);

			bool				SyncPostMessage(CString& recipient,
									HWMessage&&);
			bool				SyncPostMessage(CString& recipient,
									const HWMessage&);

private:
	mutable	std::recursive_mutex 	fObjectsMutex;
			std::recursive_mutex	fQueueMutex;
	std::condition_variable_any		fQueueCondition;

		std::vector<HWObject*>		fObjects;
	 	std::vector<HWMessage>		fMessages;
		std::thread					fThread;
};

extern HWCore* gCore;
