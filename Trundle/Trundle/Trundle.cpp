#include "PluginSDK.h"

PluginSetup("Trundle by Kornis")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboQAA;
IMenuOption* ComboW;
IMenu* RPrio;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboRhp;
IMenuOption* ComboWmin;

IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassQAA;
IMenuOption* HarassW;
IMenuOption* HarassE;
IMenuOption* HarassMana;



IMenu* DrawingMenu;
IMenuOption* DrawERange;
IMenuOption* DrawWRange;
IMenuOption* DrawRRange;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmMana;
IMenuOption* FarmQAA;
IMenuOption* FarmW;

IMenu* LastMenu;
IMenuOption* FarmQLast;
IMenu* MiscMenu;
IMenuOption* AntiGap;
IMenuOption* Interrupt;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

IUnit* Player;
IUnit* target;

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Trundle by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q Always", false);
		ComboQAA = ComboMenu->CheckBox("Use Q AA Reset", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboWmin = ComboMenu->AddInteger("W Max Range", 100, 750, 500);
		ComboE = ComboMenu->CheckBox("Use E", true);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);
		ComboRhp = ComboMenu->AddInteger("My HP for R", 10, 100, 50);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQ = HarassMenu->CheckBox("Use Q always", false);
		HarassQAA = HarassMenu->CheckBox("Use Q AA reset", true);
		HarassW = HarassMenu->CheckBox("Use W in harass", true);
		HarassE = HarassMenu->CheckBox("Use E in harass", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmMana = FarmMenu->AddInteger("Mana percent for Clear", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Q Always", false);
		FarmQAA = FarmMenu->CheckBox("Q AA Reset", true);
		FarmW = FarmMenu->CheckBox("Use W", true);
	}

	LastMenu = MainMenu->AddMenu("Last Hit");
	FarmQLast = LastMenu->CheckBox("Q Last Hit", true);
	RPrio = MainMenu->AddMenu("R Priority");

	MiscMenu = MainMenu->AddMenu("Misc.");
	Interrupt = MiscMenu->CheckBox("Interrupt E", true);
	AntiGap = MiscMenu->CheckBox("Anti Gap E", true);

	for (auto enemies : GEntityList->GetAllHeros(false, true)) {
		RPrio->CheckBox(enemies->ChampionName(), false);
	}
}

void Combo()

{
	target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, E->Range() - 50);
	for (auto target : GEntityList->GetAllHeros(false, true));
	if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
	{
		// E logic

		auto myPos = target->GetPosition();
		auto direction = GEntityList->Player()->Direction();

		auto inFront = (myPos + (direction * 50));
		auto behind = (myPos - (direction * -50));
		if (E->IsReady() && ComboE->Enabled() && target->IsValidTarget(GEntityList->Player(), E->Range()))
		{
			if (target->IsFacing(GEntityList->Player()))
			{
				E->CastOnPosition(inFront);
			}
			if (!target->IsFacing(GEntityList->Player()))
			{
				E->CastOnPosition(behind);
			}
		}
		if (Q->IsReady() && !ComboQAA->Enabled() && ComboQ->Enabled() && target->IsValidTarget(GEntityList->Player(), 300))
		{
			Q->CastOnPlayer();
		}

		if (W->IsReady() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), ComboWmin->GetInteger()))
		{
			W->CastOnPlayer();
		}
		if (R->IsReady() && ComboR->Enabled())
		{
			IMenuOption* temp = RPrio->GetOption(target->ChampionName());
			if (ComboRhp->GetInteger() >= GEntityList->Player()->HealthPercent() && temp->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()))
			{
				R->CastOnTarget(target);
			}
			if (ComboRhp->GetInteger() >= GEntityList->Player()->HealthPercent() && (!temp->Enabled() || !target->IsValidTarget(GEntityList->Player(), R->Range())))
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
			if (FarmQLast->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 300) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
			{
				if (Q->CastOnPlayer())
				{
					GOrbwalking->SetOverrideTarget(Minion);
					return;
				}
			}
		}
	}
}

void Mixed()
{
	target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, E->Range() - 50);
	for (auto target : GEntityList->GetAllHeros(false, true));
	if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
	{
		// E logic
		auto myPos = target->GetPosition();
		auto direction = GEntityList->Player()->Direction();

		auto inFront = (myPos + (direction * 50));
		auto behind = (myPos - (direction * -50));
		if (E->IsReady() && HarassE->Enabled() && target->IsValidTarget(GEntityList->Player(), E->Range()))
		{
			if (target->IsFacing(GEntityList->Player()))
			{
				E->CastOnPosition(behind);
			}
			if (!target->IsFacing(GEntityList->Player()))
			{
				E->CastOnPosition(inFront);
			}
		}
		if (Q->IsReady() && !HarassQAA->Enabled() && HarassQ->Enabled() && target->IsValidTarget(GEntityList->Player(), 300))
		{
			Q->CastOnPlayer();
		}

		if (W->IsReady() && HarassW->Enabled() && target->IsValidTarget(GEntityList->Player(), W->Range()))
		{
			W->CastOnPlayer();
		}
	}
}

void Farm()
{
	if (Player->ManaPercent() > FarmMana->GetInteger())
	{
		for (auto Minion : GEntityList->GetAllMinions(false, true, true))
		{
			if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsCreep() || Minion->IsJungleCreep())
			{
				if (!FarmQAA->Enabled() && FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 400))
				{
					Q->CastOnPlayer();
				}
				if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 400))
				{
					W->CastOnPlayer();
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

PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (Interrupt->Enabled() && (Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length() < E->Range() && E->IsReady() && Args.Target->IsValidTarget())
	{
		E->CastOnTarget(Args.Target);
	}
}


PLUGIN_EVENT(void) OnAfterAttack(IUnit* source, IUnit* target)
{
	if (source != Player || target == nullptr)
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
	case kModeMixed:
		for (auto hero : GEntityList->GetAllHeros(false, true)) {
			if (HarassQAA->Enabled() && Q->IsReady() && (hero->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 300)
			{
				Q->CastOnPlayer();
			}
		}
		break;
	case kModeLaneClear:
		for (auto minion : GEntityList->GetAllMinions(false, true, true)) {
			if (FarmQAA->Enabled() && Q->IsReady() && (minion->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 300)
			{
				Q->CastOnPlayer();
			}
		}
		break;
	}
}

PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{
	if (Args.Sender != GEntityList->Player()
		&& Args.Sender->IsEnemy(GEntityList->Player())
		&& GEntityList->Player()->IsValidTarget(Args.Sender, E->Range())
		&& AntiGap->Enabled() && E->IsReady())
	{
		E->CastOnTarget(Args.Sender);
	}
}

PLUGIN_EVENT(void) OnRender()
{
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), ComboWmin->GetInteger()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
}



PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	Player = GEntityList->Player();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
}