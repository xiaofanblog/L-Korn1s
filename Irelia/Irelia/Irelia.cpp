#include "PluginSDK.h"
#include "SimpleLib.h"
#include <string>

PluginSetup("Kornis's Irelia")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboQmin;
IMenuOption* ComboRgap;
IMenuOption* ComboQgap;
IMenuOption* ComboItems;
IMenuOption* ComboIgnite;

IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassW;
IMenuOption* HarassE;


IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;
IMenuOption* DrawQkill;

IMenu* LastHitMenu;
IMenuOption* LasthitQ;
IMenuOption* LasthitMana;

IMenu* LaneClearMenu;
IMenuOption* LaneQ;
IMenuOption* LanekillQ;
IMenuOption* LaneW;
IMenuOption* LaneMana;


IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSE;
IMenuOption* KSR;

IInventoryItem* Botrk;
IInventoryItem* Cutlass;


ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Ignite;


IUnit* Player;

int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, true, kCollidesWithYasuoWall);
	if (GEntityList->Player()->GetSpellSlot("SummonerDot") != kSlotUnknown)
		Ignite = GPluginSDK->CreateSpell2(GEntityList->Player()->GetSpellSlot("SummonerDot"), kTargetCast, false, false, kCollidesWithNothing);

	Botrk = GPluginSDK->CreateItemForId(3153, 550);
	Cutlass = GPluginSDK->CreateItemForId(3144, 550);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Irelia");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
		ComboW = ComboMenu->CheckBox("Use W in Combo", true);
		ComboE = ComboMenu->CheckBox("Use E in Combo", true);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);
		ComboQmin = ComboMenu->AddInteger("Min. Q Range ", 20, 400, 250);
		ComboQgap = ComboMenu->CheckBox("Use Q to Gapclose", true);
		ComboRgap = ComboMenu->CheckBox("Use R to Gapclose", true);
		ComboItems = ComboMenu->CheckBox("Use Items", true);
		ComboIgnite = ComboMenu->CheckBox("Use ignite", true);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQ = HarassMenu->CheckBox("Use Q to Harass", true);
		HarassW = HarassMenu->CheckBox("Use W to Harass", true);
		HarassE = HarassMenu->CheckBox("Use E to Harass", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawQkill = DrawingMenu->CheckBox("Draw Minions Killable With Q", true);
	}
	LaneClearMenu = MainMenu->AddMenu("Lane Clear");
	{
		LaneMana = LaneClearMenu->AddInteger("Mana for Lane Clear", 10, 100, 50);
		LaneQ = LaneClearMenu->CheckBox("Lane Clear with Q", true);
		LanekillQ = LaneClearMenu->CheckBox("Q Only If Minion Killable", true);
		LaneW = LaneClearMenu->CheckBox("Lane Clear with W", true);
	}
	LastHitMenu = MainMenu->AddMenu("Last Hit");
	{
		LasthitMana = LastHitMenu->AddInteger("Mana for Last Hit", 10, 100, 50);
		LasthitQ = LastHitMenu->CheckBox("Use Q to Last Hit", true);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{

		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSE = KillstealMenu->CheckBox("Use E", true);
		KSR = KillstealMenu->CheckBox("Use R", true);
	}
}

void minionQ(IUnit* Enemy)
{
	if (Enemy != nullptr && ComboQgap->Enabled() && !Enemy->IsDead() && Q->IsReady())
	{
		for (auto minionQ : GEntityList->GetAllMinions(false, true, true))
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), minionQ, kSlotQ);
			float myDistance = SimpleLib::SimpleLib::GetDistance(Enemy, Player);
			if (myDistance < Q->Range())
			{
				return;
			}
			else if (minionQ->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > minionQ->GetHealth() && (Enemy->GetPosition() - minionQ->GetPosition()).Length() < Q->Range() && Player->GetMana() > Q->ManaCost() * 2)
			{
				if (minionQ != nullptr)
				{
					Q->CastOnUnit(minionQ);
					return;
				}
			}
		}
		
	}

}

void minionR(IUnit* Enemy)
{
	if (Enemy != nullptr && ComboRgap->Enabled() && !Enemy->IsDead() && Q->IsReady() && R->IsReady())
	{
		for (auto minionR : GEntityList->GetAllMinions(false, true, true))
		{
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), minionR, kSlotR);
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), minionR, kSlotQ);
				if (minionR->IsValidTarget(GEntityList->Player(), Q->Range()) && (Enemy->GetPosition() - minionR->GetPosition()).Length() < Q->Range() && minionR->GetHealth() > QDamage && (minionR->GetHealth() - RDamage * 2) < QDamage)
				{
					if (minionR != nullptr)
					{
						R->CastOnUnit(minionR);
					}
				}
		}
	}
}

void Combo()
{
	auto Etarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, E->Range());
	auto Rtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, R->Range());
	auto Qtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
	auto Wtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range() - 200);

	bool hasIgnite = GEntityList->Player()->GetSpellState(GEntityList->Player()->GetSpellSlot("SummonerDot")) == Ready;
	if (Qtarget != nullptr && ComboIgnite->Enabled() && hasIgnite && GDamage->GetSummonerSpellDamage(GEntityList->Player(), Qtarget, kSummonerSpellIgnite) > Qtarget->GetHealth())
	{
		Ignite->CastOnTarget(Qtarget);
	}
	if (Botrk->IsOwned() && Botrk->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Botrk->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			Botrk->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			Botrk->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 550));
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


	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (!Enemy->IsDead())
		{
			if (Q->IsReady())
			{
				minionQ(Enemy);
			}
			float myDistance = SimpleLib::SimpleLib::GetDistance(Enemy, Player);
			if (myDistance > Q->Range())
			{
				minionR(Enemy);
			}
			if (ComboQ->Enabled() && Q->IsReady() && Q->Range())
			{
				if (Qtarget != nullptr)
				{
					if ((Qtarget->GetPosition() - Player->GetPosition()).Length() > ComboQmin->GetInteger())
					{
						Q->CastOnTarget(Qtarget);
					}
				}
			}
			if (ComboW->Enabled() && W->IsReady() && Q->Range() - 200)
			{
				if (Wtarget != nullptr)
				{
					W->CastOnPlayer();
				}
			}
			if (ComboE->Enabled() && E->IsReady() && E->Range())
			{
				if (Etarget != nullptr)
				{
					E->CastOnTarget(Etarget);
				}
			}
			if (ComboR->Enabled() && R->IsReady() && !Player->HasBuff("IreliaTranscendentBladesSpell") && R->Range())
			{
				if (Rtarget != nullptr)
				{
					R->CastOnTarget(Rtarget, 5);
				}
			}
			if (Player->HasBuff("IreliaTranscendentBladesSpell"))
			{
				R->CastOnTarget(Rtarget, 5);
			}
		}
	}
}
void LastHit()
{
	if (Player->ManaPercent() > LasthitMana->GetInteger())
	{ 
		for (auto Minion : GEntityList->GetAllMinions(false, true, true))
		{
			if (!Minion->IsDead() && Minion != nullptr)
			{
				if (LasthitQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
				{
					Q->CastOnUnit(Minion);
				}
			}
		}
	}
}

void Mixed()
{
	auto Qtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
	auto Etarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
	auto Wtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range() - 200);
	if (HarassQ->Enabled() && Q->IsReady() && Q->Range())
	{
		if (Qtarget != nullptr)
		{
			if ((Qtarget->GetPosition() - Player->GetPosition()).Length() > ComboQmin->GetInteger())
			{
				Q->CastOnTarget(Qtarget);
			}
		}
	}
	if (HarassW->Enabled() && W->IsReady() && Q->Range() - 200)
	{
		if (Wtarget != nullptr)
		{
			W->CastOnPlayer();
		}
	}
	if (HarassE->Enabled() && E->IsReady() && E->Range())
	{
		if (Etarget != nullptr)
		{
			E->CastOnTarget(Etarget);
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
			if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage*3 > Enemy->GetHealth())
			{
				if (!Player->HasBuff("IreliaTranscendentBladesSpell"))
				{
					R->CastOnTarget(Enemy);
				}
			}  
			if (Player->HasBuff("IreliaTranscendentBladesSpell"))
			{
				R->CastOnTarget(Enemy);
			}
		}
	}
}

void Farm()
{
	if (Player->ManaPercent() > LaneMana->GetInteger())
	{
		for (auto Minion : GEntityList->GetAllMinions(false, true, true))
		{
			if (!Minion->IsDead() && Minion != nullptr)
			{
				if (LaneQ->Enabled() && Q->IsReady() && !LanekillQ->Enabled() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnUnit(Minion);
				}
				if (LanekillQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
				{
					Q->CastOnUnit(Minion);
				}
				if (LaneW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 200))
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
	Killsteal();
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 0, 0, 255), float(ComboQmin->GetInteger()));
	if (DrawQkill->Enabled())
	{
		for (auto minionQ : GEntityList->GetAllMinions(false, true, false))
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), minionQ, kSlotQ);
			if (QDamage > minionQ->GetHealth() && minionQ != nullptr)
			{
				GRender->DrawOutlinedCircle(minionQ->GetPosition(), Vec4(255, 255, 0, 255), 80.f);
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