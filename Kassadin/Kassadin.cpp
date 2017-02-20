#include "PluginSDK.h"

PluginSetup("Kassadin")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboRD;
IMenuOption* IgniteCombo;


IMenu* HarassMenu;
IMenuOption* HarassQ;


IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmW;

IMenu* LastMenu;
IMenuOption* LastQ;
IMenuOption* LastW;



ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Ignite;


IUnit* Player;


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithYasuoWall);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kConeCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, kCollidesWithNothing);
	if (GEntityList->Player()->GetSpellSlot("SummonerDot") != kSlotUnknown)
		Ignite = GPluginSDK->CreateSpell2(GEntityList->Player()->GetSpellSlot("SummonerDot"), kTargetCast, false, false, kCollidesWithNothing);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kassadin");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		ComboR = ComboMenu->CheckBox("Use R", true);
		IgniteCombo = ComboMenu->CheckBox("Use Ignite", true);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQ = HarassMenu->CheckBox("Use Q", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmQ = FarmMenu->CheckBox("Lane Clear with Q", true);
		FarmW = FarmMenu->CheckBox("Lane Clear with W", true);
		FarmE = FarmMenu->CheckBox("Lane Clear with E", true);
	}

	LastMenu = MainMenu->AddMenu("Last Hit");
	{
	LastQ = FarmMenu->CheckBox("Last hit Q", true);
	LastW = FarmMenu->CheckBox("Last hit W", true);
	}
}


void Combo()
{
	{
		bool hasIgnite = GEntityList->Player()->GetSpellState(GEntityList->Player()->GetSpellSlot("SummonerDot")) == Ready;
		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				Q->CastOnTarget(target);
			}
		}
		if (ComboR->Enabled() && R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
			if (target != nullptr)
			{
				R->CastOnTarget(target);
			}
		}

		if (ComboW->Enabled() && W->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 250);
			if (target != nullptr)
			{
					W->CastOnPlayer();
			}

		}
		if (ComboE->Enabled() && E->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr)
			{
				E->CastOnTarget(target);
			}
		}
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && IgniteCombo->Enabled() && hasIgnite && GDamage->GetSummonerSpellDamage(GEntityList->Player(), target, kSummonerSpellIgnite) > target->GetHealth())
		{
			Ignite->CastOnTarget(target);
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
				W->CastOnPlayer();
			}
		}
	}
}

void Mixed()
{
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
	if (FarmQ->Enabled())
	{
		if (Q->IsReady())
		{
			Q->AttackMinions();
		}
	}
	if (FarmE->Enabled())
	{
		if (E->IsReady())
		{
			E->AttackMinions();
		}
	}
	if (FarmW->Enabled())
	{
		if (W->IsReady())
		{
			W->CastOnPlayer();
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
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
	{
		LastHit();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Mixed();
	}
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

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
}