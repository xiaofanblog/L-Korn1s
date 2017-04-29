#pragma once
#include "PluginSDK.h"
#include <algorithm>


struct FarmLocation
{
	Vec3 CastPosition;
	Vec3 CastPositionFrom;
	int HitCount = 0;
	IUnit* CastOnUnit = nullptr;
	std::vector<IUnit*> UnitsHit;
};

namespace Rembrandt
{
	auto LengthSqr(Vec3 v) -> float;
	auto IsInRange(Vec2 PositionA, Vec2 PositionB, float Range) -> bool;
	auto IsInRange(Vec3 PositionA, Vec3 PositionB, float Range) -> bool;
	auto FindBestLineCastPosition(std::vector<Vec3> RangeCheckFroms, float range, float castrange, float radius, float jungle, bool Minions, bool Heroes, FarmLocation& Output) -> void;

}
