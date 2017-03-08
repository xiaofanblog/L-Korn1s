#include "PluginSDK.h"
#include <string>

PluginSetup("Kornis's Zilean")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenu* ComboQ;
IMenuOption* ComboQenable;
IMenuOption* ComboQcc;
IMenuOption* ComboQinterrupt;
IMenu* ComboW;
IMenuOption* ComboWenable;
IMenuOption* ComboWQreset;
IMenu* ComboE;
IMenuOption* ComboEenable;
IMenuOption* ComboEslow;
IMenuOption* ComboEspeedup;
IMenuOption* ComboEflee;
IMenu* ComboR;
IMenuOption* ComboRenable;
IMenuOption* ComboRhp;
IMenuOption* ComboRkills;


IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassW;



IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSW;


ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;


IUnit* Player;


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, true, false, kCollidesWithNothing);
	Q->SetOverrideRange(900);
	Q->SetOverrideRadius(250.f);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Zilean");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->AddMenu("Q settings");
		ComboQenable = ComboQ->CheckBox("Use Q", true);
		ComboQcc = ComboQ->CheckBox("Auto Q on CC", true);
		ComboQinterrupt = ComboQ->CheckBox("Use QWQ to Interrupt", true);
		ComboW = ComboMenu->AddMenu("W settings");
		ComboWenable = ComboW->CheckBox("Use W", true);
		ComboWQreset = ComboW->CheckBox("Use W to reset Q", true);
		ComboE = ComboMenu->AddMenu("E settings");
		ComboEenable = ComboE->CheckBox("Use E", true);
		ComboEslow = ComboE->CheckBox("Use auto E on slowed Ally", true);
		ComboEflee = ComboE->AddKey("Flee with E", 'G');
		ComboR = ComboMenu->AddMenu("R settings");
		ComboRenable = ComboR->CheckBox("Use R", true);
		//ComboRkills = ComboR->CheckBox("Auto R if damage kills Ally", true);
		ComboRhp = ComboR->AddInteger("Use R if HP <", 5, 30, 15);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQ = HarassMenu->CheckBox("Use Q", true);
		HarassW = HarassMenu->CheckBox("Use W to reset", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSW = KillstealMenu->CheckBox("Use QWQ", true);
	}
}

void Combo()
{
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
	if (target != nullptr && !target->IsInvulnerable() && !target->IsDead())
		if (ComboEenable->Enabled() && E->IsReady() && E->Range() && !target->HasBuffOfType(BUFF_Slow))
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr)
			{
				E->CastOnTarget(target);
			}
		}
	if (ComboQenable->Enabled() && Q->IsReady() && Q->Range())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr)
		{
			Q->CastOnTarget(target, kHitChanceHigh);
		}
	}
	if (ComboWenable->Enabled() && W->IsReady() && Q->Range())
	{
		if (!Q->IsReady() && Player->GetMana() > W->ManaCost() + Q->ManaCost())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				W->CastOnPlayer();
				Q->CastOnTarget(target, kHitChanceHigh);
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
		if (HarassW->Enabled() && W->IsReady() && Q->Range())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (!Q->IsReady())
				if (target != nullptr)
				{
					W->CastOnPlayer();
					Q->CastOnTarget(target);
				}

		}

	}
}

void Killsteal()
{
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
	if (target != nullptr && !target->IsInvulnerable() && !target->IsDead())
	{
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);


		if (KSQ->Enabled() && Q->IsReady() && target->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > target->GetHealth())
		{
			Q->CastOnTarget(target, kHitChanceHigh);
		}
		if (!Q->IsReady())
		{
			if (KSQ->Enabled() && KSW->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > target->GetHealth())
			{
				W->CastOnPlayer();
				Q->CastOnTarget(target, kHitChanceHigh);
			}
		}
		if (KSQ->Enabled() && KSW->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage * 2 > target->GetHealth())
		{

			if (!Q->IsReady())
			{
				W->CastOnPlayer();
				Q->CastOnTarget(target);
			}

		}
	}
}

void Auto()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy != nullptr && !Enemy->IsDead())
		{
			if (ComboQcc->Enabled())
			{
				if (Enemy->IsValidTarget(Player, Q->Range()) && Q->Range())
				{
					if (Enemy->HasBuffOfType(BUFF_Stun) || Enemy->HasBuffOfType(BUFF_Charm) || Enemy->HasBuffOfType(BUFF_Fear) || Enemy->HasBuffOfType(BUFF_Taunt) || Enemy->HasBuffOfType(BUFF_Suppression) || Enemy->HasBuffOfType(BUFF_Snare))
					{
						Q->CastOnTarget(Enemy);
					}

				}
			}
		}
	}
	for (auto Ally : GEntityList->GetAllHeros(true, false))
	{
		if (Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && Ally->HasBuffOfType(BUFF_Slow))
		{
			if (ComboEslow->Enabled() && E->Range() && E->IsReady() )
				E->CastOnTarget(Ally);
		}
	}
	for (auto Ally : GEntityList->GetAllHeros(true, false))
	{
		if (Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), R->Range()))
		{
			if (ComboRenable->Enabled() && R->Range() && R->IsReady())
			{
				/*if (Ally->HasIncomingDamage() > Ally->GetHealth())
				{
					R->CastOnTarget(Ally);
				}*/
			}
			if (ComboRhp->GetInteger() > Ally->HealthPercent() && R->Range() && R->IsReady())
			{
				R->CastOnTarget(Ally);
			}
		}
	}
	if (ComboRhp->GetInteger() > Player->HealthPercent() && R->Range() && R->IsReady())
	{
		R->CastOnPlayer();
	}
	/*if (GEntityList->Player()->HasIncomingDamage() > Player->GetHealth() && ComboRkills->Enabled())
	{
		R->CastOnPlayer();
	}*/
}


void Flee()
{
	if (!GGame->IsChatOpen())
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
		if (E->IsReady())
		{
			E->CastOnPlayer();
		}

	}
}

PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (ComboQinterrupt->Enabled() && (Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length() < Q->Range() && Q->IsReady() && Args.Target->IsValidTarget())
	{
		Q->CastOnTarget(Args.Target);
	}
	if (!Q->IsReady())
	{
		if (ComboQinterrupt->Enabled() && (Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length() < Q->Range() && Args.Target->IsValidTarget())
		{
			W->CastOnPlayer();
			Q->CastOnTarget(Args.Target);
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

	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Mixed();
	}
	if (GetAsyncKeyState(ComboEflee->GetInteger()) & 0x8000)
	{
		Flee();
	}
	Killsteal();
	Auto();
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
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
}