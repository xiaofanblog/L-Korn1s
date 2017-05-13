#pragma once
#include "Menu.h"
#include "EventManager.h"
#include "DamageTracker.h"

class NautilusBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Nautilus - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ShieldMenu = ComboMenu->AddMenu("W Settings");
			{
				ShieldE = ShieldMenu->CheckBox("Use W", true);
				ShieldHP = ShieldMenu->AddFloat("W if X HP", 1, 100, 50);
				ShieldHPinc = ShieldMenu->AddFloat("W if Inc. Damage Percent", 1, 100, 10);
				ShieldInc = ShieldMenu->CheckBox("W only Incoming Damage(sometimes not accurate)", true);
				ShieldKill = ShieldMenu->CheckBox("W if inc. damage will kill", true);
			}
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			BlacklistMenu = ComboMenu->AddMenu("R Blacklist");

			for (auto enemy : GEntityList->GetAllHeros(false, true)) {
				BlacklistMenu->CheckBox(enemy->ChampionName(), false);
			}

		}
		HarassMenu = MainMenu->AddMenu("Harass");
		{
			HarassMana = HarassMenu->AddFloat("Mana Percent", 10, 100, 50);
			HarassQ = HarassMenu->CheckBox("Use Q in Harass", true);
			HarassW = HarassMenu->CheckBox("Use W in Harass", true);
			HarassE = HarassMenu->CheckBox("Use E in Harass", true);

		}

		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
			DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);

		}
		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			InterruptQ = MiscMenu->CheckBox("Q Interrupt", true);
			InterruptR = MiscMenu->CheckBox("R Interrupt", true);
			AntiGapQ = MiscMenu->CheckBox("Q AntiGap", true);
		}
		FarmMenu = MainMenu->AddMenu("Farming");
		{
			FarmMana = FarmMenu->AddFloat("Mana Percent", 10, 100, 50);
			FarmE = FarmMenu->CheckBox("Use E to Farm", true);
		}
	}


	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall | kCollidesWithMinions | kCollidesWithWalls));
		Q->SetSkillshot(0.1f, 70.f, 2000.f, 1080.f);

		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, (kCollidesWithNothing));
		W->SetSkillshot(0.25f, 0.f, 1000.f, 0.f);

		E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, (kCollidesWithNothing));
		E->SetSkillshot(0.25f, 450.f, 3200.f, 520.f);

		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, (kCollidesWithNothing));
		R->SetSkillshot(0.25f, 60.f, 1000.f, 825.f);


	}


	int GetEnemiesInRange(IUnit* Source, float range)
	{
		auto enemies = GEntityList->GetAllHeros(false, true);
		auto enemiesInRange = 0;

		for (auto enemy : enemies)
		{
			if (enemy->IsValidTarget() && !enemy->IsDead() && enemy != nullptr && enemy->GetTeam() != GEntityList->Player()->GetTeam())
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
	void Farm()
	{
		if (FarmMana->GetFloat() < GEntityList->Player()->ManaPercent())
		{
			for (auto Minion : GEntityList->GetAllMinions(false, true, true))
			{
				if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnPlayer();
				}
			}
		}
	}
	void AutoW()
	{
		if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		{
			static DamageTracker tracker = DamageTracker(GPluginSDK, true);
			for (auto ally : GEntityList->GetAllHeros(true, false))
			{
				if (ShieldE->Enabled() && !GEntityList->Player()->IsRecalling())
				{
					if (ally == GEntityList->Player())
					{
						if (GEntityList->Player() != nullptr && !GEntityList->Player()->IsDead())
						{

							if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat())
							{
								W->CastOnPlayer();
							}
							if (GetEnemiesInRange(GEntityList->Player(), W->Range()) >= 1 && GEntityList->Player()->HealthPercent() <= ShieldHP->GetFloat())
							{
								W->CastOnPlayer();
							}
						}

						if (!tracker.hasIncomingDamage(GEntityList->Player()))
							return;
						if (ShieldKill->Enabled())
						{
							if (tracker.getIncomingDamage(ally) >= GEntityList->Player()->GetHealth())
							{
								W->CastOnPlayer();
							}
						}

					}

					if (ShieldInc->Enabled())
					{
						if (ally == GEntityList->Player())
						{
							if (GEntityList->Player() != nullptr && !GEntityList->Player()->IsDead())
							{

								if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat())
								{
									W->CastOnPlayer();
								}
							}
							if (!tracker.hasIncomingDamage(GEntityList->Player()))
								return;
							if (ShieldKill->Enabled())
							{
								if (tracker.getIncomingDamage(GEntityList->Player()) >= GEntityList->Player()->GetHealth())
								{
									W->CastOnPlayer();
								}
							}

						}



					}
				}
			}
		}
		if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
		{
			static DamageTracker tracker = DamageTracker(GPluginSDK, true);
			for (auto ally : GEntityList->GetAllHeros(true, false))
			{
				if (HarassW->Enabled())
				{
					if (ally == GEntityList->Player())
					{
						if (GEntityList->Player() != nullptr && !GEntityList->Player()->IsDead())
						{

							if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat())
							{
								W->CastOnPlayer();
							}
							if (GetEnemiesInRange(GEntityList->Player(), W->Range()) >= 1 && GEntityList->Player()->HealthPercent() <= ShieldHP->GetFloat())
							{
								W->CastOnPlayer();
							}
						}

						if (!tracker.hasIncomingDamage(GEntityList->Player()))
							return;
						if (ShieldKill->Enabled())
						{
							if (tracker.getIncomingDamage(ally) >= GEntityList->Player()->GetHealth())
							{
								W->CastOnPlayer();
							}
						}

					}

					if (ShieldInc->Enabled())
					{
						if (ally == GEntityList->Player())
						{
							if (GEntityList->Player() != nullptr && !GEntityList->Player()->IsDead())
							{

								if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat())
								{
									W->CastOnPlayer();
								}
							}
							if (!tracker.hasIncomingDamage(GEntityList->Player()))
								return;
							if (ShieldKill->Enabled())
							{
								if (tracker.getIncomingDamage(GEntityList->Player()) >= GEntityList->Player()->GetHealth())
								{
									W->CastOnPlayer();
								}
							}

						}



					}
				}
			}
		}

	}
	void Combo()
	{
		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				Q->CastOnTarget(target);
			}
		}
		if (ComboE->Enabled() && E->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				E->CastOnPlayer();
			}
		}
		if (ComboR->Enabled())
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				IMenuOption * temp = BlacklistMenu->GetOption(Enemy->ChampionName());

				if (Enemy != nullptr && Enemy->IsValidTarget() && !Enemy->IsDead())
				{
					if (R->IsReady() && !temp->Enabled())
					{
						if ((!Enemy->HasBuffOfType(BUFF_SpellShield) || !Enemy->HasBuffOfType(BUFF_SpellImmunity)) && Enemy->IsValidTarget(GEntityList->Player(), R->Range()))
						{
							R->CastOnTarget(Enemy);
						}
					}

				}
			}
		}
	}
	void Harass()
	{
		if (HarassQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				AdvPredictionOutput outputfam;
				Q->RunPrediction(target, false, kCollidesWithMinions | kCollidesWithWalls | kCollidesWithYasuoWall, &outputfam);
				if (outputfam.HitChance >= kHitChanceHigh)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
					if (InSpellRange(Q, pred))
					{
						Q->CastOnPosition(pred);
					}
				}
			}
		}
		if (ComboE->Enabled() && E->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				E->CastOnPlayer();
			}
		}
	}

	void Interrupt(InterruptibleSpell const& Args)
	{
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, Q->Range()) && InterruptQ->Enabled() && Q->IsReady())
		{
			Q->CastOnTarget(Args.Target);
		}
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, R->Range()) && InterruptR->Enabled() && R->IsReady())
		{
			if (Args.DangerLevel == kHighDanger)

			{
				R->CastOnTarget(Args.Target);
			}
		}
	}
	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, Q->Range() + Args.Sender->BoundingRadius())
			&& AntiGapQ->Enabled() && E->IsReady())
		{
			Q->CastOnPosition(Args.EndPosition);
		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(145, 255, 255, 255), Q->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	}
};