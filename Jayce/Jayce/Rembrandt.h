#pragma once
#include "PluginSDK.h"
#include "algorithm"
#include "Geometry.h"

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
	auto DamageModifierFromMasteries(IUnit* Hero, IUnit* Target) -> double;
	auto DrawDamageOnChampionHPBar(IUnit* Hero, double Damage, const char *Text, Vec4 BarColor) -> void;
	auto DrawDamageOnChampionHPBar(IUnit* Hero, double Damage, Vec4 BarColor) -> void;
	auto LengthSqr(Vec3 v) -> float;
	auto IsInRange(Vec2 PositionA, Vec2 PositionB, float Range) -> bool;
	auto IsInRange(Vec3 PositionA, Vec3 PositionB, float Range) -> bool;
	auto FindBestLineCastPosition(std::vector<Vec3> RangeCheckFroms, float range, float castrange, float radius, bool Minions, bool Heroes, FarmLocation& Output) -> void;
	auto UnitsInRange(Vec3 Position, std::vector<IUnit*> UnitsToCheck, float Range) -> int;
	auto CircleCircleIntersection(Vec2 center1, Vec2 center2, float radius1, float radius2)->std::vector<Vec2>;
	auto Perp(Vec2 v)->Vec2;
	auto Dist2D(Vec3 from, Vec3 to) -> float;
	auto Dist2D(Vec2 from, Vec2 to) -> float;
	auto To2D(Vec3 p)->Vec2;
	auto To3D(Vec2 p)->Vec3;
}

