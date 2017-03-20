#pragma once
#include "Menu.h"
#include <string>


class MorganaBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Morgana :: Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboW = ComboMenu->CheckBox("Use W in Combo", true);
			ComboWenable = ComboMenu->CheckBox("W Only if Immobile", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			ComboRmin = ComboMenu->AddInteger("Use R if enemies X >", 1, 5, 2);
			SupportMode = ComboMenu->CheckBox("Support Mode", true);

		}

		FarmMenu = MainMenu->AddMenu("Farming");
		{
			FarmMana = FarmMenu->AddInteger("Mana percent", 10, 100, 50);
			FarmW = FarmMenu->CheckBox("Use W", true);
			FarmWmin = FarmMenu->AddInteger("Min. minions", 1, 6, 3);
		}
		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			AutoQ = MiscMenu->CheckBox("Auto Q on CC/Immobile", true);
			AntiGapQ = MiscMenu->CheckBox("Auto Q on Gapclose", true);
		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
			DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		}

		KillstealMenu = MainMenu->AddMenu("Killsteal");
		{
			KSQ = KillstealMenu->CheckBox("Use Q", true);
		}
		ESettings = MainMenu->AddMenu("E Settings");
		{
			ComboE = ESettings->CheckBox("Auto E", true);
		}
	}


	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall, kCollidesWithMinions));
		Q->SetOverrideDelay(0.25);
		Q->SetOverrideSpeed(2000);
		Q->SetOverrideRange(1100);
		Q->SetOverrideRadius(60);
		W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));


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

	void AAdisable()
	{
		if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
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

	void AutoE(CastedSpell const& Args)
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (ComboE->Enabled() && Ally->IsValidTarget(GEntityList->Player(), E->Range())) {
				if (Args.Target_ == Ally && Args.Caster_->IsHero() && Args.Caster_->IsEnemy(Ally)) {

					auto data = Args.Data_;
					auto target = GSpellData->GetTarget(data);
					std::string spellName = Args.Name_;
					if (spellName.find("BasicAttack") != std::string::npos) {
						return;
					}
					if (Args.Target_->IsValidTarget(GEntityList->Player(), E->Range()))
					{
						GGame->PrintChat(target->ChampionName());
						GGame->PrintChat("Was targeted by ");
						GGame->PrintChat(Args.Name_);
						E->CastOnTarget(Args.Target_);
					}
				}
			}
		}

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
	bool CanMove(IUnit* target)
	{
		if (target->HasBuffOfType(BUFF_Stun) || target->HasBuffOfType(BUFF_Snare) || target->HasBuffOfType(BUFF_Fear) || target->HasBuffOfType(BUFF_Knockup) ||
			target->HasBuff("Recall") || target->HasBuffOfType(BUFF_Knockback) || target->HasBuffOfType(BUFF_Charm) || target->HasBuffOfType(BUFF_Taunt) || target->HasBuffOfType(BUFF_Suppression))
		{
			return true;
		}
		else
			return false;
	}

	void AutoLogic()
	{
		if (Q->IsReady() && AutoQ->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				if (CanMove(target))
				{
					Q->CastOnTarget(target);
				}

			}
		}
		if (W->IsReady() && ComboWenable->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr)
			{
				if (CanMove(target))
				{
					W->CastOnTarget(target);
				}

			}
		}
	}
	void Combo()
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
		{
			if (ComboQ->Enabled() && GEntityList->Player()->IsValidTarget(target, Q->Range()) && Q->IsReady())
			{
				Q->CastOnTarget(target, kHitChanceHigh);
			}
			if (ComboW->Enabled() && !ComboWenable->Enabled() && GEntityList->Player()->IsValidTarget(target, W->Range()) && W->IsReady())
			{
				W->CastOnTarget(target);
			}
			if (ComboR->Enabled() && R->IsReady())
			{
				if (GEntityList->Player()->IsValidTarget(target, R->Range()))
				{
					if (GetEnemiesInRange(GEntityList->Player(), R->Range()) >= ComboRmin->GetInteger())
					{
						R->CastOnPlayer();
					}
				}
			}
		}
	}
	void Farm()
	{
		if (!SupportMode->Enabled() && FarmW->Enabled() && GEntityList->Player()->ManaPercent() >= FarmMana->GetInteger())
		{
			for (auto minion : GEntityList->GetAllMinions(false, true, false))
			{
				if (minion != nullptr && minion->IsEnemy(GEntityList->Player()) && !minion->IsDead())
				{
					if (W->IsReady() && GEntityList->Player()->IsValidTarget(minion, W->Range()))
					{
						Vec3 pos;
						int Whit;
						GPrediction->FindBestCastPosition(W->Range(), W->Radius(), false, true, false, pos, Whit, W->GetDelay());
						if (Whit >= FarmWmin->GetInteger())
						{
							W->CastOnPosition(pos);
						}
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

			if (Enemy != nullptr && !Enemy->IsDead())
			{
				if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
				{
					Q->CastOnTarget(Enemy);
				}

			}
		}
	}

	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, Q->Range() + Args.Sender->BoundingRadius())
			&& AntiGapQ->Enabled() && Q->IsReady())
		{
			Q->CastOnTarget(Args.Sender);
		}
	}


	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), R->Range()); }
	}
};