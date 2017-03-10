#include "PluginSDK.h"

PluginSetup("Kornis's Xin Zhao")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboQAA;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboEmin;
IMenuOption* ComboR;
IMenuOption* ComboRmin;
IMenuOption* ComboItems;
IMenuOption* ComboSmite;

IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;

IMenu* SmiteMenu;
IMenuOption* SmiteUse;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmW;
IMenuOption* FarmE;
IMenuOption* FarmMana;

IMenu* KillstealMenu;
IMenuOption* KSR;
IMenuOption* KSE;

IMenu* MiscMenu;
IMenuOption* InterruptR;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Smite;


IUnit* Player;

IInventoryItem* Botrk;
IInventoryItem* Tiamat;
IInventoryItem* Cutlass;
IInventoryItem* Titanic_Hydra;
IInventoryItem* Ravenous_Hydra;
IInventoryItem* Youmuu;


void LoadSpells()
{
	auto PlayerSum1 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot1);
	auto PlayerSum2 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot2);
	if (strstr(PlayerSum1, "SummonerSmite")) { Smite = GPluginSDK->CreateSpell2(kSummonerSlot1, kTargetCast, false, false, kCollidesWithNothing); }
	if (strstr(PlayerSum2, "SummonerSmite")) { Smite = GPluginSDK->CreateSpell2(kSummonerSlot2, kTargetCast, false, false, kCollidesWithNothing); }
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, kCollidesWithNothing);
	Cutlass = GPluginSDK->CreateItemForId(3144, 550);
	Botrk = GPluginSDK->CreateItemForId(3153, 550);
	Youmuu = GPluginSDK->CreateItemForId(3142, 0);
	Titanic_Hydra = GPluginSDK->CreateItemForId(3748, 385);
	Ravenous_Hydra = GPluginSDK->CreateItemForId(3074, 385);
	Tiamat = GPluginSDK->CreateItemForId(3077, 385);

}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Xin Zhao");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
		ComboQAA = ComboMenu->CheckBox("Reset AA with Q", true);
		ComboW = ComboMenu->CheckBox("Use W in Combo", true);
		ComboE = ComboMenu->CheckBox("Use E in Combo", true);
		ComboEmin = ComboMenu->AddInteger("E min range", 20, 400, 250);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);
		ComboRmin = ComboMenu->AddInteger("Use R if enemies X >", 1, 5, 2);
		ComboItems = ComboMenu->CheckBox("Use Items", true);
		ComboSmite = ComboMenu->CheckBox("Use Smite in Combo", true);

	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmMana = FarmMenu->AddInteger("Mana percent for clear", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Lane Clear with Q", true);
		FarmW = FarmMenu->CheckBox("Lane Clear with W", true);
		FarmE = FarmMenu->CheckBox("Lane Clear with E", true);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSE = KillstealMenu->CheckBox("Use E", true);
		KSR = KillstealMenu->CheckBox("Use R", true);
	}
	SmiteMenu = MainMenu->AddMenu("Smite menu");
	{
		SmiteUse = SmiteMenu->CheckBox("Use Smite", true);
	}
	MiscMenu = MainMenu->AddMenu("Misc menu");
	{
		InterruptR = MiscMenu->CheckBox("Use R to Interrupt", true);
	}
}

void AutoSmite() // AUTO SMITE PRO BY REMBRANDT
{
	if (Smite != nullptr && Smite->IsReady() && SmiteUse->Enabled()) {
		auto minions = GEntityList->GetAllMinions(false, false, true);
		for (IUnit* minion : minions)
		{
			if (strstr(minion->GetObjectName(), "Red") || strstr(minion->GetObjectName(), "Blue") || strstr(minion->GetObjectName(), "Dragon") || strstr(minion->GetObjectName(), "Rift") || strstr(minion->GetObjectName(), "Baron"))
			{
				if (minion != nullptr && !minion->IsDead() && minion->GetHealth() <= GDamage->GetSummonerSpellDamage(GEntityList->Player(), minion, kSummonerSpellSmite))
				{
					Smite->CastOnUnit(minion);
				}
			}
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
void Combo()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy->IsValidTarget())
		{
			if (Smite != nullptr && Smite->IsReady() && ComboSmite->Enabled()) // AUTO SMITE PRO BY REMBRANDT
			{

				if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && (GTargetSelector->GetFocusedTarget()->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 500)
				{

					Smite->CastOnTarget(GTargetSelector->GetFocusedTarget());
				}
				else
				{
					Smite->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 700));
				}
			}
			if (Cutlass->IsOwned() && Cutlass->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
			{
				if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Cutlass->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
				{
					Cutlass->CastOnTarget(GTargetSelector->GetFocusedTarget());
				}
				else
				{
					Cutlass->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 550));
				}
			}
			if (Botrk->IsOwned() && Botrk->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
			{
				if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Cutlass->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
				{
					Botrk->CastOnTarget(GTargetSelector->GetFocusedTarget());
				}
				else
				{
					Botrk->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 550));
				}
			}
			if (Youmuu->IsOwned() && Youmuu->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
			{
				if (EnemiesInRange(Player, 500) > 0)
				{
					Youmuu->CastOnPlayer();
				}
			}
			if (Ravenous_Hydra->IsOwned() && Ravenous_Hydra->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
			{
				if (EnemiesInRange(Player, 385) > 0)
				{
					Ravenous_Hydra->CastOnPlayer();
				}
			}
			if (Tiamat->IsOwned() && Tiamat->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
			{
				if (EnemiesInRange(Player, 385) > 0)
				{
					Tiamat->CastOnPlayer();
				}
			}
			if (Titanic_Hydra->IsOwned() && Titanic_Hydra->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
			{
				if (EnemiesInRange(Player, 385) > 0)
				{
					Titanic_Hydra->CastOnPlayer();
				}
			}
			if (!ComboQAA->Enabled() && ComboQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()))
			{
				Q->CastOnPlayer();
			}
			if (!Enemy->IsDead() && Enemy != nullptr && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && ComboE->Enabled())
			{
				if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() > ComboEmin->GetInteger())
				{
					E->CastOnTarget(Enemy);
				}
				if (GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE) >= Enemy->GetHealth())
				{
					E->CastOnTarget(Enemy);
				}
				if (Enemy->IsDashing())
				{
					E->CastOnTarget(Enemy);
				}
				if (Enemy->HealthPercent() < 15)
				{
					E->CastOnTarget(Enemy);
				}
			}
			if (ComboW->Enabled() && W->IsReady() && W->Range())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 250);
				if (target != nullptr)
				{
					W->CastOnPlayer();
				}

			}
			if (ComboR->Enabled() && R->IsReady() && R->Range())
			{
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);
				if (EnemiesInRange(Player, R->Range()) >= ComboRmin->GetInteger())
				{
					if (Enemy->HasBuff("xenzhaointimidate"))
					{
						R->CastOnTarget(Enemy);

					}

				}
				if (Enemy->GetHealth() < RDamage)
				{
					R->CastOnTarget(Enemy);

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
		auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);

		if (Enemy != nullptr && !Enemy->IsDead())
		{
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


void Farm()
{
	if (Player->ManaPercent() > FarmMana->GetInteger())
	{
		for (auto Minion : GEntityList->GetAllMinions(false, true, true))
		{
			if (!Minion->IsDead() && Minion != nullptr)
			{
				if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnPlayer();
				}
				if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnUnit(Minion);
				}
				if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 250))
				{
					W->CastOnPlayer();
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
		for (auto hero : GEntityList->GetAllHeros(false, true)) {
			if (ComboQAA->Enabled() && Q->IsReady() && (hero->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 300)
			{
				Q->CastOnPlayer();
			}
		}
		break;
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
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{

	}
	AutoSmite();
	KillSteal();
}


PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (InterruptR->Enabled() && (Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length() < R->Range() && R->IsReady() && Args.Target->IsValidTarget())
	{
		if (!Args.Target->HasBuff("xenzhaointimidate"))
		R->CastOnTarget(Args.Target);
	}
}

PLUGIN_EVENT(void) OnRender()
{
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
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
}