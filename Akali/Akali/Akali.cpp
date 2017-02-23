#include "PluginSDK.h"
#include "SimpleLib.h"

PluginSetup("Kornis's Akali")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboWR;
IMenuOption* ComboRStack;
IMenuOption* ItemsCombo;
IMenuOption* IgniteCombo;


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
IMenuOption* FarmQL;
IMenuOption* FarmEnergy;

IMenu* FleeMenu;
IMenuOption* FleeW;
IMenuOption* FleeR;
IMenuOption* FleeKey;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSE;
IMenuOption* KSR;
IMenuOption* KSRGap;

IInventoryItem* Gunblade;
IInventoryItem* Cutlass;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Ignite;


IUnit* Player;


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithYasuoWall);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, true, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, true, kCollidesWithNothing);
	Gunblade = GPluginSDK->CreateItemForId(3146, 700);
	Cutlass = GPluginSDK->CreateItemForId(3144, 550);
	if (GEntityList->Player()->GetSpellSlot("SummonerDot") != kSlotUnknown)
		Ignite = GPluginSDK->CreateSpell2(GEntityList->Player()->GetSpellSlot("SummonerDot"), kTargetCast, false, false, kCollidesWithNothing);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Akali");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
		ComboW = ComboMenu->CheckBox("Use W to gapclose(Useful pre-6)", true);
		ComboE = ComboMenu->CheckBox("Use E in Combo", true);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);
		ComboRStack = ComboMenu->AddInteger("Save x R for Killsecure", 0, 3, 1);
		ComboWR = ComboMenu->AddKey("Chase combo(W Gap Close for R)", 'T');
		ItemsCombo = ComboMenu->CheckBox("Use Items", true);
		IgniteCombo = ComboMenu->CheckBox("Use Ignite", true);

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
	{
		FarmEnergy = FarmMenu->AddInteger("Energy percent for clear", 10, 100, 50);
		FarmQL = FarmMenu->CheckBox("Last Hit with Q", true);
		FarmQ = FarmMenu->CheckBox("Lane Clear with Q", true);
		FarmE = FarmMenu->CheckBox("Lane Clear with E", true);
	}
	FleeMenu = MainMenu->AddMenu("Flee menu");
	{
		FleeW = FleeMenu->CheckBox("Flee with W", true);
		FleeR = FleeMenu->CheckBox("Flee with R(SOON)", true);
		FleeKey = FleeMenu->AddKey("Flee Key", 'G');
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSE = KillstealMenu->CheckBox("Use E", true);
		KSR = KillstealMenu->CheckBox("Use R", true);
		KSRGap = KillstealMenu->CheckBox("Use R to gap", true);
	}
}

void ComboWRG()
{
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
		bool hasIgnite = GEntityList->Player()->GetSpellState(GEntityList->Player()->GetSpellSlot("SummonerDot")) == Ready;

		if (Gunblade->IsOwned() && Gunblade->IsReady() && ItemsCombo->Enabled() && !(Player->IsDead()))
		{
			if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Gunblade->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
			{
				Gunblade->CastOnTarget(GTargetSelector->GetFocusedTarget());
			}
			else
			{
				Gunblade->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 700));
			}
		}
		if (Cutlass->IsOwned() && Cutlass->IsReady() && ItemsCombo->Enabled() && !(Player->IsDead()))
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
		if (ComboR->Enabled() && R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
			if (target != nullptr)
			{
				R->CastOnTarget(target);
			}
		}
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && IgniteCombo->Enabled() && hasIgnite && GDamage->GetSummonerSpellDamage(GEntityList->Player(), target, kSummonerSpellIgnite) > target->GetHealth())
		{
			Ignite->CastOnTarget(target);
		}
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
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range() + 120);
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
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range() + R->Range());
			if (target != nullptr)
			{
				float myDistance = SimpleLib::SimpleLib::GetDistance(target, Player);
				if (myDistance >= 700) {
					W->CastOnPosition(target->ServerPosition());
				}

			}
		}
	}

}

int GetUltStacks()
{
	return GEntityList->Player()->HasBuff("AkaliShadowDance") ? GEntityList->Player()->GetBuffCount("AkaliShadowDance") : 0;
}

void Combo()
{
	{
		bool hasIgnite = GEntityList->Player()->GetSpellState(GEntityList->Player()->GetSpellSlot("SummonerDot")) == Ready;
		if (Gunblade->IsOwned() && Gunblade->IsReady() && ItemsCombo->Enabled() && !(Player->IsDead()))
		{
			if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Gunblade->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
			{
				Gunblade->CastOnTarget(GTargetSelector->GetFocusedTarget());
			}
			else
			{
				Gunblade->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 700));
			}
		}
		if (Cutlass->IsOwned() && Cutlass->IsReady() && ItemsCombo->Enabled() && !(Player->IsDead()))
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
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && IgniteCombo->Enabled() && hasIgnite && GDamage->GetSummonerSpellDamage(GEntityList->Player(), target, kSummonerSpellIgnite) > target->GetHealth())
		{
			Ignite->CastOnTarget(target);
		}
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
				E->CastOnPlayer();
			}
		}
		if (ComboW->Enabled() && W->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range() + 120);
			if (target != nullptr)
			{
				float myDistance = SimpleLib::SimpleLib::GetDistance(target, Player);
				if (myDistance >= 200)
				{
					W->CastOnTarget(target);
				}
			}

		}
		if (ComboR->Enabled() && R->IsReady() && GetUltStacks() > ComboRStack->GetInteger())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
			if (target != nullptr)
			{
				R->CastOnTarget(target);
			}
		}

	}
}

void CheckRKS(IUnit* Enemy)
{
	for (auto target : GEntityList->GetAllMinions(false, true, true))
	{
		if (target->IsValidTarget(GEntityList->Player(), R->Range()) && (Enemy->GetPosition() - target->GetPosition()).Length() < R->Range())
		{
			R->CastOnTarget(target);
		}
	}
}
void Flee()
{
	if (!GGame->IsChatOpen())
	{
		/*if (FleeR->Enabled())
		{
			GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
			auto minion = GTargetSelector->FindTarget(ClosestToCursorPriority, SpellDamage, R->Range());
			for (auto minion : GEntityList->GetAllMinions(false, true, true))
				if (R->IsReady())
				{
					if (minion != nullptr)
					{
						if (minion->IsEnemy(GEntityList->Player()) && !minion->IsDead() && GEntityList->Player()->IsValidTarget(minion, R->Range()))
						{
							R->CastOnTarget(minion);
						}
					}
				}
		}*/

		if (FleeW->Enabled() && W->IsReady())
		{
			W->CastOnPosition(GGame->CursorPosition());
		}

	}
}

void LastHit()
{
	for (auto Minion : GEntityList->GetAllMinions(false, true, true))
	{
		if (!Minion->IsDead() && Minion != nullptr)
		{
			if (FarmQL->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
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

void Killsteal()
{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
			auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
			auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);

			if (Enemy != nullptr && !Enemy->IsDead())
			{
				if (KSQ->Enabled() && KSRGap->Enabled() && Q->IsReady() && R->IsReady() && GetUltStacks() > 1 && Enemy->IsValidTarget(GEntityList->Player(), R->Range() * 2) && QDamage > Enemy->GetHealth())
				{
					CheckRKS(Enemy);
					R->CastOnTarget(Enemy);
				}
				if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
				{
					Q->CastOnTarget(Enemy);
				}
				if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
				{
					E->CastOnTarget(Enemy);
				}
				if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage > Enemy->GetHealth())
				{
					R->CastOnTarget(Enemy);
				}
			}
		}
}


void Farm()
{
	if (Player->ManaPercent() < FarmEnergy->GetInteger())
		return;
	if (FarmE->Enabled())
	{
		if (E->IsReady())
		{
			E->AttackMinions();
		}
	}
	if (FarmQ->Enabled())
	{
		if (Q->IsReady())
		{
			Q->AttackMinions();
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
	if (GetAsyncKeyState(ComboWR->GetInteger()) & 0x8000)
	{
		ComboWRG();
	}
	if (GetAsyncKeyState(FleeKey->GetInteger()) & 0x8000)
	{
		Flee();
	}
	Auto();
	Killsteal();
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range() + 120); }
	if (DrawWRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range() + R->Range()); }
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