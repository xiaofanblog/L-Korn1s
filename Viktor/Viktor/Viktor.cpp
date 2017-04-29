#include "PluginSDK.h"
#include <string>
#include "Rembrandt.h"

PluginSetup("Viktor")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenu* SetQ;
IMenuOption* ComboQ;
IMenuOption* ComboQAA;
IMenu* SetW;
IMenuOption* ComboW;
IMenuOption* ComboWAlways;
IMenuOption* ComboWSlow;
IMenuOption* ComboE;
IMenu* SetR;
IMenuOption* ComboR;
IMenuOption* ComboRmin;
IMenuOption* ComboRtickdmg;
IMenuOption* ComboRkillable;
IMenuOption* ComboRfollow;
IMenuOption* ComboRfollowset;
IMenuOption* ComboRcheck;


IMenu* HarassMenu;
IMenuOption* HarassMana;
IMenuOption* HarassQ;
IMenuOption* HarassE;

IMenu* MiscMenu;
IMenuOption* AntiGapW;
IMenuOption* InterruptW;
IMenuOption* InterruptR;
IMenuOption* AutoW;

IMenu* FarmMenu;
IMenuOption* FarmMana;
IMenuOption* FarmKey;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmQJ;
IMenuOption* FarmEJ;
IMenu* FleeMenu;
IMenuOption* FleeKey;
IMenuOption* FleeQ;

IMenu* Killsteal;
IMenuOption* KSQ;
IMenuOption* KSE;

IMenu* Drawings;
IMenuOption* DrawQ;
IMenuOption* DrawW;
IMenuOption* DrawE;
IMenuOption* DrawR;
IMenuOption* DrawDamage;
IMenuOption* DrawToggle;
ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

bool Farmenable = true;
float KeyPre;

IUnit* Player;

int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(25, 255, 0, 200);



void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, (kCollidesWithNothing));
	Q->SetSkillshot(0.25f, 0.f, 1000.f, 660.f);

	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, (kCollidesWithNothing));
	W->SetSkillshot(0.25f, 300.f, 1000.f, 700.f);

	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, false, true, (kCollidesWithYasuoWall));
	E->SetSkillshot(0.25f, 80.f, 780.f, 1200.f);


	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, (kCollidesWithNothing));
	R->SetSkillshot(0.25f, 300.f, 1000.f, 700.f);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Viktor by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		SetQ = ComboMenu->AddMenu("Q Settings");
		ComboQ = SetQ->CheckBox("Use Q", true);
		ComboQAA = SetQ->CheckBox("Dont AA without Q Passive", false);
		SetW = ComboMenu->AddMenu("W Settings");
		ComboW = SetW->CheckBox("Enable W", true);
		ComboWAlways = SetW->CheckBox("Use W Always", false);
		ComboWSlow = SetW->CheckBox("Use W only if Slowed/CC'd/Immobile", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		SetR = ComboMenu->AddMenu("R Settings");
		ComboR = SetR->CheckBox("Enable R", true);
		ComboRkillable = SetR->CheckBox("Use R only if Killable with Combo", true);
		ComboRtickdmg = SetR->AddFloat("^- R Ticks to Include: ", 1, 4, 1);
		ComboRmin = SetR->AddFloat("Use R only if Hits X", 1, 5, 1);
		ComboRfollow = SetR->CheckBox("Auto R Follow", true);
		ComboRfollowset = SetR->CheckBox("^- If no enemies, move to minions", true);
		ComboRcheck = SetR->AddFloat("Dont waste R if Enemy HP lower than", 0, 500, 100);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassMana = HarassMenu->AddFloat("Mana Percent", 10, 100, 50);
		HarassQ = HarassMenu->CheckBox("Use Q", true);
		HarassE = HarassMenu->CheckBox("Use E", true);
	}
	Killsteal = MainMenu->AddMenu("Killsteal");
	{
		KSQ = Killsteal->CheckBox("Killsteal with Q", true);
		KSE = Killsteal->CheckBox("Killsteal with E", true);
	}
	MiscMenu = MainMenu->AddMenu("Misc.");
	{
		InterruptW = MiscMenu->CheckBox("Interrupt W", true);
		AntiGapW = MiscMenu->CheckBox("Anti Gap W", true);
		InterruptR = MiscMenu->CheckBox("Interrupt R", true);
		AutoW = MiscMenu->CheckBox("Auto W on CC", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmMana = FarmMenu->AddFloat("Mana Percent", 10, 100, 50);
		FarmKey = FarmMenu->AddKey("Farm Toggle", 'T');
		FarmQ = FarmMenu->CheckBox("Use Q to Last Hit", true);
		FarmE = FarmMenu->CheckBox("Use E to LaneClear", true);
		FarmEJ = FarmMenu->CheckBox("Use E to JungleClear", true);
		FarmQJ = FarmMenu->CheckBox("Use Q to JungleClear", true);
	}
	Drawings = MainMenu->AddMenu("Drawings");
	{
		DrawQ = Drawings->CheckBox("Draw Q Range", true);
		DrawW = Drawings->CheckBox("Draw W Range", false);
		DrawE = Drawings->CheckBox("Draw E Range", true);
		DrawR = Drawings->CheckBox("Draw R Range", true);
		DrawDamage = Drawings->CheckBox("Draw Damage", true);
		DrawToggle = Drawings->CheckBox("Draw Farm Toggle", true);
	}
	FleeMenu = MainMenu->AddMenu("Flee");
	{
		FleeKey = FleeMenu->AddKey("Flee key", 'G');
		FleeQ = FleeMenu->CheckBox("Use Q", true);
	}


}	
bool CanMove(IUnit* target)
{
	if (target->HasBuffOfType(BUFF_Stun) || target->HasBuffOfType(BUFF_Snare) || target->HasBuffOfType(BUFF_Fear) || target->HasBuffOfType(BUFF_Knockup) ||
		target->HasBuff("Recall") || target->HasBuffOfType(BUFF_Knockback) || target->HasBuffOfType(BUFF_Charm) || target->HasBuffOfType(BUFF_Taunt) || target->HasBuffOfType(BUFF_Suppression))
	{
		return true;
	}
	else
		return false;
}
void AutoWLogic()
{
	if (W->IsReady() && AutoW->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr)
		{
			if (CanMove(target))
			{
				W->CastOnTarget(target);
			}

		}
	}
}

void Flee()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
		if (FleeQ->Enabled() && Q->IsReady())
		{
			for (auto target : GEntityList->GetAllHeros(false, true))
			{
				if (target != nullptr && target->IsHero() && !target->IsDead())
				{
					if (target->IsValidTarget() && target->IsValidTarget(GEntityList->Player(), Q->Range()) && !target->IsDead())
					{
						Q->CastOnTarget(target);
					}
				}

			}
			for (auto Minion : GEntityList->GetAllMinions(false, true, true))
			{
				if (!Minion->IsDead() && Minion != nullptr && Minion->IsValidTarget())
				{
					if (Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						Q->CastOnTarget(Minion);
					}
				}
			}
		}
	}
}
void Killsteals()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);

		if (Enemy != nullptr && !Enemy->IsDead() & Enemy->IsValidTarget())
		{
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				Q->CastOnTarget(Enemy);
			}
			if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
			{
				if ((Enemy->ServerPosition() - GEntityList->Player()->ServerPosition()).Length2D() < E->Range() && (Enemy->ServerPosition() - GEntityList->Player()->ServerPosition()).Length2D() > 500)
				{
					auto castStartPos = GEntityList->Player()->ServerPosition().Extend(Enemy->GetPosition(), 500);
					E->SetRangeCheckFrom(castStartPos);
					E->SetFrom(castStartPos);
					AdvPredictionOutput outputfam;
					E->RunPrediction(Enemy, true, kCollidesWithYasuoWall, &outputfam);
					if (outputfam.HitChance >= kHitChanceHigh)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
						E->CastFrom(castStartPos, pred);
						E->SetFrom(Vec3(0, 0, 0));
					}
				}
				if ((Enemy->ServerPosition() - GEntityList->Player()->ServerPosition()).Length2D() < 500)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
					E->CastOnPosition(pred);
				}
			}
		}
	}
}
void Harass()
{
	if (HarassMana->GetFloat() <= GEntityList->Player()->ManaPercent())
	{
		if (E->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), E->Range()) && HarassE->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				if ((target->ServerPosition() - GEntityList->Player()->ServerPosition()).Length2D() < E->Range() && (target->ServerPosition() - GEntityList->Player()->ServerPosition()).Length2D() > 500)
				{
					auto castStartPos = GEntityList->Player()->ServerPosition().Extend(target->GetPosition(), 500);
					E->SetRangeCheckFrom(castStartPos);
					E->SetFrom(castStartPos);
					AdvPredictionOutput outputfam;
					E->RunPrediction(target, true, kCollidesWithYasuoWall, &outputfam);
					if (outputfam.HitChance >= kHitChanceHigh)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
						E->CastFrom(castStartPos, pred);
						E->SetFrom(Vec3(0, 0, 0));
					}
				}
				if ((target->ServerPosition() - GEntityList->Player()->ServerPosition()).Length2D() < 500)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
					E->CastOnPosition(pred);
				}
			}
		}
		if (HarassQ->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), Q->Range()) && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				Q->CastOnTarget(target);
			}

		}
	}
}

static double GetRDamage(IUnit* target)
{

	double rdmg = 0;
	double tickdmg = 0;
	if (GEntityList->Player()->GetSpellLevel(kSlotR) == 1)
	{
		rdmg = 100;
		tickdmg = 150;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotR) == 2)
	{
		rdmg = 175;
		tickdmg = 250;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
	{
		rdmg = 250;
		tickdmg = 350;
	}
	auto baser = GEntityList->Player()->TotalMagicDamage() * 0.5 + rdmg;
	auto rtick = GEntityList->Player()->TotalMagicDamage() * 0.6 + tickdmg;
	auto full = (rtick * ComboRtickdmg->GetFloat()) + baser;
	auto damage = GDamage->CalcMagicDamage(GEntityList->Player(), target, full);
	return damage;
}
	

inline int CountEnemy(Vec3 Location, int range)
{
	int Count = 0;

	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if ((Enemy->GetPosition() - Location).Length2D() < range && Enemy->IsValidTarget() && !Enemy->IsDead())
		{
			Count++;
		}
	}
	return (Count);
}
void FarmTog()
{
	if (GetAsyncKeyState(FarmKey->GetInteger()))
	{
		if (Farmenable == true && GGame->Time() > KeyPre)
		{
			Farmenable = false;
			KeyPre = GGame->Time() + 0.250;

		}
		if (Farmenable == false && GGame->Time() > KeyPre)
		{
			Farmenable = true;
			KeyPre = GGame->Time() + 0.250;

		}

	}
}
static int GetMinionsE(float range)
{
	auto minions = GEntityList->GetAllMinions(false, true, false);
	auto minionsInRange = 0;
	for (auto minion : minions)
	{
		if (minion != nullptr && minion->IsValidTarget() && minion->IsEnemy(GEntityList->Player()) && !minion->IsDead())
		{
			auto minionDistance = (minion->GetPosition() - GEntityList->Player()->GetPosition()).Length2D();
			if (minionDistance < range)
			{
				minionsInRange++;
			}
		}
	}
	return minionsInRange;
}
void Farm()
{
	if (Farmenable == true)
	{
		if (FarmMana->GetFloat() <= GEntityList->Player()->ManaPercent())
		{
			for (auto Minion : GEntityList->GetAllMinions(false, true, false))
			{
				if (FarmE->Enabled())
				{
					if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
					{
						Vec3 pos;
						int hit;
						if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							auto castStartPos = Minion->ServerPosition();
							E->SetRangeCheckFrom(castStartPos);
							E->SetFrom(castStartPos);
							std::vector<Vec3> CastPos;
							CastPos.push_back(Minion->ServerPosition());
							FarmLocation Farmlocation;
							Rembrandt::FindBestLineCastPosition(CastPos, E->Range(), E->Range(), E->Radius(), false, true, true, Farmlocation);
							if (Minion->IsValidTarget(GEntityList->Player(), 550))
							{
								E->CastFrom(Minion->ServerPosition(), Farmlocation.CastPosition);
								E->SetFrom(Vec3(0, 0, 0));
							}



						}
					}
				}
				if (FarmQ->Enabled())
				{
					if  (Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
					{
						if (GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) > GHealthPrediction->GetPredictedHealth(Minion, kLastHitPrediction, static_cast<int>(((Minion->ServerPosition() - GEntityList->Player()->GetPosition()).Length2D() * 1000) / Q->Speed()) - 125, static_cast<int>(Q->GetDelay() * 1000)))
						{
							Q->CastOnUnit(Minion);
						}
					}
				}
			}
		}
	}
}

void Jungle()
{
	if (Farmenable == true)
	{
		if (FarmMana->GetFloat() <= GEntityList->Player()->ManaPercent())
		{
			for (auto Minion : GEntityList->GetAllMinions(false, false, true))
			{
				if (FarmEJ->Enabled())
				{
					if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() &&  Minion->IsJungleCreep())
					{
						Vec3 pos;
						int hit;
						if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							auto castStartPos = Minion->ServerPosition();
							E->SetRangeCheckFrom(castStartPos);
							E->SetFrom(castStartPos);
							std::vector<Vec3> CastPos;
							CastPos.push_back(Minion->ServerPosition());
							FarmLocation Farmlocation;
							Rembrandt::FindBestLineCastPosition(CastPos, E->Range(), E->Range(), E->Radius(), true, false, true, Farmlocation);
							if (Minion->IsValidTarget(GEntityList->Player(), 550))
							{
								E->CastFrom(Minion->ServerPosition(), Farmlocation.CastPosition);
								E->SetFrom(Vec3(0, 0, 0));
							}



						}
					}
				}
				if (FarmQJ->Enabled())
				{
					if (Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
					{

						Q->CastOnUnit(Minion);

					}
				}
			}
		}
	}
}

void LastHit()
{
	for (auto Minion : GEntityList->GetAllMinions(false, true, false))
	{

		if (FarmQ->Enabled())
		{
			if (Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
			{
				if (GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) > GHealthPrediction->GetPredictedHealth(Minion, kLastHitPrediction, static_cast<int>(((Minion->ServerPosition() - GEntityList->Player()->GetPosition()).Length2D() * 1000) / Q->Speed()) - 125, static_cast<int>(Q->GetDelay() * 1000)))
				{
					Q->CastOnUnit(Minion);
				}
			}
		}
	}
}
void Combo()
{
	if (E->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), E->Range()) && ComboE->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			if ((target->ServerPosition() - GEntityList->Player()->ServerPosition()).Length2D() < E->Range() && (target->ServerPosition() - GEntityList->Player()->ServerPosition()).Length2D() > 500)
			{
				auto castStartPos = GEntityList->Player()->ServerPosition().Extend(target->GetPosition(), 500);
				E->SetRangeCheckFrom(castStartPos);
				E->SetFrom(castStartPos);
				AdvPredictionOutput outputfam;
				E->RunPrediction(target, true, kCollidesWithYasuoWall, &outputfam);
				if (outputfam.HitChance >= kHitChanceHigh)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
					E->CastFrom(castStartPos, pred);
					E->SetFrom(Vec3(0, 0, 0));
				}
			}
			if ((target->ServerPosition() - GEntityList->Player()->ServerPosition()).Length2D() < 500)
			{
				Vec3 pred;
				GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
				E->CastOnPosition(pred);
			}
		}
	}
	if (ComboQ->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), Q->Range()) && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			Q->CastOnTarget(target);
		}

	}
	if (ComboW->Enabled() && ComboWAlways->Enabled() && !ComboWSlow->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			Vec3 pred;
			GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
			W->CastOnPosition(pred);
		}

	}
	if (ComboW->Enabled() && !ComboWAlways->Enabled() && ComboWSlow->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			if (target->HasBuffOfType(BUFF_Slow) || target->HasBuffOfType(BUFF_Stun) || target->HasBuffOfType(BUFF_Snare) || target->HasBuffOfType(BUFF_Fear) || target->HasBuffOfType(BUFF_Knockup) || target->HasBuffOfType(BUFF_Knockback) || target->HasBuffOfType(BUFF_Charm) || target->HasBuffOfType(BUFF_Taunt) || target->HasBuffOfType(BUFF_Suppression))
			{
				Vec3 pred;
				GPrediction->GetFutureUnitPosition(target, 0.25f, true, pred);
				W->CastOnPosition(pred);
			}
		}

	}
	if (ComboR->Enabled() && !GEntityList->Player()->HasBuff("viktorchaosstormtimer"))
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), R->Range()) && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			if (!ComboRkillable->Enabled() && ComboRmin->GetFloat() <= CountEnemy(GEntityList->Player()->GetPosition(), R->Range()))
			{
				R->CastOnTarget(target);
			}
			if (ComboRkillable->Enabled() && ComboRmin->GetFloat() <= CountEnemy(GEntityList->Player()->GetPosition(), R->Range()))
			{
				double QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
				double EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
				if ((GetRDamage(target) + QDamage + EDamage) >= target->GetHealth() && target->GetHealth() > ComboRcheck->GetFloat())
				{
					R->CastOnTarget(target);
				}
			}
		}

	}


}
void RFollow()
{
	if (ComboRfollow->Enabled())
	{
		if (GEntityList->Player()->HasBuff("viktorchaosstormtimer"))
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 2000);
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), 2000) && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				R->CastOnPosition(target->GetPosition());
			}
			else
			{
				if (ComboRfollowset->Enabled())
				{
					for (auto Minion : GEntityList->GetAllMinions(false, true, false))
					{
						if (!Minion->IsDead() && Minion != nullptr && Minion->IsValidTarget())
						{
							if ((Minion->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 1500)
							{
								R->CastOnPosition(Minion->GetPosition());
							}

						}
					}
				}
			}
		}
	}
}
void dmgdraw()
{
	for (auto hero : GEntityList->GetAllHeros(false, true))
	{
		Vec2 barPos = Vec2();
		if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
		{
			auto QDamage = 0;
			if (Q->IsReady())
			{
				QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
			}

			auto EDamage = 0;
			if (E->IsReady())
			{
				EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
			}
			auto RDamage = 0;
			if (R->IsReady())
			{
				RDamage = GetRDamage(hero);
			}
			auto damage = QDamage + EDamage + RDamage;
			float percentHealthAfterDamage = max(0, hero->GetHealth() - float(damage)) / hero->GetMaxHealth();
			float yPos = barPos.y + yOffset;
			float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
			float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());
			if (!hero->IsDead() && hero->IsValidTarget())
			{
				float differenceInHP = xPosCurrentHp - xPosDamage;
				float pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

				for (int i = 0; i < differenceInHP; i++)
				{
					if (damage < hero->GetHealth())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
					}
					if (damage > hero->GetHealth())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), Color2);
					}

				}
				if (!hero->IsVisible())
				{

				}
			}
		}
	}
}

PLUGIN_EVENT(void) OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	RFollow();
	FarmTog();
	if (ComboQAA->Enabled())
	{
		if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		{
			if (!GEntityList->Player()->HasBuff("ViktorPowerTransferReturn"))
			{
				GOrbwalking->SetAttacksAllowed(false);
			}
			if (GEntityList->Player()->HasBuff("ViktorPowerTransferReturn"))
			{
				GOrbwalking->SetAttacksAllowed(true);
			}
		}
		else GOrbwalking->SetAttacksAllowed(true);
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		Farm();
		Jungle();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
	{
		LastHit();
	}
	if (GetAsyncKeyState(FleeKey->GetInteger()) & 0x8000)
	{
		Flee();
	}
	Killsteals();
	AutoWLogic();
}

PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (Args.Source != nullptr && InterruptW->Enabled() && (Args.Source->GetPosition() - GEntityList->Player()->GetPosition()).Length() < W->Range() && W->IsReady() && Args.Source->IsValidTarget())
	{
		W->CastOnTarget(Args.Source);
	}
	if (Args.Source != nullptr && InterruptR->Enabled() && (Args.Source->GetPosition() - GEntityList->Player()->GetPosition()).Length() < R->Range() && R->IsReady() && Args.Source->IsValidTarget())
	{
		if (Args.DangerLevel == kHighDanger)
			R->CastOnTarget(Args.Source);
	}
}
float GetDistance(IUnit* Player, Vec3 pos)
{
	return (Player->GetPosition() - pos).Length2D();
}
PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{
	if (Args.Source != nullptr && Args.Source != GEntityList->Player()
		&& Args.Source->IsEnemy(GEntityList->Player())
		&& AntiGapW->Enabled() && W->IsReady())
	{
		if (GetDistance(GEntityList->Player(), Args.EndPosition) <= 700)
		{
			W->CastOnPosition(Args.EndPosition);
		}

	}
}

PLUGIN_EVENT(void) OnRender()
{
	if (DrawQ->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(250, 255, 0, 255), Q->Range()); }
	if (DrawE->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(145, 255, 255, 255), E->Range()); }
	if (DrawW->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	if (DrawR->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawDamage->Enabled())
	{
		dmgdraw();
	}
	if (DrawToggle->Enabled())
	{
		static IFont* pFont = nullptr;

		if (pFont == nullptr)
		{
			pFont = GRender->CreateFont("Tahoma", 16.f, kFontWeightNormal);
			pFont->SetOutline(true);
			pFont->SetLocationFlags(kFontLocationNormal);
		}
		Vec2 pos;
		if (GGame->Projection(GEntityList->Player()->GetPosition(), &pos))
		{
			if (Farmenable == true)
			{
				std::string text = std::string("Farm ON");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
			if (Farmenable == false)
			{
				std::string text = std::string("Farm OFF");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
		}
	}
}


PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	Player = GEntityList->Player();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Viktor<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#7FFF00\"> - Loaded</font></b>");
	GGame->PrintChat("<b><font color=\"#f8a101\">Version: 0.1</font></b>");
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);


}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Viktor<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#FF0000\"> - Unloaded</font></b>");
}