#include "PluginSDK.h"
#include <string>
#include "SimpleLib.h"

PluginSetup("Kornis Cassiopeia")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboWstart;
IMenu* RMenu;
IMenuOption* ComboQnotpoison;
IMenuOption* ComboRkillable;
IMenuOption* ComboRHealth;
IMenuOption* ComboRRANGE;
IMenuOption* ComboEonlyPoison;
IMenuOption* ComboRMin;
IMenuOption* ComboREnable; 
IMenuOption* ComboRflash;
IMenuOption* ComboRFacing;



IMenu* HarassMenu;
IMenuOption* HarassMana;
IMenuOption* HarassAuto;
IMenuOption* HarassQ;

IMenu* MiscMenu;
IMenuOption* ComboAA;
IMenuOption* ComboAAkey;
IMenuOption* ComboAALevel;

IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawWRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;
IMenuOption* DrawDamage;
IMenuOption* DrawFill;
IMenuOption* DrawRFlash;
IMenuOption* DrawEkill;

IMenu* FarmMenu;
IMenuOption* FarmMana;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmELH;
IMenuOption* FarmEpoison;
IMenuOption* FarmQhit;
IMenuOption* Jungle;

IMenu* LastHitMenu;
IMenuOption* LastE;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSE;
IMenuOption* KSR;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell* Flash;


IUnit* Player;


std::vector<IUnit*> minions;
int xOffset = 10;
int yOffset = 15;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);

void LoadSpells()
{
	if (GEntityList->Player()->GetSpellSlot("SummonerFlash") != kSlotUnknown)
		Flash = GPluginSDK->CreateSpell(GEntityList->Player()->GetSpellSlot("SummonerFlash"), 425);
	Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, false, false, kCollidesWithNothing);
	Q->SetSkillshot(0.6, 40.f, FLT_MAX, 850.f);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, kCollidesWithNothing);
	W->SetSkillshot(0.25, 75.f, FLT_MAX, 800.f);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithYasuoWall);
	R = GPluginSDK->CreateSpell2(kSlotR, kConeCast, false, false, kCollidesWithNothing);
	
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Cassiopeia");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
		ComboQnotpoison = ComboMenu->CheckBox("Only Q if NOT POISONED", false);
		ComboW = ComboMenu->CheckBox("Use W in Combo", true);
		ComboWstart = ComboMenu->CheckBox("Start combo with W", true);
		ComboE = ComboMenu->CheckBox("Use E in Combo", true);
		ComboEonlyPoison = ComboMenu->CheckBox("Only E if POISONED", false);
		ComboRflash = ComboMenu->AddKey("R Flash", 'T');
		ComboREnable = ComboMenu->CheckBox("Use R in Combo", true);
		RMenu = ComboMenu->AddMenu("R Settings");
		ComboRkillable = RMenu->CheckBox("Only R if Killabe with Combo", true);
		ComboRMin = RMenu->AddInteger("R if X enemies >=", 1, 5, 1);
		ComboRHealth = RMenu->AddInteger("R if Target has Health % > ", 10, 100, 60);
		ComboRRANGE = RMenu->AddInteger("R Range for usage", 125, 825, 750);
		ComboRFacing = RMenu->CheckBox("Only R if FACING", true);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassMana = HarassMenu->AddInteger("Min Mana. for Harass", 10, 100, 50);
		HarassAuto = HarassMenu->CheckBox("Use AUTO Harass", false);
		HarassQ = HarassMenu->CheckBox("Use Q to harass", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawDamage = DrawingMenu->CheckBox("Draw killable", true);
		DrawFill = DrawingMenu->CheckBox("Draw damage", true);
		DrawRFlash = DrawingMenu->CheckBox("Draw RFlash range", true);
		DrawEkill = DrawingMenu->CheckBox("Draw Minions Killable With Q", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmMana = FarmMenu->AddInteger("Mana for clear", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Lane Clear with Q", true);
		FarmQhit = FarmMenu->AddInteger("Q hit X minions >", 1, 6, 2);
		FarmE = FarmMenu->CheckBox("Lane Clear with E", true);
		FarmELH = FarmMenu->CheckBox("Last hit E in Lane Clear", true);
		FarmEpoison = FarmMenu->CheckBox("E only if poison", false);
		Jungle = FarmMenu->CheckBox("Use in Jungle", true);
	}

	LastHitMenu = MainMenu->AddMenu("Last Hit");
	{
		LastE = LastHitMenu->CheckBox("Use E for Last Hit", true);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSE = KillstealMenu->CheckBox("Use E", true);
		KSR = KillstealMenu->CheckBox("Use R", true);
	}
	MiscMenu = MainMenu->AddMenu("Misc.");
	{
		ComboAALevel = MiscMenu->AddInteger("At what level disable AA", 1, 18, 6);
		ComboAA = MiscMenu->CheckBox("Disable AA", true);
		ComboAAkey = MiscMenu->AddKey("Disable key", 32);
	}

}

int GetEnemiesInRange(float range)
{
	auto enemies = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto enemy : enemies)
	{
		if (enemy != nullptr && enemy->GetTeam() != GEntityList->Player()->GetTeam())
		{
			auto flDistance = (enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length();
			if (flDistance < range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}

void dmgdraw()
{
	if (!DrawDamage->Enabled())
		return;
	for (auto hero : GEntityList->GetAllHeros(false, true))
	{
		Vec2 barPos = Vec2();
		if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
			auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
			auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR);
			float percentHealthAfterDamage = max(0, hero->GetHealth() - float(RDamage + EDamage*4 + QDamage)) / hero->GetMaxHealth();
			float yPos = barPos.y + yOffset;
			float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
			float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());

			if (RDamage + EDamage*4 + QDamage > hero->GetHealth())
			{
				GRender->DrawTextW(Vec2(barPos.x + xOffset, barPos.y + yOffset - 13), Color, "Killable");
			}
			if (DrawFill->Enabled() && !hero->IsDead())
			{
				float differenceInHP = xPosCurrentHp - xPosDamage;
				float pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

				for (int i = 0; i < differenceInHP; i++)
				{
					GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
				}
				if (!hero->IsVisible())
				{

				}
			}
		}
	}
}

void Combo()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (!Enemy->IsDead())
		{

			auto Qtarget = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			auto Wtarget = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (!ComboWstart->Enabled())
			{
				if (!ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
				{
					if (Qtarget != nullptr)
					{
						{
							Q->CastOnTarget(Qtarget);
						}
					}
				}
				if (ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
				{
					if (Qtarget != nullptr)
					{
						if (!Qtarget->HasBuffOfType(BUFF_Poison))
						{
							Q->CastOnTarget(Qtarget);
						}
					}
				}
				if (ComboW->Enabled() && W->IsReady() && W->Range())
				{
					if (Wtarget != nullptr)
					{
						auto distance = (Player->GetPosition() - Wtarget->GetPosition()).Length();
						if (distance >= 400)
						{
							W->CastOnTarget(Wtarget, kHitChanceHigh);
						}
					}
				}

				if (ComboE->Enabled())
				{
					if (E->IsReady())
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
						if (target != nullptr)
						{
							if (!(ComboEonlyPoison->Enabled()) || target->HasBuffOfType(BUFF_Poison))
								E->CastOnTarget(target);
						}
					}
				}
			}
			if (ComboWstart->Enabled())
			{
				if (ComboW->Enabled() && W->IsReady() && W->Range())
				{
					if (Wtarget != nullptr)
					{
						auto distance = (Player->GetPosition() - Wtarget->GetPosition()).Length();
						if (distance >= 400)
						{
							W->CastOnTarget(Wtarget, kHitChanceHigh);
						}
					}
				}


				if (!ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
				{
					if (Qtarget != nullptr)
					{
						{
							Q->CastOnTarget(Qtarget);
						}
					}
				}
				if (ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
				{
					if (Qtarget != nullptr)
					{
						if (!Qtarget->HasBuffOfType(BUFF_Poison))
						{
							Q->CastOnTarget(Qtarget);
						}
					}
				}
				if (ComboE->Enabled())
				{
					if (E->IsReady())
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
						if (target != nullptr)
						{
							if (!(ComboEonlyPoison->Enabled()) || target->HasBuffOfType(BUFF_Poison))
								E->CastOnTarget(target);
						}
					}
				}
			}
		}
		if (ComboRkillable->Enabled())
		{
			if (ComboREnable->Enabled() && R->IsReady() && GetEnemiesInRange(float(ComboRRANGE->GetInteger())) >= ComboRMin->GetInteger())
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);
				if (QDamage + EDamage * 4 + RDamage > Enemy->GetHealth())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, float(ComboRRANGE->GetInteger()));
					if (target != nullptr && ComboRFacing->Enabled() && target->IsFacing(Player))
					{
						R->CastOnTarget(target);
					}
					if (target != nullptr && !ComboRFacing->Enabled())
					{
						R->CastOnTarget(target);
					}
				}
			}
		}
		if (!ComboRkillable->Enabled())
		{
			if (ComboREnable->Enabled() && R->IsReady() && GetEnemiesInRange(float(ComboRRANGE->GetInteger())) >= ComboRMin->GetInteger())
			{
				if (Enemy->HealthPercent() < ComboRHealth->GetInteger())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, float(ComboRRANGE->GetInteger()));
					if (target != nullptr && ComboRFacing->Enabled() && target->IsFacing(Player))
					{
						R->CastOnTarget(target);
					}
					if (target != nullptr && !ComboRFacing->Enabled())
					{
						R->CastOnTarget(target);
					}
				}
			}
		}
	}
}

void Rflash()
{
	GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (R->IsReady() && Flash != nullptr && Flash->IsReady() && ComboRRANGE->GetInteger() + 410 && ComboREnable->Enabled())
		{
			if ((Player->GetPosition() - Enemy->GetPosition()).Length() > R->Range() && Enemy->IsValidTarget() && Enemy != nullptr && ((Player->GetPosition() - Enemy->GetPosition()).Length()) < R->Range()+410 && !Enemy->IsDead())
			{
			
				if (R->CastOnPosition(Enemy->ServerPosition()))
				{
					Flash->CastOnPosition(Enemy->ServerPosition());
				}
			}
		}
		if (Enemy->IsValidTarget() && W->Range() && !Flash->IsReady() && !R->IsReady())
		{
			if (Enemy != nullptr)
			{

				W->CastOnTarget(Enemy);
			}
		}

	}
}


void LastHit()
{
	if (!E->IsReady() || !LastE->Enabled())
		return;
	for (auto minion : GEntityList->GetAllMinions(false, true, true))
	{
		if (minion != nullptr && !minion->IsDead())
		{
			if (GEntityList->Player()->IsValidTarget(minion, E->Range()))
			{
				if (GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotE) > GHealthPrediction->GetPredictedHealth(minion, kLastHitPrediction, static_cast<int>(((minion->ServerPosition() - GEntityList->Player()->GetPosition()).Length2D() * 1000) / E->Speed())-125, static_cast<int>(E->GetDelay()*1000)))
				{
					E->CastOnUnit(minion);
				}
			}
		}
	}
}

void Mixed()
{
	{
		if (HarassQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				Q->CastOnTarget(target);
			}
		}
	}
}

void Killsteal()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
		auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);

		if (Enemy != nullptr && !Enemy->IsDead())
		{
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				Q->CastOnTarget(Enemy);
			}
			if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
			{
				E->CastOnTarget(Enemy);
			}
			if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage > Enemy->GetHealth() && Enemy->GetHealth() > 100)
			{
				R->CastOnTarget(Enemy);
			}
		}
	}
}


void Farm()
{

	minions = GEntityList->GetAllMinions(false, true, true);
	for (IUnit* minion : minions)
	{
		if (Player->ManaPercent() < FarmMana->GetInteger())
			return;
		if (FarmE->Enabled() && E->IsReady())
		{
			if (!FarmEpoison->Enabled())
				if (E->AttackMinions())
					return;
				else
				{
					if (minion != nullptr)
					{
						if (!(minion->IsDead()) && minion->HasBuffOfType(BUFF_Poison) && SimpleLib::SimpleLib::GetDistance(Player, minion) <= E->Range())
						{
							E->CastOnUnit(minion);

						}
					}
				}
		}
		
		if (FarmQ->Enabled() && Q->IsReady())
		{
			Vec3 pos;
			int hit;
			GPrediction->FindBestCastPosition(Q->Range(), 100, false, true, false, pos, hit);
			if (hit >= FarmQhit->GetInteger())
				Q->CastOnPosition(pos);

		}
		if (!E->IsReady() || !FarmELH->Enabled())
			return;
		if (GEntityList->Player()->IsValidTarget(minion, E->Range()))
		{
			if (GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotE) > GHealthPrediction->GetPredictedHealth(minion, kLastHitPrediction, static_cast<int>(((minion->ServerPosition() - GEntityList->Player()->GetPosition()).Length2D() * 1000) / E->Speed()) - 125, static_cast<int>(E->GetDelay() * 1000)))
			{
				E->CastOnUnit(minion);

			}
		}
	}
}

void JungleClear()
{
	for (auto Minion : GEntityList->GetAllMinions(false, false, true))
	{
		if (Jungle->Enabled())
		{
			if (!Minion->IsDead() && Minion != nullptr && Minion->IsValidTarget())
			{
				if (Jungle && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Vec3 vecCastPosition;
					int iMinionsHit = 0;

					Q->FindBestCastPosition(true, false, vecCastPosition, iMinionsHit);

					if (Q->IsReady() && iMinionsHit >= 1) {
						Q->CastOnPosition(vecCastPosition);
					}
				}

			}
		}
	}
}
void Auto()
{
	if (HarassAuto->Enabled())
	{

		if (HarassQ->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				if (Q->IsReady())
				{
					Q->CastOnTarget(target);
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
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		Farm();
		JungleClear();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
	{
		LastHit();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Mixed();
	}
	if (GetAsyncKeyState(ComboAAkey->GetInteger()))
	{
		auto level = Player->GetLevel();
		if (ComboAA->Enabled() && level >= ComboAALevel->GetInteger() && Player->GetMana() > 100)
		{
			GOrbwalking->SetAttacksAllowed(false);
		}
	}
	if (!GetAsyncKeyState(ComboAAkey->GetInteger()) && Player->GetMana() < 100)
	{
		{
			GOrbwalking->SetAttacksAllowed(true);
		}
	}
	if (GetAsyncKeyState(ComboRflash->GetInteger()))
	{
		Rflash();
	}
	Auto();
	Killsteal();
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(211, 255, 0, 255), float(ComboRRANGE->GetInteger())); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(120, 255, 0, 255), W->Range()); }
	if (DrawRFlash->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 100, 255), ComboRRANGE->GetInteger() + 410); }
	dmgdraw();
	if (DrawEkill->Enabled())
	{
		for (auto minionE : GEntityList->GetAllMinions(false, true, false))
		{
			if (minionE->IsValidTarget(GEntityList->Player(), E->Range()))
			{
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), minionE, kSlotE);
				{
					if (EDamage > minionE->GetHealth() && minionE != nullptr && !minionE->IsDead())
					{
						GRender->DrawOutlinedCircle(minionE->GetPosition(), Vec4(255, 255, 0, 255), 80.f);
					}
				}
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

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
}