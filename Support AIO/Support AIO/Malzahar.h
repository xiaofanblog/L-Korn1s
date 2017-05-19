#pragma once
#include "Menu.h"
#include <string>
class MalzaharBase
{
public:

	void DrawMenu()
	{

		MainMenu = GPluginSDK->AddMenu("Malzahar - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		HarassMenu = MainMenu->AddMenu("Harass");
		LastHitMenu = MainMenu->AddMenu("LastHit");
		FarmMenu = MainMenu->AddMenu("LaneClear");
		DrawingMenu = MainMenu->AddMenu("Drawings");
		MiscMenu = MainMenu->AddMenu("Misc.");

		ComboMode = ComboMenu->AddSelection("Select combo", 0, { "Q>W>E>R", "Q>E>W>R", "R>W>Q>E", "E>W>Q>R"});
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		ComboR = ComboMenu->CheckBox("Use R", true);
		SupportMode = ComboMenu->CheckBox("Support Mode", true);

		FarmMana = FarmMenu->AddFloat("Mana Percent", 1, 100, 75);
		FarmQ = FarmMenu->CheckBox("Q creeps/monsters", false);
		FarmQmin = FarmMenu->AddInteger("Minimum to use Q", 0, 10, 3);
		FarmW = FarmMenu->CheckBox("W creeps/monsters", false);
		FarmWmin = FarmMenu->AddInteger("Minimum to use W", 0, 10, 3);
		FarmE = FarmMenu->CheckBox("E creeps/monsters", false);
		FarmEmin = FarmMenu->AddInteger("Minimum to use E", 0, 10, 3);



		HarassMana = HarassMenu->AddFloat("Mana Percent", 1, 100, 75);
		HarassQ = HarassMenu->CheckBox("Q harass", false);
		HarassW = HarassMenu->CheckBox("W harass", false);
		HarassE = HarassMenu->CheckBox("E harrass", false);



		LastHitMana = LastHitMenu->AddFloat("Mana Percent", 1, 100, 75);
		LastQ = LastHitMenu->CheckBox("Q lasthit", false);
		LastW = LastHitMenu->CheckBox("W lasthit", false);
		LastE = LastHitMenu->CheckBox("E lasthit", false);



		ManaTear = MiscMenu->AddFloat("Min. mana% for tear stacks", 1, 100, 75);
		TearQ = MiscMenu->CheckBox("Cast Q for stacks", false);
		TearW = MiscMenu->CheckBox("Cast W for stacks", false);
		InterruptR = MiscMenu->CheckBox("Interrupt R", true);
		AntiGapR = MiscMenu->CheckBox("AntiGap R", true);

		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", false);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
	}


	bool RBuff(IUnit* target)
	{
		if (target == nullptr)
			return false;

		return target->HasBuff("MalzaharR");
	}

	void AAdisable()
	{
		if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
		{
			auto target = GOrbwalking->GetLastTarget();
			if (SupportMode->Enabled() && GetAlliesInRange(GEntityList->Player(), 1000) >= 1)
			{
				if (target != nullptr && target->IsValidTarget() && (target->IsCreep() || target->IsJungleCreep()))
				{
					GOrbwalking->SetAttacksAllowed(false);
				}
			}
			else if (target != nullptr && target->IsValidTarget() && target->IsHero())
			{
				GOrbwalking->SetAttacksAllowed(true);
			}
		}
		if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		{
			if (SupportMode->Enabled() && GetAlliesInRange(GEntityList->Player(), 1000) >= 1)
			{
				GOrbwalking->SetAttacksAllowed(false);
			}
			else if (!SupportMode->Enabled() || GetAlliesInRange(GEntityList->Player(), 1000) == 0)
			{
				GOrbwalking->SetAttacksAllowed(true);
			}
		}
		else if (GOrbwalking->GetOrbwalkingMode() != kModeLaneClear && GOrbwalking->GetOrbwalkingMode() != kModeMixed)
		{
			GOrbwalking->SetAttacksAllowed(true);

		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithNothing));
		Q->SetSkillshot(0.25f, 70.f, 3200.f, 900.f);
		W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, (kCollidesWithNothing));
		W->SetSkillshot(0.25f, 250.f, 1000.f, 800.f);
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, (kCollidesWithNothing));
		E->SetSkillshot(0.25f, 0.f, 3200.f, 650.f);
		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, (kCollidesWithNothing));
		R->SetSkillshot(0.25f, 0.f, 3200.f, 700.f);
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

	int GetAlliesInRange(IUnit* Source, float range)
	{
		auto allies = GEntityList->GetAllHeros(true, false);
		auto AlliesInRange = 0;

		for (auto target : allies)
		{
			if (target != GEntityList->Player())
			{
				if (target != nullptr && !target->IsDead())
				{
					auto flDistance = (target->GetPosition() - Source->GetPosition()).Length();
					if (flDistance <= range)
					{
						AlliesInRange++;
					}
				}
			}
		}
		return AlliesInRange;
	}

	static int CountMinionsNearMe(IPluginSDK *sdk, float range)
	{
		static auto entityList = sdk->GetEntityList();
		static auto player = entityList->Player();

		auto count = 0;

		for (auto unit : entityList->GetAllMinions(false, true, true))
		{
			if (player->IsValidTarget(unit, range))
			{
				count++;
			}
		}

		return count;
	}

	static int CountEnemiesNearMe(IPluginSDK *sdk, float range)
	{
		static auto entityList = sdk->GetEntityList();
		static auto player = entityList->Player();

		auto count = 0;

		for (auto unit : entityList->GetAllHeros(false, true))
		{
			if (player->IsValidTarget(unit, range))
			{
				count++;
			}
		}

		return count;
	}
	bool EBuff(IUnit* target)
	{
		if (target == nullptr)
			return false;

		return target->HasBuff("MalzaharE");
	}
	bool QBuff(IUnit* target)
	{
		if (target == nullptr)
			return false;

		return target->HasBuff("MalzaharQMissile");
	}
	/*bool RBuff(IUnit* target)
	{
	if (target == nullptr)
	return false;

	return target->HasBuff("MalzaharR");
	}*/
	void RMove()
	{

		if (GEntityList->Player()->HasBuff("malzaharrsound") && GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		{
			GOrbwalking->SetMovementAllowed(false);
		}
		else
		{
			GOrbwalking->SetMovementAllowed(true);
		}
	}
	void Combo()
{
	auto myhero = GEntityList->Player();
	int combomode = ComboMode->GetInteger();
	switch (combomode)
	
	{
	case 0:
		{
			if (ComboQ->Enabled() && Q->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Target, 0.25f, true, pred);
						if (InSpellRange(Q, pred))
							Q->CastOnPosition(pred);
					}
				}
			}

			if (ComboW->Enabled() && W->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
					{
						W->CastOnPosition(Target->ServerPosition());
					}
				}
			}
			if (ComboE->Enabled() && E->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
					{
						E->CastOnTarget(Target);
					}
				}
			}
			if (ComboR->Enabled() && R->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, R->Range()))
					{
						R->CastOnTarget(Target);
					}
				}
			}
		}
		break;
	case 1:
		{
			if (ComboQ->Enabled() && Q->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
					{
						Q->CastOnTarget(Target);
					}
				}
			}
			if (ComboE->Enabled() && E->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
					{
						E->CastOnTarget(Target);
					}
				}
			}
			if (ComboW->Enabled() && W->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
					{
						W->CastOnPosition(Target->ServerPosition());
					}
				}
			}
			if (ComboR->Enabled() && R->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, R->Range()))
					{
						R->CastOnTarget(Target);
					}
				}
			}
		}
		break;
	case 2:
		{
			if (R->IsReady() && ComboR->Enabled())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, R->Range()))
					{
						R->CastOnTarget(Target);
					}
				}
				
			}

			if (ComboW->Enabled() && W->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
					{
						W->CastOnPosition(Target->ServerPosition());
					}
				}
			}
			if (ComboQ->Enabled() && Q->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
					{
						Q->CastOnTarget(Target);
					}
				}
			}
			if (ComboE->Enabled() && E->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
					{
						E->CastOnTarget(Target);
					}
				}
			}


		}
		break;
	case 3:
		{

			if (ComboE->Enabled() && E->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
					{
						E->CastOnTarget(Target);
					}
				}
			}
			if (ComboW->Enabled() && W->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
					{
						W->CastOnPosition(Target->ServerPosition());
					}
				}
			}

			if (ComboQ->Enabled() && Q->IsReady() && !myhero->HasBuff("malzaharrsound"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
					{
						Q->CastOnTarget(Target);
					}
				}
			}
			if (ComboR->Enabled() && R->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, R->Range()))
					{
						R->CastOnTarget(Target);
					}
				}
				
			}
		}
		break;
	}
}
	void Push()
	{
		minions = GEntityList->GetAllMinions(false, true, true);
		for (IUnit* minion : minions)
			if (GEntityList->Player()->ManaPercent() >= FarmMana->GetFloat())
			{
				{
					if (FarmE->Enabled() && E->IsReady() && CountMinionsNearMe(GPluginSDK, E->Range()) >= FarmEmin->GetInteger() && minion->HealthPercent() <= 45 && minion->IsValidTarget(minion, E->Range()) && GEntityList->Player()->IsValidTarget(minion, E->Range()) && (!(minion->IsDead())))

					{
						if (minion != nullptr)
						{
							if (E->CastOnUnit(minion)) { return; }
						}
					}

					if (FarmW->Enabled() && W->IsReady() && CountMinionsNearMe(GPluginSDK, E->Range()) >= FarmWmin->GetInteger() && minion->IsValidTarget(minion, W->Range()) && GEntityList->Player()->IsValidTarget(minion, W->Range()) && (!(minion->IsDead())))

					{
						if (minion != nullptr && EBuff(minion))
						{
							if (W->CastOnUnit(minion)) { return; }
						}
					}

					if (FarmQ->Enabled() && Q->IsReady() && (!(minion->IsDead())) && minion != nullptr)

					{
						int creeps;
						Vec3 pos;
						Q->FindBestCastPosition(true, true, pos, creeps);
						minions = GEntityList->GetAllMinions(false, true, true);
						for (IUnit* minion : minions)
							if (CountMinionsNearMe(GPluginSDK, Q->Range()) >= FarmQmin->GetInteger() && FarmQmin->GetInteger() > 1)
							{
								Q->CastOnPosition(pos);
							}
					}

					if (FarmQ->Enabled() && Q->IsReady() && (!(minion->IsDead())) && minion != nullptr && FarmQmin->GetInteger() <= 1)
					{
						if (Q->CastOnUnit(minion)) { return; }
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
				if (LastQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
				{
					Q->CastOnUnit(Minion);
				}
				if (LastW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotW) >= Minion->GetHealth())
				{
					W->CastOnUnit(Minion);
				}
				if (LastE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range()) && Minion->HealthPercent() <= 30)
				{
					E->CastOnUnit(Minion);
				}
			}

		}
	}


	void Harras()
	{
		if (HarassQ->Enabled() && Q->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			{
				Q->CastOnTarget(Target, kHitChanceHigh);
			}
		}

		if (HarassW->Enabled() && W->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			{
				W->CastOnTarget(Target);
			}
		}

		if (HarassE->Enabled() && E->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			{
				E->CastOnTarget(Target);
			}
		}

	}

	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Source != GEntityList->Player()
			&& Args.Source->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Source, R->Range() + Args.Source->BoundingRadius())
			&& AntiGapR->Enabled() && R->IsReady())
		{
			R->CastOnTarget(Args.Source);
		}
	}
	void Interrupt(InterruptibleSpell const& Args)
	{
		if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, E->Range()) && Args.DangerLevel == kHighDanger && InterruptR->Enabled() && R->IsReady())
		{
			R->CastOnTarget(Args.Source);
		}
	}

	void tear()
	{
		{
			if (GEntityList->Player()->ManaPercent() >= ManaTear->GetFloat())
			{
				if (TearQ->Enabled() && Q->IsReady() && (CountEnemiesNearMe(GPluginSDK, 1500) == 0) && (CountMinionsNearMe(GPluginSDK, 1000) == 0))
				{
					Q->CastOnPosition(GGame->CursorPosition());

				}
				if (TearW->Enabled() && W->IsReady() && (CountEnemiesNearMe(GPluginSDK, 1500) == 0) && (CountMinionsNearMe(GPluginSDK, 1000) == 0))
				{
					W->CastOnPosition(GGame->CursorPosition());

				}
			}
		}
	}

	void Draw() const
	{
		{
			if (DrawQRange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

			if (DrawWRange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }

			if (DrawERange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }

			if (DrawRRange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
		}
	}
};