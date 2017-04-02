#pragma once
#include "Menu.h"

#include <string>
class KarmaBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Karma - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		FarmMenu = MainMenu->AddMenu("Farm");
		DrawingMenu = MainMenu->AddMenu("Drawings");
		MiscMenu = MainMenu->AddMenu("Misc.");

		ComboMode = ComboMenu->AddSelection("Select R mode", 0, { "R>Q", "R>W", "R>E" });
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboR = ComboMenu->CheckBox("Use R", true);
		SupportMode = ComboMenu->CheckBox("Support Mode", true);
		AntiGapW = MiscMenu->CheckBox("Auto W for Gapcloser", true);
		AntiGapE = MiscMenu->CheckBox("Auto E for Gapcloser", true);
		FarmMana = FarmMenu->AddFloat("Mana Percent", 1, 100, 50);
		FarmQ = FarmMenu->CheckBox("Q Minions/Jungle", true);
		FarmW = FarmMenu->CheckBox("W JungleCreeps", true);
		DrawQRange = DrawingMenu->CheckBox("Draw Q", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W", false);
		DrawERange = DrawingMenu->CheckBox("Draw E", false);
		Chase = MainMenu->AddKey("Chase Combo", 'T');
		Survival = MainMenu->AddKey("Survive Combo", 'Z');
		QWQMouse = MainMenu->AddKey("RQ to Mouse", 'G');
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

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall, kCollidesWithMinions));
		Q->SetSkillshot(0.25f, 50.f, 1000000000.f, 950.f);
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, (kCollidesWithNothing));
		W->SetSkillshot(2.0f, 0.f, 1000.f, 675.f);
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, (kCollidesWithNothing));
		E->SetSkillshot(0.4f, 0.f, 1000.f, 800.f);
		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, (kCollidesWithNothing));
		R->SetSkillshot(0.25f, 0.f, 1000.f, FLT_MAX);


	}
	bool RBuff(IUnit* target)
	{
		if (target == nullptr)
			return false;

		return target->HasBuff("KarmaMantra");
	}

	void ComboSurvive()
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
		if (R->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero() && Target->IsValidTarget(GEntityList->Player(), W->Range()))
			{
				if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
				{
					R->CastOnPlayer();
				}
			}
		}
		if (W->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero() && Target->IsValidTarget(GEntityList->Player(), W->Range()))
			{
				if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
				{
					W->CastOnTarget(Target);
				}
			}
		}
		if (E->IsReady() && !GEntityList->Player()->HasBuff("KarmaMantra"))
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			{
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
					{
						E->CastOnPlayer();
					}
				}
			}
		}
		if (Q->IsReady() && !GEntityList->Player()->HasBuff("KarmaMantra"))
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

	}
	void ComboChase()
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
		if (E->IsReady() && !GEntityList->Player()->HasBuff("KarmaMantra"))
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			{
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
					{
						E->CastOnPlayer();
					}
				}
			}
		}
		if (W->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			W->CastOnTarget(Target);
		}
		if (R->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero() && Target->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
				{
					R->CastOnPlayer();
				}
			}
		}
		if (Q->IsReady() && GEntityList->Player()->HasBuff("KarmaMantra"))
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero() && !Target->IsValidTarget(GEntityList->Player(), W->Range()))
			{
				if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
				{
					Q->CastOnTarget(Target);
				}
			}
		}
		if (Q->IsReady() && !GEntityList->Player()->HasBuff("KarmaMantra"))
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
			{
				if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
				{
					Q->CastOnTarget(Target);
				}
			}
		}
	}
	void Combo()
	{

		auto myhero = GEntityList->Player();
		int combomode = ComboMode->GetInteger();
		switch (combomode)

		{

			//if R>Q
		case 0:
		{
			if (ComboR->Enabled() && R->IsReady() & Q->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
					
						if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
						{
							R->CastOnPlayer();
						}
					}
				}
			}

			if (ComboQ->Enabled() && Q->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
						{
							Vec3 pred;
							auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
						    if ((GPrediction->SimulateMissile(GEntityList->Player()->ServerPosition(), Target, 1700, 50.f, 950, 0.25f, kCollidesWithMinions, pred)) == target)
							{
								Q->CastOnPosition(pred);
							}
						}
					}
				}
			}

			if (ComboW->Enabled() && W->IsReady() && !GEntityList->Player()->HasBuff("KarmaMantra"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
						{
							W->CastOnTarget(Target);
						}
					}
				}
			}
			if (ComboE->Enabled() && E->IsReady() && !GEntityList->Player()->HasBuff("KarmaMantra"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
						{
							E->CastOnPlayer();
						}
					}
				}
			}
		}
		break;
		//if R>W
		case 1:
		{
			if (ComboR->Enabled() && R->IsReady() & W->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
						{
							R->CastOnPlayer();
						}
					}
				}
			}
			if (ComboW->Enabled() && W->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());

				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
						{
							W->CastOnTarget(Target);
						}
					}
				}
			}
			if ((!R->IsReady() || !W->IsReady()) && ComboQ->Enabled() && Q->IsReady() && (!(GEntityList->Player()->HasBuff("KarmaMantra"))))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				{

					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
						{
							Q->CastOnTarget(Target);
						}
					}
				}
			}


			else if (ComboE->Enabled() && E->IsReady() && (!(GEntityList->Player()->HasBuff("KarmaMantra"))))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
						{
							E->CastOnPlayer();
						}
					}
				}
			}
		}
		break;
		//if R>E
		case 2:
		{
			if (ComboR->Enabled() && R->IsReady() & E->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, Q->Range()-10))
						{
							R->CastOnPlayer();
						}
					}
				}

			}
			if (ComboE->Enabled() && E->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
						{
							E->CastOnPlayer();
						}
					}
				}
			}
			if (ComboQ->Enabled() && Q->IsReady() && !GEntityList->Player()->HasBuff("KarmaMantra"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
						{
							Q->CastOnTarget(Target);
						}
					}
				}
			}

			if (ComboW->Enabled() && W->IsReady() && !GEntityList->Player()->HasBuff("KarmaMantra"))
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());

				{
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
						{
							W->CastOnTarget(Target);
						}
					}
				}
			}

		}
		break;
		}
	}
	/*void Combowithreturn()
	{
	if (QCombo->Enabled() && Q->IsReady())
	{
	auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
	{
	if (Q->CastOnTarget(Target, kHitChanceMedium)) { return; }
	}
	}
	if (ECombo->Enabled() && E->IsReady())
	{
	auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
	{
	if (E->CastOnTarget(Target)) { return; }
	}
	}
	if (WCombo->Enabled() && W->IsReady())
	{
	auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
	{
	if (W->CastOnTarget(Target)) { return; }
	}
	}

	}
	void Combowithbuffs()
	{
	if (QCombo->Enabled() && W->IsReady() && Q->IsReady())
	{
	auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
	{
	Q->CastOnTarget(Target, kHitChanceVeryHigh);
	}
	}
	if (QCombo->Enabled() && Q->IsReady())
	{
	auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
	if (WBuff(Target))
	{
	Q->CastOnTarget(Target, kHitChanceVeryHigh);
	}
	}
	if (WCombo->Enabled() && W->IsReady() && !Q->IsReady() && !E->IsReady())
	{
	auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
	W->CastOnTarget(Target);
	}
	if (ECombo->Enabled() && !Q->IsReady() && E->IsReady())
	{
	auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
	E->CastOnTarget(Target);
	}

	}*/

	void RQ()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			if (R->IsReady() && Q->IsReady())
			{
				R->CastOnPlayer();
				Q->CastOnPosition(GGame->CursorPosition());
			}
			if (!R->IsReady() && Q->IsReady())
			{
				Q->CastOnPosition(GGame->CursorPosition());
			}
		}
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


	void Farm()
	{
		if (!SupportMode->Enabled())
		{
			minions = GEntityList->GetAllMinions(false, true, true);
			for (IUnit* minion : minions)
				if (GEntityList->Player()->ManaPercent() >= FarmMana->GetFloat())
				{
					{
						if (FarmQ->Enabled() && Q->IsReady())
						{
							if (minion != nullptr)
							{
								if (!(minion->IsDead()) && GEntityList->Player()->IsValidTarget(minion, Q->Range()))
								{
									Q->CastOnUnit(minion);
								}
							}
						}
						if (FarmW->Enabled() && W->IsReady())
						{
							if (minion != nullptr)
							{
								if (!(minion->IsDead()) && GEntityList->Player()->IsValidTarget(minion, W->Range()))
								{
									W->CastOnUnit(minion);
								}
							}
						}

					}
				}
		}
	}
	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, W->Range() + Args.Sender->BoundingRadius())
			&& AntiGapW->Enabled() && W->IsReady())
		{
			W->CastOnTarget(Args.Sender);
		}
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, E->Range() + Args.Sender->BoundingRadius())
			&& AntiGapE->Enabled() && E->IsReady())
		{
			E->CastOnPlayer();
		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	}
};