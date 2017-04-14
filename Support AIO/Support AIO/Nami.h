#pragma once
#include "Menu.h"

class NamiBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Nami - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboQdash = ComboMenu->CheckBox("Auto Q on Dash", true);
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboW = ComboMenu->CheckBox("Save W for Heal", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			ComboRmin = ComboMenu->AddInteger("Use R if enemies X >", 1, 5, 2);
			ComboRally = ComboMenu->AddInteger("Use R if nearby allies X >", 1, 5, 1);
			ForceR = ComboMenu->AddKey("Force R", 'T');
			SupportMode = ComboMenu->CheckBox("Support Mode", true);

		}
		HealMenu = MainMenu->AddMenu("Healing");
		{
			HealW = HealMenu->CheckBox("Use auto W", true);
			HealWally = HealMenu->AddInteger("Ally HP percent(if lower than)", 10, 100, 50);
			HealWmyself = HealMenu->CheckBox("Use W on myself", true);
			HealWmy = HealMenu->AddInteger("My HP percent(if lower than)", 10, 100, 40);
		}

		/*BlacklistMenuW = MainMenu->AddMenu("E Priority");

		for (auto ally : GEntityList->GetAllHeros(true, false)) {
			BlacklistMenuW->CheckBox(ally->ChampionName(), false);
		}*/



		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
			DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
			DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		}

		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			InterruptQ = MiscMenu->CheckBox("Use Q to interrupt", true);
			InterruptR = MiscMenu->CheckBox("Use R to interupt", true);
			AutoQ = MiscMenu->CheckBox("Auto Q if CC", true);
		}

		BlacklistMenuW = MainMenu->AddMenu("E Priority");
		{
			Epriority();
		}
		BlacklistMenuR = MainMenu->AddMenu("W Priority");
		{
			HealWa();
		}

	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, true, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		Q->SetSkillshot(0.15f, 100, 4000, 865);
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, false, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall));
		R->SetOverrideRange(2000);


	}

	void Semi()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				if (!Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), R->Range() - 100) && !Enemy->IsDead())
				{
					R->CastOnTarget(Enemy, kHitChanceHigh);
				}
			}
		}
	}

	void Epriority()
	{
		int indx = 0;
		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (auto teamate : Teamates)
		{
			{
				if (indx == 0)
				{
					indx++;
					EPriority1 = BlacklistMenuW->CheckBox(teamate->ChampionName(), false);
					EPriority01 = teamate;
				}
				else if (indx == 1)
				{
					indx++;
					EPriority2 = BlacklistMenuW->CheckBox(teamate->ChampionName(), false);
					EPriority02 = teamate;
				}
				else if (indx == 2)
				{
					indx++;
					EPriority3 = BlacklistMenuW->CheckBox(teamate->ChampionName(), false);
					EPriority03 = teamate;
				}
				else if (indx == 3)
				{
					indx++;
					EPriority4 = BlacklistMenuW->CheckBox(teamate->ChampionName(), false);
					EPriority04 = teamate;
				}
				else if (indx == 4)
				{
					EPriority5 = BlacklistMenuW->CheckBox(teamate->ChampionName(), false);
					EPriority05 = teamate;
					return;
				}
			}
		}
	}

	void HealWa()
	{
		int indx = 0;
		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (auto teamate : Teamates)
		{
			if (indx == 0)
			{
				indx++;
				WHeal1 = BlacklistMenuR->CheckBox(teamate->ChampionName(), false);
				WHeal01 = teamate;
			}
			else if (indx == 1)
			{
				indx++;
				WHeal2 = BlacklistMenuR->CheckBox(teamate->ChampionName(), false);
				WHeal02 = teamate;
			}
			else if (indx == 2)
			{
				indx++;
				WHeal3 = BlacklistMenuR->CheckBox(teamate->ChampionName(), false);
				WHeal03 = teamate;
			}
			else if (indx == 3)
			{
				indx++;
				WHeal4 = BlacklistMenuR->CheckBox(teamate->ChampionName(), false);
				WHeal04 = teamate;
			}
			else if (indx == 4)
			{
				WHeal5 = BlacklistMenuR->CheckBox(teamate->ChampionName(), false);
				WHeal05 = teamate;
				return;
			}
		}
	}

	void AutoQd()
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (ComboQdash->Enabled())
			{
				if (Enemy != nullptr)
				{
					if (Q->IsReady() && Enemy->IsValidTarget())
					{
						if (Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
						{
							AdvPredictionOutput outputfam;
							Q->RunPrediction(Enemy, false, kCollidesWithMinions, &outputfam);
							if (outputfam.HitChance == kHitChanceDashing)
							{
								Q->CastOnTarget(Enemy, kHitChanceDashing);
							}
						}
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
			if (enemy->IsValidTarget() && !enemy->IsDead() &&enemy != nullptr && enemy->GetTeam() != GEntityList->Player()->GetTeam())
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

	void Combo()
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
		{
			if (ComboQ->Enabled() && GEntityList->Player()->IsValidTarget(target, Q->Range()) && Q->IsReady())
			{
				Q->CastOnTarget(target);
			}
			if (!ComboW->Enabled() && GEntityList->Player()->IsValidTarget(target, W->Range()) && W->IsReady())
			{
				W->CastOnTarget(target);
			}
			if (GEntityList->Player()->IsValidTarget(target, W->Range()) && W->IsReady())
			{
				if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
				{
					if (GEntityList->Player()->HealthPercent() < 90)
					{
						W->CastOnTarget(target);
					}
					if (target->HealthPercent() < 15)
					{
						W->CastOnTarget(target);
					}
				}
			}

			if (ComboR->Enabled() && R->IsReady())
			{
				if (GEntityList->Player()->IsValidTarget(target, R->Range()))
				{
					if (GetEnemiesInRange(GEntityList->Player(), R->Range()) >= ComboRmin->GetInteger() && GetAlliesInRange(GEntityList->Player(), R->Range() - 100) >= ComboRally->GetInteger())
					{
						R->CastOnTarget(target, kHitChanceHigh);
					}
				}
			}
		}
	}


	void Healing()
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (Ally != GEntityList->Player() && !GEntityList->Player()->IsDead() && !Ally->IsDead())
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
			if (Ally != nullptr && Ally->IsValidTarget())
			{

				if (!GEntityList->Player()->IsDead() && HealW->Enabled() && HealWmy->GetInteger() > GEntityList->Player()->HealthPercent() && W->IsReady() && !GEntityList->Player()->IsRecalling() && !GUtility->IsPositionInFountain(GEntityList->Player()->ServerPosition()))
				{
					W->CastOnPlayer();
				}
			}
		}
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

	void AutoQLogic()
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
	}

	/*int (IUnit* Source, float range)
	{
		auto Targets = GEntityList->GetAllHeros(false, true);
		auto AlliesInRange = 0;

		for (auto target : Targets)
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
		return AlliesInRange;
	}*/
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

	void AutoE()
	{
		if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		{
			if (GetAlliesInRange(GEntityList->Player(), E->Range()) >= 1)
			{

				for (auto Ally : GEntityList->GetAllHeros(true, false))
				{
					if (EPriority01->ChampionName() == "Nami" || EPriority02->ChampionName() == "Nami" || EPriority03->ChampionName() == "Nami" || EPriority04->ChampionName() == "Nami" || EPriority04->ChampionName() == "Nami")
					{
						return;
					}

					if (Ally != GEntityList->Player())
					{
						if (EPriority1 != nullptr && EPriority1->Enabled() && EPriority01->ChampionName() == Ally->ChampionName())
						{
							if (EPriority01->IsWindingUp())
							{
								if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
								{
									if (Ally != nullptr)
									{
										E->CastOnUnit(EPriority01);
									}

								}
							}
						}
						else if (EPriority1->Enabled() && !EPriority01->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
							{
								if (Ally->IsWindingUp())
								{
									if (Ally != nullptr)
									{
										E->CastOnUnit(Ally);
									}
								}
							}
						}
						if (EPriority2 != nullptr && EPriority2->Enabled() && EPriority02->ChampionName() == Ally->ChampionName())
						{
							if (EPriority02->IsWindingUp())
							{
								if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
								{
									if (Ally != nullptr)
									{
										E->CastOnUnit(EPriority02);
									}
								}

							}
						}
						else if (EPriority2->Enabled() && !EPriority02->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
							{
								if (Ally->IsWindingUp())
								{
									if (Ally != nullptr)
									{
										E->CastOnUnit(Ally);
									}
								}
							}
						}
						if (EPriority3 != nullptr && EPriority3->Enabled() && EPriority03->ChampionName() == Ally->ChampionName())
						{
							if (EPriority03->IsWindingUp())
							{
								if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
								{
									if (Ally != nullptr)
									{
										E->CastOnUnit(EPriority03);
									}
								}
							}
						}
						else if (EPriority3->Enabled() && !EPriority03->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
							{
								if (Ally != nullptr)
								{
									if (Ally->IsWindingUp())
									{
										if (Ally != nullptr)
										{
											E->CastOnUnit(Ally);
										}
									}
								}
							}
						}
						if (EPriority4 != nullptr && EPriority4->Enabled() && EPriority04->ChampionName() == Ally->ChampionName())
						{
							if (EPriority04->IsWindingUp())
							{
								if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
								{
									if (Ally != nullptr)
									{
										E->CastOnUnit(EPriority04);
									}
								}
							}
						}
						else if (EPriority4->Enabled() && !EPriority04->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
							{
								if (Ally->IsWindingUp())
								{
									if (Ally != nullptr)
									{
										E->CastOnUnit(Ally);
									}
								}
							}
						}
						if (EPriority5 != nullptr && EPriority5->Enabled() && EPriority05->ChampionName() == Ally->ChampionName())
						{
							if (EPriority05->IsWindingUp())
							{
								if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
								{
									
									if (Ally != nullptr)
									{
										E->CastOnUnit(EPriority05);
									}
								}
							}
						}
						else if (EPriority5->Enabled() && !EPriority05->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
							{
								if (Ally->IsWindingUp())
								{
									if (Ally != nullptr)
									{
										E->CastOnUnit(Ally);
									}
								}
							}
						}
						if (!EPriority1->Enabled() && !EPriority2->Enabled() && !EPriority3->Enabled() && !EPriority4->Enabled() && !EPriority5->Enabled())
						{
							if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
							{
								if (Ally->IsWindingUp())
								{
									if (Ally != nullptr)
									{
										E->CastOnUnit(Ally);
									}
								}
							}
						}
					}
				}
			}
			else if (GetAlliesInRange(GEntityList->Player(), E->Range()) == 0)
			{
				if (ComboE->Enabled() && E->IsReady())
				{
					if (GEntityList->Player()->IsWindingUp())
					{
						E->CastOnPlayer();
					}
				}
			}
		}
	}
	

	/*static void AutoE(CastedSpell const& Args)
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (Ally != nullptr && Ally->IsValidTarget())
			{
				if (Ally != GEntityList->Player())
				{
					auto autoattack = Ally->GetAutoAttack();
					if (autoattack.Target_ != nullptr && autoattack.Caster_ != nullptr && autoattack.Target_->IsHero() && autoattack.Caster_ == Ally)
					{
						if (ComboE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							E->CastOnUnit(Ally);
						}
					}
				}
			}
		}
	}*/


	void Interrupt(InterruptibleSpell const& Args)
	{
		float endtime;
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, R->Range()) && InterruptR->Enabled() && R->IsReady())
		{
			if (Args.Target->IsCastingImportantSpell(&endtime))
			{
				R->CastOnTarget(Args.Target);
			}
		}
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, Q->Range()) && InterruptQ->Enabled() && Q->IsReady())
		{
			Q->CastOnTarget(Args.Target);
		}
	}


	/*void OnAfterAttack(IUnit* source, IUnit* target)
	{
		if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && source != nullptr)
		{
			if (source->IsValidTarget() && !source->IsEnemy(GEntityList->Player()) && source->IsHero() && source != GEntityList->Player())
			{
				GGame->PrintChat("lul");
				if (ComboE->Enabled() && E->IsReady())
				{
					GGame->PrintChat("lul1");
					if ((GEntityList->Player()->GetPosition() - source->GetPosition()).Length2D() <= E->Range())
					{
						GGame->PrintChat("lul2");
						E->CastOnUnit(source);
					}
				}
			}
		}
	}*/
	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), W->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), E->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), R->Range()); }
	}
};