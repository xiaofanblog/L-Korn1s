#pragma once
#include "PluginSDK.h"
#include <algorithm>

using namespace std;

#pragma region template
template <class T>
class SArray {
public:
	SArray() {};
	SArray(vector<T> Svector) { elems = Svector; };
public:
	vector<T> elems;     // elements 

public:
	SArray<T> Add(T);
	SArray<T> RemoveAll(std::function<bool(T)>);
	SArray<T> RemoveAll()
	{
		elems.clear();
		return SArray<T>(elems);
	}
	SArray<T> Where(std::function<bool(T)>);
	SArray<T> AddRange(SArray<T>);
	SArray<T> AddRange(vector<T>);
	bool Any(std::function<bool(T)>);
	bool Any()
	{
		return !elems.empty();
	}
	vector<T> ToVector()
	{
		return elems;
	}
	template<class T2>
	SArray<T2> Select(std::function<T2(T)>);
	T FirstOrDefault(std::function<bool(T)>);
	T FirstOrDefault()
	{
		if (elems.empty())
			return T();
		return elems.front();
	};
	T LastOrDefault(std::function<bool(T)>);
	T LastOrDefault()
	{
		if (elems.empty())
			return T();
		return elems.back();
	};
	template <class T2>
	T MinOrDefault(std::function<T2(T)>);
	template <class T2>
	T MaxOrDefault(std::function<T2(T)>);
	int Count()
	{
		return elems.size();
	}
	template <class T2>
	SArray<T> OrderBy(std::function<T2(T)>);
};


template <class T>
SArray<T> SArray<T>::Add(T elem)
{
	elems.push_back(elem);
	return SArray<T>(elems);
}
template <class T>
SArray<T> SArray<T>::AddRange(SArray<T> elemstoadd)
{
	vector<T> vec = elemstoadd.elems;
	elems.insert(elems.end(), vec.begin(), vec.end());
	return SArray<T>(elems);
}
template <class T>
SArray<T> SArray<T>::AddRange(vector<T> vec)
{
	elems.insert(elems.end(), vec.begin(), vec.end());
	return SArray<T>(elems);
}


template <class T>
SArray<T> SArray<T>::RemoveAll(std::function<bool(T)> Remove)
{
	elems.erase(std::remove_if(elems.begin(), elems.end(), [&](T i)
	{
		return Remove(i);
	}), elems.end());
	return SArray(elems);
}

template<class T>
SArray<T> SArray<T>::Where(std::function<bool(T)> Where)
{
	vector<T> newvec = elems;
	newvec.erase(std::remove_if(newvec.begin(), newvec.end(), [&](T i)
	{
		return !Where(i);
	}), newvec.end());
	return SArray(newvec);
}

template<class T>
bool SArray<T>::Any(std::function<bool(T)> Any)
{
	return std::find_if(elems.begin(), elems.end(), [&](T i) {return Any(i); }) != elems.end();
}

template<class T>
template<class T2>
SArray<T2> SArray<T>::Select(std::function<T2(T)> Select)
{
	vector<T2> newelems;
	for each (T i in elems)
	{
		newelems.push_back(Select(i));
	}
	return SArray<T2>(newelems);
}

template <class T>
T SArray<T>::FirstOrDefault(std::function<bool(T)> FirstOrDefault)
{
	vector<T> newvec;
	newvec = this->Where([&](T i) {return FirstOrDefault(i); }).elems;
	if (newvec.empty())
		return T();
	return newvec.front();
}

template <class T>
T SArray<T>::LastOrDefault(std::function<bool(T)> LastOrDefault)
{
	vector<T> newvec;
	newvec = this->Where([&](T i) {return LastOrDefault(i); }).elems;
	if (newvec.empty())
		return T();
	return newvec.back();
}

template <class T>
template <class T2>
T SArray<T>::MinOrDefault(std::function<T2(T)> MinOrDefault)
{
	vector<T> newvec;
	newvec = elems;
	T returnelem;
	returnelem = T();
	if (newvec.empty())
		return returnelem;
	std::sort(newvec.begin(), newvec.end(), [&](T a, T b) { return MinOrDefault(a) < MinOrDefault(b); });
	returnelem = newvec.front();
	return returnelem;
}

template <class T>
template <class T2>
T SArray<T>::MaxOrDefault(std::function<T2(T)> MaxOrDefault)
{
	vector<T> newvec;
	newvec = elems;
	T returnelem;
	returnelem = T();
	if (newvec.empty())
		return returnelem;
	std::sort(newvec.begin(), newvec.end(), [&](T a, T b) { return MaxOrDefault(a) < MaxOrDefault(b); });
	returnelem = newvec.back();
	return returnelem;
}

template<class T>
template<class T2>
SArray<T> SArray<T>::OrderBy(std::function<T2(T)> OrderBy)
{
	std::sort(elems.begin(), elems.end(), [&](T a, T b) { return OrderBy(a) < OrderBy(b); });
	return SArray<T>(elems);
}


#pragma  endregion 

class FarmLocation
{
public:
	Vec3 CastPosition;
	Vec3 CastPositionFrom;
	int HitCount = 0;
	IUnit* CastOnUnit = nullptr;
	FarmLocation(Vec3 castposition, Vec3 castpositionfrom, int hitcount, IUnit* castonunit)
	{
		CastPosition = castposition;
		CastPositionFrom = castpositionfrom;
		HitCount = hitcount;
		CastOnUnit = castonunit;
	}
	FarmLocation() {};
};