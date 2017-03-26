#pragma once
#include "PluginSDK.h"
#include "Template.h"
#include "cmath"
#define PI 3.14159265f
using namespace std;

#pragma region extension
//linqq ne`
template<class T>
vector<T> Add(vector<T> vec, T i)
{
	vector<T> newvec;
	newvec = vec;
	newvec.push_back(i);
	return newvec;
}
template<class T>
vector<T> AddRange(vector<T> vec, vector<T> vecToAdd)
{
	vector<T> newvec;
	newvec = vec;
	newvec.insert(newvec.end(), vecToAdd.begin(), vecToAdd.end());
	return newvec;
}
template<class T>
vector<T> RemoveAll(vector<T> vec, function<bool(T)> removefunc)
{
	vector<T> newvec;
	for each (T i in vec)
	{
		if (!removefunc(i))
			newvec.push_back(i);
	}
	return newvec;
}
template<class T>
vector<T> Where(vector<T> vec, function<bool(T)> wherefunc)
{
	vector<T> newvec;
	for each (T i in vec)
	{
		if (wherefunc(i))
			newvec.push_back(i);
	}
	return newvec;
}
template<class T>
bool Any(vector<T> vec, function<bool(T)> anyfunc)
{
	vector<T> newvec;
	newvec = vec;
	newvec = Where(newvec, anyfunc);
	return !newvec.empty();
}
template<class T>
bool Any(vector<T> vec)
{
	vector<T> newvec;
	newvec = vec;
	return !newvec.empty();
}
template<class T, class T2>
vector<T2> Select(vector<T> vec, function<T2(T)> selectfunc)
{
	vector<T> newvec;
	newvec = vec;
	vector<T2> returnvec;
	for each (T i in newvec)
	{
		returnvec.push_back(selectfunc(i));
	}
	return returnvec;
}
template<class T>
T FirstOrDefault(vector<T> vec, function<bool(T)> fst_function)
{
	vector<T> newvec;
	newvec = vec;

	newvec = Where(newvec, fst_function);
	if (newvec.empty())
		return T();
	return newvec.front();
}
template<class T>
T LastOrDefault(vector<T> vec, function<bool(T)> lst_function)
{
	vector<T> newvec;
	newvec = vec;

	newvec = Where(newvec, lst_function);
	if (newvec.empty())
		return T();
	return newvec.back();
}
template<class T, class T2>
T MinOrDefault(vector<T> vec, function<T2(T)> min_function)
{
	vector<T> newvec;
	newvec = vec;
	T returnelem;
	returnelem = T();
	if (newvec.empty())
		return returnelem;
	returnelem = newvec.begin();
	for each (T i in newvec)
	{
		if (min_function(i) < min_function(returnelem))
			returnelem = i;
	}
	return returnelem;
}
template<class T, class T2>
T MaxOrDefault(vector<T> vec, function<T2(T)> max_function)
{
	vector<T> newvec;
	newvec = vec;
	T returnelem;
	returnelem = T();
	if (newvec.empty())
		return returnelem;
	returnelem = newvec.begin();
	for each (T i in newvec)
	{
		if (max_function(i) < max_function(returnelem))
			returnelem = i;
	}
	return returnelem;
}
inline string SpellSlotToString (eSpellSlot slot)
{
	return slot == kSlotQ ? "Q" : slot == kSlotW ? "W" : slot == kSlotE ? "E" : slot == kSlotR ? "R" : "UnKnown";
}
// keydown
inline bool IsKeyDown(IMenuOption* menuOption)
{
	return (GetAsyncKeyState(menuOption->GetInteger()) & 0x8000) != 0;
}
//select target
inline IUnit* SelectTarget(eDamageType type, float range)
{
	return GTargetSelector->FindTarget(QuickestKill, type, range);
}
// get selected
inline IUnit* GetSelectedTarget()
{
	return GTargetSelector->GetFocusedTarget();
}
//String viet thuong
inline string ToLower(string StringToLower)
{
	auto Lowered = StringToLower;
	for (int i = 0; i < Lowered[i]; i++)
	{
		Lowered[i] = tolower(Lowered[i]);
	}
	return Lowered;
}
// string bao gom string khac
inline bool Contains(string Container, string Contained, bool Lower = true)
{
	auto LoweredContainer = Lower ?  ToLower(Container) : Container;
	auto LoweredContained = Lower?  ToLower(Contained) : Contained;
	if (LoweredContainer.find(LoweredContained) != string::npos) return true;
	else return false;
}
inline bool Equals(string a , string b)
{
	if (strcmp(a.c_str(), b.c_str()) == 0)
		return true;
	return false;
}
//isward
inline bool IsWard(IUnit* target)
{
	Vec2 pos;
	return Contains(target->GetObjectName(), "ward") && !Contains(target->GetObjectName(), "corpse");
}
//lay khoang cach ne may ban
inline Vec2 ToVec2(Vec3 vec)
{
	return Vec2(vec.x, vec.z);
}
inline Vec3 ToVec3(Vec2 vec)
{
	return Vec3(vec.x, 0, vec.y);
}
inline Vec3 Normalize(Vec3 x)
{
	float sqr = x.x * x.x + x.y * x.y + x.z * x.z;
	return x * (1.0f / sqrt(sqr));
}
inline Vec3 Pendicular (Vec3 x)
{
	auto X1 = ToVec2(x);
	Vec2 X2;
	X2.x = -X1.y;
	X2.y = X1.x;
	return ToVec3(X2);
}

inline float Distance(Vec3 from, Vec3 to)
{
	return (from - to).Length2D();
}
inline float AngleBetween(Vec3 a, Vec3 center, Vec3 c)
{
	float a1 = Distance(c, center);
	float b1 = Distance(a, c);
	float c1 = Distance(center, a);
	if (a1 == 0 || c1 == 0) { return 0; }
	else
	{
		return acos((a1 * a1 + c1 * c1 - b1 * b1) / (2 * a1 * c1)) * (180 / PI);
	}
}
inline float AngleToRadian(float Angle)
{
	return Angle * PI / 180.f;
}
inline Vec3 RotateAround(Vec3 pointToRotate3D, Vec3 centerPoint3D, float angleInDegree)
{
	auto angleInRadians = AngleToRadian(angleInDegree);
	double cosTheta = cos(angleInRadians);
	double sinTheta = cos(angleInRadians);
	Vec2 pointToRotate = ToVec2(pointToRotate3D);
	Vec2 centerPoint = ToVec2(centerPoint3D);
	Vec2 vec2Return
	(
		(cosTheta * (pointToRotate.x - centerPoint.x) -
		sinTheta * (pointToRotate.y - centerPoint.y) + centerPoint.x),

		(sinTheta * (pointToRotate.x - centerPoint.x) +
			cosTheta * (pointToRotate.y - centerPoint.y) + centerPoint.y)
	);
	return ToVec3(vec2Return);
}
inline bool InTheCone(Vec3 pos, Vec3 centerconePolar, Vec3 centerconeEnd, float coneAngle)

{

	return AngleBetween(pos, centerconePolar, centerconeEnd) < coneAngle / 2

		&& Distance(pos, centerconePolar) < Distance(centerconePolar, centerconeEnd);

}
inline float Distance(Vec3 point, Vec3 segmentStart, Vec3 segmentEnd, bool onlyIfOnSegment = false)
{
	auto a = ToVec2(point);
	auto b = ToVec2(segmentStart);
	auto c = ToVec2(segmentEnd);
	float STriangle = 0.5f * abs((b.x - a.x)*(c.y - a.y) - (c.x - a.x)*(b.y - a.y));
	float botLength = Distance(segmentStart, segmentEnd);
	float distance = 2 * STriangle / botLength;
	if (!onlyIfOnSegment)
		return distance;
	if (AngleBetween(point, segmentStart, segmentEnd) > 90 || AngleBetween(point, segmentEnd, segmentStart) > 90)
		return 100000.f;
	return distance;
}
inline float Distance(IUnit* from, IUnit* to)
{
	return (from->GetPosition() - to->GetPosition()).Length2D();
}
inline float Distance(IUnit* from, Vec3 to)
{
	return (from->GetPosition() - to).Length2D();
}
inline float Distance(Vec2 from, Vec2 to)
{
	return (from - to).Length();
}
//dich chuyen ex- vector tend
inline Vec3 Extend(Vec3 from, Vec3 to, float distance)
{
	float realDistance = (from - to).Length() * distance / (from - to).Length2D();
	auto direction = (to - from).VectorNormalize();
	return from + direction * realDistance;
}
inline SArray<Vec3> GetCircleCircleIntersections(Vec3 center1, Vec3 center2, float radius1, float radius2)
{
	SArray<Vec3> result;
	float D = Distance(center2, center1);
	//The Circles dont intersect:
	if (D > radius1 + radius2 || (D <= abs(radius1 - radius2)))
	{
		return result;
	}

	float A = (radius1 * radius1 - radius2 * radius2 + D * D) / (2 * D);
	float H = sqrt(radius1 * radius1 - A * A);
	Vec3 Direction = (center2 - center1).VectorNormalize();
	Vec3 PA = ToVec3(ToVec2(center1) + A * ToVec2(Direction));
	Vec3 S1 = ToVec3(ToVec2(PA) + H * ToVec2(Pendicular(Direction)));
	Vec3 S2 = ToVec3((ToVec2(PA) - H * ToVec2(Pendicular(Direction))));
	result.Add(S1);
	result.Add(S2);
	return result;
}
inline SArray<IUnit*> ValidTargets(vector<IUnit*> input)
{
	SArray<IUnit*> targets = SArray<IUnit*>(input);
	return  targets.Where([](IUnit* i) {return i != nullptr && i->IsValidTarget(); });
}
inline SArray<IUnit*> ValidAllies(vector<IUnit*> input)
{
	SArray<IUnit*> targets = SArray<IUnit*>(input);
	return  targets.Where([](IUnit* i) {return i != nullptr && !i->IsDead(); });
}
inline bool IsValidTarget(IUnit* target, float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return target != nullptr && GEntityList->Player()->IsValidTarget(target, range) && Distance(target, RangeCheckFrom) <= range;
}
inline bool IsValidAllies(IUnit* target, float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return target != nullptr && !target->IsDead() && Distance(target, RangeCheckFrom) <= range;
}
inline bool IsValidBoth(IUnit* target, float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return (target->IsEnemy(GEntityList->Player()) && IsValidTarget(target, range, RangeCheckFrom))
		|| (!target->IsEnemy(GEntityList->Player()) && IsValidAllies(target, range, RangeCheckFrom));
}
inline SArray<IUnit*> EnemyMinions(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllMinions(false, true, false)).Where([&](IUnit* i) {return IsValidTarget(i, range, RangeCheckFrom) && !IsWard(i) && !Contains(i->GetObjectName(), "barrel"); });
}
inline SArray<IUnit*> AllyMinions(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllMinions(true, false, false)).Where([&](IUnit* i) {return IsValidAllies(i, range, RangeCheckFrom) && !IsWard(i) && !Contains(i->GetObjectName(), "barrel"); });
}
inline SArray<IUnit*> NeutralMinions(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllMinions(false, false, true)).Where([&](IUnit* i) {return IsValidTarget(i, range, RangeCheckFrom) && !IsWard(i) && !Contains(i->GetObjectName(), "barrel"); });
}
inline SArray<IUnit*> AllMinions(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllMinions(true, true, true)).Where([&](IUnit* i) {return IsValidBoth(i, range, RangeCheckFrom) && !IsWard(i) && !Contains(i->GetObjectName(), "barrel"); });
}
inline SArray<IUnit*> WardMinions(float range = 100000, bool jungle = true, bool ally = true, bool enemy = true, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	SArray<IUnit*> allwards = SArray<IUnit*>(GEntityList->GetAllMinions(true, true, true))
		.Where([&](IUnit* i) {return IsValidBoth(i, range, RangeCheckFrom) && IsWard(i); });
	SArray<IUnit*> junglewards = allwards.Where([](IUnit* i) {return i->IsJungleCreep(); });
	SArray<IUnit*> enemywards = allwards.Where([](IUnit* i) {return GEntityList->Player()->IsEnemy(i); });
	SArray<IUnit*> allywards = allwards.Where([](IUnit* i) {return !GEntityList->Player()->IsEnemy(i); });
	SArray<IUnit*> returnwards;
	if (jungle) returnwards.AddRange(junglewards);
	if (ally) returnwards.AddRange(allywards);
	if (enemy) returnwards.AddRange(enemywards);
	return returnwards;
}
inline int CountEnemiesInRange(Vec3 Position, float Range)
{
	vector<IUnit*> a;
	a = SArray<IUnit*>(GEntityList->GetAllHeros(false, true)).Where([&](IUnit* i) { return
		IsValidTarget(i) && Distance(i->GetPosition(), Position) <= Range; }).ToVector();
	return a.size();
}
inline int CountMinionsInRange(Vec3 Position, float Range, bool enemy = true, bool neutral = false, bool ally = false)
{
	int i = 0;
	if (enemy) i += EnemyMinions().Where([&](IUnit* i) {return Distance(i, Position) <= Range; }).Count();
	if (neutral) i += NeutralMinions().Where([&](IUnit* i) {return Distance(i, Position) <= Range; }).Count();
	if (ally) i += AllyMinions().Where([&](IUnit* i) {return Distance(i, Position) <= Range; }).Count();
	return i;
}
inline SArray<IUnit*> ValidEnemies(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllHeros(false, true)).Where([&](IUnit* i) {return IsValidTarget(i, range, RangeCheckFrom); });
}
inline SArray<IUnit*> ValidAllies(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllHeros(true, false)).Where([&](IUnit* i) {return IsValidAllies(i, range, RangeCheckFrom); });
}
inline SArray<IUnit*> ValidAllHeroes(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllHeros(true, true)).Where([&](IUnit* i) {return IsValidBoth(i, range, RangeCheckFrom); });
}
inline bool InAutoAttackRange(IUnit* target)
{
	return Distance(GEntityList->Player(), target) <= GEntityList->Player()->BoundingRadius() + GEntityList->Player()->AttackRange() + target->BoundingRadius();
}
inline bool InSpellRange(ISpell2* spell, IUnit* target)
{
	return Distance(GEntityList->Player(), target) <= spell->Range();
}
inline bool InSpellRange(ISpell2* spell, Vec3 position)
{
	return Distance(GEntityList->Player(), position) <= spell->Range();
}
inline double GetSpellDamage(IUnit* target, int slot)
{
	return GDamage->GetSpellDamage(GEntityList->Player(), target, slot);
}
inline bool HasSummonerSpell(ISpell*  spell)
{
	return spell->GetSpellSlot() != kSlotUnknown && spell->IsReady();
}
inline void CastItemOnUnit(int itemid, float range, IUnit* target)
{
	if (GEntityList->Player()->HasItemId(itemid))
	{
		IInventoryItem* Item = GPluginSDK->CreateItemForId(itemid, range);
		if (IsValidTarget(target) && Item->IsReady())
		{
			Item->CastOnTarget(target);
		}
		if (target == nullptr)
		{
			Item->CastOnPlayer();
		}
	}
}
inline bool IsCCed (IUnit * hero)
{
	return hero->HasBuffOfType(BUFF_Stun) || hero->HasBuffOfType(BUFF_Snare) || hero->HasBuffOfType(BUFF_Suppression) || hero->HasBuffOfType(BUFF_Charm) || hero->HasBuffOfType(BUFF_Snare);
}
inline bool IsEnoughMana(IMenuOption* Slider)
{
	return GEntityList->Player()->ManaPercent() >= Slider->GetInteger();
}
void FindBestLineCastPosition(vector<Vec3> RangeCheckFroms, float range, float radius, bool Minions, bool Heroes, Vec3& CastPosition , int& HitCounts, Vec3& CastPositionFrom)
{
	HitCounts = 0;
	for (Vec3 RangeCheckFrom : RangeCheckFroms)
	{
		auto minions = EnemyMinions(range, RangeCheckFrom);
		auto heroes = ValidEnemies(range, RangeCheckFrom);
		SArray<IUnit*> targets;
		if (Minions)
			targets.AddRange(minions);
		if (Heroes)
			targets.AddRange(heroes);
		for (auto target : targets.ToVector())
		{
			Vec3 endpos = Extend(RangeCheckFrom, target->GetPosition(), range);
			int count = targets.Where([&](IUnit* i) 
				{ return Distance(i->GetPosition(), Extend(RangeCheckFrom, endpos, -radius/2), Extend(endpos, RangeCheckFrom, - radius/2), true) <= radius / 2 + i->BoundingRadius(); }).Count();
			if (count > HitCounts)
			{
				HitCounts = count;
				CastPosition = endpos;
				CastPositionFrom = RangeCheckFrom;
			}
		}
	}

}
inline Vec4 Red() { return Vec4(255, 0, 0, 255); }
inline Vec4 Green() { return Vec4(0, 255, 0, 255); }
inline Vec4 Pink() { return Vec4(255, 0, 255, 255); }
inline Vec4 Yellow() { return Vec4(255, 255, 0, 255); }

inline bool IsWallBetween (Vec3 start, Vec3 end)
{
	for (int i = 0; i < Distance(start, end); i = i + 25)
	{
		auto flag = GNavMesh->GetCollisionFlagsForPoint(Extend(start, end, i));
		if (flag&kWallMesh || flag&kBuildingMesh)
		{
			return true;
		}
	}
	auto endflag = GNavMesh->GetCollisionFlagsForPoint(end);
	if (endflag&kWallMesh || endflag&kBuildingMesh)
	{
		return true;
	}
	return false;
}
inline bool IsGoingToWard(IUnit* ThisUnit, IUnit* ToUnit)
{
	Vec3 pred;
	GPrediction->GetFutureUnitPosition(ThisUnit, 0.2f, false, pred);
	if (Distance(ToUnit, pred) < Distance(ToUnit, ThisUnit))
	{
		return true;
	}
	return false;
}
inline bool IsInAutoAttackRange(Vec3 position)
{
	return GEntityList->Player()->GetRealAutoAttackRange(GEntityList->Player()) >= Distance(GEntityList->Player(), position);
}
inline bool IsInAutoAttackRange(IUnit* target)
{
	return GEntityList->Player()->GetRealAutoAttackRange(target) >= Distance(GEntityList->Player(), target);
}
#pragma endregion 

#pragma  region  Delayaction
struct Action
{
	Action(int time, std::function<void()> callback)
	{
		Time = time + GGame->TickCount();
		CallbackObject = callback;
	}

	void Call()
	{
		CallbackObject();
	}

	std::function<void()> CallbackObject;
	int Time;

};
struct ActionIUnit
{
	ActionIUnit(int time, IUnit* unit, std::function<void(IUnit*)> callback)
	{
		Time = time + GGame->TickCount();
		CallbackObject = callback;
		Unit = unit;
	}

	void Call()
	{
		if (Unit != nullptr)
			CallbackObject(Unit);
	}

	std::function<void(IUnit*)> CallbackObject;
	int Time;
	IUnit* Unit;
};
struct ActionPosition
{
	ActionPosition(int time, Vec3 position, std::function<void(Vec3)> callback)
	{
		Time = time + GGame->TickCount();
		CallbackObject = callback;
		Position = position;
	}

	void Call()
	{
		CallbackObject(Position);
	}

	std::function<void(Vec3)> CallbackObject;
	int Time;
	Vec3 Position;
};

class DelayAction
{
public:
	void OnGameUpdate()
	{
		Actions.erase(std::remove_if(Actions.begin(), Actions.end(), [](Action& Args)
		{
			if (GGame->TickCount() >= Args.Time)
			{
				Args.Call();
				return true;
			}

			return false;
		}), Actions.end());
		ActionsIunit.erase(std::remove_if(ActionsIunit.begin(), ActionsIunit.end(), [](ActionIUnit& Args)
		{
			if (GGame->TickCount() >= Args.Time)
			{
				Args.Call();
				return true;
			}

			return false;
		}), ActionsIunit.end());
		ActionsPosition.erase(std::remove_if(ActionsPosition.begin(), ActionsPosition.end(), [](ActionPosition& Args)
		{
			if (GGame->TickCount() >= Args.Time)
			{
				Args.Call();
				return true;
			}

			return false;
		}), ActionsPosition.end());
	}

	void Add(int Time, std::function<void()> Callback)
	{
		Actions.emplace_back(Time, Callback);
	}
	void AddIUnit(int Time, IUnit* Unit, std::function<void(IUnit*)> Callback)
	{
		ActionsIunit.emplace_back(Time, Unit, Callback);
	}
	void AddPosition(int Time, Vec3 Position, std::function<void(Vec3)> Callback)
	{
		ActionsPosition.emplace_back(Time, Position, Callback);
	}
private:
	std::vector<Action> Actions;
	std::vector<ActionIUnit> ActionsIunit;
	std::vector<ActionPosition> ActionsPosition;
};
#pragma endregion