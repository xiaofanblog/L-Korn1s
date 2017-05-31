#pragma once
#include "PluginSDK.h"
#include "BaseMenu.h"
#include "Points.h"
#include <string>

#define M_PI 3.14159265358979323846f


inline double GetCorrectDamage(IUnit* target)
{
	auto BaseDamage = static_cast<float>(GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR));
	auto CalculatedDamage = BaseDamage;
	return GDamage->CalcPhysicalDamage(GEntityList->Player(), target, CalculatedDamage);
}

inline double GetRDamage(IUnit* target)
{
	if (target == nullptr || !target->IsHero() || !target->HasBuff("willrevive"))
		return 0;

	float BaseDamage = 0;
	 
	if (Q->IsReady() && Q->ManaCost() < GEntityList->Player()->GetMana())
	{
		BaseDamage += GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
	}

	if (W->IsReady() && W->ManaCost() < GEntityList->Player()->GetMana())
	{
		BaseDamage += GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
	}

	if (E->IsReady() && E->ManaCost() < GEntityList->Player()->GetMana())
	{
		BaseDamage += GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
	}

	if (R->IsReady() && R->ManaCost() < GEntityList->Player()->GetMana())
	{
		BaseDamage += GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
	}

	if (GEntityList->Player()->HasBuff("SummonerExhaust"))
		BaseDamage *= 0.6;

	if (GEntityList->Player()->HasBuff("urgotentropypassive"))
		BaseDamage *= 0.85;

	auto bondofstoneBuffCount = target->GetBuffCount("MasteryWardenOfTheDawn");
	if (bondofstoneBuffCount > 0)
		BaseDamage *= 1 - 0.06 * bondofstoneBuffCount;

	if (target->HasBuff("FerociousHowl"))
		BaseDamage *= 0.6 - std::vector<double>({ 0.1, 0.2, 0.3 }).at(target->GetSpellBook()->GetLevel(kSlotR) - 1);

	if (target->HasBuff("Tantrum"))
		BaseDamage *= std::vector<double>({ 2, 4, 6, 8, 10 }).at(target->GetSpellBook()->GetLevel(kSlotE) - 1);

	if (target->HasBuff("BraumShieldRaise"))
		BaseDamage *= std::vector<double>({ 0.3, 0.325, 0.35, 0.375, 0.4 }).at(target->GetSpellBook()->GetLevel(kSlotE) - 1);

	if (target->HasBuff("GragasWSelf"))
		BaseDamage *= std::vector<double>({ 0.1, 0.12, 0.14, 0.16, 0.18 }).at(target->GetSpellBook()->GetLevel(kSlotW) - 1);

	auto phantomdancerBuff = target->GetBuffDataByName("itemphantomdancerdebuff");
	if (phantomdancerBuff != nullptr && GBuffData->GetCaster(phantomdancerBuff) == target)
		BaseDamage *= 0.88;

	if (target->HasBuff("GalioIdolOfDurand"))
		BaseDamage *= 0.5;

	if (target->HasBuff("GarenW"))
		BaseDamage *= 0.7;

	if (target->HasBuff("KatarinaEReduction"))
		BaseDamage *= 0.85;

	/*if (target->ChampionName() == "Moredkaiser")
		BaseDamage *= target->ManaPercent;

	if (target->HasBuff("BlitzcrankManaBarrierCD") && target->HasBuff("ManaBarrier"))
		BaseDamage *= (target->ManaPercent/2);*/
	
	return (float)BaseDamage;
}

inline bool CheckTarget(IUnit* target)
{
	if (target != nullptr && !target->IsDead() && !target->IsInvulnerable() && target->IsVisible())
	{
		return true;
	}
	return false;
}

inline string ToLower(string String)
{
	auto L = String;
	for (int i = 0; i < L[i]; i++)
	{
		L[i] = tolower(L[i]);
	}
	return L;
}

inline int Compare(const char a[], const char b[])
{
	for (int i = 0; a[i] != '\0'; i++)
		if (toupper(a[i]) != toupper(b[i]))
			return 0;

	return 1;
}

inline std::string SpellLetra(eSpellSlot Spellslot)
{
	return Spellslot == kSlotQ ? "Q" : Spellslot == kSlotW ? "W" : Spellslot == kSlotE ? "E" : Spellslot == kSlotR ? "R" : "UnKnown";
}

static bool CheckIsWard(IUnit* minion)
{
	if (strstr(minion->GetBaseSkinName(), "YellowTrinket") ||
		strstr(minion->GetBaseSkinName(), "JammerDevice") ||
		strstr(minion->GetBaseSkinName(), "SightWard") ||
		strstr(minion->GetBaseSkinName(), "ZyraSeed") ||
		strstr(minion->GetBaseSkinName(), "NidaleeSpear") ||
		strstr(minion->GetBaseSkinName(), "CaitlynTrap") ||
		strstr(minion->GetBaseSkinName(), "JhinTrap") ||
		strstr(minion->GetBaseSkinName(), "IvernTotem") ||
		strstr(minion->GetBaseSkinName(), "TeemoMushroom") ||
		strstr(minion->GetBaseSkinName(), "KalistaSpawn") ||
		strstr(minion->GetBaseSkinName(), "BardFollower") ||
		strstr(minion->GetBaseSkinName(), "BardHealthShrine") ||
		strstr(minion->GetBaseSkinName(), "TestCubeRenderwCollision") ||
		strstr(minion->GetBaseSkinName(), "ThreshLantern") || 
		strstr(minion->GetBaseSkinName(), "JarvanIVWall") ||
		strstr(minion->GetBaseSkinName(), "JarvanIVStandard") ||
		strstr(minion->GetBaseSkinName(), "IllaoiMinion"))		
	{
		return true;
	}

	return false;
}

float GetDistancePos(Vec3 from, Vec3 to)
{
	float x1 = from.x;
	float x2 = to.x;
	float y1 = from.y;
	float y2 = to.y;
	float z1 = from.z;
	float z2 = to.z;
	return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0)));
}
float GetDistance(IUnit * player, IUnit * target)
{
	auto x1 = player->GetPosition().x;
	auto x2 = target->GetPosition().x;
	auto y1 = player->GetPosition().y;
	auto y2 = target->GetPosition().y;
	auto z1 = player->GetPosition().z;
	auto z2 = target->GetPosition().z;
	return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0)));
}
bool IsUnderTurret(IUnit * Source, bool CheckAllyTurrets, bool CheckEnemyTurrets)
{
	for (auto turret : GEntityList->GetAllTurrets(CheckAllyTurrets, CheckEnemyTurrets))
	{
		if (Source->IsValidTarget(turret, 950.0f))
			return true;
	}

	return false;
}
bool PosUnderTurret(Vec3 Pos, bool CheckAllyTurrets, bool CheckEnemyTurrets)
{
	for (auto turret : GEntityList->GetAllTurrets(CheckAllyTurrets, CheckEnemyTurrets))
	{
		if (GetDistancePos(turret->GetPosition(), Pos) <= 950)
			return true;
	}

	return false;
}
int CountEnemiesInRange(float range)
{
	int enemies = 0;
	for (auto enemy : GEntityList->GetAllHeros(false, true))
	{
		if (enemy != nullptr && GEntityList->Player()->IsValidTarget(enemy, range))
			enemies++;
	}
	return enemies;
}
int CountMinionsInRange(Vec3 pos, float range)
{
	auto mingons = GEntityList->GetAllMinions(false, true, false);
	auto mingonsInRange = 0;
	for (auto mingon : mingons)
	{
		//counts enemies checking if they are enemy heroes and are within radius parameter
		if (mingon != nullptr && mingon->IsValidTarget() && !mingon->IsDead() && mingon->IsCreep())
		{
			auto mingonDistance = (mingon->GetPosition() - pos).Length();
			if (mingonDistance < range)
			{
				mingonsInRange++;
			}
		}
	}
	return mingonsInRange;
}


int CountMinionsInRange(float range)
{
	int minions = 0;
	for (auto enemy : GEntityList->GetAllMinions(false, true, false))
	{
		if (enemy != nullptr && !enemy->IsDead() && enemy->IsVisible() && enemy->IsCreep() && GEntityList->Player()->IsValidTarget(enemy, range))
			minions++;
	}
	return minions;
}
int CountKillableMinionsInRange(float range)
{
	int minionCount = 0;
	for (auto minion : GEntityList->GetAllMinions(false, true, false))
	{
		if (GEntityList->Player()->IsValidTarget(minion, range) && minion->IsVisible() && !minion->IsDead() && minion->IsCreep() && GDamage->CalcPhysicalDamage(GEntityList->Player(), minion, GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotQ)) > minion->GetHealth())
		{
			minionCount++;
		}
	}
	return minionCount;
}
int AutosToKill(IUnit * target)
{
	return target->GetHealth() / GDamage->CalcPhysicalDamage(GEntityList->Player(), target, GEntityList->Player()->PhysicalDamage());
}
inline double GetQAttackDamage(IUnit* target)
{
	std::vector<double> AttackDamage = { 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80, 90, 110, 130, 150, 170, 190, 210 };

	return AttackDamage[GEntityList->Player()->GetLevel() - 1] + GEntityList->Player()->TotalMagicDamage() * 0.5 + GEntityList->Player()->TotalPhysicalDamage();
}

inline double GetDamageTeste(IUnit* target, bool CalCulateAttackDamage = true,
	bool CalCulateQDamage = true, bool CalCulateWDamage = true,
	bool CalCulateEDamage = true, bool CalCulateRDamage = true)
{
	if (CheckTarget(target))
	{
		double Damage = 0;
		std::string ChampionName = GEntityList->Player()->ChampionName();
		std::string TargetName = target->ChampionName();		

		if(GEntityList->Player()->GetSpellSlot("SummonerDot") != kSlotUnknown)
		{
			Ignite = GPluginSDK->CreateSpell(GEntityList->Player()->GetSpellSlot("SummonerDot"), 600);
		}

		if (CalCulateAttackDamage)
		{
			Damage += GDamage->GetAutoAttackDamage(GEntityList->Player(), target, false);
		}

		if (CalCulateQDamage)
		{			
			if (ChampionName == "Ahri")
			{
				Damage += Q->IsReady() ? GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ) * 2 : 0;
			}
			else if (ChampionName == "Viktor")
			{
				Damage += GEntityList->Player()->HasBuff("ViktorPowerTransferReturn") ? GetQAttackDamage(target) : 0;
			}
			else if (ChampionName == "Vladimir")
			{
				Damage += Q->IsReady() ? GEntityList->Player()->HasBuff("vladimirqfrenzy") ? GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ) * 2 : GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ) : 0;
			}			
			else
			{
				Damage += Q->IsReady() ? GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ) : 0;
			}
		}

		if (CalCulateWDamage)
		{
			Damage += W->IsReady() ? GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW) : 0;
		}

		if (CalCulateEDamage)
		{
			Damage += E->IsReady() ? GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE) : 0;
		}

		if (CalCulateRDamage)
		{
			if (ChampionName == "Ahri")
			{
				Damage += R->IsReady() ? GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR) * 3 : 0;
			}
			else
			{
				Damage += R->IsReady() ? GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR) : 0;
			}
		}

		if (GEntityList->Player()->GetSpellSlot("SummonerDot") != kSlotUnknown  && Ignite->IsReady())
		{
			Damage += 50 + 20 * GEntityList->Player()->GetLevel() - (target->HPRegenRate() / 5 * 3);
		}

		if (TargetName == "Moredkaiser")
			Damage -= target->GetMana();

		// exhaust
		if (GEntityList->Player()->HasBuff("SummonerExhaust"))
			Damage = Damage * 0.6f;

		// blitzcrank passive
		if (target->HasBuff("BlitzcrankManaBarrierCD") && target->HasBuff("ManaBarrier"))
			Damage -= target->GetMana() / 2;

		// kindred r
		if (target->HasBuff("KindredRNoDeathBuff"))
			Damage = 0;

		// tryndamere r
		if (target->HasBuff("UndyingRage") && GBuffData->GetEndTime(GEntityList->Player()->GetBuffDataByName("UndyingRage")) - GGame->Time() > 0.3)
			Damage = 0;

		// kayle r
		if (target->HasBuff("JudicatorIntervention"))
			Damage = 0;

		// zilean r
		if (target->HasBuff("ChronoShift") && GBuffData->GetEndTime(GEntityList->Player()->GetBuffDataByName("ChronoShift")) - GGame->Time() > 0.3)
			Damage = 0;

		// fiora w
		if (target->HasBuff("FioraW"))
			Damage = 0;

		return Damage;
	}
	return 0;
}


inline bool ToggleSpeall(int Spellslote)
{
	if (GEntityList->Player()->GetSpellBook()->GetToggleState(Spellslote) == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool InFountain(IUnit* Source)
{
	if (Source->GetPosition().x < 1276 && Source->GetPosition().z < 1344) { return true; }
	if (Source->GetPosition().x > 13540 && Source->GetPosition().z > 13470) { return true; }

	return false;
}

inline bool IsUnderTurret(IUnit* source)
{
	for (auto turret : GEntityList->GetAllTurrets(false, true))
	{
		if (source->IsValidTarget(turret, 900) && turret->GetHealth() >= 1)
			return true;
	}

	return false;
}

inline bool FoundEnemies(IUnit* source, float range)
{
	for (auto enemys : GEntityList->GetAllHeros(false, true))
	{
		if (source->IsValidTarget(enemys, range))
			return true;
	}

	return false;
}

inline int CountEnemy(Vec3 Location, int range)
{
	int Count = 0;

	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if ((Enemy->GetPosition() - Location).Length() < range && Enemy->IsValidTarget() && !Enemy->IsDead())
		{
			Count++;
		}
	}
	return (Count);
}

inline int CountAlly(Vec3 Location, int range)
{
	int Count = 0;

	for (auto Ally : GEntityList->GetAllHeros(true, false))
	{
		if ((Ally->GetPosition() - Location).Length() < range && Ally->IsValidTarget() && !Ally->IsDead() && Ally != GEntityList->Player())
		{
			Count++;
		}
	}
	return (Count);
}

inline int CountCaitTrap(Vec3 Location, int range)
{
	int Count = 0;

	for (auto Ally : GEntityList->GetAllUnits())
	{
		if ((Ally->GetPosition() - Location).Length() < range && strstr(Ally->GetObjectName(), "Cupcake Trap") && Ally->IsVisible() && Ally->GetTeam() == GEntityList->Player()->GetTeam())
		{
			Count++;
		}
	}
	return (Count);
}

inline int CountWards(Vec3 Location, int range)
{
	int Count = 0;

	for (auto wards : GEntityList->GetAllUnits())
	{
		if (wards != nullptr && (wards->GetPosition() - Location).Length() < range && (wards->IsWard() || strstr(wards->GetObjectName(), "JammerDevice")) && wards->GetTeam() == GEntityList->Player()->GetTeam() &&
			wards->IsVisible())
		{
			Count++;
		}
	}
	return (Count);
}

inline int GetEnemiesInRange(float range)
{
	int enemies = 0;
	for (auto enemy : GEntityList->GetAllHeros(false, true))
	{
		if (enemy != nullptr && GEntityList->Player()->IsValidTarget(enemy, range))
			enemies++;
	}
	return enemies;
}

inline int GetMinionsInRange(Vec3 Position, float Range)
{
	auto mingons = GEntityList->GetAllMinions(false, true, false);
	auto mingonsInRange = 0;
	for (auto mingon : mingons)
	{
		//counts enemies checking if they are enemy heroes and are within radius parameter
		if (mingon != nullptr && mingon->IsValidTarget() && !mingon->IsDead() && mingon->IsVisible() && mingon->IsCreep())
		{
			auto mingonDistance = (mingon->GetPosition() - Position).Length();
			if (mingonDistance < Range)
			{
				mingonsInRange++;
			}
		}
	}
	return mingonsInRange;
}

inline bool FoundMinionsNeutral(float range)
 {
	 for (auto Minions : GEntityList->GetAllMinions(false, false, true))
	 {
		 if (GEntityList->Player()->IsValidTarget(Minions, range) && !Minions->IsDead() && Minions->IsVisible() && !strstr(Minions->GetObjectName(), "WardCorpse"))
			 return true;
	 }

	 return false;
 }

inline bool FoundMinions(float range)
 {
	 for (auto Minions : GEntityList->GetAllMinions(false, true, false))
	 {
		 if (GEntityList->Player()->IsValidTarget(Minions, range) && !Minions->IsDead() && Minions->IsVisible() && Minions->IsCreep())
			 return true;
	 }

	 return false;
 }

inline bool FoundNeutralEpic(float range)
{
	for (auto minions : GEntityList->GetAllMinions(false, false, true))
	{
		if (GEntityList->Player()->IsValidTarget(minions, range) && !minions->IsDead() && minions->IsVisible() && minions->IsWindingUp() &&
			(strstr(minions->GetObjectName(), "Dragon")
				|| strstr(minions->GetObjectName(), "Baron")
				|| strstr(minions->GetObjectName(), "Red")
				|| strstr(minions->GetObjectName(), "Blue")))
		{
			return true;
		}
	}

	return false;
}

inline int CountMinions(Vec3 Location, int range)
 {
	 int Count = 0;

	 for (auto Minions : GEntityList->GetAllMinions(false, true, false))
	 {		 
		 if ((Minions->GetPosition() - Location).Length() < range && Minions->IsValidTarget() && !Minions->IsDead() && Minions->IsVisible() &&  Minions->IsCreep())
		 {
			 Count++;
		 }
	 }
	 return (Count);
 }

inline int CountMinionsAlly(Vec3 Location, int range)
{
	int Count = 0;

	for (auto Minions : GEntityList->GetAllMinions(true, false, false))
	{
		if ((Minions->GetPosition() - Location).Length() < range && Minions->IsValidTarget() && Minions->IsVisible() && !Minions->IsDead() && Minions->IsCreep())
		{
			Count++;
		}
	}
	return (Count);
}

inline int CountMinionsWithBuff(Vec3 Location, int range, const char* buff)
{
	int Count = 0;

	for (auto Minions : GEntityList->GetAllMinions(false, true, false))
	{
		if ((Minions->GetPosition() - Location).Length() < range && Minions->IsValidTarget() && !Minions->IsDead() && Minions->HasBuff(buff))
		{
			Count++;
		}
	}
	return (Count);
}

inline int CountMinionsNeutral(Vec3 Location, int range)
{
	int Count = 0;

	for (auto Minions : GEntityList->GetAllMinions(false, false, true))
	{
		if ((Minions->GetPosition() - Location).Length() < range && Minions->IsValidTarget() && !Minions->IsDead() && Minions->IsVisible() && !strstr(Minions->GetObjectName(), "WardCorpse"))
		{
			Count++;
		}
	}
	return (Count);
}

inline float GetDistanceVectors(Vec3 from, Vec3 to)
{
	float x1 = from.x;
	float x2 = to.x;
	float y1 = from.y;
	float y2 = to.y;
	float z1 = from.z;
	float z2 = to.z;
	return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0)));
}

inline bool IsUnderTurretPos(Vec3 source)
{
	for (auto turret : GEntityList->GetAllTurrets(false, true))
	{
		if (GetDistanceVectors(turret->GetPosition(), source) < 900 && turret->GetHealth() >= 1)
			return true;
	}

	return false;
}

inline Vec3 returnNearPosTower(IUnit* target)
{
	for (auto turret : GEntityList->GetAllTurrets(false, true))
	{
		if (turret != nullptr && turret->GetHealth() >= 1 && GetDistance(target, turret) <= 1500)
		{
			return turret->GetPosition();
		}
	}

	return Vec3(0, 0, 0);
}

inline bool IsUnderTurretPosAlly(Vec3 source)
{
	for (auto turret : GEntityList->GetAllTurrets(true, false))
	{
		if (GetDistanceVectors(turret->GetPosition(), source) < 800 && turret->GetHealth() >= 1)
			return true;
	}

	return false;
}

static bool IsKeyDown(IMenuOption *menuOption)
{
	return GetAsyncKeyState(menuOption->GetInteger()) & 0x8000;
}

inline bool ValidUlt(IUnit* target)
{
	if (target->HasBuffOfType(BUFF_PhysicalImmunity) || target->HasBuffOfType(BUFF_SpellImmunity)

		|| target->IsInvulnerable() || target->HasBuffOfType(BUFF_Invulnerability) || target->HasBuff("kindredrnodeathbuff")

		|| target->HasBuffOfType(BUFF_SpellShield))
	{
		return false;
	}
	else
	{
		return true;
	}
}

inline Vec3 GetTrapPos(float range)
{

	for (auto trap : GEntityList->GetAllHeros(false, true))
	{

		if (trap->IsValidTarget() && GetDistance(GEntityList->Player(), trap) < range && (trap->HasBuff("BardRStasis") || trap->HasBuffOfType(BUFF_Invulnerability)))
		{
			return trap->GetPosition();
		}


		for (auto object : GEntityList->GetAllUnits())
		{
			if (object->IsValidObject() && GetDistance(GEntityList->Player(), object) < range)
			{

				auto name = object->GetObjectName();

				if (strstr(object->GetObjectName(), "Pantheon_Base_R_indicator_red.troy") || strstr(object->GetObjectName(), "gatemarker_red.troy") || 
					strstr(object->GetObjectName(), "global_ss_teleport_target_red.troy") ||
					strstr(object->GetObjectName(), "lifeaura") && GetDistance(trap, object) < 200 || strstr(object->GetObjectName(), "r_indicator_red.troy"))
				{
					return object->GetPosition();
				}
			}

		}
	}

	return Vec3(0, 0, 0);
}

inline bool CanMove(IUnit* target)
{
	if (target->MovementSpeed() < 50 || target->HasBuffOfType(BUFF_Stun) || target->HasBuffOfType(BUFF_Fear) || target->HasBuffOfType(BUFF_Snare) || target->HasBuffOfType(BUFF_Knockup) || target->HasBuff("Recall") ||

		target->HasBuffOfType(BUFF_Knockback) || target->HasBuffOfType(BUFF_Charm) || target->HasBuffOfType(BUFF_Taunt) || target->HasBuffOfType(BUFF_Suppression))

	{
		return false;
	}
	else
	{
		return true;
	}

}

inline bool CheckWalls(IUnit* player, IUnit* enemy)
{
	auto distance = GetDistance(player, enemy);

	for (auto i = 1; i < 6; i++)
	{		
		Vec3 position;
		Vec3 pPos = GEntityList->Player()->GetPosition();
		auto delay = E->GetDelay() + distance / E->Speed();
		GPrediction->GetFutureUnitPosition(enemy, delay, true, position);

		Vec3 PositionTarget = pPos.Extend(position, distance + 60 * i);

		if (GNavMesh->IsPointWall(PositionTarget))
		{
			return true;
		}
	}
	return false;
}

static bool CheckWallsVectores(Vec3 from, Vec3 to)
{
	auto steps = 6.f;
	auto stepLength = GetDistanceVectors(from, to) / steps;

	for (auto i = 1; i < steps + 1; i++)
	{
		auto PositionTarget = from.Extend(to, stepLength * i);
		
		if (GNavMesh->IsPointWall(PositionTarget))
		{
			return true;
		}
	}

	return false;
}

static bool CheckShielded(IUnit* target)
{
	if (!target->HasBuff("BlackShield") && !target->HasBuff("bansheesveil") && !target->HasBuff("itemmagekillerveil") && !target->HasBuffOfType(BUFF_SpellShield) &&
		!target->HasBuffOfType(BUFF_SpellImmunity) && !target->HasBuff("OlafRagnarok") && !target->HasBuff("SionR") && !target->HasBuff("UndyingRage"))
	{
		return true;
	}

	return false;
}

static Vec3 Get3DPoint(Vec2 const& Position)
{
	return Vec3(Position.x, GNavMesh->GetHeightForPoint(Position), Position.y);
}

static void DrawLine(Vec2 const Start, Vec2 const End, Vec4 const Color)
{
	Vec3 vecStart3D = Get3DPoint(Start);
	Vec3 vecEnd3D = Get3DPoint(End);

	Vec3 vecOut1;
	GGame->Projection(vecStart3D, &vecOut1);

	Vec3 vecOut2;
	GGame->Projection(vecEnd3D, &vecOut2);

	if (vecOut1.z < 1.f && vecOut2.z < 1.f)
		GRender->DrawLine(Vec2(vecOut1.x, vecOut1.y), Vec2(vecOut2.x, vecOut2.y), Color);
}

static void DrawCircleMinimap(IRender *renderer, IGame *game, const Vec3 &position, const Vec4 &color, float radius, int quality = 30)
{
	//Credits to L#.Common
	//Creds to Tsughi
	auto points = std::vector<Vec3>();
	for (auto i = 0; i < quality; i++)
	{
		auto angle = i * M_PI * 2 / quality;
		points.emplace_back(
			Vec3(
				position.x + radius * ::cosf(angle),
				position.y,
				position.z + radius * ::sinf(angle)
			)
		);
	}

	for (size_t i = 0; i < points.size(); i++)
	{
		auto a = points[i];
		auto b = points[i == points.size() - 1 ? 0 : i + 1];

		Vec2 aonScreen, bonScreen;
		game->WorldToMinimap(a, aonScreen);
		game->WorldToMinimap(b, bonScreen);

		renderer->DrawLine(aonScreen, bonScreen, color);
	}

}

static int GetWardSlot()
{
	static IInventoryItem* pWardItems[] = { nullptr };

	if (pWardItems[0] == nullptr)
	{
		int wardIds[] =
		{
			2045, 2049, 2050, 2301, 2302, 2303, 3340, 3361, 3362, 3711, 1408, 1409, 1410, 1411, 2043, 2055
		};

		for (auto i = 0; i < 16; i++)
		{
			pWardItems[i] = GPluginSDK->CreateItemForId(i, 590);
			//GGame->PrintChat(std::to_string(wardIds[i]).data());
		}
	}

	for (auto i : pWardItems)
	{
		if (i->IsOwned() && i->IsReady())
		{		
			//GGame->PrintChat(std::to_string(i->ItemId()).data());
			return i->ItemSpellSlot();
		}
	}

	return kSlotUnknown;
}

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

std::vector<Vec3> CirclePoints(float CircleLineSegmentN, float radius, Vec3 position)
{
	std::vector<Vec3> points;

	for (auto i = 1; i <= CircleLineSegmentN; i++)
	{
		auto angle = i * 2 * 3.1415927 / CircleLineSegmentN;
		auto point = Vec3(position.x + radius * static_cast<float>(std::cos(angle)), position.y + radius * static_cast<float>(std::sin(angle)), position.z);

		points.push_back(point);
	}
	return points;
}

float DegreeToRadian(double angle)
{
	return float(M_PI * angle / 180.0);
}

static int Random(int min, int max)
{
	auto r = rand() / RAND_MAX;
	return min + r * (max - min);
}

static void ProjectOn(Vec2 const& Point, Vec2 const& SegmentStart, Vec2 const& SegmentEnd, ProjectionInfo& Out)
{
	auto cx = Point.x;
	auto cy = Point.y;
	auto ax = SegmentStart.x;
	auto ay = SegmentStart.y;
	auto bx = SegmentEnd.x;
	auto by = SegmentEnd.y;

	auto rL = ((cx - ax) * (bx - ax) + (cy - ay) * (by - ay)) / (powf(bx - ax, 2) + powf(by - ay, 2));
	auto pointLine = Vec2(ax + rL * (bx - ax), ay + rL * (by - ay));
	float rS;

	if (rL < 0)
	{
		rS = 0;
	}
	else if (rL > 1)
	{
		rS = 1;
	}
	else
	{
		rS = rL;
	}

	auto isOnSegment = (rS == rL);
	auto pointSegment = isOnSegment ? pointLine : Vec2(ax + rS * (bx - ax), ay + rS * (by - ay));

	Out = ProjectionInfo(isOnSegment, pointSegment, pointLine);
}

static float Distance(Vec2 point, Vec2 start, Vec2 End, bool onlyOnSegment)
{
	ProjectionInfo projection_info;

	ProjectOn(point, start, End, projection_info);

	if (projection_info.IsOnSegment || onlyOnSegment == false)
	{
		return projection_info.SegmentPoint.DistanceToSqr(point);
	}
	return FLT_MAX;
}

inline bool IsValidTarget(IUnit* target, float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return target != nullptr && GEntityList->Player()->IsValidTarget(target, range) && GetDistanceVectors(target->GetPosition(), RangeCheckFrom) <= range;
}

inline SArray<IUnit*> EnemyMinions(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllMinions(false, true, false)).Where([&](IUnit* i) {return IsValidTarget(i, range, RangeCheckFrom); });
}
inline SArray<IUnit*> NeutralMinions(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllMinions(false, false, true)).Where([&](IUnit* i) {return IsValidTarget(i, range, RangeCheckFrom); });
}
inline SArray<IUnit*> ValidEnemies(float range = 100000, Vec3 RangeCheckFrom = GEntityList->Player()->GetPosition())
{
	return SArray<IUnit*>(GEntityList->GetAllHeros(false, true)).Where([&](IUnit* i) {return IsValidTarget(i, range, RangeCheckFrom); });
}

// Credits: Malachite
FarmLocation FindBestLineCastPosition(vector<Vec3> RangeCheckFroms, float range, float castrange, float radius, bool Minions, bool Heroes)
{
	FarmLocation result;
	result.HitCount = 0;
	for (Vec3 RangeCheckFrom : RangeCheckFroms)
	{
		auto minions = EnemyMinions(range, RangeCheckFrom);
		auto heroes = ValidEnemies(range, RangeCheckFrom);
		auto castminions = EnemyMinions(castrange, RangeCheckFrom);
		auto castheroes = ValidEnemies(castrange, RangeCheckFrom);
		SArray<IUnit*> targets;
		SArray<IUnit*> casttargets;
		if (Minions)
		{
			targets.AddRange(minions);
			casttargets.AddRange(castminions);
		}
		if (Heroes)
		{
			targets.AddRange(heroes);
			casttargets.AddRange(castheroes);
		}
		for (auto target : casttargets.ToVector())
		{
			Vec3 endpos = Extend(RangeCheckFrom, target->GetPosition(), range);
			int count = targets.Where([&](IUnit* i)
			{ return Distance(i->GetPosition(), Extend(RangeCheckFrom, endpos, -radius / 2), Extend(endpos, RangeCheckFrom, -radius / 2), true) <= radius / 2 + i->BoundingRadius(); }).Count();
			if (count > result.HitCount)
			{
				result.HitCount = count;
				result.CastPosition = endpos;
				result.CastPositionFrom = RangeCheckFrom;
				result.CastOnUnit = target;
			}
		}
	}
	return result;
}

FarmLocation FindBestLineCastPositionNeutral(vector<Vec3> RangeCheckFroms, float range, float castrange, float radius, bool Minions, bool Heroes)
{
	FarmLocation result;
	result.HitCount = 0;
	for (Vec3 RangeCheckFrom : RangeCheckFroms)
	{
		auto minions = NeutralMinions(range, RangeCheckFrom);
		auto heroes = ValidEnemies(range, RangeCheckFrom);
		auto castminions = NeutralMinions(castrange, RangeCheckFrom);
		auto castheroes = ValidEnemies(castrange, RangeCheckFrom);
		SArray<IUnit*> targets;
		SArray<IUnit*> casttargets;
		if (Minions)
		{
			targets.AddRange(minions);
			casttargets.AddRange(castminions);
		}
		if (Heroes)
		{
			targets.AddRange(heroes);
			casttargets.AddRange(castheroes);
		}
		for (auto target : casttargets.ToVector())
		{
			Vec3 endpos = Extend(RangeCheckFrom, target->GetPosition(), range);
			int count = targets.Where([&](IUnit* i)
			{ return Distance(i->GetPosition(), Extend(RangeCheckFrom, endpos, -radius / 2), Extend(endpos, RangeCheckFrom, -radius / 2), true) <= radius / 2 + i->BoundingRadius(); }).Count();
			if (count > result.HitCount)
			{
				result.HitCount = count;
				result.CastPosition = endpos;
				result.CastPositionFrom = RangeCheckFrom;
				result.CastOnUnit = target;
			}
		}
	}
	return result;
}

struct EnemyData
{
	bool is_stopped_;
	std::vector<Vec2> last_waypoint_;
	uint32_t last_waypoint_tick_;
	uint32_t stop_tick_;
	float avg_tick_;
	float avg_path_lenght_;
	uint32_t count_;
	uint32_t last_aa_tick_;
	uint32_t last_windup_tick_;
	bool is_windup_checked_;
	uint32_t orbwalk_count_;
	float avg_orbwalk_time_;
	float last_angle_diff_;
	UnitDash last_dash_info_;

	EnemyData()
		: is_stopped_(false)
		, last_waypoint_tick_(0)
		, stop_tick_(0)
		, avg_tick_(0.0f)
		, avg_path_lenght_(0.0f)
		, count_(0)
		, last_aa_tick_(0)
		, last_windup_tick_(0)
		, is_windup_checked_(false)
		, orbwalk_count_(0)
		, avg_orbwalk_time_(0.0f)
		, last_angle_diff_(0.0f)
	{

	}

	EnemyData(const std::vector<Vec2>& wp)
		: last_waypoint_(wp)
		, is_stopped_(false)
		, last_waypoint_tick_(0)
		, stop_tick_(0)
		, avg_tick_(0.0f)
		, avg_path_lenght_(0.0f)
		, count_(0)
		, last_aa_tick_(0)
		, last_windup_tick_(0)
		, is_windup_checked_(false)
		, orbwalk_count_(0)
		, avg_orbwalk_time_(0.0f)
		, last_angle_diff_(0.0f)
	{

	}
};

Vec3 To3D(Vec2 p)
{
	return Vec3(p.x, GNavMesh->GetHeightForPoint(p), p.y);
}

std::map<uint32_t, EnemyData> g_EnemyInfo;

inline float GetAvgPathLenght(IUnit *hero)
{
	return g_EnemyInfo[hero->GetNetworkId()].avg_path_lenght_;
}
