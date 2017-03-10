#include "PluginSDK.h"
#include "SimpleLib.h"
#include <string>

PluginSetup("Kornis's Irelia")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboEStun;
IMenuOption* ComboR;
IMenuOption* ComboQmin;
IMenuOption* ComboRgap;
IMenuOption* ComboRgapks;
IMenuOption* ComboQgap;
IMenuOption* ComboItems;
IMenuOption* Rkillable;

IMenu* HarassMenu;
IMenuOption* HarassQmin;
IMenuOption* HarassQ;
IMenuOption* HarassW;
IMenuOption* HarassE;


IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;
IMenuOption* DrawQkill;
IMenuOption* DrawDamage;
IMenuOption* DrawFill;

IMenu* LastHitMenu;
IMenuOption* LasthitQ;
IMenuOption* LasthitMana;
IMenuOption* LasthitQmin;

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
int yOffset = 15;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	E->SetOverrideRange(320);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithYasuoWall);
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
		ComboEStun = ComboMenu->CheckBox("Use E only for stun", true);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);
		Rkillable = ComboMenu->CheckBox("Use R only if killable with combo", true);
		ComboQmin = ComboMenu->AddInteger("Min. Q Range ", 20, 400, 250);
		ComboQgap = ComboMenu->CheckBox("Use Q to Gapclose", true);
		ComboRgap = ComboMenu->CheckBox("Use R to Gapclose", true);
		ComboRgapks = ComboMenu->CheckBox("R gap only when killable", true);
		ComboItems = ComboMenu->CheckBox("Use Items", true);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQmin = HarassMenu->CheckBox("Use Q to gapclose on minion", true);
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
		DrawDamage = DrawingMenu->CheckBox("Draw if killable", true);
		DrawFill = DrawingMenu->CheckBox("Fill damage", true);
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
		LasthitQmin = LastHitMenu->CheckBox("Dont Q AA range", false);
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
	if (Enemy != nullptr && ComboRgap->Enabled() && !Enemy->IsDead() && Q->IsReady() && R->IsReady() && ComboRgapks->Enabled())
	{
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
		auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);
		if (RDamage * 3 + EDamage + QDamage > Enemy->GetHealth())
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
	if (Enemy != nullptr && ComboRgap->Enabled() && !Enemy->IsDead() && Q->IsReady() && R->IsReady() && !ComboRgapks->Enabled())
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
			float percentHealthAfterDamage = max(0, hero->GetHealth() - float(RDamage * 3 + EDamage + QDamage)) / hero->GetMaxHealth();
			float yPos = barPos.y + yOffset;
			float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
			float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());

			if (RDamage * 3 + EDamage + QDamage > hero->GetHealth())
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
	auto Etarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, E->Range());
	auto Rtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, R->Range());
	auto Qtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
	auto Wtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range() - 200);

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
			if (ComboE->Enabled() && E->IsReady() && E->Range() && !ComboEStun->Enabled())
			{
				if (Etarget != nullptr)
				{
					E->CastOnTarget(Etarget);
				}
			}
			if (ComboE->Enabled() && E->IsReady() && E->Range() && ComboEStun->Enabled())
			{
				if (Enemy->HealthPercent() > Player->HealthPercent())
				{
					E->CastOnTarget(Etarget);
				}
			}

			if (ComboR->Enabled() && R->IsReady() && R->Range() && !Rkillable->Enabled())
			{
				if (Q->IsReady() && E->IsReady())
				{
					if (Rtarget != nullptr)
					{
						if (!Player->HasBuff("IreliaTranscendentBladesSpell"))
						{
							R->CastOnTarget(Rtarget);
						}
					}
				}
				if (Player->HasBuff("IreliaTranscendentBladesSpell"))
				{
					{
						R->CastOnTarget(Rtarget);
					}
				}
			}
			if (ComboR->Enabled() && R->IsReady() && Q->Range() && Rkillable->Enabled())
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);
				if (RDamage*3 + EDamage + QDamage > Enemy->GetHealth() && Q->IsReady() && E->IsReady())
				{
					if (!Player->HasBuff("IreliaTranscendentBladesSpell"))
					{
						R->CastOnTarget(Qtarget);
					}
				}
				if (Player->HasBuff("IreliaTranscendentBladesSpell"))
				{
					{
						R->CastOnTarget(Rtarget);
					}
				}
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
				if (!LasthitQmin->Enabled() && LasthitQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
				{
					Q->CastOnUnit(Minion);
				}
				if (LasthitQmin->Enabled() && LasthitQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
				{
					if (!Minion->IsValidTarget(Player, 200))
					{
						Q->CastOnUnit(Minion);
					}
				}
			}
		}
	}
}

void Mixed()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (!Enemy->IsDead())
		{
			auto Qtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
			auto Etarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, E->Range());
			auto Wtarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range() - 200);
			if (HarassQ->Enabled() && Q->IsReady() && Q->Range() && !HarassQmin->Enabled())
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
			if (HarassQ->Enabled() && Q->IsReady() && HarassQmin->Enabled())
			{

				if (Q->IsReady() && (Enemy->GetPosition() - Player->GetPosition()).Length() > ComboQmin->GetInteger())
				{
					minionQ(Enemy);
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
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				Q->CastOnTarget(Enemy);
			}
			else if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
			{
				E->CastOnTarget(Enemy);
			}
			else if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage*3 > Enemy->GetHealth())
			{
				if (!Player->HasBuff("IreliaTranscendentBladesSpell"))
				{
					R->CastOnTarget(Enemy);
				}
				if (Player->HasBuff("IreliaTranscendentBladesSpell"))
				{
					R->CastOnTarget(Enemy);
				}
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
			if (minionQ->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), minionQ, kSlotQ);
				{
					if (QDamage > minionQ->GetHealth() && minionQ != nullptr && !minionQ->IsDead())
					{
						GRender->DrawOutlinedCircle(minionQ->GetPosition(), Vec4(255, 255, 0, 255), 80.f);
					}
				}
			}

		}
	}
	dmgdraw();
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