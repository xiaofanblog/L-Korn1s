#include "Rembrandt.h"

/*				UPDATED PATCH #7.7				*/
auto Rembrandt::DamageModifierFromMasteries(IUnit* Hero, IUnit* Target) -> double
{
	double Modifier = 1;
	std::vector<HeroMastery> MyMasteryBuffer;

	if (Hero->GetMasteries(MyMasteryBuffer))
	{
		for (auto Mastery : MyMasteryBuffer)
		{
			//SORCERY: Increases ability and spell damage by 0.4 / 0.8 / 1.2 / 1.6 / 2 %
			if (Mastery.MasteryId == 40)
			{
				Modifier += (0.4 * Mastery.Points) / 100;
			}
			//DOUBLE EDGED SWORD: You deal 3% increased damage from all sources, but take 1.5% increased damage from all sources.
			else if (Mastery.MasteryId == 12)
			{
				Modifier += 0.03;
			}
			//ASSASSAIN: Grants 2% increased damage against enemy champions while no allied champions are nearby - 800 range
			else if (Mastery.MasteryId == 186)
			{
				bool IsActive = true;
				for (auto Friend : GEntityList->GetAllHeros(true, false))
				{
					if (Friend != Hero && (Hero->GetPosition() - Friend->GetPosition()).Length2D() <= 800)
					{
						IsActive = false;
						break;
					}
				}

				if (IsActive) { Modifier += 0.02; }
			}
			//MERCILESS: Grants 0.6 / 1.2 / 1.8 / 2.4 / 3 % increased damage against champions below 40 % health.
			else if (Mastery.MasteryId == 232)
			{
				if (Target->HealthPercent() < 40)
					Modifier += (0.6 * Mastery.Points) / 100;
			}
		}
	}

	//check if enemy has double edged sword
	std::vector<HeroMastery> TarMasteryBuffer;
	if (Target->GetMasteries(TarMasteryBuffer))
	{
		for (auto Mastery : TarMasteryBuffer)
		{
			//DOUBLE EDGED SWORD: You deal 3% increased damage from all sources, but take 1.5% increased damage from all sources.
			if (Mastery.MasteryId == 12)
			{
				Modifier += 0.015;
				break;
			}
		}
	}

	return Modifier;
}

auto Rembrandt::DrawDamageOnChampionHPBar(IUnit* Hero, double Damage, const char *Text, Vec4 BarColor) -> void
{
	Vec2 HPBarPos;
	if (Hero->GetHPBarPosition(HPBarPos))
	{
		Vec2 HPBarSize = Vec2(103 * (Damage / Hero->GetMaxHealth()), 8);
		if (HPBarSize.x > 103) HPBarSize.x = 103;
		HPBarPos = Vec2(HPBarPos.x + 10, HPBarPos.y += 20);

		Vec2 LinePos1 = Vec2(HPBarPos.x + HPBarSize.x, HPBarPos.y);
		Vec2 LinePos2 = Vec2(HPBarPos.x + HPBarSize.x - 5, HPBarPos.y - 7);

		GRender->DrawFilledBox(HPBarPos, HPBarSize, BarColor);
		GRender->DrawLine(LinePos1, LinePos2, Vec4(255, 255, 255, 255));
		GRender->DrawLine(LinePos1, LinePos1 + Vec2(0, 8), Vec4(255, 255, 255, 255));
		GRender->DrawTextW(LinePos2 - Vec2(13, 13), Vec4(255, 255, 255, 255), Text);
	}
}

auto Rembrandt::DrawDamageOnChampionHPBar(IUnit* Hero, double Damage, Vec4 BarColor) -> void
{
	Vec2 HPBarPos;
	if (Hero->GetHPBarPosition(HPBarPos))
	{
		Vec2 HPBarSize = Vec2(103 * (Damage / Hero->GetMaxHealth()), 8);
		if (HPBarSize.x > 103) HPBarSize.x = 103;
		HPBarPos = Vec2(HPBarPos.x + 10, HPBarPos.y += 20);

		Vec2 LinePos1 = Vec2(HPBarPos.x + HPBarSize.x, HPBarPos.y);
		Vec2 LinePos2 = Vec2(HPBarPos.x + HPBarSize.x - 5, HPBarPos.y - 7);

		GRender->DrawFilledBox(HPBarPos, HPBarSize, BarColor);
	}
}

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
auto Rembrandt::FindBestLineCastPosition(std::vector<Vec3> RangeCheckFroms, float range, float castrange, float radius, bool Minions, bool Heroes, FarmLocation& Output) -> void
{
	FarmLocation result;
	result.HitCount = 0;
	for (Vec3 RangeCheckFrom : RangeCheckFroms)
	{
		std::vector<IUnit*> targets, casttargets;

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
				if (Distance(i->GetPosition(), Extend(RangeCheckFrom, endpos, -radius / 2), Extend(endpos, RangeCheckFrom, -radius / 2), true) <= radius / 2 + i->BoundingRadius())
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

auto Rembrandt::UnitsInRange(Vec3 Position, std::vector<IUnit*> UnitsToCheck, float Range) -> int
{
	int Count = 0;
	std::for_each(UnitsToCheck.begin(), UnitsToCheck.end(), [&](IUnit* Unit)
	{
		if (!Unit->IsDead() && IsInRange(Position, Unit->GetPosition(), Range)) Count++;
	});
	return Count;
}

auto Rembrandt::CircleCircleIntersection(Vec2 center1, Vec2 center2, float radius1, float radius2) -> std::vector<Vec2>
{
	auto D = Dist2D(center1, center2);

	//The Circles dont intersect:
	if (D > radius1 + radius2 || (D <= std::abs(radius1 - radius2))) {
		return std::vector<Vec2>();
	}

	auto A = (radius1 * radius1 - radius2 * radius2 + D * D) / (2 * D);
	auto H = static_cast<float>(std::sqrt(radius1 * radius1 - A * A));
	auto Direction = (center2 - center1).VectorNormalize();
	auto PA = center1 + A * Direction;
	auto S1 = PA + H * Perp(Direction);
	auto S2 = PA - H * Perp(Direction);
	return std::vector<Vec2>({ S1, S2 });
}

auto Rembrandt::Perp(Vec2 v) -> Vec2
{
	return Vec2(-v.y, v.x);
}

auto Rembrandt::Dist2D(Vec3 from, Vec3 to) -> float
{
	return (from - to).Length2D();
}

auto Rembrandt::Dist2D(Vec2 from, Vec2 to) -> float
{
	return (from - to).Length();
}

auto Rembrandt::To2D(Vec3 p) -> Vec2
{
	return Vec2(p.x, p.z);
}

auto Rembrandt::To3D(Vec2 p) -> Vec3
{
	return Vec3(p.x, 0, p.y);
}