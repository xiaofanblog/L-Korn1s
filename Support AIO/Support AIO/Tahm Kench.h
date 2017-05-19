#pragma once
#include "Menu.h"
#include <string>
#include "EventManager.h"
#include "DamageTracker.h"
#include "lpputils.h"

class TahmBase
{
public:
	std::vector<IUnit*> minions;
	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Tahm Kench - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		FarmMenu = MainMenu->AddMenu("Farm");
		HarassMenu = MainMenu->AddMenu("Harass");
		DrawingMenu = MainMenu->AddMenu("Drawings");
		MiscMenu = MainMenu->AddMenu("Misc.");

		ComboQ = ComboMenu->CheckBox("Use Q in combo", true);
		ComboW = ComboMenu->CheckBox("Use W in combo", true);
		HealW = ComboMenu->CheckBox("Use W on Ally", true);
		HealWally = ComboMenu->AddFloat("W Ally if X HP", 1, 100, 25);
		HealWmy = ComboMenu->CheckBox("Use W on Ally if IncomingDamage", true);
		ComboE = ComboMenu->AddFloat("Use E if my HP", 1, 100, 25);

		AntiGapQ = MiscMenu->CheckBox("Auto Q for Gapcloser", true);
		AntiGapW = MiscMenu->CheckBox("AntiGap Smart W Ally", true);
		InterruptQ = MiscMenu->CheckBox("Use Q to Interrupt if will stun", true);
		InterruptW = MiscMenu->CheckBox("Auto W Ally if CC", true);
		FarmQ = FarmMenu->CheckBox("Q Farm", true);
		FarmW = FarmMenu->CheckBox("W Farm", true);

		HarassQ = HarassMenu->CheckBox("Q Harass", false);
		HarassW = HarassMenu->CheckBox("W Harass", false);

		FarmMana = FarmMenu->AddFloat("Mana Percent", 1, 100, 50);
		HarassMana = HarassMenu->AddFloat("Mana Percent", 1, 100, 50);

		DrawQRange = DrawingMenu->CheckBox("Draw Q", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W", false);
	}



	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall | kCollidesWithMinions));
		Q->SetSkillshot(0.25f, 80.f, 10000000.f, 800.f);
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, (kCollidesWithNothing));
		W->SetSkillshot(0.25f, 250.f, 3200.f, 320.f);
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, (kCollidesWithNothing));
		E->SetSkillshot(0.25f, 0.f, 3200.f, FLT_MAX);
		R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, (kCollidesWithNothing));
		R->SetSkillshot(1.0f, 250.f, 3200.f, 4000.f);
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

	void Combo()
	{


		auto myhero = GEntityList->Player();
		if (E->IsReady())
		{
			if (myhero->HealthPercent() <= ComboE->GetFloat() && !myhero->IsRecalling())
			{
				E->CastOnPlayer();
			}
		}

		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
			{
				if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
				{
					Q->CastOnTarget(Target, kHitChanceHigh);
				}
			}
		}

		if (ComboW->Enabled() && HealW->Enabled() && W->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());

			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
			{
				if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
				{
					if (Target->HasBuff("tahmkenchpdevourable"))
					{
						W->CastOnTarget(Target);
					}
				}
			}
		}
	
	}

	void AutoSmth()
	{
		static DamageTracker tracker = DamageTracker(GPluginSDK, true);
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (Ally != GEntityList->Player())
			{
				if (Ally != nullptr && Ally->IsValidTarget() && Ally->IsHero())
				{
					if (HealW->Enabled() && W->IsReady())
					{
						if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
						{
							if (Ally->HealthPercent() <= HealWally->GetFloat())
							{

								if (HealWmy->Enabled() && !Ally->IsRecalling() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
								{
									float extraDamage;
									if (tracker.hasIncomingDamage(Ally) && (tracker.getIncomingDamage(Ally) > 50 || tracker.hasIncomingLethalDamage(Ally, extraDamage) > 50 || tracker.getIncomingDamage(Ally) > Ally->GetHealth()))
									{
										W->CastOnTarget(Ally);
									}
								}
								
								if (!HealWmy->Enabled() && !Ally->IsRecalling() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
								{
									W->CastOnTarget(Ally);

								}
							}

						}
					}
				}
			}
		}
	}
	void Push()
	{
		minions = GEntityList->GetAllMinions(false, true, true);
		for (IUnit* minion : minions)
		{
			if (GEntityList->Player()->ManaPercent() >= FarmMana->GetFloat())
			{

				if (FarmW->Enabled() && W->IsReady())
				{

					if (minion != nullptr)
					{

						if (!minion->IsDead() && GEntityList->Player()->IsValidTarget(minion, W->Range()) && CountMinionsNearMe(GPluginSDK, 700) >= 2)						{
							W->CastOnUnit(minion);
						}
					}


				}


				if (FarmQ->Enabled() && Q->IsReady())
				{
					if (minion != nullptr)
					{
						if (!minion->IsDead() && GEntityList->Player()->IsValidTarget(minion, Q->Range()))
						{
							Q->CastOnUnit(minion);
						}
					}
				}
			}
		}
	}

	void Harass()
	{
		minions = GEntityList->GetAllMinions(false, true, true);
		for (IUnit* minion : minions)
			if (GEntityList->Player()->ManaPercent() >= HarassMana->GetFloat())
			{
				if (HarassQ->Enabled() && Q->IsReady())
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
						{
							Q->CastOnTarget(Target, kHitChanceHigh);
						}
					}
				}
				if (HarassW->Enabled() && W->IsReady())
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
					///if (!(target->IsDead()) && GEntityList->Player()->IsValidTarget(target, W->Range()) && target != nullptr)
					if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
					{
						if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
						{
							if (minion != nullptr)
							{
								if (!minion->IsDead() && GEntityList->Player()->IsValidTarget(minion, W->Range()) && minion != nullptr)
								{
									W->CastOnUnit(minion);
								}
								if (minion->HasBuff("tahmkenchwdevoured"))
								{
									///auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
									W->CastOnTarget(Target);
								}
							}
						}
					}
				}
			}
	}
	void Auto()
	{
		if (InterruptW->Enabled())
		{
			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{
				if (Ally != GEntityList->Player())
				{
					if (Ally != nullptr && Ally->IsValidTarget() && Ally->IsHero())
					{
						if (W->IsReady())
						{
							if (GEntityList->Player()->IsValidTarget(Ally, W->Range()))
							{
								if (Ally->HasBuffOfType(eBuffType::BUFF_Stun)
									|| Ally->HasBuffOfType(eBuffType::BUFF_Taunt)
									|| Ally->HasBuffOfType(eBuffType::BUFF_Stun)
									|| Ally->HasBuffOfType(eBuffType::BUFF_Snare)
									|| Ally->HasBuffOfType(eBuffType::BUFF_Charm)
									|| Ally->HasBuffOfType(eBuffType::BUFF_Knockup)
									|| Ally->HasBuffOfType(eBuffType::BUFF_Knockback))
								{
									W->CastOnTarget(Ally);
								}

							}
						}
					}
				}
			}
		}
	}

	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, 200 + Args.Source->BoundingRadius()) && AntiGapQ->Enabled() && Q->IsReady())
		{
			Q->CastOnTarget(Args.Source);
		}
		if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, 200 + Args.Source->BoundingRadius()) && AntiGapW->Enabled() && W->IsReady())
		{
			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{
				if (Ally != GEntityList->Player())
				{
					if (Ally != nullptr && Ally->IsValidTarget() && Ally->IsHero() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally->HealthPercent() < 50)
						{
							if (Args.Source->HealthPercent() > Ally->HealthPercent())
							{
								W->CastOnTarget(Ally);
							}
						}
					}
				}
			}
		}
	}


	void Interrupt(InterruptibleSpell const& Args)
	{
		float endtime;
		if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, 860) && InterruptQ->Enabled() && Q->IsReady())
		{
			if (Args.Source->HasBuff("tahmkenchpdevourable"))
			{
				if (Args.Source->IsCastingImportantSpell(&endtime))
				{
					Q->CastOnTarget(Args.Source, kHitChanceHigh);
				}
			}

		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	}
};