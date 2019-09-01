#pragma once

#include "Globals.h"


class FSWatchEvent {
public:
		SString						path;
		uint32						mask;		//raw Inotify mask
		bool						modified;
		bool						created;
		bool						deleted;
		bool						accessed;

		SString						contents;	// only filled for proc/sysfs
private:
	friend class FSWatcher;
									FSWatchEvent(CString&, uint32);
									~FSWatchEvent();
};


/*
	Callbacks are called from a dedicated watcher thread.
*/
class FSWatcher {
	using Callback = std::function<bool(const FSWatchEvent&)>;
		// return false to stop watching.
public:
	explicit						FSWatcher	();
									~FSWatcher	();

#define FPARAMS CString& path, Callback&& obvious, uint32 maxInterval

			bool					WatchAll		(FPARAMS);
			bool					WatchModified	(FPARAMS);
			bool					WatchCreated	(FPARAMS);
			bool					WatchDeleted	(FPARAMS);
			bool					WatchAccessed	(FPARAMS);

			bool					Watch			(FPARAMS, uint32 mask = 0);
private:
			void					_EventLoop		();
	struct Watched {
		SString						path;
		bool						inotify;
		int							fd;
		uint32						mask;
		Callback					callback;
		time_point_t				lastUpdated;
		SString						lastHash;
		bool						hashFail;
		time_ms_t					maxInterval; // only for sysfs/procfs

		Watched() : inotify(false), fd(0), mask(0), hashFail(false) {}
	};

				time_ms_t			_RefreshManual	(Watched&);

		std::recursive_mutex		fMutex;
		std::deque<Watched>			fWatched;
		std::thread					fThread;
};

extern FSWatcher* gFSWatcher;
