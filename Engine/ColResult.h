#pragma once
#include "Box.h"
#include <unordered_map>
#include <functional>
#include <map>
#include <typeindex>

using TypePair = std::pair< const std::type_index, const std::type_index>;

namespace std
{
	template <>
	struct hash<TypePair>
	{
		size_t operator()(const TypePair& Ct) const
		{
			const auto hasher = Ct.first.hash_code();
			return hasher ^ (Ct.second.hash_code() + 0x9e3779b9 + (hasher << 6) + (hasher >> 2));
		}
	};
}

class ColResult
{
public:
	std::function<void(Box& a, Box& b)>& Case(TypePair Ct)
	{
		return CollisionMap[Ct];
	}
	void Switch (Box& a, Box& b) const
	{
		auto i = CollisionMap.find({ typeid(a.GetColorTrait()),
									 typeid(b.GetColorTrait()) });
		if (i != CollisionMap.end())
		{
			
			i->second(a, b);
		}
		else
		{
			def(a, b);
		}
	}

private:
	std::unordered_map<TypePair, std::function<void(Box& a, Box& b)>> CollisionMap;
	std::function<void(Box& a, Box& b)> def = [](Box&, Box&){};
};