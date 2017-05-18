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
IMenuOption* DrawDamage;


IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmQL;
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
IMenu* MiscMenu;
IMenuOption* InterruptQ;
IMenu* BurstMenu;
IMenuOption* BurstKey;
IMenuOption* BurstE;



ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

ISpell* Flash;


IUnit* Player;
int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(25, 255, 0, 200);



void LoadSpells()
{
	if (GEntityList->Player()->GetSpellSlot("SummonerFlash") != kSlotUnknown)
		Flash = GPluginSDK->CreateSpell(GEntityList->Player()->GetSpellSlot("SummonerFlash"), 425);
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
		DrawDamage = DrawingMenu->CheckBox("Draw Damage", true);
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
		FarmQL = FarmMenu->CheckBox("Last Hit Q", true);
	}

	MiscMenu = MainMenu->AddMenu("Misc");
	{
		InterruptQ = MiscMenu->CheckBox("Interrupt with Q", true);
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
	BurstMenu = MainMenu->AddMenu("Burst Settings");
	{
		BurstKey = BurstMenu->AddKey("Burst Key", 'T');
		BurstE = BurstMenu->CheckBox("Wait for E", true);
	}

}

static double GetWdmg(IUnit* target)
{
	int qedmg = 0;	
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 1)
	{
		qedmg = 80;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 2)
	{
		qedmg = 105;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 3)
	{
		qedmg = 130;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 4)
	{
		qedmg = 155;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 5)
	{
		qedmg = 180;

	}
	auto calc = GEntityList->Player()->TotalMagicDamage() * 0.7 ;
	auto full = calc + qedmg;
	auto damage = GDamage->CalcMagicDamage(GEntityList->Player(), target, full);
	return damage;

}
void dmgdraw()
{
	for (auto hero : GEntityList->GetAllHeros(false, true))
	{
		Vec2 barPos = Vec2();
		if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
		{
			auto QDamage = 0;

			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) > 0)
			{
				QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
			}


			auto WDamage = 0;
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotW) > 0)
			{
				WDamage = GetWdmg(hero);
			}
			auto EDamage = 0;
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotW) > 0)
			{
				EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
			}
			auto RDamage = 0;
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotR) > 0)
			{
				RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR);
			}
			auto damage = QDamage + EDamage + RDamage +WDamage;
			float percentHealthAfterDamage = max(0, hero->GetHealth() - float(damage)) / hero->GetMaxHealth();
			float yPos = barPos.y + yOffset;
			float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
			float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());
			if (!hero->IsDead() && hero->IsValidTarget())
			{
				float differenceInHP = xPosCurrentHp - xPosDamage;
				float pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

				for (int i = 0; i < differenceInHP; i++)
				{
					if (damage < hero->GetHealth())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
					}
					if (damage > hero->GetHealth())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), Color2);
					}

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
				E->CastOnPosition(target->GetPosition());
			}
		}

	}
}
void Burst()
{
	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (Enemy->IsValidTarget() && Enemy != nullptr && !Enemy->IsDead())
			{

				if (BurstE->Enabled() && GEntityList->Player()->HasBuff("forcepulsecancast"))
				{
					if (R->IsReady() && Flash != nullptr && Flash->IsReady())
					{
						if (Enemy->IsValidTarget() && Enemy != nullptr && ((Player->GetPosition() - Enemy->GetPosition()).Length()) < R->Range() + 550 && !Enemy->IsDead())
						{
							if ((Player->GetPosition() - Enemy->GetPosition()).Length() > R->Range())
							{
								if (R->CastOnPosition(Enemy->ServerPosition()))
								{
									Flash->CastOnPosition(Enemy->ServerPosition());
								}
							}
						}
					}
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 250);
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							W->CastOnPlayer();
						}
					}
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							E->CastOnPosition(target->GetPosition());
						}
					}
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							Q->CastOnTarget(target);
						}
					}


					if ((Player->GetPosition() - Enemy->GetPosition()).Length() < R->Range())
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							R->CastOnTarget(target);
						}
					}
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 250);
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							W->CastOnPlayer();
						}
					}
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							E->CastOnPosition(target->GetPosition());
						}
					}
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							Q->CastOnTarget(target);
						}
					}


				}


				if (!BurstE->Enabled())
				{
					if (R->IsReady() && Flash != nullptr && Flash->IsReady())
					{
						if (Enemy->IsValidTarget() && Enemy != nullptr && ((Player->GetPosition() - Enemy->GetPosition()).Length()) < R->Range() + 550 && !Enemy->IsDead())
						{
							if ((Player->GetPosition() - Enemy->GetPosition()).Length() > R->Range())
							{
								if (R->CastOnPosition(Enemy->ServerPosition()))
								{
									Flash->CastOnPosition(Enemy->ServerPosition());
								}
							}
						}
						{
							auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 250);
							if (target != nullptr && target->IsValidTarget() && !target->IsDead())
							{
								W->CastOnPlayer();
							}
						}
						{
							auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
							if (target != nullptr && target->IsValidTarget() && !target->IsDead())
							{
								E->CastOnPosition(target->GetPosition());
							}
						}
						{
							auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
							if (target != nullptr && target->IsValidTarget() && !target->IsDead())
							{
								Q->CastOnTarget(target);
							}
						}
					}


					if ((Player->GetPosition() - Enemy->GetPosition()).Length() < R->Range())
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							R->CastOnTarget(target);
						}

					}

					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 250);
					if (target != nullptr && target->IsValidTarget() && !target->IsDead())
					{
						W->CastOnPlayer();
					}
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							E->CastOnPosition(target->GetPosition());
						}
					}
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							Q->CastOnTarget(target);
						}
					}

				}


			}


			if (GEntityList->Player()->HasBuff("NetherBlade"))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 250);
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					GGame->IssueOrder(GEntityList->Player(), kAttackUnit, target);
				}
			}
		}
	}
}
void LastHit()
{
	for (auto Minion : GEntityList->GetAllMinions(false, true, true))
	{
		if (!Minion->IsDead() && Minion != nullptr && Minion->IsValidTarget())
		{
			if (LastQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
			{
				Q->CastOnUnit(Minion);
			}
			if (LastW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), W->Range() + 200) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, GetWdmg(Minion)) >= Minion->GetHealth())
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
				E->CastOnPosition(Enemy->GetPosition());
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

PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (Args.Source != nullptr && InterruptQ->Enabled() && (Args.Source->GetPosition() - GEntityList->Player()->GetPosition()).Length() < Q->Range() && Q->IsReady() && Args.Source->IsValidTarget())
	{
		Q->CastOnTarget(Args.Source);
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
			if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
			{
				if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnTarget(Minion);
				}
				if (FarmQL->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
				{
					Q->CastOnTarget(Minion);
				}
				if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), R->Range()))
				{
					E->CastOnTarget(Minion);
				}
				if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 200))
				{
					W->CastOnPlayer();
				}
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
	if (GetAsyncKeyState(BurstKey->GetInteger()) & 0x8000)
	{
		Burst();
	}
	Killsteal();
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawDamage->Enabled())
	{
		dmgdraw();
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
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
}
