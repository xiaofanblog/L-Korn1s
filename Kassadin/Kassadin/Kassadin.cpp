#include "PluginSDK.h"
#include <string>

PluginSetup("Kassadin")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboWAA;
IMenuOption* ComboE;
IMenuOption* ComboR;


IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassMana;


IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmW;
IMenuOption* FarmMana;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSE;
IMenuOption* KSR;
IMenuOption* KSRgap;

IMenu* LastMenu;
IMenuOption* LastQ;
IMenuOption* LastW;

IMenu* FleeMenu;
IMenuOption* FleeKey;



ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;


IUnit* Player;


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithYasuoWall);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kConeCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, kCollidesWithNothing);
	E->SetOverrideRange(580);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kassadin");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboW = ComboMenu->CheckBox("Use W Always", false);
		ComboWAA = ComboMenu->CheckBox("Use W AA Reset", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		ComboR = ComboMenu->CheckBox("Use R", true);


	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassMana = HarassMenu->AddInteger("Mana Percent for Harass", 10, 100, 50);
		HarassQ = HarassMenu->CheckBox("Use Q", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
	}
	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Killsteal with Q", true);
		KSE = KillstealMenu->CheckBox("Killsteal with E", true);
		KSR = KillstealMenu->CheckBox("Killsteal with R", true);
		KSRgap = KillstealMenu->CheckBox("Gap R for Q range KS", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmMana = FarmMenu->AddInteger("Mana Percent for LaneClear", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Lane Clear with Q", true);
		FarmW = FarmMenu->CheckBox("Lane Clear with W", true);
		FarmE = FarmMenu->CheckBox("Lane Clear with E", true);
	}

	LastMenu = MainMenu->AddMenu("Last Hit");
	{
		LastQ = LastMenu->CheckBox("Last hit Q", true);
		LastW = LastMenu->CheckBox("Last hit W", true);
	}
	FleeMenu = MainMenu->AddMenu("Flee");
	{
		FleeKey = FleeMenu->AddKey("Flee with R", 'G');
	}
}


void Combo()
{
	{

		bool hasIgnite = GEntityList->Player()->GetSpellState(GEntityList->Player()->GetSpellSlot("SummonerDot")) == Ready;
		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				Q->CastOnTarget(target);
			}
		}
		if (ComboR->Enabled() && R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				R->CastOnTarget(target, kHitChanceMedium);
			}
		}

		if (ComboW->Enabled() && W->IsReady() && !ComboWAA->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 250);
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				W->CastOnPlayer();
			}

		}
		if (ComboE->Enabled() && E->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				E->CastOnPosition(target->ServerPosition());
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
			if (LastQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
			{
				Q->CastOnUnit(Minion);
			}
			if (LastW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), W->Range() + 200) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotW) >= Minion->GetHealth())
			{
				if (W->CastOnPlayer())
				{
					GOrbwalking->SetOverrideTarget(Minion);
					return;
				}
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
			if (KSQ->Enabled() && Q->IsReady() && QDamage > Enemy->GetHealth() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				Q->CastOnTarget(Enemy);
			}
			if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
			{
				E->CastOnPosition(Enemy->ServerPosition());
			}
			if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage > Enemy->GetHealth())
			{
				R->CastOnTarget(Enemy, kHitChanceMedium);
			}

			if (KSRgap->Enabled() && R->IsReady() && Q->IsReady() && KSQ->Enabled() && QDamage > Enemy->GetHealth() && (Player->GetPosition() - Enemy->GetPosition()).Length() < 1100)
			{
				if (!Enemy->IsDead() && Player->GetMana() > Q->ManaCost() + R->ManaCost() && (Player->GetPosition() - Enemy->GetPosition()).Length() > Q->Range())
				{

					R->CastOnPosition(Enemy->ServerPosition());
					Q->CastOnTarget(Enemy);

				}
			}

		}

	}
}

void Flee()
{
	if (!GGame->IsChatOpen())
	{

			GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
			R->CastOnPosition(GGame->CursorPosition());

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
				Q->CastOnTarget(target);
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
			if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				Q->CastOnUnit(Minion);
			}
			if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), R->Range()))
			{
				E->CastOnUnit(Minion);
			}
			if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 200))
			{
				W->CastOnPlayer();
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
			if (!ComboW->Enabled() && ComboWAA->Enabled() && W->IsReady() && (hero->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 300)
			{
				W->CastOnPlayer();
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
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
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


}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
}
