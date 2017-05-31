#include "Rembrandt.h"
#include "Geometrya.h"

/*				UPDATED PATCH #7.7				*/
auto Rembrandt::LengthSqr(Vec3 v) -> float
{
	return (v.x * v.x + v.y * v.y + v.z * v.z);
}

//Range functions that are lightweight and optimized
auto Rembrandt::IsInRange(Vec2 PositionA, Vec2 PositionB, float Range) -> bool
{
	Vec2 DistanceVector = Vec2(PositionA.x - PositionB.x, PositionA.y - PositionB.y);

	return DistanceVector.x * DistanceVector.x + DistanceVector.y * DistanceVector.y - Range * Range < 0;
}

auto Rembrandt::IsInRange(Vec3 PositionA, Vec3 PositionB, float Range) -> bool
{
	Vec2 DistanceVector = Vec2(PositionA.x - PositionB.x, PositionA.z - PositionB.z);

	return DistanceVector.x * DistanceVector.x + DistanceVector.y * DistanceVector.y - Range * Range < 0;
}

// Credits: LeagueSharp Farm Protocols
auto Rembrandt::FindBestLineCastPosition(std::vector<Vec3> RangeCheckFroms, float range, float castrange, float radius, float jungle, bool Minions, bool Heroes, FarmLocation& Output) -> void
{
	FarmLocation result;
	result.HitCount = 0;
	for (Vec3 RangeCheckFrom : RangeCheckFroms)
	{
		std::vector<IUnit*> targets, casttargets;

		if (jungle)
		{
			for (auto Minion : GEntityList->GetAllMinions(false, false, true))
			{
				if (!Minion->IsValidTarget() || Minion->IsWard())
					continue;

				if (IsInRange(RangeCheckFrom, Minion->GetPosition(), range)) targets.push_back(Minion);
				if (IsInRange(RangeCheckFrom, Minion->GetPosition(), castrange)) casttargets.push_back(Minion);
			}
		}
		if (Minions)
		{
			for (auto Minion : GEntityList->GetAllMinions(false, true, false))
			{
				if (!Minion->IsValidTarget() || Minion->IsWard())
					continue;

				if (IsInRange(RangeCheckFrom, Minion->GetPosition(), range)) targets.push_back(Minion);
				if (IsInRange(RangeCheckFrom, Minion->GetPosition(), castrange)) casttargets.push_back(Minion);
			}
		}

		if (Heroes)
		{
			for (auto Champ : GEntityList->GetAllHeros(false, true))
			{
				if (!Champ->IsValidTarget() || Champ->IsClone())
					continue;

				if (IsInRange(RangeCheckFrom, Champ->GetPosition(), range)) targets.push_back(Champ);
				if (IsInRange(RangeCheckFrom, Champ->GetPosition(), castrange)) casttargets.push_back(Champ);
			}
		}

		for (auto target : casttargets)
		{
			Vec3 endpos = RangeCheckFrom.Extend(target->GetPosition(), range);
			std::vector<IUnit*> UnitsHitHolder;
			int count = 0;
			for (auto i : targets)
			{
				if (Distancea(i->GetPosition(), Extenda(RangeCheckFrom, endpos, -radius / 2), Extenda(endpos, RangeCheckFrom, -radius / 2), true) <= radius / 2 + i->BoundingRadius())
				{
					count++;
					UnitsHitHolder.push_back(i);
				}
			}
			if (count > result.HitCount)
			{
				result.HitCount = count;
				result.CastPosition = endpos;
				result.CastPositionFrom = RangeCheckFrom;
				result.CastOnUnit = target;
				result.UnitsHit = UnitsHitHolder;
			}
		}
	}
	Output = result;
}