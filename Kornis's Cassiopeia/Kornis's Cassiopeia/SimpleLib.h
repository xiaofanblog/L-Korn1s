#pragma once
#include "PluginSDK.h"

namespace SimpleLib
{
	class SimpleLib
	{
	public:
		static float GetDistance(IUnit* unit1, IUnit* unit2);
		static float GetDistanceVectors(Vec3 from, Vec3 to);
		static void DrawSpellRange(ISpell2* spell, Vec4 color);
		static ISpell2* LoadSkillshot(char slot, float delay, float range, float speed, float radius, eSpellType type, bool isMissile, bool isAoE, eCollisionFlags collision);
		static void SpellClear(ISpell2* spell, int numToHit);
	};
}