#pragma once
#include "Box.h"
#include <unordered_map>
#include <functional>


class ColResult
{
public:
	std::function<void()>& Case(Box* Ct)
	{
		return CollisionMap[Ct->GetColorTrait()];
	}
	void operator[] (const Box* Ct)
	{
		auto i = CollisionMap.find(Ct->GetColorTrait());
		if (i != CollisionMap.end())
		{
			i->second();
		}
		else
		{
			def();
		}
	}

private:
	std::unordered_map<Box::ColorTrait, std::function<void()>> CollisionMap;
	std::function<void()> def = [](){};
};