#include "PluginSDK.h"
#include <string>
PluginSetup("Blitzcrank")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboElanded;
IMenuOption* ComboR;
IMenuOption* ComboRinterrupt;
IMenuOption* ComboRmin;
IMenuOption* ComboQmin;
IMenuOption* ComboQmax;

IMenu* HookMenu;
IMenuOption* GrabQ;
IMenuOption* AutoQ;

IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawRRange;
IMenuOption* DrawQmin;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSR;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

IUnit* Player;


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall, kCollidesWithMinions));
	Q->SetSkillshot(0.25, 210.f, 1800.f, 925.f);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, static_cast<eCollisionFlags>(kCollidesWithNothing));


}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Blitzcrank");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
		ComboE = ComboMenu->CheckBox("Use E in Combo", true);
		ComboElanded = ComboMenu->CheckBox("Use E only if Q landed", true);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);
		ComboRinterrupt = ComboMenu->CheckBox("Use R to Interrupt", true);
		ComboRmin = ComboMenu->AddInteger("Use R if enemies X >", 1, 5, 2);

	}
	HookMenu = MainMenu->AddMenu("Q settings");
	{
		GrabQ = HookMenu->AddKey("Grab Q", 'T');
		AutoQ = HookMenu->CheckBox("Use auto Q", false);
		ComboQmin = HookMenu->AddInteger("Min Q range", 10, 400, 300);
		ComboQmax = HookMenu->AddInteger("Max Q range", 600, 900, 900);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q max", true);
		DrawQmin = DrawingMenu->CheckBox("Draw Q minimum", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSR = KillstealMenu->CheckBox("Use R", true);
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

void Combo()
{

	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (ComboQ->Enabled() && Q->IsReady() && Q->Range())
		{
			if (Enemy != nullptr && (Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() <= ComboQmax->GetInteger() && (Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() >= ComboQmin->GetInteger())
			{
				Q->CastOnTarget(Enemy);

			}
				
		}
		if (ComboE->Enabled() && E->IsReady() && Q->Range())
		{
			if (!ComboElanded->Enabled())
			{
				if (Enemy != nullptr && (Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 300)
				{
					if (E->CastOnPlayer())
					{
						GOrbwalking->SetOverrideTarget(Enemy);
					}
				}
			}
			if (ComboElanded->Enabled())
			{
				if (Enemy->HasBuff("rocketgrab2") && (Enemy != nullptr))
				{
					if (E->CastOnPlayer())
					{
						GOrbwalking->SetOverrideTarget(Enemy);
					}
				}
			}
		}
		if (ComboR->Enabled() && R->IsReady() && R->Range())
		{
			if (Enemy != nullptr && !Enemy->IsDead() && !Enemy->IsInvulnerable() && GetEnemiesInRange(R->Range()) >= ComboRmin->GetInteger())
			{
				R->CastOnPlayer();
			}
		}
	}
}

void Hook()
{
	if (AutoQ->Enabled())
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

void Killsteal()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
		auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);

		if (Enemy != nullptr && !Enemy->IsDead())
		{
			if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage > Enemy->GetHealth())
			{
				R->CastOnPlayer();
			}
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				Q->CastOnTarget(Enemy);
			}

		}
	}
}


PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (ComboRinterrupt->Enabled() && (Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length() < R->Range() && R->IsReady() && Args.Target->IsValidTarget())
	{
		R->CastOnTarget(Args.Target);
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

	}
	if (GetAsyncKeyState(GrabQ->GetInteger()) & 0x8000)
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
	Killsteal();
	Hook();
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), ComboQmax->GetInteger()); }
	if (DrawQmin->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), ComboQmin->GetInteger()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), R->Range()); }

}




PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	Player = GEntityList->Player();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
}