#include "PluginSDK.h"
#include <string>

PluginSetup("Nasus by Kornis")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboQAA;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboRhp;
IMenuOption* ComboWmin;
IMenuOption* Combomin;

IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassQAA;
IMenuOption* HarassW;
IMenuOption* HarassE;
IMenuOption* HarassMana;
IMenuOption* HarassFarm;



IMenu* DrawingMenu;
IMenuOption* DrawERange;
IMenuOption* DrawWRange;
IMenuOption* DrawRRange;
IMenuOption* DrawQ;
IMenuOption* DrawQkill;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmMana;
IMenuOption* FarmQLS;
IMenuOption* FarmE;
IMenuOption* FarmEmin;

IMenu* LastMenu;
IMenuOption* FarmQLast;
IMenu* MiscMenu;
IMenuOption* AntiGap;
IMenuOption* KillstealE;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

IUnit* Player;
IUnit* target;

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
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Nasus by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q Always", false);
		ComboQAA = ComboMenu->CheckBox("Use Q AA Reset", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);
		ComboRhp = ComboMenu->AddInteger("My HP for R", 10, 100, 30);
		Combomin = ComboMenu->AddFloat("R if enemy in X range", 10, 1500, 1000);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassMana = HarassMenu->AddFloat("Mana Percent", 10, 100, 50);
		HarassQ = HarassMenu->CheckBox("Use Q always", false);
		HarassQAA = HarassMenu->CheckBox("Use Q AA reset", true);
		HarassE = HarassMenu->CheckBox("Use E in harass", true);
		HarassFarm = HarassMenu->CheckBox("Last hit in Harass", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawQ = DrawingMenu->CheckBox("Q Helper Minions", true);
		DrawQkill = DrawingMenu->CheckBox("Q Damage on Heroes", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmMana = FarmMenu->AddInteger("Mana percent for Clear", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Q Always", false);
		FarmQLS = FarmMenu->CheckBox("Q Last Hit Only", true);
		FarmE = FarmMenu->CheckBox("Use E", true);
		FarmEmin = FarmMenu->AddFloat("Min. minion for E", 1, 6, 3);
	}

	LastMenu = MainMenu->AddMenu("Last Hit");
	{
		FarmQLast = LastMenu->CheckBox("Q Last Hit", true);
	}

	MiscMenu = MainMenu->AddMenu("Misc.");
	{
		AntiGap = MiscMenu->CheckBox("Anti Gap W", true);
		KillstealE = MiscMenu->CheckBox("Killsteal E", true);
	}
	
}

void dmgdraw()
{
	if (!DrawQkill->Enabled())
		return;
	for (auto hero : GEntityList->GetAllHeros(false, true))
	{
		Vec2 barPos = Vec2();
		if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ) + GDamage->GetAutoAttackDamage(GEntityList->Player(), hero, true);
			float percentHealthAfterDamage = max(0, hero->GetHealth() - float(QDamage)) / hero->GetMaxHealth();
			float yPos = barPos.y + yOffset;
			float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
			float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());
			if ( !hero->IsDead() && hero->IsValidTarget())
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
	
	target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, E->Range());
	for (auto target : GEntityList->GetAllHeros(false, true));
	if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget() && !target->IsDead())
	{
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ) + GDamage->GetAutoAttackDamage(GEntityList->Player(), target, true);
		if (Q->IsReady() && !ComboQAA->Enabled() && ComboQ->Enabled() && target->IsValidTarget(GEntityList->Player(), 300))
		{
			Q->CastOnPlayer();
		}
		if (Q->IsReady() && ComboQAA->Enabled() && !ComboQ->Enabled() && target->IsValidTarget(GEntityList->Player(), 300) && QDamage > target->GetHealth())
		{
			Q->CastOnPlayer();
		}
		if (E->IsReady() && ComboE->Enabled() && target->IsValidTarget(GEntityList->Player(), E->Range()))
		{
			E->CastOnTarget(target);
		}
		if (W->IsReady() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), W->Range()))
		{
			W->CastOnTarget(target);
		}
		if (R->IsReady() && ComboR->Enabled())
		{
			if (ComboRhp->GetInteger() >= GEntityList->Player()->HealthPercent() && target->IsValidTarget(GEntityList->Player(), Combomin->GetInteger()))
			{
				R->CastOnPlayer();
			}
		}
	}
}

void Killsteal()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
		if (Enemy != nullptr && !Enemy->IsDead())
		{
			if (KillstealE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage >= Enemy->GetHealth())
			{
				E->CastOnTarget(Enemy);
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
			if (FarmQLast->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 300))
			{

				if (Minion->GetTeam() != GEntityList->Player()->GetTeam()
					&& !Minion->IsDead()
					&& Minion->GetHealth() <= (GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ)
						+ GDamage->GetAutoAttackDamage(GEntityList->Player(), Minion, true)))
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
}

void Mixed()
{
	if (Player->ManaPercent() > HarassMana->GetFloat())
	{
		target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, E->Range());
		for (auto target : GEntityList->GetAllHeros(false, true));
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget() && !target->IsDead())
		{
			if (Q->IsReady() && !HarassQAA->Enabled() && HarassQ->Enabled() && target->IsValidTarget(GEntityList->Player(), 300))
			{
				Q->CastOnPlayer();
			}
			if (E->IsReady() && HarassE->Enabled() && target->IsValidTarget(GEntityList->Player(), E->Range()))
			{
				E->CastOnTarget(target);
			}
		}
		if (HarassFarm->Enabled())
		{
			for (auto Minion : GEntityList->GetAllMinions(false, true, true))
			{
				if (!Minion->IsDead() && Minion != nullptr)
				{
					if (Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 300))
					{

						if (Minion->GetTeam() != GEntityList->Player()->GetTeam()
							&& !Minion->IsDead()
							&& Minion->GetHealth() <= (GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ)
								+ GDamage->GetAutoAttackDamage(GEntityList->Player(), Minion, true)))
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
				if (!FarmQLS->Enabled() && FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 400))
				{
					Q->CastOnPlayer();
				}
				if (FarmQLS->Enabled() && !FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 300))
				{
					if (Minion->GetTeam() != GEntityList->Player()->GetTeam()
						&& !Minion->IsDead()
						&& Minion->GetHealth() <= (GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ)
							+ GDamage->GetAutoAttackDamage(GEntityList->Player(), Minion, true)))
					{
						if (Q->CastOnPlayer())
						{
							GOrbwalking->SetOverrideTarget(Minion);
							return;
						}
					}
				}
				if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					Vec3 pos;
					int Ehit;
					GPrediction->FindBestCastPosition(E->Range(), 340, false, true, false, pos, Ehit, E->GetDelay());
					if (Ehit >= FarmEmin->GetFloat())
					{
						E->CastOnPosition(pos);
					}
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
	}
}

PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{
	if (Args.Sender != GEntityList->Player()
		&& Args.Sender->IsEnemy(GEntityList->Player())
		&& GEntityList->Player()->IsValidTarget(Args.Sender, W->Range())
		&& AntiGap->Enabled() && W->IsReady())
	{
		W->CastOnTarget(Args.Sender);
	}
}

PLUGIN_EVENT(void) OnRender()
{
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Combomin->GetFloat()); }
	if (DrawQ->Enabled())
	{
		for (auto minionQ : GEntityList->GetAllMinions(false, true, true))
		{
			if (minionQ->IsValidTarget(GEntityList->Player(), 400))
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), minionQ, kSlotQ);
				{
					if (!minionQ->IsDead() && minionQ != nullptr && (minionQ->IsCreep() || minionQ->IsJungleCreep())
							&& minionQ->GetHealth() <= (GDamage->GetSpellDamage(GEntityList->Player(), minionQ, kSlotQ)
								+ GDamage->GetAutoAttackDamage(GEntityList->Player(), minionQ, true)))
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
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
}