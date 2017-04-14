#pragma once
#include "PluginData.h"

class OrbManager
{
public:
	static int GetNetworkId();

	static void OnProcessSpellCast(CastedSpell const& args);
	static void OnPauseAnimation(IUnit* source);

	static IUnit* GetWObject(bool orbsOnly);

	static std::vector<Vec3> GetOrbs(bool toGrab);
	static void ProjectOn(Vec2 const& Point, Vec2 const& SegmentStart, Vec2 const& SegmentEnd, ProjectionInfo& Out);
	static float GetDistance(Vec3 from, Vec3 to);
	static float GetDistanceV2(Vec2 from, Vec2 to);

	static Vec3 GetOrbToGrab(int range);

	static float Distance(Vec2 point, Vec2 start, Vec2 End, bool onlyOnSegment);
};