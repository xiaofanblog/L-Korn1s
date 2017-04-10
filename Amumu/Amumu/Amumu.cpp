#include "PluginSDK.h"
#include "Extension.h"

PluginSetup("Kornis's Amumu")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboQAA;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboRmin;

IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawWRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;

IMenu* FarmMenu;
IMenuOption* FarmWmin;
IMenuOption* FarmW;
IMenuOption* FarmE;
IMenuOption* FarmMana;
IMenuOption* FarmQlane;
IMenuOption* FarmQjungle;

IMenu* KillstealMenu;
IMenuOption* KSR;
IMenuOption* KSE;
IMenuOption* KSQ;

IMenu* MiscMenu;
IMenuOption* InterruptR;
IMenuOption* InterruptQ;
IMenuOption* Wsmart;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

IUnit* Player;
IUnit* Enemy;

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithMinions | kCollidesWithYasuoWall));
	Q->SetSkillshot(0.25f, 50.f, 2000.f, 1000.f);

	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, true, (kCollidesWithNothing));
	W->SetSkillshot(0.25f, 300.f, 1000.f, 280.f);

	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, true, (kCollidesWithNothing));
	E->SetSkillshot(0.25f, 350.f, 1000.f, 330.f);

	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, (kCollidesWithNothing));
	R->SetSkillshot(0.25f, 550.f, 1000.f, 500.f);

}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Amumu");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
		ComboQAA = ComboMenu->AddFloat("Q min. range", 10, 500, 300);
		ComboW = ComboMenu->CheckBox("Use W in Combo", true);
		ComboE = ComboMenu->CheckBox("Use E in Combo", true);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);
		ComboRmin = ComboMenu->AddInteger("Use R if enemies X >", 1, 5, 2);

	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmMana = FarmMenu->AddInteger("Mana percent for clear", 10, 100, 50);
		FarmQlane = FarmMenu->CheckBox("Farm Q LaneClear", false);
		FarmQjungle = FarmMenu->CheckBox("Farm Q in Jungle", true);
		FarmW = FarmMenu->CheckBox("Farm with W", true);
		FarmWmin = FarmMenu->AddFloat("Min. minions for W", 1, 6, 1);
		FarmE = FarmMenu->CheckBox("Farm with E", true);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSE = KillstealMenu->CheckBox("Use E", true);
		KSR = KillstealMenu->CheckBox("Use R", true);
	}
	MiscMenu = MainMenu->AddMenu("Misc menu");
	{
		InterruptQ = MiscMenu->CheckBox("Use Q to Interrupt", true);
		InterruptR = MiscMenu->CheckBox("Use R to Interrupt", true);
		Wsmart = MiscMenu->CheckBox("Smart W toggle off", true);
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
void Combo()
{
	Enemy = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
	for (auto Enemy : GEntityList->GetAllHeros(false, true));
	{
		if (Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && Enemy->IsValidTarget() && !Enemy->IsDead())
		{
			if (ComboQAA->GetFloat() <= ((GEntityList->Player()->GetPosition() - Enemy->GetPosition()).Length2D()) && ComboQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				AdvPredictionOutput outputfam;
				Q->RunPrediction(Enemy, false, kCollidesWithMinions, &outputfam);
				if (outputfam.HitChance >= kHitChanceHigh)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
					if (InSpellRange(Q, pred))
						Q->CastOnPosition(pred);
				}
			}
			if (ComboW->Enabled() && W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && !GEntityList->Player()->HasBuff("AuraofDespair"))
			{
				W->CastOnPlayer();
			}
			if (ComboE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()))
			{
				E->CastOnPlayer();
			}
			if (ComboR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()))
			{
				if (EnemiesInRange(GEntityList->Player(), R->Range()) >= ComboRmin->GetInteger())
				{

					R->CastOnPlayer();
				}

			}
		}

	}
}




void KillSteal()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
		auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);

		if (Enemy != nullptr && !Enemy->IsDead() && Enemy->IsValidTarget())
		{
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				AdvPredictionOutput outputfam;
				Q->RunPrediction(Enemy, false, kCollidesWithMinions, &outputfam);
				if (outputfam.HitChance >= kHitChanceHigh)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
					if (InSpellRange(Q, pred))
						Q->CastOnPosition(pred);
				}
			}
			if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage > Enemy->GetHealth())
			{
				R->CastOnTarget(Enemy);
			}
			if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
			{
				E->CastOnTarget(Enemy);
			}

		}
	}
}


static int CountMinionsNearMe(IPluginSDK *sdk, float range)
{
	static auto entityList = sdk->GetEntityList();
	static auto player = entityList->Player();

	auto count = 0;

	for (auto unit : entityList->GetAllMinions(false, true, true))
	{
		if (player->IsValidTarget(unit, range))
		{
			count++;
		}
	}

	return count;
}

void Farm()
{
	if (Player->ManaPercent() > FarmMana->GetInteger())
	{
		for (auto Minion : GEntityList->GetAllMinions(false, false, true))
		{
			if (!Minion->IsDead() && Minion != nullptr && Minion->IsValidTarget())
			{
				if (FarmQjungle->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnUnit(Minion);
				}
				if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnUnit(Minion);
				}
				if (!GEntityList->Player()->HasBuff("AuraofDespair") && CountMinionsNearMe(GPluginSDK, W->Range()) >= FarmWmin->GetFloat() && FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					W->CastOnPlayer();
				}

			}

		}
		for (auto Minion : GEntityList->GetAllMinions(false, true, false))
		{
			if (!Minion->IsDead() && Minion != nullptr && Minion->IsValidTarget())
			{
				if (FarmQlane->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnUnit(Minion);
				}
				if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnUnit(Minion);
				}
				if (!GEntityList->Player()->HasBuff("AuraofDespair") && CountMinionsNearMe(GPluginSDK, W->Range()) >= FarmWmin->GetFloat() && FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					W->CastOnPlayer();
				}
			}
		}
	}
}
void AutoW()
{
	if (Wsmart->Enabled())
	{
		if (GEntityList->Player()->HasBuff("AuraofDespair"))
		{
			if (CountMinionsNearMe(GPluginSDK, W->Range()) == 0 && EnemiesInRange(GEntityList->Player(), W->Range()) == 0)
			{
				W->CastOnPlayer();
			}
		}
	}
}

PLUGIN_EVENT(void) OnGameUpdate()
{
	AutoW();
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		Farm();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{

	}
	KillSteal();
}


PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (InterruptR->Enabled() && (Args.Source->GetPosition() - GEntityList->Player()->GetPosition()).Length() < R->Range() && R->IsReady() && Args.Source->IsValidTarget())
	{
		if (Args.DangerLevel == kHighDanger)
		{
			R->CastOnPlayer();
		}
	}
	if (InterruptQ->Enabled() && (Args.Source->GetPosition() - GEntityList->Player()->GetPosition()).Length() < Q->Range() && Q->IsReady() && Args.Source->IsValidTarget())
	{
		if (Args.DangerLevel == kHighDanger)
		{
			Q->CastOnTarget(Args.Source);
		}
	}
}

PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }

}



PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	Player = GEntityList->Player();


	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
}