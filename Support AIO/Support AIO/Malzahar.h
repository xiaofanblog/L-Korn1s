#pragma once
#include "Menu.h"
#include <string>
class MalzaharBase
{
public:

	void DrawMenu()
	{

		MainMenu = GPluginSDK->AddMenu("Malzahar - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo Menu)");
		HarassMenu = MainMenu->AddMenu("Harass");
		LastHitMenu = MainMenu->AddMenu("LastHit");
		FarmMenu = MainMenu->AddMenu("LaneClear");
		DrawingMenu = MainMenu->AddMenu("Drawings");
		MiscMenu = MainMenu->AddMenu("Misc.");

		ComboMode = ComboMenu->AddSelection("Select combo", 0, { "Q>W>E>R", "Q>W>R>E", "Q>E>W>R", "Q>E>R>W", "Q>R>W>E", "W>R>Q>E", "R>W>Q>E", "R>Q>W>E", "E>W>Q>R", "W>E>Q>E>R" });
		AA = ComboMenu->CheckBox("Enable AutoAttacks in combo mode", true);
		SupportMode = ComboMenu->CheckBox("Support Mode", true);


		FarmQ = FarmMenu->CheckBox("Q creeps/monsters", false);
		FarmQmin = FarmMenu->AddInteger("Minimum to use Q", 0, 10, 3);
		FarmW = FarmMenu->CheckBox("W creeps/monsters", false);
		FarmWmin = FarmMenu->AddInteger("Minimum to use W", 0, 10, 3);
		FarmE = FarmMenu->CheckBox("E creeps/monsters", false);
		FarmEmin = FarmMenu->AddInteger("Minimum to use E", 0, 10, 3);



		HarassMana = MiscMenu->AddFloat("Mana Percent", 1, 100, 75);
		HarassQ = HarassMenu->CheckBox("Q harass", false);
		HarassW = HarassMenu->CheckBox("W harass", false);
		HarassE = HarassMenu->CheckBox("E harrass", false);



		LastHitMana = MiscMenu->AddFloat("Mana Percent", 1, 100, 75);
		LastQ = LastHitMenu->CheckBox("Q lasthit", false);
		LastW = LastHitMenu->CheckBox("W lasthit", false);
		LastE = LastHitMenu->CheckBox("E lasthit", false);



		MiscMana = MiscMenu->AddFloat("Mana Percent", 1, 100, 75);
		ManaTear = MiscMenu->AddFloat("Min. mana% for tear stacks", 1, 100, 75);
		TearQ = MiscMenu->CheckBox("Cast Q for stacks", false);
		TearW = MiscMenu->CheckBox("Cast W for stacks", false);

		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", false);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
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
		Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, true, (kCollidesWithYasuoWall));
		R->SetSkillshot(0.25f, 140.f, 2400.f, 1000.f);
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

	void Semi()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				if (!Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), 860) && !Enemy->IsDead())
				{
					R->CastOnTarget(Enemy, kHitChanceHigh);
				}
			}
		}
	}

	void Auto()
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && Ally->HasBuffOfType(BUFF_Slow))
			{
				if (ComboEslow->Enabled() && E->Range() && E->IsReady())
				{
					E->CastOnTarget(Ally);
				}
			}
		}
	}


	void Healing()
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (HealW->Enabled() && W->IsReady())
			{
				if (!Ally->IsRecalling() && Ally != nullptr && Ally->HealthPercent() <= HealWally->GetInteger() && Ally != GEntityList->Player() && !Ally->IsDead() && GetAlliesInRange(GEntityList->Player(), W->Range()) >= HealWenemy->GetInteger())
				{
					W->CastOnTarget(Ally);
				}
				else if (Ally != nullptr && Ally == GEntityList->Player() && !GEntityList->Player()->IsDead())
				{
					if (!GEntityList->Player()->IsRecalling() && GEntityList->Player()->HealthPercent() <= HealWmyself->GetInteger() && !GUtility->IsPositionInFountain(GEntityList->Player()->GetPosition(), true, false))
					{
						W->CastOnPlayer();
					}
				}
			}
		}
	}

	void Combo()
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true)) {
			if (Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && !Enemy->IsDead())
			{
				if (ComboQ->Enabled() && Q->IsReady())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
					if (GEntityList->Player()->IsValidTarget(target, Q->Range()))
					{
						Q->CastOnTarget(target);
					}
				}
				if (ComboE->Enabled() && E->IsReady())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
					if (GEntityList->Player()->IsValidTarget(target, E->Range()))
					{
						E->CastOnTarget(target);
					}
				}
				if (ComboR->Enabled() && R->IsReady())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
					if (GEntityList->Player()->IsValidTarget(target, R->Range()))
					{
						R->CastOnTargetAoE(target, ComboRmin->GetInteger(), kHitChanceHigh);
					}
				}
			}
		}
	}

	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player() && Args.Sender->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Sender, 200 + Args.Sender->BoundingRadius()) && AntiGapE->Enabled() && E->IsReady())
		{
			E->CastOnPlayer();
		}
	}


	void Interrupt(InterruptibleSpell const& Args)
	{
		float endtime;
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, 860) && InterruptR->Enabled() && R->IsReady())
		{
			if (Args.Target->IsCastingImportantSpell(&endtime))
			{
				R->CastOnTarget(Args.Target, kHitChanceHigh);
			}

		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled())
		{
			Vec4 color;
			QColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, Q->Range());
		}
		if (DrawWRange->Enabled())
		{
			Vec4 color;
			WColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, W->Range());
		}
		if (DrawERange->Enabled())
		{
			Vec4 color;
			EColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, E->Range());
		}
		if (DrawRRange->Enabled())
		{
			Vec4 color;
			RColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, R->Range());
		}
	}
};