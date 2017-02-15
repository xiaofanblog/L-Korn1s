#include "PluginSDK.h"
#include "SimpleLib.h"

PluginSetup("Akali")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboWR;


IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassE;



IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;
IMenuOption* DrawWRange;
IMenuOption* DrawWRRange;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmE;


ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

IUnit* Player;


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithYasuoWall);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, true, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, true, kCollidesWithNothing);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Akali");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
		ComboW = ComboMenu->CheckBox("Use W to gapclose(Useful pre-6)", true);
		ComboWR = ComboMenu->CheckBox("Use W to get In Range for R", true);
		ComboE = ComboMenu->CheckBox("Use E in Combo", true);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQ = HarassMenu->CheckBox("Use Q to harass", true);
		HarassE = HarassMenu->CheckBox("Use AUTO E if enemy in range", false);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W max Range", true);
		DrawWRRange = DrawingMenu->CheckBox("Draw W R Range", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	FarmQ = FarmMenu->CheckBox("Last hit with Q", true);
	FarmE = FarmMenu->CheckBox("Lane Clear with E", true);
}

void Combo()

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
		if (ComboE->Enabled() && E->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr)
			{
				E->CastOnTarget(target);
			}
		}
		if (ComboW->Enabled() && W->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range()+120);
			if (target != nullptr)
			{
				float myDistance = SimpleLib::SimpleLib::GetDistance(target, Player);
				if (myDistance >= 200)
				{
					W->CastOnTarget(target);
				}
			}

		}
		if (ComboWR->Enabled() && R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range()+R->Range());
			if (target != nullptr)
			{
				float myDistance = SimpleLib::SimpleLib::GetDistance(target, Player);
				if (myDistance >= 700){
					W->CastOnPosition(target->ServerPosition());
				}

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
	}
}

void LastHit()
{
	for (auto Minion : GEntityList->GetAllMinions(false, true, true))
	{
		if (!Minion->IsDead() && Minion != nullptr)
		{
			if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
			{
				Q->CastOnUnit(Minion);
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
	if (FarmE->Enabled())
	{
		if (E->IsReady())
		{
				E->AttackMinions();
		}
	}
}
void Auto()
{
	if (HarassE->Enabled())
	{
		auto target = GTargetSelector->FindTarget(ClosestPriority, SpellDamage, E->Range());
		if (target != nullptr)
		{
			if (E->IsReady())
			{
				E->CastOnTarget(target);
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
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
	{
		LastHit();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Mixed();
	}
	Auto();
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()+120); }
	if (DrawWRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()+R->Range()); }


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
