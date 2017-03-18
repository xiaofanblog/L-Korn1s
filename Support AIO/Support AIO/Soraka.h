#pragma once
#include "Menu.h"
#include <string>
class SorakaBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Soraka by Kornis");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboEinterrupt = ComboMenu->CheckBox("Use E to Interrupt", true);
			ComboEgapclose = ComboMenu->CheckBox("Use E on Gapclose", true);

		}

		HarassMenu = MainMenu->AddMenu("Harass");
		{
			HarassMana = HarassMenu->AddInteger("Mana Percent", 10, 100, 50);
			HarassQ = HarassMenu->CheckBox("Use Q in Harass", true);
			HarassE = HarassMenu->CheckBox("Use E in Harass", true);
			HarassAA = HarassMenu->CheckBox("Disable AA", true);

		}
		MiscMenu = MainMenu->AddMenu("Healing");
		{
			HealW = MiscMenu->CheckBox("Use auto W", true);
			HealWally = MiscMenu->AddInteger("Ally HP percent(if lower than)", 10, 100, 50);
			HealWmy = MiscMenu->AddInteger("My HP percent(if higher than)", 10, 100, 30);
			HealR = MiscMenu->CheckBox("Use auto R", true);
			HealRhp = MiscMenu->AddInteger("Ally R HP percent (if lower than)", 10, 100, 20);
		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q range", true);
			DrawWRange = DrawingMenu->CheckBox("Draw W range", true);
			DrawERange = DrawingMenu->CheckBox("Draw E range", true);
		}
		BlacklistMenuR = MainMenu->AddMenu("R Blacklist");

		for (auto ally : GEntityList->GetAllHeros(true, false)) {
			BlacklistMenuR->CheckBox(ally->ChampionName(), false);
		}
		BlacklistMenuW = MainMenu->AddMenu("W Priority");
		Wpriority();
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, true, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		Q->SetOverrideDelay(0.25);
		Q->SetOverrideRadius(80);
		Q->SetOverrideSpeed(1950);
		Q->SetOverrideRange(830);
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		W->SetOverrideRange(550);
		E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, false, true, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E->SetOverrideDelay(0.25);
		E->SetOverrideRange(925);
		E->SetOverrideRadius(50);
		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
	}

	void Combo()
	{
		if (ComboE->Enabled() && E->IsReady() && E->Range())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				E->CastOnTarget(target, kHitChanceHigh);
			}
		}
		if (ComboQ->Enabled() && Q->IsReady() && Q->Range())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				Q->CastOnTarget(target, kHitChanceHigh);
			}
		}
	}

	void Healing()
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (Ally != GEntityList->Player())
			{
				if (WHeal1 != nullptr && WHeal1->Enabled() && WHeal01->ChampionName() == Ally->ChampionName())
				{
					if (HealW->Enabled() && W->IsReady() && WHeal01->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (WHeal01->HealthPercent() < HealWally->GetInteger())
						{
							W->CastOnUnit(WHeal01);
						}
						else if (WHeal01->HealthPercent() > HealWally->GetInteger())
						{
							if (WHeal02->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal02->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal02);
								}
							}
							if (WHeal03->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal03->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal03);
								}
							}
							if (WHeal04->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal04->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal04);
								}
							}
							if (WHeal05->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal05->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal05);
								}
							}
						}

					}
				}

				else if (WHeal1->Enabled() && !WHeal01->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (HealW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally->HealthPercent() < HealWally->GetInteger())
						{
							if (Ally != nullptr)
							{
								W->CastOnUnit(Ally);
							}
						}
					}
				}
				if (WHeal2 != nullptr && WHeal2->Enabled() && WHeal02->ChampionName() == Ally->ChampionName())
				{
					if (HealW->Enabled() && W->IsReady() && WHeal02->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (WHeal02->HealthPercent() < HealWally->GetInteger())
						{
							W->CastOnUnit(WHeal02);
						}
						else if (WHeal02->HealthPercent() > HealWally->GetInteger())
						{
							if (WHeal01->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal01->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal01);
								}
							}
							if (WHeal03->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal03->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal03);
								}
							}
							if (WHeal04->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal04->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal04);
								}
							}
							if (WHeal05->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal05->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal05);
								}
							}
						}

					}
				}

				else if (WHeal2->Enabled() && !WHeal02->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (HealW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally->HealthPercent() < HealWally->GetInteger())
						{
							if (Ally != nullptr)
							{
								W->CastOnUnit(Ally);
							}
						}
					}
				}
				if (WHeal3 != nullptr && WHeal3->Enabled() && WHeal03->ChampionName() == Ally->ChampionName())
				{
					if (HealW->Enabled() && W->IsReady() && WHeal03->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (WHeal03->HealthPercent() < HealWally->GetInteger())
						{
							W->CastOnUnit(WHeal03);
						}
						else if (WHeal03->HealthPercent() > HealWally->GetInteger())
						{
							if (WHeal01->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal01->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal01);
								}
							}
							if (WHeal02->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal02->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal02);
								}
							}
							if (WHeal04->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal04->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal04);
								}
							}
							if (WHeal05->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal05->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal05);
								}
							}

						}
					}
				}
				else if (WHeal3->Enabled() && !WHeal03->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (HealW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally->HealthPercent() < HealWally->GetInteger())
						{
							if (Ally != nullptr)
							{
								W->CastOnUnit(Ally);
							}
						}
					}
				}
				if (WHeal4 != nullptr && WHeal4->Enabled() && WHeal04->ChampionName() == Ally->ChampionName())
				{
					if (HealW->Enabled() && W->IsReady() && WHeal04->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (WHeal04->HealthPercent() < HealWally->GetInteger())
						{
							W->CastOnUnit(WHeal04);
						}
						else if (WHeal04->HealthPercent() > HealWally->GetInteger())
						{
							if (WHeal01->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal01->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal01);
								}
							}
							if (WHeal02->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal02->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal02);
								}
							}
							if (WHeal03->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal03->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal03);
								}
							}
							if (WHeal05->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal05->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal05);
								}
							}

						}
					}
				}

				else if (WHeal4->Enabled() && !WHeal04->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (HealW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally->HealthPercent() < HealWally->GetInteger())
						{
							if (Ally != nullptr)
							{
								W->CastOnUnit(Ally);
							}
						}
					}
				}
				if (WHeal5 != nullptr && WHeal5->Enabled() && WHeal05->ChampionName() == Ally->ChampionName())
				{
					if (HealW->Enabled() && W->IsReady() && WHeal05->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (WHeal05->HealthPercent() < HealWally->GetInteger())
						{
							W->CastOnUnit(WHeal05);
						}
						else if (WHeal05->HealthPercent() > HealWally->GetInteger())
						{
							if (WHeal02->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal02->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal02);
								}
							}
							if (WHeal03->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal03->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal03);
								}
							}
							if (WHeal04->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal04->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal04);
								}
							}
							if (WHeal01->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (WHeal01->HealthPercent() < HealWally->GetInteger())
								{
									W->CastOnUnit(WHeal01);
								}
							}

						}
					}
				}

				else if (WHeal5->Enabled() && !WHeal05->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (HealW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally->HealthPercent() < HealWally->GetInteger())
						{
							if (Ally != nullptr)
							{
								W->CastOnUnit(Ally);
							}
						}
					}
				}
				if (!WHeal1->Enabled() && !WHeal2->Enabled() && !WHeal3->Enabled() && !WHeal4->Enabled() && !WHeal5->Enabled())
				{
					if (HealW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally->HealthPercent() < HealWally->GetInteger())
						{
							if (Ally != nullptr)
							{
								W->CastOnUnit(Ally);
							}
						}
					}
				}
			}
		}
	}

	void Harass()
	{
		if (GEntityList->Player()->ManaPercent() > HarassMana->GetInteger())
		{
			if (HarassQ->Enabled() && Q->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr && Q->Range())
				{
					Q->CastOnTarget(target);
				}

			}
			if (HarassE->Enabled() && E->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (target != nullptr && E->Range())
				{
					E->CastOnTarget(target);
				}

			}
		}
	}

	void Wpriority()
	{
		int indx = 0;
		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (auto teamate : Teamates)
		{
			if (indx == 0)
			{
				indx++;
				WHeal1 = BlacklistMenuW->CheckBox(teamate->ChampionName(), true);
				WHeal01 = teamate;
			}
			else if (indx == 1)
			{
				indx++;
				WHeal2 = BlacklistMenuW->CheckBox(teamate->ChampionName(), true);
				WHeal02 = teamate;
			}
			else if (indx == 2)
			{
				indx++;
				WHeal3 = BlacklistMenuW->CheckBox(teamate->ChampionName(), true);
				WHeal03 = teamate;
			}
			else if (indx == 3)
			{
				indx++;
				WHeal4 = BlacklistMenuW ->CheckBox(teamate->ChampionName(), true);
				WHeal04 = teamate;
			}
			else if (indx == 4)
			{
				WHeal5 = BlacklistMenuW->CheckBox(teamate->ChampionName(), true);
				WHeal05 = teamate;
				return;
			}
		}
	}

	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, E->Range() + Args.Sender->BoundingRadius())
			&& ComboEgapclose->Enabled() && E->IsReady())
		{
			E->CastOnTarget(Args.Sender);
		}
	}


	void Interrupt(InterruptibleSpell const& Args)
	{
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, E->Range()) && ComboEinterrupt->Enabled() && E->IsReady())
		{
			E->CastOnTarget(Args.Target, kHitChanceHigh);
		}

	}

	void Draw() const
	{
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	}
};