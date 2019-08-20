#pragma once

#include <climits>
#include <list>


#ifndef uint32
#	define uint32 unsigned long
#	define uint64 unsigned long long
#endif



/*
	HWHistory

	Specialization of std::list<T> to allow a size limit.
*/
template <typename T = float>
class HWHistory : public std::list<T> {
public:
	HWHistory (const HWHistory<T>& copy);
	HWHistory (int maxCount);
	HWHistory ();

	void 	SetMaxCount	(int);
	int		maxCount	() const;

	void push(const T& val){
		std::list<T>::push_back(val);
		if (std::list<T>::size() > fMaxCount)
			std::list<T>::pop_front();

		if (std::list<T>::size() == 1) {
			fRunningAverage = val;
		} else {
			fRunningAverage += val;
			fRunningAverage /= 2;
		}
	}

	void push(T&& val){
		std::list<T>::push_back(std::move(val));
		if (std::list<T>::size() > fMaxCount)
			std::list<T>::pop_front();

		if (std::list<T>::size() == 1) {
			fRunningAverage = val;
		} else {
			fRunningAverage += val;
			fRunningAverage /= 2;
		}
	}

	T	running_average() const {
		return fRunningAverage;
	}

private:

	void push_back(T&& val);
	void push_back(const T& val);

	mutable T	fRunningAverage;
	int			fMaxCount;
};


template <typename T>
HWHistory<T>::HWHistory(const HWHistory<T>& copy)
 :	std::list<T>(copy),
	fRunningAverage(copy.fRunningAverage),
 	fMaxCount(copy.fMaxCount)
{
}


template <typename T>
HWHistory<T>::HWHistory(int maxCount)
 : 	std::list<T>(),
	fRunningAverage(0),
 	fMaxCount(maxCount)
{
}


template <typename T>
HWHistory<T>::HWHistory()
 : std::list<T>(),
	fRunningAverage(0),
 	fMaxCount(5)
{
}
