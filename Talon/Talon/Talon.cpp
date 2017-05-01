#include "PluginSDK.h"
#include <string>

PluginSetup("Talon by Kornis")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboGap;
IMenuOption* ComboMode;
IMenuOption* ComboModeChange;
IMenuOption* ComboW;
IMenuOption* ComboQAA;
IMenuOption* ComboRmin;
IMenu* Rset;
IMenuOption* ComboR;
IMenuOption* ComboRkill;



IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassMana;
IMenuOption* HarassW;
IMenuOption* HarassGap;
IMenuOption* HarassQAA;


IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;
IMenuOption* DrawMode;
IMenuOption* DrawDamage;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmW;
IMenuOption* FarmMana;
IMenu* FarmMenuJ;
IMenuOption* FarmQJ;
IMenuOption* FarmWJ;
IMenuOption* FarmManaJ;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSE;

IMenu* LastMenu;
IMenuOption* LastQ;
IMenuOption* LastW;

IMenu* FleeMenu;
IMenuOption* FleeKey;
IMenu* MiscMenu;
IMenuOption* InterruptQ;

int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(25, 255, 0, 200);


ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;


IInventoryItem* Youmuus;

IUnit* Player;

float Keypress;
void LoadSpells()
{
	Youmuus = GPluginSDK->CreateItemForId(3142, 0);
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, (kCollidesWithNothing));
	Q->SetOverrideRange(500);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, (kCollidesWithNothing));
	W = GPluginSDK->CreateSpell2(kSlotW, kConeCast, true, false, (kCollidesWithYasuoWall));
	W->SetSkillshot(0.25f, 80.f, 2300.f, 800.f);

	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, (kCollidesWithYasuoWall));

}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Talon by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboMode = ComboMenu->AddSelection("Combo Mode", 0, { "Q>W>R", "W>Q>R", "R>Q>W" });
		ComboModeChange = ComboMenu->AddKey("Mode Change", 'T');
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboGap = ComboMenu->CheckBox("Use Q to GapClose", true);
		ComboQAA = ComboMenu->CheckBox("Use Q AA Reset if AA range", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		Rset = ComboMenu->AddMenu("R Settings");
		ComboR = Rset->CheckBox("Enable R", true);
		ComboRkill = Rset->CheckBox("Use R only if Killable", true);
		ComboRmin = Rset->AddFloat("Use R only if X Enemies in Range", 1, 5, 1);


	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassMana = HarassMenu->AddInteger("Mana Percent for Harass", 10, 100, 50);
		HarassQ = HarassMenu->CheckBox("Use Q", true);
		HarassGap = HarassMenu->CheckBox("Use Q to GapClose", true);
		HarassQAA = HarassMenu->CheckBox("Use Q AA Reset if AA range", true);
		HarassW = HarassMenu->CheckBox("Use W", true);
	}

	FarmMenu = MainMenu->AddMenu("Lane Clear");
	{
		FarmMana = FarmMenu->AddInteger("Mana Percent for LaneClear", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Lane Clear with Q", true);
		LastQ = FarmMenu->CheckBox("Last Hit with Q", true);
		FarmW = FarmMenu->CheckBox("Lane Clear with W", true);
		FarmE = FarmMenu->AddFloat("^- if Hits ", 1, 6, 3);
	}
	FarmMenuJ = MainMenu->AddMenu("Jungle Clear");
	{
		FarmManaJ = FarmMenuJ->AddInteger("Mana Percent for JungleClear", 10, 100, 50);
		FarmQJ = FarmMenuJ->CheckBox("Jungle Clear with Q", true);
		FarmWJ = FarmMenuJ->CheckBox("Jungle Clear with W", true);
	}
	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawDamage = DrawingMenu->CheckBox("Draw Damage", true);
		DrawMode = DrawingMenu->CheckBox("Draw Current Mode", true);
	}
	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Killsteal with Q", true);
		KSE = KillstealMenu->CheckBox("Killsteal with W", true);
	}
	FleeMenu = MainMenu->AddMenu("Flee");
	{
		FleeKey = FleeMenu->AddKey("Flee with E", 'G');
	}
}


void ChangeMode()
{
	if (GetAsyncKeyState(ComboModeChange->GetInteger()) && !GGame->IsChatOpen())
	{
		if (ComboMode->GetInteger() == 0 && GGame->Time() > Keypress)
		{
			ComboMode->UpdateInteger(1);
			Keypress = GGame->Time() + 0.25;
		}
		if (ComboMode->GetInteger() == 1 && GGame->Time() > Keypress)
		{
			ComboMode->UpdateInteger(2);
			Keypress = GGame->Time() + 0.25;
		}
		else if (GGame->Time() > Keypress)
			{
				ComboMode->UpdateInteger(0);
				Keypress = GGame->Time() + 0.25;
			}
	
	}
}
int EnemiesInRange(IUnit* Source, float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto target : Targets)
	{
		if (target != nullptr && !target->IsDead())
		{
			auto flDistance = (target->GetPosition() - Source->GetPosition()).Length();
			if (flDistance < range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}
static double Qmax(IUnit* target)
{
	if (target == nullptr || !target->IsValidTarget() || target->HasBuffOfType(BUFF_SpellShield) || target->HasBuffOfType(BUFF_SpellImmunity))
		return 0;
	double psvlvl = 0;
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 1)
	{
		psvlvl = 60;
	}
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 2)
	{
		psvlvl = 85;
	}
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 3)
	{
		psvlvl = 110;
	}
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 4)
	{
		psvlvl = 135;
	}
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 5)
	{
		psvlvl = 160;
	}

	auto psvdmgad = (GEntityList->Player()->BonusDamage()*1.10) + psvlvl;
	auto damage = GDamage->CalcPhysicalDamage(GEntityList->Player(), target, psvdmgad);
	return damage;

}
static double Qmin(IUnit* target)
{
	if (target == nullptr || !target->IsValidTarget() || target->HasBuffOfType(BUFF_SpellShield) || target->HasBuffOfType(BUFF_SpellImmunity))
		return 0;
	double psvlvl = 0;
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 1)
	{
		psvlvl = 60;
	}
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 2)
	{
		psvlvl = 85;
	}
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 3)
	{
		psvlvl = 110;
	}
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 4)
	{
		psvlvl = 135;
	}
	if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) == 5)
	{
		psvlvl = 160;
	}

	auto psvdmgad = (GEntityList->Player()->BonusDamage()*1.10) + psvlvl;
	auto melee = (psvdmgad * 0.5) * (1 + GEntityList->Player()->CritDamageMultiplier());
	auto helalmoney = psvdmgad + melee;
	auto damage = GDamage->CalcPhysicalDamage(GEntityList->Player(), target, helalmoney);
	return damage;

}
void Combo()
{ 
	if (ComboMode->GetInteger() == 0)
	{
		if (Youmuus->IsOwned() && Youmuus->IsReady() && !(Player->IsDead()))
		{
			if (EnemiesInRange(Player, Q->Range()) > 0)
			{
				Youmuus->CastOnPlayer();
			}
		}
		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				if (!ComboQAA->Enabled() && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
				{
					Q->CastOnTarget(target);
				}
				if (ComboGap->Enabled() && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 250)
				{
					Q->CastOnTarget(target);
				}
			}
		}
		if (ComboW->Enabled() && W->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead() && !Q->IsReady())
			{

				W->CastOnTarget(target);

			}
		}
		if (ComboR->Enabled() && R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range()-10);
			if (target != nullptr && target->IsValidTarget() && !target->IsDead() && !W->IsReady())
			{
				if (!ComboRkill->Enabled())
				{
					if (ComboRmin->GetFloat() <= EnemiesInRange(GEntityList->Player(), R->Range()))
					{
						R->CastOnTarget(target);
					}
				}
				if (ComboRkill->Enabled())
				{
					if (ComboRmin->GetFloat() <= EnemiesInRange(GEntityList->Player(), R->Range()))
					{
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
						{
							auto QDamage = Qmin(target);
							auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
							auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
							if (target->GetHealth() < QDamage + WDamage + RDamage)
							{
								R->CastOnTarget(target);
							}
						}
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 250)
						{
							auto QDamage = Qmax(target);
							auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
							auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
							if (target->GetHealth() < QDamage + WDamage + RDamage)
							{
								R->CastOnTarget(target);
							}
						}
					}
				}
			}
		}
	}
	if (ComboMode->GetInteger() == 1)
	{
		if (Youmuus->IsOwned() && Youmuus->IsReady() && !(Player->IsDead()))
		{
			if (EnemiesInRange(Player, Q->Range()) > 0)
			{
				Youmuus->CastOnPlayer();
			}
		}
		if (ComboW->Enabled() && W->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{

				W->CastOnTarget(target);

			}
		}
		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				if (!ComboQAA->Enabled() && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
				{
					Q->CastOnTarget(target);
				}
				if (ComboGap->Enabled() && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 250)
				{
					Q->CastOnTarget(target);
				}
			}
		}
		if (ComboR->Enabled() && R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range() - 10);
			if (target != nullptr && target->IsValidTarget() && !target->IsDead() && !W->IsReady())
			{
				if (!ComboRkill->Enabled())
				{
					if (ComboRmin->GetFloat() <= EnemiesInRange(GEntityList->Player(), R->Range()))
					{
						R->CastOnTarget(target);
					}
				}
				if (ComboRkill->Enabled())
				{
					if (ComboRmin->GetFloat() <= EnemiesInRange(GEntityList->Player(), R->Range()))
					{
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
						{
							auto QDamage = Qmin(target);
							auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
							auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
							if (target->GetHealth() < QDamage + WDamage + RDamage)
							{
								R->CastOnTarget(target);
							}
						}
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 250)
						{
							auto QDamage = Qmax(target);
							auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
							auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
							if (target->GetHealth() < QDamage + WDamage + RDamage)
							{
								R->CastOnTarget(target);
							}
						}
					}
				}
			}
		}
	}
	if (ComboMode->GetInteger() == 2)
	{
		if (Youmuus->IsOwned() && Youmuus->IsReady() && !(Player->IsDead()))
		{
			if (EnemiesInRange(Player, Q->Range()) > 0)
			{
				Youmuus->CastOnPlayer();
			}
		}
		if (ComboR->Enabled() && R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				if (!ComboRkill->Enabled())
				{
					if (ComboRmin->GetFloat() <= EnemiesInRange(GEntityList->Player(), R->Range()))
					{
						R->CastOnTarget(target);	
					}
				}
				if (ComboRkill->Enabled())
				{
					if (ComboRmin->GetFloat() <= EnemiesInRange(GEntityList->Player(), R->Range()))
					{
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
						{
							auto QDamage = Qmin(target);
							auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
							auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
							if (target->GetHealth() < QDamage + WDamage + RDamage)
							{
								R->CastOnTarget(target);
							}
						}
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 250)
						{
							auto QDamage = Qmax(target);
							auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
							auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
							if (target->GetHealth() < QDamage + WDamage + RDamage)
							{
								R->CastOnTarget(target);
							}
						}
					}
				}
			}
		}
		if (ComboW->Enabled() && W->IsReady() && !R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{

				W->CastOnTarget(target);

			}
		}
		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				if (!ComboQAA->Enabled() && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
				{
					Q->CastOnTarget(target);
				}
				if (ComboGap->Enabled() && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 250)
				{
					Q->CastOnTarget(target);
				}
			}
		}
	}

}
void LastHit()
{
	for (auto Minion : GEntityList->GetAllMinions(false, true, true))
	{
		if (!Minion->IsDead() && Minion != nullptr)
		{
			if (LastQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 220) && Qmin(Minion) >= Minion->GetHealth())
			{
				Q->CastOnUnit(Minion);
			}
		}
	}
}

void Killsteal()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto QDamage = 0;
		if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
		{

			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) > 0)
			{
				QDamage = Qmin(Enemy);
			}
		}
		if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 250)
		{

			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) > 0)
			{
				QDamage = Qmax(Enemy);
			}
		}
		auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotW);

		if (Enemy != nullptr && !Enemy->IsDead())
		{
			if (KSQ->Enabled() && Q->IsReady() && QDamage > Enemy->GetHealth() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				Q->CastOnTarget(Enemy);
			}
			if (KSE->Enabled() && W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && WDamage > Enemy->GetHealth())
			{
				W->CastOnTarget(Enemy);
			}

		}

	}
}

void Flee()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		E->CastOnPosition(GGame->CursorPosition());

	}
	if (!E->IsReady())
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
	}
}

void Mixed()
{
	if (Player->ManaPercent() > HarassMana->GetInteger())
	{
		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				if (!HarassQAA->Enabled() && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
				{
					Q->CastOnTarget(target);
				}
				if (HarassGap->Enabled() && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 250)
				{
					Q->CastOnTarget(target);
				}
			}
		}
		if (ComboW->Enabled() && W->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{

				W->CastOnTarget(target);

			}
		}
	}
}

static int GetMinionsW(float range)
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
static int GetJungleW(float range)
{
	auto minions = GEntityList->GetAllMinions(false, false, true);
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
	if (Player->ManaPercent() > FarmMana->GetInteger())
	{
		for (auto Minion : GEntityList->GetAllMinions(false, true, false))
		{
			if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsCreep())
			{
				if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnTarget(Minion);
				}
				if (LastQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 220) && Qmin(Minion) >= Minion->GetHealth())
				{
					Q->CastOnUnit(Minion);
				}
				if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 190))
				{
					Vec3 pos;
					int hit;
					if (GetMinionsW(W->Range()) < 7 && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						GPrediction->FindBestCastPosition(W->Range(), 200, false, true, false, pos, hit);
						if (hit >= FarmE->GetFloat())
						{
							W->CastOnPosition(pos);
						}
					}
					if (GetMinionsW(W->Range()) > 7 && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						W->CastOnUnit(Minion);
					}
				}
			}
		}
	}
}

void FarmJ()
{
	if (Player->ManaPercent() > FarmManaJ->GetInteger())
	{
		for (auto Minion : GEntityList->GetAllMinions(false, false, true))
		{
			if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() &&  Minion->IsJungleCreep())
			{
				if (FarmQJ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnTarget(Minion);
				}
				if (FarmWJ->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 190))
				{
					Vec3 pos;
					int hit;
					if (GetJungleW(W->Range()) < 7 && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						GPrediction->FindBestCastPosition(W->Range(), W->Radius(), false, true, false, pos, hit);
						W->CastOnPosition(pos);
					}
					if (GetJungleW(W->Range()) > 7 && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						W->CastOnUnit(Minion);
					}
				}
			}
		}
	}
}
PLUGIN_EVENT(void) OnAfterAttack(IUnit* source, IUnit* target)
{
	if (source != Player || target == nullptr || !target->IsHero())
		return;

	switch (GOrbwalking->GetOrbwalkingMode())
	{
	case kModeCombo:
		for (auto hero : GEntityList->GetAllHeros(false, true))
		{
			if (ComboQAA->Enabled() && Q->IsReady() && (hero->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
			{
				Q->CastOnTarget(hero);
			}
		}
		break;
	case kModeMixed:
		for (auto hero : GEntityList->GetAllHeros(false, true)) {
			if (HarassQAA->Enabled() && Q->IsReady() && (hero->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
			{
				Q->CastOnTarget(hero);
			}
		}
		break;
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
			if ((hero->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 250)
			{

				if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) > 0)
				{
					QDamage = Qmin(hero);
				}
			}
			if ((hero->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 250)
			{

				if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) > 0)
				{
					QDamage = Qmax(hero);
				}
			}
		

			auto EDamage = 0;
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotW) > 0)
			{
				EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotW);
			}
			auto RDamage = 0;
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotR) > 0)
			{
				RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR);
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
	ChangeMode();
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		Farm();
		FarmJ();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
	{
		LastHit();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Mixed();
	}
	if (GetAsyncKeyState(FleeKey->GetInteger()) & 0x8000)
	{
		Flee();
	}
	Killsteal();
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), 220); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	if (DrawMode->Enabled())
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
			if (ComboMode->GetInteger() == 0)
			{
				std::string text = std::string("Mode: Q>W>R");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
			if (ComboMode->GetInteger() == 1)
			{
				std::string text = std::string("Mode: W>Q>R");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
			if (ComboMode->GetInteger() == 2)
			{
				std::string text = std::string("Mode: R>Q>W");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
		}
	}
	if (DrawDamage->Enabled())
	{
		dmgdraw();
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
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Talon<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#7FFF00\"> - Loaded</font></b>");
	GGame->PrintChat("<b><font color=\"#f8a101\">Version: <b><font color=\"#FFFFFF\">0.1</font></b>");

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Viktor<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#FF0000\"> - Unloaded</font></b>");
}
