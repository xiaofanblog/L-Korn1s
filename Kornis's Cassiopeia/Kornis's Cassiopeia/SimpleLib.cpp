#include "PluginSDK.h"
#include "SimpleLib.h"
#include <cmath>

namespace SimpleLib
{
	float SimpleLib::GetDistance(IUnit* source, IUnit* target)
	{
		float x1 = source->GetPosition().x;
		float x2 = target->GetPosition().x;
		float y1 = source->GetPosition().y;
		float y2 = target->GetPosition().y;
		float z1 = source->GetPosition().z;
		float z2 = target->GetPosition().z;
		return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0)));
	}

	float SimpleLib::GetDistanceVectors(Vec3 from, Vec3 to)
	{
		float x1 = from.x;
		float x2 = to.x;
		float y1 = from.y;
		float y2 = to.y;
		float z1 = from.z;
		float z2 = to.z;
		return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0)));
	}

	void SimpleLib::DrawSpellRange(ISpell2* spell, Vec4 color)
	{
		GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, spell->Range());
	}

	ISpell2* SimpleLib::LoadSkillshot(char slot, float delay, float range, float speed, float radius, eSpellType type, bool isMissile, bool isAoE, eCollisionFlags collision)
	{
		ISpell2* spell = nullptr;
		switch (slot)
		{
		case 'q':
		case 'Q':
			spell = GPluginSDK->CreateSpell2(kSlotQ, type, isMissile, isAoE, collision);
			break;
		case 'w':
		case 'W':
			spell = GPluginSDK->CreateSpell2(kSlotW, type, isMissile, isAoE, collision);
			break;
		case 'e':
		case 'E':
			spell = GPluginSDK->CreateSpell2(kSlotE, type, isMissile, isAoE, collision);
			break;
		case 'r':
		case 'R':
			spell = GPluginSDK->CreateSpell2(kSlotR, type, isMissile, isAoE, collision);
			break;
		default:
			break;
		}
		spell->SetOverrideRange(range);
		spell->SetOverrideDelay(delay);
		spell->SetOverrideSpeed(speed);
		spell->SetOverrideRadius(radius);

		return spell;
	}

	void SimpleLib::SpellClear(ISpell2* spell, int numToHit)
	{
		auto enemies = 0;
		auto pos = Vec3();
		spell->FindBestCastPosition(true, false, pos, enemies);
		if (enemies >= numToHit)
			spell->CastOnPosition(pos);
	}
}