#pragma once
#include "Menu.h"
#include <unordered_map>
#include "EventManager.h"
#include "DamageTracker.h"
#include "lpputils.h"

class LuluBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Lulu - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboW = ComboMenu->CheckBox("Use W in Combo", true);
			ComboWenable = ComboMenu->CheckBox("^- Block Myself", true);
			ComboE = ComboMenu->CheckBox("Use E in Combo With Logic", true);
			SupportMode = ComboMenu->CheckBox("Support Mode", true);
			ComboEally = ComboMenu->AddKey("Semi W>E Ally", 'T');
			ComboWQreset = ComboMenu->CheckBox("^- Block Myself", true);

		}
		ShieldMenu = MainMenu->AddMenu("Shielding");
		{
			ShieldE = ShieldMenu->CheckBox("Auto E", true);
			ShieldHP = ShieldMenu->AddFloat("Auto Shield if X HP", 1, 100, 50);
			ShieldHPinc = ShieldMenu->AddFloat("Auto Shield if Inc. Damage Percent", 1, 100, 10);
			ShieldInc = ShieldMenu->CheckBox("Auto Shield only Incoming Damage(sometimes not accurate)", true);
			ShieldKill = ShieldMenu->CheckBox("Auto Shield if inc. damage will kill", true);
			ShieldAA = ShieldMenu->CheckBox("Priority Allies over me", true);
			ShieldSelf = ShieldMenu->CheckBox("Shield myself", true);
			ShieldSelfHP = ShieldMenu->AddFloat("Start Shielding Self if HP", 0, 100, 70);
		}
		RSettings = MainMenu->AddMenu("R Settings");
		{
			ComboR = RSettings->CheckBox("Auto R", true);
			ComboRmin = RSettings->AddFloat("^- if Knocks up ", 0, 5, 2);
			ComboRhp = RSettings->CheckBox("^- if Incoming Damage will Kill", true);
			ComboRally = RSettings->CheckBox("Auto R if Ally Health is Low", false);
			ComboR1vs1 = RSettings->AddFloat("^- if Ally Health", 1, 100, 20);

		}
		BlacklistMenuW = MainMenu->AddMenu("W Priority");
		{
			Wpriority();
		}

		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
			DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
			DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
			DrawEmin = DrawingMenu->CheckBox("Draw Pix Position", true);
			DrawDamage = DrawingMenu->CheckBox("Draw Ranges from Ally", true);
		}
		FarmMenu = MainMenu->AddMenu("Farming");
		{
			FarmMana = FarmMenu->AddFloat("Mana Manager", 10, 100, 50);
			FarmQ = FarmMenu->CheckBox("Use Q", true);
			FarmQmin = FarmMenu->AddFloat("^- if hits", 0, 6, 3);
		}
		KillstealMenu = MainMenu->AddMenu("Killsteal");
		{
			KSQ = KillstealMenu->CheckBox("Killsteal with Q and E Logic", true);
		}
		HarassMenu = MainMenu->AddMenu("Harass");
		{
			HarassQ = HarassMenu->CheckBox("Use Q", true);
			HarassQ2 = HarassMenu->CheckBox("Use E>Q Harass Logic", true);
		}
		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			InterruptW = MiscMenu->CheckBox("Use W to interrupt", true);
			InterruptR = MiscMenu->CheckBox("Use R to interrupt", true);
			InterruptQ = MiscMenu->CheckBox("^- Only Dangerous Spells", true);
			AntiGapQ = MiscMenu->CheckBox("Anti Gap Q", true);
			AntiGapW = MiscMenu->CheckBox("Anti Gap W", true);
			AutoQ = MiscMenu->CheckBox("Auto W Ally if Slowed", true);
		}
		FleeMenu = MainMenu->AddMenu("Flee");
		{
			FleeKey = FleeMenu->AddKey("Flee Key", 'G');
		}

	}
	void Harass()
	{
		if (HarassQ->Enabled())
		{
			if (Q->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(target, 0.4f, true, pred);

					Q->CastOnPosition(pred);
				}
			}
		}
		if (HarassQ2->Enabled())
		{
			for (auto Minions : GEntityList->GetAllMinions(false, true, true))
			{
				if (Minions->IsValidTarget() && Minions != nullptr && !Minions->IsDead() && (Minions->IsCreep() || Minions->IsJungleCreep()))
				{

					for (auto targets : GEntityList->GetAllHeros(false, true))
					{
						if (targets != nullptr && !targets->IsDead() & targets->IsValidTarget())
						{
							if (Minions->IsValidTarget(GEntityList->Player(), E->Range()))
							{
								if (Minions->IsValidTarget(targets, Q->Range()) && Q->IsReady())
								{
									E->CastOnTarget(Minions);
								}
							}
							if (Minions->HasBuff("lulufaerieburn") || Minions->HasBuff("lulufaerieshield"))
							{
								if (Minions->IsValidTarget(GEntityList->Player(), 2000))
								{
									if ((targets->GetPosition() - Minions->GetPosition()).Length2D() < Q->Range())
									{
										Q->SetRangeCheckFrom(Minions->GetPosition());
										Q->SetFrom(Minions->GetPosition());
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(targets, 0.4f, true, pred);

										Q->CastOnPosition(pred);
									}
								}
							}
						}
					}
				}
			}
			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{

				for (auto targets : GEntityList->GetAllHeros(false, true))
				{
					if (targets != nullptr && !targets->IsDead() & targets->IsValidTarget())
					{

						if (Ally->IsValidTarget(GEntityList->Player(), E->Range()) && Ally->IsValidTarget(targets, Q->Range()))
						{
							if (Ally->IsValidTarget() && Ally != nullptr && !Ally->IsDead() && Q->IsReady())
							{
								E->CastOnTarget(Ally);
							}
						}


						if (Ally->HasBuff("lulufaerieshield") && !Ally->IsDead() && Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), 2000))
						{
							if ((targets->GetPosition() - Ally->GetPosition()).Length2D() < Q->Range())
							{
								Q->SetRangeCheckFrom(Ally->GetPosition());
								Q->SetFrom(Ally->GetPosition());
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(targets, 0.4f, true, pred);

								Q->CastOnPosition(pred);
							}
						}
					}
				}
			}
		}
	}
	void Something()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{

			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{
				if (EPriority1 != nullptr && EPriority1->Enabled() && EPriority01 == Ally)
				{

					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally != nullptr)
						{
							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(EPriority01);
									E->CastOnUnit(EPriority01);
								}
							}
							if (!ComboWQreset->Enabled())
							{

								W->CastOnUnit(EPriority01);
								E->CastOnUnit(EPriority01);

							}
						}

					}
				}
				else if (EPriority1->Enabled() && !EPriority01->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{

						if (Ally != nullptr)
						{

							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
							}
							if (!ComboWQreset->Enabled())
							{
								if (GetAlliesInRange(GEntityList->Player(), W->Range()) > 0)
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
								if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
								{
									W->CastOnUnit(GEntityList->Player());
									E->CastOnUnit(GEntityList->Player());
								}

							}
						}
					}
				}
				if (EPriority2 != nullptr && EPriority2->Enabled() && EPriority02 == Ally)
				{

					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally != nullptr)
						{

							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(EPriority02);
									E->CastOnUnit(EPriority02);
								}
							}
							if (!ComboWQreset->Enabled())
							{

								W->CastOnUnit(EPriority02);
								E->CastOnUnit(EPriority02);

							}
						}
					}

				}
				else if (EPriority2->Enabled() && !EPriority02->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{

						if (Ally != nullptr)
						{
							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
							}
							if (!ComboWQreset->Enabled())
							{
								if (GetAlliesInRange(GEntityList->Player(), W->Range()) > 0)
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
								if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
								{
									W->CastOnUnit(GEntityList->Player());
									E->CastOnUnit(GEntityList->Player());
								}

							}
						}
					}
				}
				if (EPriority3 != nullptr && EPriority3->Enabled() && EPriority03 == Ally)
				{

					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally != nullptr)
						{

							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(EPriority03);
									E->CastOnUnit(EPriority03);
								}
							}
							if (!ComboWQreset->Enabled())
							{

								W->CastOnUnit(EPriority03);
								E->CastOnUnit(EPriority03);

							}
						}
					}
				}
				else if (EPriority3->Enabled() && !EPriority03->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally != nullptr)
						{

							if (Ally != nullptr)
							{

								if (ComboWQreset->Enabled())
								{
									if (Ally != GEntityList->Player())
									{
										W->CastOnUnit(Ally);
										E->CastOnUnit(Ally);
									}
								}
								if (!ComboWQreset->Enabled())
								{

									if (GetAlliesInRange(GEntityList->Player(), W->Range()) > 0)
									{
										W->CastOnUnit(Ally);
										E->CastOnUnit(Ally);
									}
									if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
									{
										W->CastOnUnit(GEntityList->Player());
										E->CastOnUnit(GEntityList->Player());
									}

								}
							}
						}
					}
				}
				
				if (EPriority4 != nullptr && EPriority4->Enabled() && EPriority04 == Ally)
				{

					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (Ally != nullptr)
						{

							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(EPriority04);
									E->CastOnUnit(EPriority04);
								}
							}
							if (!ComboWQreset->Enabled())
							{

								W->CastOnUnit(EPriority04);
								E->CastOnUnit(EPriority04);

							}
						}
					}
				}
				else if (EPriority4->Enabled() && !EPriority04->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{

						if (Ally != nullptr)
						{

							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
							}
							if (!ComboWQreset->Enabled())
							{

								if (GetAlliesInRange(GEntityList->Player(), W->Range()) > 0)
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
								if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
								{
									W->CastOnUnit(GEntityList->Player());
									E->CastOnUnit(GEntityList->Player());
								}
							}
						}
					}
				}
					
				if (EPriority5 != nullptr && EPriority5->Enabled() && EPriority05 == Ally)
				{

					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{

						if (Ally != nullptr)
						{

							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(EPriority05);
									E->CastOnUnit(EPriority05);
								}
							}
							if (!ComboWQreset->Enabled())
							{

								W->CastOnUnit(EPriority05);
								E->CastOnUnit(EPriority05);

							}
						}
					}
				}
				
				else if (EPriority5->Enabled() && !EPriority05->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{

						if (Ally != nullptr)
						{
							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
							}
							if (!ComboWQreset->Enabled())
							{
								if (GetAlliesInRange(GEntityList->Player(), W->Range()) > 0)
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
								if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
								{
									W->CastOnUnit(GEntityList->Player());
									E->CastOnUnit(GEntityList->Player());
								}

							}
						}
					}
				}
				if (!EPriority1->Enabled() && !EPriority2->Enabled() && !EPriority3->Enabled() && !EPriority4->Enabled() && !EPriority5->Enabled())
				{
					if (Ally->IsValidTarget(GEntityList->Player(), W->Range()))
					{

						if (Ally != nullptr)
						{
							if (ComboWQreset->Enabled())
							{
								if (Ally != GEntityList->Player())
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
							}
							if (!ComboWQreset->Enabled())
							{
								if (GetAlliesInRange(GEntityList->Player(), W->Range()) > 0)
								{
									W->CastOnUnit(Ally);
									E->CastOnUnit(Ally);
								}
								if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
								{
									W->CastOnUnit(GEntityList->Player());
									E->CastOnUnit(GEntityList->Player());
								}

							}
						}
					}
				}
			}
		}
	}
	void Auto()
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), W->Range()) && Ally->HasBuffOfType(BUFF_Slow))
			{
				if (AutoQ->Enabled() && W->Range() && W->IsReady())
					W->CastOnTarget(Ally);
			}
		}
	}
	void Flee()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			GGame->IssueOrderEx(GEntityList->Player(), kMoveTo, GGame->CursorPosition(), false);
			if (W->IsReady())
			{
				W->CastOnPlayer();
			}
		}
	}
	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall));
		Q->SetSkillshot(0.2, 60.f, 10000000.f, 900.f);

		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, (kCollidesWithNothing));

		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, (kCollidesWithNothing));

		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, (kCollidesWithNothing));
	}
	static int GetMinionsQ(float range)
	{
		auto minions = GEntityList->GetAllMinions(false, true, true);
		auto minionsInRange = 0;
		for (auto minion : minions)
		{
			if (minion != nullptr && minion->IsValidTarget() && minion->IsEnemy(GEntityList->Player()) && !minion->IsDead())
			{
				auto minionDistance = (minion->GetPosition() - GEntityList->Player()->GetPosition()).Length2D();
				if (minionDistance < range)
				{
					minionsInRange++;
				}
			}
		}
		return minionsInRange;
	}
	void Farm()
	{
		if (!SupportMode->Enabled())
		{
			if (FarmMana->GetFloat() < GEntityList->Player()->ManaPercent())
			{
				for (auto Minion : GEntityList->GetAllMinions(false, true, false))
				{

					if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						if (GetMinionsQ(Q->Range()) <= 7)
						{
							Vec3 pos;
							int hit;
							GPrediction->FindBestCastPosition(Q->Range(), Q->Radius(), true, true, false, pos, hit);
							if (FarmQ->Enabled())
							{
								if (hit >= FarmQmin->GetFloat())
								{
									Q->CastOnPosition(pos);
								}
							}
						}
						if (GetMinionsQ(Q->Range()) > 7)
						{
							Q->CastOnTarget(Minion);
						}
					}
				}
				for (auto Minions : GEntityList->GetAllMinions(false, false, true))
				{
					if (FarmQ->Enabled() && Q->IsReady() && Minions->IsValidTarget() && !Minions->IsDead() && Minions->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						Q->CastOnUnit(Minions);
					}
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

	void AutoE()
	{

		static DamageTracker tracker = DamageTracker(GPluginSDK, true);
		for (auto ally : GEntityList->GetAllHeros(true, false))
		{
			if (ShieldE->Enabled() && !GEntityList->Player()->IsRecalling())
			{
				if (ally != GEntityList->Player())
				{
					if (!ShieldInc->Enabled())
					{
						if (ally != nullptr && !ally->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(ally, E->Range()))
						{

							if (ally != nullptr && !ally->IsDead() && (GEntityList->Player()->GetPosition() - ally->GetPosition()).Length2D() <= E->Range()
								&& ally->HealthPercent() <= ShieldHP->GetFloat() && GetEnemiesInRange(ally, E->Range()) >= 1)
							{
								E->CastOnUnit(ally);
							}
							if (GetAlliesInRange(GEntityList->Player(), E->Range()) == 0)
							{
								if (GetEnemiesInRange(GEntityList->Player(), E->Range()) >= 1 && GEntityList->Player()->HealthPercent() <= ShieldHP->GetFloat())
								{
									E->CastOnPlayer();
								}

							}
							if (ShieldKill->Enabled())
							{
								if (tracker.getIncomingDamage(ally) >= ally->GetHealth())
								{
									E->CastOnUnit(ally);
								}
							}
							if (ally != nullptr)
							{
								if (tracker.getIncomingDamagePercent(ally) >= ShieldHPinc->GetFloat())
								{

									E->CastOnUnit(ally);
								}
							}
							if (tracker.hasIncomingDamage(ally))
							{

								if (ShieldKill->Enabled())
								{
									if (tracker.getIncomingDamage(ally) >= ally->GetHealth())
									{
										E->CastOnUnit(ally);
									}
								}
							}

						}
					}
				}
				if (ShieldAA->Enabled())
				{
					if (ally != GEntityList->Player())
					{
						if (GetAlliesInRange(GEntityList->Player(), E->Range()) >= 1)
						{
							if (ally != nullptr && !ally->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(ally, E->Range()))
							{
								if (ally->HealthPercent() <= GEntityList->Player()->HealthPercent())
								{
									if (ally != nullptr)
									{
										if (tracker.getIncomingDamagePercent(ally) >= ShieldHPinc->GetFloat())
										{

											E->CastOnUnit(ally);
										}
									}
									if (!tracker.hasIncomingDamage(ally))
										return;
									if (ShieldKill->Enabled())
									{
										if (tracker.getIncomingDamage(ally) >= ally->GetHealth())
										{
											E->CastOnUnit(ally);
										}
									}
								}
							}
						}
						if (GetAlliesInRange(GEntityList->Player(), E->Range()) == 0)
						{
							if (ShieldSelf->Enabled())
							{
								if (GEntityList->Player() != nullptr && !GEntityList->Player()->IsDead())
								{

									if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat())
									{
										E->CastOnPlayer();
									}
								}
								if (GetEnemiesInRange(GEntityList->Player(), E->Range()) >= 1 && GEntityList->Player()->HealthPercent() <= ShieldHP->GetFloat())
								{
									E->CastOnPlayer();
								}


								if (!tracker.hasIncomingDamage(GEntityList->Player()))
									return;
								if (ShieldKill->Enabled())
								{
									if (tracker.getIncomingDamage(GEntityList->Player()) >= GEntityList->Player()->GetHealth())
									{
										E->CastOnPlayer();
									}
								}
							}
						}
					}
				}

				if (!ShieldAA->Enabled())
				{
					if (ally == GEntityList->Player())
					{
						if (ShieldSelf->Enabled())
						{
							if (GEntityList->Player() != nullptr && !GEntityList->Player()->IsDead())
							{

								if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat() && GEntityList->Player()->HealthPercent() < ShieldSelfHP->GetFloat() && ally->HealthPercent() > GEntityList->Player()->HealthPercent())
								{
									E->CastOnPlayer();
								}
								if (GetEnemiesInRange(GEntityList->Player(), E->Range()) >= 1 && GEntityList->Player()->HealthPercent() <= ShieldHP->GetFloat() && GEntityList->Player()->HealthPercent() < ShieldSelfHP->GetFloat() && ally->HealthPercent() > GEntityList->Player()->HealthPercent())
								{
									E->CastOnPlayer();
								}
							}

							if (!tracker.hasIncomingDamage(GEntityList->Player()))
								return;
							if (ShieldKill->Enabled())
							{
								if (tracker.getIncomingDamage(GEntityList->Player()) >= GEntityList->Player()->GetHealth())
								{
									E->CastOnPlayer();
								}
							}

						}
					}

				}
				if (ShieldInc->Enabled())
				{
					if (ShieldAA->Enabled())
					{
						if (ally != GEntityList->Player())
						{
							if (GetAlliesInRange(GEntityList->Player(), E->Range()) >= 1)
							{
								if (ally != nullptr && !ally->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(ally, E->Range()))
								{
									if (ally->HealthPercent() <= GEntityList->Player()->HealthPercent())
									{
										if (ally != nullptr)
										{
											if (tracker.getIncomingDamagePercent(ally) >= ShieldHPinc->GetFloat())
											{

												E->CastOnUnit(ally);
											}
										}
										if (!tracker.hasIncomingDamage(ally))
											return;
										if (ShieldKill->Enabled())
										{
											if (tracker.getIncomingDamage(ally) >= ally->GetHealth())
											{
												E->CastOnUnit(ally);
											}
										}
									}
									if (ShieldSelf->Enabled())
									{
										if (ally->HealthPercent() >= GEntityList->Player()->HealthPercent())
										{

											if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat() && GEntityList->Player()->HealthPercent() < ShieldSelfHP->GetFloat() && ally->HealthPercent() > GEntityList->Player()->HealthPercent())
											{

												E->CastOnPlayer();
											}
										}
										if (!tracker.hasIncomingDamage(GEntityList->Player()))
											return;
										if (ShieldKill->Enabled())
										{
											if (tracker.getIncomingDamage(GEntityList->Player()) >= ally->GetHealth())
											{
												E->CastOnPlayer();
											}

										}
									}
								}

							}
						}
						if (GetAlliesInRange(GEntityList->Player(), E->Range()) == 0)
						{
							if (ShieldSelf->Enabled())
							{
								if (GEntityList->Player() != nullptr && !GEntityList->Player()->IsDead())
								{

									if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat())
									{
										E->CastOnPlayer();
									}
								}
								if (!tracker.hasIncomingDamage(GEntityList->Player()))
									return;
								if (ShieldKill->Enabled())
								{
									if (tracker.getIncomingDamage(GEntityList->Player()) >= GEntityList->Player()->GetHealth())
									{
										E->CastOnPlayer();
									}
								}
							}
						}
					}
					if (!ShieldAA->Enabled())
					{
						if (ally == GEntityList->Player())
						{
							if (GEntityList->Player() != nullptr && !GEntityList->Player()->IsDead())
							{
								if (ShieldSelf->Enabled())
								{

									if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat() && GEntityList->Player()->HealthPercent() < ShieldSelfHP->GetFloat() && ally->HealthPercent() > GEntityList->Player()->HealthPercent())
									{
										E->CastOnPlayer();
									}
								}
								if (!tracker.hasIncomingDamage(GEntityList->Player()))
									return;
								if (ShieldKill->Enabled())
								{
									if (tracker.getIncomingDamage(GEntityList->Player()) >= GEntityList->Player()->GetHealth())
									{
										E->CastOnPlayer();
									}
								}

							}
						}

					}

				}
			}
		}
	}
	void AutoR()
	{

		static DamageTracker tracker = DamageTracker(GPluginSDK, true);
		for (auto ally : GEntityList->GetAllHeros(true, false))
		{
			if (ComboR->Enabled())
			{
				if (GetEnemiesInRange(ally, 800) > 0)
				{
					if (ComboRally->Enabled() && !ComboRhp->Enabled())
					{
						if (ally->IsValidTarget(GEntityList->Player(), R->Range()))
						{
							if (ally->IsValidTarget() && ally != nullptr && !ally->IsDead() && GetEnemiesInRange(ally, 350) >= ComboRmin->GetFloat() && tracker.hasIncomingDamage(ally) && tracker.getIncomingDamage(ally) > 1)
							{
								if (GEntityList->Player()->IsValidTarget(ally, R->Range()))
								{
									if (ally->HealthPercent() < ComboR1vs1->GetFloat())
									{
										R->CastOnTarget(ally);
									}
								}
							}
						}
					}
					if (!ComboRally->Enabled() && ComboRhp->Enabled())
					{
						if (ally->IsValidTarget(GEntityList->Player(), R->Range()))
						{
							if (ally->IsValidTarget() && ally != nullptr && !ally->IsDead() && tracker.getIncomingDamage(ally) > ally->GetHealth() && ally->HealthPercent() < 30)
							{
								if (GEntityList->Player()->IsValidTarget(ally, R->Range()))
								{
									R->CastOnTarget(ally);
								}
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

	void Killsteal()
	{
		if (KSQ->Enabled())
		{

			for (auto Minions : GEntityList->GetAllMinions(false, true, true))
			{
				if (Minions->IsValidTarget() && Minions != nullptr && !Minions->IsDead() && (Minions->IsCreep() || Minions->IsJungleCreep()))
				{


					for (auto targets : GEntityList->GetAllHeros(false, true))
					{
						if (targets != nullptr && !targets->IsDead() & targets->IsValidTarget())
						{
							auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), targets, kSlotQ);
							if (Minions->IsValidTarget(GEntityList->Player(), E->Range()) && targets->GetHealth() < QDamage)
							{
								if (Minions->IsValidTarget(targets, Q->Range()) && Q->IsReady())
								{
									E->CastOnTarget(Minions);
								}
							}
							if (Minions->HasBuff("lulufaerieburn") || Minions->HasBuff("lulufaerieshield"))
							{
								if (Minions->IsValidTarget(GEntityList->Player(), 2000))
								{
									if ((targets->GetPosition() - Minions->GetPosition()).Length2D() < Q->Range() && targets->GetHealth() < QDamage)
									{
										Q->SetRangeCheckFrom(Minions->GetPosition());
										Q->SetFrom(Minions->GetPosition());
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(targets, 0.4f, true, pred);

										Q->CastOnPosition(pred);
									}
								}
							}
						}
					}
				}
			}
			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{

				for (auto targets : GEntityList->GetAllHeros(false, true))
				{
					if (targets != nullptr && !targets->IsDead() & targets->IsValidTarget())
					{
						auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), targets, kSlotQ);
						if (Ally->IsValidTarget(GEntityList->Player(), E->Range()) && Ally->IsValidTarget(targets, Q->Range()))
						{
							if (Ally->IsValidTarget() && Ally != nullptr && !Ally->IsDead() && Q->IsReady() && targets->GetHealth() < QDamage)
							{
								E->CastOnTarget(Ally);
							}
						}


						if (Ally->HasBuff("lulufaerieshield") && !Ally->IsDead() && Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), 2000))
						{
							auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), targets, kSlotQ);
							if ((targets->GetPosition() - Ally->GetPosition()).Length2D() < Q->Range() && targets->GetHealth() < QDamage)
							{
								Q->SetRangeCheckFrom(Ally->GetPosition());
								Q->SetFrom(Ally->GetPosition());
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(targets, 0.4f, true, pred);

								Q->CastOnPosition(pred);
							}
						}
					}
				}
			}
			for (auto targets : GEntityList->GetAllHeros(false, true))
			{
				if (Enemy != nullptr && !Enemy->IsDead() & Enemy->IsValidTarget())
				{
					auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), targets, kSlotQ);
					if (Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(targets, 0.4f, true, pred);

						Q->CastOnPosition(pred);
					}
				}
			}
		}
	}
	void Combo()
	{

		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range() + 1300);
			if (target != nullptr && target->IsValidTarget() && target->IsHero())
			{
				for (auto Minions : GEntityList->GetAllMinions(true, true, true))
				{
					if (Minions->IsValidTarget() && Minions != nullptr && !Minions->IsDead() && Minions->IsValidTarget(GEntityList->Player(), 2000))
					{
						if (Minions->HasBuff("lulufaerieburn") || Minions->HasBuff("lulufaerieshield"))
						{
							if ((target->GetPosition() - Minions->GetPosition()).Length2D() < Q->Range())
							{
								Q->SetRangeCheckFrom(Minions->GetPosition());
								Q->SetFrom(Minions->GetPosition());
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(target, 0.4f, true, pred);

								Q->CastOnPosition(pred);
							}
						}
					}
				}
				for (auto targets : GEntityList->GetAllHeros(false, true))
				{
					if (targets->HasBuff("lulufaerieburn"))
					{

						if (targets->IsValidTarget() && targets != nullptr && !targets->IsDead() && targets->IsValidTarget(GEntityList->Player(), 2000))
						{

							if (targets->HasBuff("lulufaerieburn"))
							{
								if ((targets->GetPosition() - target->GetPosition()).Length2D() < Q->Range())
								{
									Q->SetRangeCheckFrom(targets->GetPosition());
									Q->SetFrom(targets->GetPosition());
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(target, 0.4f, true, pred);

									Q->CastOnPosition(pred);
								}
							}
						}
					}
				}
				for (auto Ally : GEntityList->GetAllHeros(true, false))
				{
					if (target->IsValidTarget() && !target->IsDead() && target != nullptr &&  target->IsValidTarget(GEntityList->Player(), 1300 + Q->Range()))
					{
						if (GetAlliesInRange(GEntityList->Player(), 1300) == 0 || !Ally->HasBuff("lulufaerieshield"))
						{
							if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < Q->Range())
							{
								if (target != nullptr && target->IsValidTarget() && !target->IsDead())
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(target, 0.4f, true, pred);

									Q->CastOnPosition(pred);
								}
							}
						}
						if (Ally->IsValidTarget(GEntityList->Player(), 1300) && !target->IsValidTarget(Ally, Q->Range()) || !Ally->HasBuff("lulufaerieshield"))
						{



							if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < Q->Range())
							{
								if (target != nullptr && target->IsValidTarget() && !target->IsDead())
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(target, 0.4f, true, pred);

									Q->CastOnPosition(pred);
								}
							}
						}

						if (Ally->HasBuff("lulufaerieshield") && !Ally->IsDead() && Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), 2000) && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 1300 + Q->Range())
						{

							if (target->IsValidTarget(Ally, Q->Range()))
							{

								if (target != nullptr && target->IsValidTarget() && !target->IsDead())
								{
									
									Q->SetRangeCheckFrom(Ally->GetPosition());
									Q->SetFrom(Ally->GetPosition());
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(target, 0.4f, true, pred);

									Q->CastOnPosition(pred);
								}
							}
						}
					}
				}

			}
		}

		if (ComboE->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
			{
				if ((GetAlliesInRange(GEntityList->Player(), E->Range()) == 0 || target->HealthPercent() < 5) && GEntityList->Player()->HealthPercent() > 20)
				{
					if (GEntityList->Player()->IsValidTarget(target, E->Range()) && E->IsReady())
					{
						E->CastOnTarget(target);

					}
				}
			}
		}


		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
			{
				if (ComboW->Enabled() && GEntityList->Player()->IsValidTarget(target, W->Range()) && W->IsReady())
				{
					if (EPriority1 != nullptr && EPriority1->Enabled() && EPriority01 == Ally)
					{
						if (EPriority01->IsWindingUp())
						{
							if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (Ally != nullptr)
								{

									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(EPriority01);
											
										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(EPriority01);
										

									}
								}

							}
						}
					}
					else if (EPriority1->Enabled() && !EPriority01->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
						{
							if (Ally->IsWindingUp())
							{
								if (Ally != nullptr)
								{


									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(Ally);

										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(Ally);


									}
								}
							}
						}
					}
					if (EPriority2 != nullptr && EPriority2->Enabled() && EPriority02 == Ally)
					{
						if (EPriority02->IsWindingUp())
						{
							if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (Ally != nullptr)
								{

									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(EPriority02);

										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(EPriority02);


									}
								}
							}

						}
					}
					else if (EPriority2->Enabled() && !EPriority02->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
						{
							if (Ally->IsWindingUp())
							{
								if (Ally != nullptr)
								{


									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(Ally);

										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(Ally);


									}
								}
							}
						}
					}
					if (EPriority3 != nullptr && EPriority3->Enabled() && EPriority03 == Ally)
					{
						if (EPriority03->IsWindingUp())
						{
							if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (Ally != nullptr)
								{

									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(EPriority03);

										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(EPriority03);


									}
								}
							}
						}
					}
					else if (EPriority3->Enabled() && !EPriority03->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
						{
							if (Ally != nullptr)
							{
								if (Ally->IsWindingUp())
								{
									if (Ally != nullptr)
									{


										if (ComboWenable->Enabled())
										{
											if (Ally != GEntityList->Player())
											{
												W->CastOnUnit(Ally);

											}
										}
										if (!ComboWenable->Enabled())
										{

											W->CastOnUnit(Ally);


										}
									}
								}
							}
						}
					}
					if (EPriority4 != nullptr && EPriority4->Enabled() && EPriority04 == Ally)
					{
						if (EPriority04->IsWindingUp())
						{
							if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								if (Ally != nullptr)
								{

									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(EPriority04);

										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(EPriority04);


									}
								}
							}
						}
					}
					else if (EPriority4->Enabled() && !EPriority04->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
						{
							if (Ally->IsWindingUp())
							{
								if (Ally != nullptr)
								{


									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(Ally);

										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(Ally);


									}
								}
							}
						}
					}
					if (EPriority5 != nullptr && EPriority5->Enabled() && EPriority05 == Ally)
					{
						if (EPriority05->IsWindingUp())
						{
							if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
							{

								if (Ally != nullptr)
								{

									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(EPriority05);

										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(EPriority05);


									}
								}
							}
						}
					}
					else if (EPriority5->Enabled() && !EPriority05->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
						{
							if (Ally->IsWindingUp())
							{
								if (Ally != nullptr)
								{


									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(Ally);

										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(Ally);


									}
								}
							}
						}
					}
					if (!EPriority1->Enabled() && !EPriority2->Enabled() && !EPriority3->Enabled() && !EPriority4->Enabled() && !EPriority5->Enabled())
					{
						if (ComboW->Enabled() && W->IsReady() && Ally->IsValidTarget(GEntityList->Player(), W->Range()))
						{
							if (Ally->IsWindingUp())
							{
								if (Ally != nullptr)
								{

									if (ComboWenable->Enabled())
									{
										if (Ally != GEntityList->Player())
										{
											W->CastOnUnit(Ally);

										}
									}
									if (!ComboWenable->Enabled())
									{

										W->CastOnUnit(Ally);


									}
								}
							}
						}
					}
				}
			}

		}
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

	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Source != GEntityList->Player()
			&& Args.Source->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Source, Q->Range() + Args.Source->BoundingRadius())
			&& AntiGapQ->Enabled() && Q->IsReady())
		{
			Q->CastOnPosition(Args.EndPosition);
		}
		if (Args.Source != GEntityList->Player()
			&& Args.Source->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Source, W->Range() + Args.Source->BoundingRadius())
			&& AntiGapW->Enabled() && W->IsReady())
		{
			W->CastOnTarget(Args.Source);
		}
	}


	void Interrupt(InterruptibleSpell const& Args)
	{


		if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, W->Range()) && InterruptW->Enabled() && W->IsReady())
		{
			W->CastOnTarget(Args.Source);



		}

		for (auto ally : GEntityList->GetAllHeros(true, false))
		{
			if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, R->Range()) && InterruptR->Enabled() && R->IsReady())
			{
				if (Args.Source->IsValidTarget(GEntityList->Player(), 350))
				{
					if (Args.DangerLevel == kHighDanger && InterruptQ->Enabled())
					{
						R->CastOnPlayer();

					}
				}
				else if (ally != GEntityList->Player() && ally->IsValidTarget(Args.Source, 350) && ally->IsValidTarget() && !ally->IsDead())
				{
					if (Args.DangerLevel == kHighDanger && InterruptQ->Enabled())
					{
						R->CastOnTarget(ally);
					}
				}
			}

		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 50, 50, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(125, 155, 255, 255), E->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
		if (DrawEmin->Enabled())
		{
			for (auto obj : GEntityList->GetAllUnits())
			{
				if (obj != nullptr && obj->IsValidObject() && obj->GetTeam() == GEntityList->Player()->GetTeam() && strcmp(obj->GetObjectName(), "RobotBuddy") == 0)
				{
					GPluginSDK->GetRenderer()->DrawOutlinedCircle(obj->GetPosition(), Vec4(220, 20, 60, 255), 100);
				}
			}
		}
		if (DrawDamage->Enabled())
		{
			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{
				if (Ally->IsValidTarget() && Ally != nullptr && !Ally->IsDead() && Ally->IsValidTarget(GEntityList->Player(), 1800))
				{
					if (Ally->HasBuff("lulufaerieshield"))
					{
						if (DrawQRange->Enabled())
						{
							GPluginSDK->GetRenderer()->DrawOutlinedCircle(Ally->GetPosition(), Vec4(255, 255, 0, 255), Q->Range());
						}
					}
				}
			}
			for (auto target : GEntityList->GetAllHeros(false, true))
			{
				if (target->IsValidTarget() && target != nullptr && !target->IsDead() && target->IsValidTarget(GEntityList->Player(), 1800))
				{
					if (target->HasBuff("lulufaerieburn"))
					{
						if (DrawQRange->Enabled())
						{
							GPluginSDK->GetRenderer()->DrawOutlinedCircle(target->GetPosition(), Vec4(255, 255, 0, 255), Q->Range());
						}
					}
				}
			}
			for (auto Minions : GEntityList->GetAllMinions(true, true, true))
			{
				if (Minions->IsValidTarget() && Minions != nullptr && !Minions->IsDead() && Minions->IsValidTarget(GEntityList->Player(), 1800))
				{
					if (Minions->HasBuff("lulufaerieburn") || Minions->HasBuff("lulufaerieshield"))
					{
						if (DrawQRange->Enabled())
						{
							GPluginSDK->GetRenderer()->DrawOutlinedCircle(Minions->GetPosition(), Vec4(255, 255, 0, 255), Q->Range());
						}
					}
				}
			}
		}
	}
};