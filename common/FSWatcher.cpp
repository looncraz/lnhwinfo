#include <sys/inotify.h>
#include "FSWatcher.h"



FSWatchEvent::FSWatchEvent(CString& p, uint32 m)
	:
	path		(p),
	mask		(m),
	modified	((mask & IN_MODIFY) != 0),
	created		((mask & IN_CREATE) != 0),
	deleted		((mask & IN_DELETE) != 0),
	accessed	((mask & IN_ACCESS) != 0)
{}


FSWatchEvent::~FSWatchEvent(){}


FSWatcher* gFSWatcher = nullptr;


FSWatcher::FSWatcher	()
	:
	fThread(std::thread([&](){_EventLoop();}))
{
	gFSWatcher = this;
	fThread.detach();
}


FSWatcher::~FSWatcher	()
{}


bool
FSWatcher::WatchAll		(CString& path, Callback&& cb, uint32 mI)
{
	return Watch(path, std::move(cb), mI, IN_ACCESS | IN_CREATE | IN_DELETE
 		| IN_MODIFY);
}


bool
FSWatcher::WatchModified(CString& path, Callback&& cb, uint32 mI)
{
	return Watch(path, std::move(cb), mI, IN_MODIFY);
}


bool
FSWatcher::WatchCreated	(CString& path, Callback&& cb, uint32 mI)
{
	return Watch(path, std::move(cb), mI, IN_CREATE);
}


bool
FSWatcher::WatchDeleted	(CString& path, Callback&& cb, uint32 mI)
{
	return Watch(path, std::move(cb), mI, IN_DELETE);
}


bool
FSWatcher::WatchAccessed(CString& path, Callback&& cb, uint32 mI)
{
	return Watch(path, std::move(cb), mI, IN_ACCESS);
}


bool
FSWatcher::Watch(CString& path, Callback&& cb, uint32 mI, uint32 mask)
{
	Watched w;
	w.maxInterval = time_ms_t(mI);
	/*
		Detect proc or sysfs
	*/

	if (path.find("/sys/") != 0 && path.find("/proc/") != 0) {
		w.inotify = true;
		w.fd = inotify_init();
		int ret = inotify_add_watch(w.fd, path.c_str(), mask);
		if (ret < 0) {
			close(w.fd);
			fprintf(stderr, "Failed to watch \"%s\"\n", path.c_str());
			return false;
		}
	} else {
		if (mask != IN_MODIFY) {
			fprintf(stderr, "Sysfs or Procfs can only be watched for changes!\n");
			return false;
		}

	}

	w.path = path;
	w.mask = mask;
	w.callback = std::move(cb);

	std::lock_guard<std::recursive_mutex> _(fMutex);
	fWatched.push_back(std::move(w));
	return true;
}

#include <iostream>

void
FSWatcher::_EventLoop()
{
	struct timeval time;
	int ret = 0;
	auto ievSize = sizeof(struct inotify_event) + PATH_MAX + 1;
	/*
		We have very fast updates to worry about!
	*/
	time.tv_sec = 0;
	time.tv_usec = 50000; // 50ms timeout for select()
	/*
		This method is used because we might gain (or lose) watchers
		while this loop is active and need to start listening.
		Spinning on read() presents the issue where we only start listening
		to new watchers only after something else we're watching is active.
	*/

	bool didOne = false;

	while (gIsRunning) {
		fd_set rfds;
		int nfds = 0;
		time_ms_t timeman = time_ms_t(0);
		FD_ZERO(&rfds);

		fMutex.lock();

		if (fWatched.size() > 0) {
			fWatched.erase(
				std::remove_if(fWatched.begin(), fWatched.end(),
				[](auto& w){
					bool erase = w.fd < 0 || w.hashFail;
					if (erase)
						printf("Stopped watching \"%s\"\n", w.path.c_str());
					return erase;
				}),
				fWatched.end()
			);
		}

		for (auto& watched : fWatched) {
			if (watched.inotify) {
				FD_SET(watched.fd, &rfds);
				++nfds;
			} else
				timeman += _RefreshManual(watched);
		}
		fMutex.unlock();

		if (nfds == 0) {
			std::this_thread::sleep_for(time_ms_t(100) - timeman);
			continue;
		}

		ret = select(nfds, &rfds, NULL, NULL, &time);
		if (ret < 0) {
			fprintf(stderr, "FSWatcher select error!\n");
		} else if (ret == 0) {
			continue; // no updates
		} else {	// updates!

			bool removeAny = false;

			fMutex.lock();
			for (auto watched : fWatched) {
				if (FD_ISSET(watched.fd, &rfds)) {
					struct inotify_event* iev = (inotify_event*)malloc(ievSize);
					if (iev == nullptr)	// bad_malloc probably thrown, anyway.
						break;

					ret = read(watched.fd, iev, ievSize);
					if (ret <= 0) {
						fprintf(stderr, "read fail\n");
						delete iev;
						continue;
					}

					FSWatchEvent ev(watched.path, iev->mask);
					if (watched.callback(ev)) {
						close(watched.fd);
						watched.fd = -1;
						removeAny = true;
					}
				}
			}

			fMutex.unlock();
		}
	}
}


time_ms_t
FSWatcher::_RefreshManual	(Watched& w)
{
	auto start = std::chrono::system_clock::now();

	if (w.hashFail)	// this is tested for in our removal loop
		return time_ms_t(0);

	if (w.lastHash == "") {	// create the first hash and return
		w.lastHash = HWUtils::HashFile(w.path);

		if (w.lastHash == "")
			goto failed_hash;

		w.lastUpdated = start;
		return std::chrono::duration_cast<time_ms_t>(
			std::chrono::system_clock::now() - start);
	}

	if (start - w.lastUpdated > time_ms_t(w.maxInterval)) {
		w.lastUpdated = start;
		SString data;
		if (!HWUtils::ReadFileAsString(w.path, &data))
			goto failed_hash;

		SString hash = HWUtils::HashString(data);
		if (hash == "")
			goto failed_hash;

		if (w.lastHash != hash) {
			FSWatchEvent ev(w.path, IN_MODIFY);
			ev.contents = std::move(data);
			w.lastHash = std::move(hash);
			if (!w.callback(ev))
				w.fd = -1;
		}
	}

	// return how much time we spent refreshing
	return std::chrono::duration_cast<time_ms_t>(
		std::chrono::system_clock::now() - start);

	failed_hash:
	w.hashFail = true;
	return std::chrono::duration_cast<time_ms_t>(
		std::chrono::system_clock::now() - start);

}
