#include "Core.h"


HWCore* gCore = nullptr;


#define kTarget "___dedicatedRecipient___hwcore___"


HWCore::HWCore()
{
	gCore = this;

	fThread = std::thread(
	[this](){
		std::unique_lock <std::recursive_mutex> lock(fQueueMutex);
		while (gIsRunning && gCore != nullptr) {
			fQueueCondition.wait(lock, [this](){return fMessages.size() != 0;});

			fObjectsMutex.lock();

			for (auto& message : fMessages) {
				SString recipient;
				message.Find(kTarget, recipient);

				for (auto& object : fObjects) {
					if (recipient == "" || recipient == object->Name()) {
						object->LockObject();
						object->MessageReceived(message);
						object->UnlockObject();
					}
				}
			}

			fObjectsMutex.unlock();
			fMessages.clear();
		}
		puts("Goodbye");
	}
	);
	fThread.detach();
}


HWCore::~HWCore	()
{
	/*
		This never gets called
	*/
}


void
HWCore::Sync()
{
	while (fMessages.size() != 0)
		fQueueCondition.notify_one();
}


void
HWCore::Register	(HWObject* object)
{
	std::lock_guard<std::recursive_mutex> _(fObjectsMutex);
	fObjects.push_back(object);
}


void
HWCore::Unregister	(HWObject* object)
{
	std::lock_guard<std::recursive_mutex> _(fObjectsMutex);

	auto iter = std::find_if(fObjects.begin(), fObjects.end(),
		[object](HWObject* compare) {return object == compare;});

	if (iter != fObjects.end())
		fObjects.erase(iter);
	else
		fprintf(stderr, "HWCore:: Attempt to remove non-existing object!\n");
}


int
HWCore::CountObjects() const
{
	std::lock_guard<std::recursive_mutex> _(fObjectsMutex);
	return fObjects.size();
}


void
HWCore::ForEachObject(std::function<void(HWObject*)> func)
{
	std::lock_guard<std::recursive_mutex> _(fObjectsMutex);
	for (auto& object : fObjects)
		func(object);
}


void
HWCore::ForEachObject(std::function<void(const HWObject*)> func) const
{
	std::lock_guard<std::recursive_mutex> _(fObjectsMutex);
	for (auto& object : fObjects)
		func(object);
}

/*
							Messaging
*/

void
HWCore::BroadcastMessage(HWMessage&& message)
{
	std::unique_lock<std::recursive_mutex> _(fQueueMutex);
	fMessages.emplace_back(std::move(message));
	fQueueCondition.notify_all();
}


void
HWCore::BroadcastMessage(const HWMessage& message)
{
	std::unique_lock<std::recursive_mutex> _(fQueueMutex);
	fMessages.emplace_back(message);
	fQueueCondition.notify_all();
}


void
HWCore::PostMessage(CString& recipient, HWMessage&& message)
{
	std::unique_lock<std::recursive_mutex> _(fQueueMutex);
	message.Set(kTarget, recipient);
	fMessages.emplace_back(std::move(message));
	fQueueCondition.notify_all();
}


void
HWCore::PostMessage(CString& recipient, const HWMessage& message)
{
	PostMessage(recipient, std::move(HWMessage(message)));
}

/*
Blocks until each object has returned from their MessageReceived()
	hook for the given message.
*/
void
HWCore::SyncBroadcastMessage(const HWMessage& message)
{
	std::lock_guard<std::recursive_mutex> _(fQueueMutex);
	std::lock_guard<std::recursive_mutex> __(fObjectsMutex);

	for (auto& object : fObjects) {
		object->LockObject();
		object->MessageReceived(message);
		object->UnlockObject();
	}
}


bool
HWCore::SyncPostMessage(CString& recipient, const HWMessage& message)
{
	std::lock_guard<std::recursive_mutex> _(fQueueMutex);
	std::lock_guard<std::recursive_mutex> __(fObjectsMutex);

	for (auto& object : fObjects) {
		if (recipient == object->Name()) {
			object->LockObject();
			object->MessageReceived(message);
			object->UnlockObject();
			return true;
		}
	}

	return false;
}
