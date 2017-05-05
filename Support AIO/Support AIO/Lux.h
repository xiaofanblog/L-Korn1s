#pragma once
#include "Menu.h"
#include <string>
#include "EventManager.h"
#include "DamageTracker.h"
class LuxBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Lux - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		FarmMenu = MainMenu->AddMenu("Farming");
		DrawingMenu = MainMenu->AddMenu("Drawings");
		HarassMenu = MainMenu->AddMenu("Harass");
		ShieldMenu = MainMenu->AddMenu("Shield menu");
		JungleKS = MainMenu->AddMenu("Jungle Steal");
		KillstealMenu = MainMenu->AddMenu("KillSteal");
		MiscMenu = MainMenu->AddMenu("Misc.");
		ComboQ = ComboMenu->CheckBox("Q in combo", true);
		ComboW = ComboMenu->CheckBox("W in combo", true);
		ComboE = ComboMenu->CheckBox("E in combo", true);
		RSettings = ComboMenu->AddMenu("R Settings");
		KSQ = KillstealMenu->CheckBox("Q killsteal", true);
		KSE = KillstealMenu->CheckBox("E killsteal", true);
		KSR = KillstealMenu->CheckBox("R killsteal", true);
		KSRmin = KillstealMenu->CheckBox("R only on Slow/CC and etc(Hits more)", true);
		ComboR = RSettings->CheckBox("R in combo", true);
		ComboRally = RSettings->CheckBox("R Burst Logic", true);
		Rmin = RSettings->AddFloat("R min. range", 500, 2000, 500);
		ForceR = RSettings->AddKey("Semi R", 'T');
		FarmMana = FarmMenu->AddFloat("Mana Percent", 1, 100, 50);
		FarmQ = FarmMenu->CheckBox("Q minions", true);
		FarmE = FarmMenu->CheckBox("E minions", true);
		FarmEmin = FarmMenu->AddInteger("Minimum minions to hit with E", 1, 20, 3);
		FarmQmin = FarmMenu->AddInteger("Minimum minions to hit with Q", 1, 2, 1);
		AutoRjungle = JungleKS->CheckBox("R Jungle Stealer", true);
		Rdragon = JungleKS->CheckBox("Steal Dragon", true);
		Rbaron = JungleKS->CheckBox("Steal Baron", true);
		Rred = JungleKS->CheckBox("Steal Red", true);
		Rblue = JungleKS->CheckBox("Steal Blue", true);
		Rally = JungleKS->CheckBox("Steal from Ally", false);
		HarassMana = HarassMenu->AddFloat("Mana Percent", 1, 100, 50);
		HarassQ = HarassMenu->CheckBox("Q harras", true);
		HarassE = HarassMenu->CheckBox("E harras", true);
		ShieldMenu = MainMenu->AddMenu("Shield Menu");
		ShieldE = ShieldMenu->CheckBox("Auto W", true);
		ShieldHP = ShieldMenu->AddFloat("Auto Shield if X HP", 1, 100, 50);
		ShieldHPinc = ShieldMenu->AddFloat("Auto Shield if Inc. Damage Percent", 1, 100, 10);
		ShieldInc = ShieldMenu->CheckBox("Auto Shield only Incoming Damage(sometimes not accurate)", true);
		ShieldKill = ShieldMenu->CheckBox("Auto Shield if inc. damage will kill", true);
		ShieldAA = ShieldMenu->CheckBox("Priority Allies over me", true);
		AntiGapQ = MiscMenu->CheckBox("Auto Q for Gapcloser", true);
		AntiGapW = MiscMenu->CheckBox("Auto W for Gapcloser", true);
		DrawQRange = DrawingMenu->CheckBox("Draw Q", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W", false);
		DrawERange = DrawingMenu->CheckBox("Draw E", false);
		DrawRRange = DrawingMenu->CheckBox("Draw R", true);
		DrawKillable = DrawingMenu->CheckBox("Draw Damage", true);
		SupportMode = MainMenu->CheckBox("Support Mode", false);

	}



	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall | kCollidesWithMinions));
		Q->SetSkillshot(0.f, 80, 1000000000, 1050);

		W = GPluginSDK->CreateSpell2(kSlotW, kLineCast, true, false, (kCollidesWithNothing));
		W->SetSkillshot(0.25f, 150.f, 1000.f, 1075.f);

		E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, true, false, (kCollidesWithYasuoWall));
		E->SetSkillshot(0.25f, 80.f, 2000, 1100.f);

		R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, false, (kCollidesWithNothing));
		R->SetSkillshot(0.25f, 90.f, 10000000.f, 3500.f);
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

	void AutoW()
	{

		static DamageTracker tracker = DamageTracker(GPluginSDK, true);
		for (auto ally : GEntityList->GetAllHeros(true, false))
		{
			if (ShieldE->Enabled())
			{
				if (ally != GEntityList->Player())
				{
					if (!ShieldInc->Enabled())
					{
						if (ally != nullptr && !ally->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(ally, W->Range()))
						{

							if (ally != nullptr && !ally->IsDead() && (GEntityList->Player()->GetPosition() - ally->GetPosition()).Length2D() <= W->Range()
								&& ally->HealthPercent() <= ShieldHP->GetFloat() && GetEnemiesInRange(ally, W->Range()) >= 1)
							{
								W->CastOnUnit(ally);
							}
							if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
							{
								if (GetEnemiesInRange(GEntityList->Player(), W->Range()) >= 1 && GEntityList->Player()->HealthPercent() <= ShieldHP->GetFloat())
								{
									W->CastOnPlayer();
								}

							}
							if (ShieldKill->Enabled())
							{
								if (tracker.getIncomingDamage(ally) >= ally->GetHealth())
								{
									W->CastOnUnit(ally);
								}
							}
							if (ally != nullptr)
							{
								if (tracker.getIncomingDamagePercent(ally) >= ShieldHPinc->GetFloat())
								{

									W->CastOnUnit(ally);
								}
							}
							if (tracker.hasIncomingDamage(ally))
							{

								if (ShieldKill->Enabled())
								{
									if (tracker.getIncomingDamage(ally) >= ally->GetHealth())
									{
										W->CastOnUnit(ally);
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
						if (GetAlliesInRange(GEntityList->Player(), W->Range()) >= 1)
						{
							if (ally != nullptr && !ally->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(ally, W->Range()))
							{
								if (ally->HealthPercent() <= GEntityList->Player()->HealthPercent())
								{
									if (ally != nullptr)
									{
										if (tracker.getIncomingDamagePercent(ally) >= ShieldHPinc->GetFloat())
										{

											W->CastOnUnit(ally);
										}
									}
									if (!tracker.hasIncomingDamage(ally))
										return;
									if (ShieldKill->Enabled())
									{
										if (tracker.getIncomingDamage(ally) >= ally->GetHealth())
										{
											W->CastOnUnit(ally);
										}
									}
								}
							}
						}
						if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
						{
							if (GEntityList->Player() != nullptr && !GEntityList->Player()->IsDead())
							{

								if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat())
								{
									W->CastOnPlayer();
								}
							}
							if (GetEnemiesInRange(GEntityList->Player(), W->Range()) >= 1 && GEntityList->Player()->HealthPercent() <= ShieldHP->GetFloat())
							{
								W->CastOnPlayer();
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

				if (!ShieldAA->Enabled())
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

				}
				if (ShieldInc->Enabled())
				{
					if (ShieldAA->Enabled())
					{
						if (ally != GEntityList->Player())
						{
							if (GetAlliesInRange(GEntityList->Player(), W->Range()) >= 1)
							{
								if (ally != nullptr && !ally->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(ally, W->Range()))
								{
									if (ally->HealthPercent() <= GEntityList->Player()->HealthPercent())
									{
										if (ally != nullptr)
										{
											if (tracker.getIncomingDamagePercent(ally) >= ShieldHPinc->GetFloat())
											{

												W->CastOnUnit(ally);
											}
										}
										if (!tracker.hasIncomingDamage(ally))
											return;
										if (ShieldKill->Enabled())
										{
											if (tracker.getIncomingDamage(ally) >= ally->GetHealth())
											{
												W->CastOnUnit(ally);
											}
										}
									}
									if (ally->HealthPercent() >= GEntityList->Player()->HealthPercent())
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
										if (tracker.getIncomingDamage(GEntityList->Player()) >= ally->GetHealth())
										{
											W->CastOnPlayer();
										}

									}
								}

							}
						}
						if (GetAlliesInRange(GEntityList->Player(), W->Range()) == 0)
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
					if (!ShieldAA->Enabled())
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

	//zFederal <3
	void KsJungle()
	{

		if (AutoRjungle->Enabled())
		{
			for (auto mob : GEntityList->GetAllMinions(false, false, true))
			{
				if (mob->GetHealth() == mob->GetMaxHealth())
				{
					continue;
				}

				if (((strstr(mob->GetObjectName(), "Dragon") && Rdragon->Enabled())
					|| (strstr(mob->GetObjectName(), "Baron") && Rbaron->Enabled())
					|| (strstr(mob->GetObjectName(), "Red") && Rred->Enabled())
					|| (strstr(mob->GetObjectName(), "Blue") && Rblue->Enabled()))
					&& (GetAlliesInRange(mob, 500) == 0 || Rally->Enabled())
					&& (GEntityList->Player()->GetPosition() - mob->GetPosition()).Length2D() > 500)
				{
					if (DragonDmg == 0)
						DragonDmg = mob->GetHealth();

					if (GGame->Time() - DragonTime > 3)
					{
						if (DragonDmg - mob->GetHealth() > 0)
						{
							DragonDmg = mob->GetHealth();
						}
						DragonTime = GGame->Time();
					}
					else
					{
						auto DmgSec = (DragonDmg - mob->GetHealth()) * (std::abs(DragonTime - GGame->Time()) / 3);

						if (DragonDmg - mob->GetHealth() > 0)
						{
							auto timeTravel = 0.8;
							auto timeR = (mob->GetHealth() - GDamage->GetSpellDamage(GEntityList->Player(), mob, kSlotR)) / (DmgSec / 3);

							if (timeTravel > timeR && !mob->IsDead())
							{
								if (GEntityList->Player()->IsValidTarget(mob, R->Range()))
								{
									R->CastOnPosition(mob->GetPosition());
								}
							}
						}
						else
						{
							DragonDmg = mob->GetHealth();
						}
					}
				}
			}
		}
	}

	int GetRDamage(int PlayerLevel)
	{
		switch (PlayerLevel)
		{
		case 1:
			return 300;
		case 2:
			return 400;
		case 3:
			return 500;
		}
	}

	static void OnCreateObject(IUnit* Source)
	{
		if (strstr(Source->GetObjectName(), "Lux_Base_E_mis.troy"))
		{
			EMissile = Source;
		}
	}

	static void OnDeleteObject(IUnit* Source)
	{
		if (strstr(Source->GetObjectName(), "Lux_Base_E_tar_nova.troy"))
		{
			EMissile = nullptr;
		}
	}


	static bool ESkillToggle()
	{
		if (strstr(GEntityList->Player()->GetSpellName(kSlotE), "LuxLightstrikeToggle"))
		{
			return true;
		}

		return false;
	}

	inline int CountEnemy(Vec3 Location, int range)
	{
		int Count = 0;

		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if ((Enemy->GetPosition() - Location).Length() < range && Enemy->IsValidTarget() && !Enemy->IsDead())
			{
				Count++;
			}
		}
		return (Count);
	}

	void Combo()
	{
		for (auto target : GEntityList->GetAllHeros(false, true))
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
			auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
			auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
			auto psv = GDamage->GetAutoAttackDamage(GEntityList->Player(), target, true);
			if (target != nullptr && !target->IsDead() && target->IsValidTarget())
			{
				if (ComboQ->Enabled() && Q->IsReady() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
					{
						if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
						{
							if (!Target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(Target))
								|| (Target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(Target))
									&& !Target->HasBuff("LuxIlluminatingFraulein")))
							{

								AdvPredictionOutput outputfam;
								Q->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
								if (outputfam.HitChance >= kHitChanceHigh)
								{
									Vec3 EstimatedEnemyPos;
									GPrediction->GetFutureUnitPosition(Target, 0.2f, true, EstimatedEnemyPos);
									Q->CastOnPosition(EstimatedEnemyPos);
								}
							}

						}
					}
				}


				if (!ESkillToggle() && ComboE->Enabled() && E->IsReady())
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
					if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
					{
						if (!Target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(Target))
							|| (Target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(Target))
								&& !Target->HasBuff("LuxIlluminatingFraulein")))
						{
							E->CastOnTarget(Target);


						}
					}
				}
				if (ESkillToggle() && EMissile != nullptr && CountEnemy(EMissile->GetPosition(), 320) >= 1)
				{
					E->CastOnPlayer();

				}
				if (ComboR->Enabled() && R->IsReady())
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
					if (ComboRally->Enabled())
					{
						if (QDamage + psv + EDamage + RDamage >= target->GetHealth())
						{
							if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
							{
								if (target != nullptr && !target->IsDead() && target->IsValidTarget())
								{
									if (Q->IsReady() && E->IsReady() && R->IsReady())
									{
										AdvPredictionOutput outputfam;
										Q->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
										if (outputfam.HitChance >= kHitChanceHigh)
										{
											Vec3 EstimatedEnemyPos;
											GPrediction->GetFutureUnitPosition(Target, 0.2f, true, EstimatedEnemyPos);
											Q->CastOnPosition(EstimatedEnemyPos);
										}
										if (!Q->IsReady())
										{
											AdvPredictionOutput outputfam;
											Vec3 EstimatedEnemyPos;
											GPrediction->GetFutureUnitPosition(Target, 0.2f, true, EstimatedEnemyPos);
											E->CastOnPosition(EstimatedEnemyPos);
										}
										if (ESkillToggle())
										{
											if (target->HasBuffOfType(BUFF_Slow) || target->HasBuffOfType(BUFF_Stun) || target->HasBuffOfType(BUFF_Snare) || target->HasBuffOfType(BUFF_Fear) || target->HasBuffOfType(BUFF_Knockup) || target->HasBuffOfType(BUFF_Knockback) || target->HasBuffOfType(BUFF_Charm) || target->HasBuffOfType(BUFF_Taunt) || target->HasBuffOfType(BUFF_Suppression))
											{
												Vec3 EstimatedEnemyPos;
												GPrediction->GetFutureUnitPosition(Target, 0.2f, true, EstimatedEnemyPos);
												R->CastOnPosition(EstimatedEnemyPos);
											}
										}
										if (ESkillToggle() && EMissile != nullptr && CountEnemy(EMissile->GetPosition(), 320) >= 1)
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
	}


	void Semi()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				if (!Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), R->Range() - 100) && !Enemy->IsDead() && Enemy->IsValidTarget())
				{
					Vec3 EstimatedEnemyPos;
					GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, EstimatedEnemyPos);
					R->CastOnPosition(EstimatedEnemyPos);
				}
			}
		}
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

	void Push()
	{
		minions = GEntityList->GetAllMinions(false, true, true);
		for (IUnit* minion : minions)
			if (GEntityList->Player()->ManaPercent() >= FarmMana->GetFloat())
			{
				if (FarmQ->Enabled() && Q->IsReady())
				{

					if (minion != nullptr)
					{
						if (!minion->IsDead() && minion->IsValidTarget() && GEntityList->Player()->IsValidTarget(minion, Q->Range()))
						{
							Vec3 bla;
							int blabla;
							GPrediction->FindBestCastPosition(Q->Range(), Q->Radius(), true, true, false, bla, blabla);
							if (blabla >= FarmQmin->GetInteger())
							{
								Q->CastOnPosition(bla);
							}

						}
					}


				}


				if (FarmE->Enabled() && E->IsReady())
				{
					if (minion != nullptr)
					{
						if (!(minion->IsDead()) && GEntityList->Player()->IsValidTarget(minion, E->Range()) && minion != nullptr && (CountMinionsNearMe(GPluginSDK, 1100) >= 2))
						{

							Vec3 bla;
							int blabla;
							GPrediction->FindBestCastPosition(E->Range(), 320, false, true, false, bla, blabla);
							if (blabla >= FarmEmin->GetInteger())
								E->CastOnPosition(bla);

						}
					}
				}

			}



	}

	void harass()
	{
		if (GEntityList->Player()->ManaPercent() >= HarassMana->GetFloat())
		{
			for (auto target : GEntityList->GetAllHeros(false, true))
			{
				if (target != nullptr && !target->IsDead() && target->IsValidTarget())
				{
					if (HarassQ->Enabled() && Q->IsReady() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
						{
							if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
							{
								if (!Target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(Target))
									|| (Target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(Target))
										&& !Target->HasBuff("LuxIlluminatingFraulein")))
								{

									AdvPredictionOutput outputfam;
									Q->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
									if (outputfam.HitChance >= kHitChanceHigh)
									{
										Vec3 EstimatedEnemyPos;
										GPrediction->GetFutureUnitPosition(Target, 0.2f, true, EstimatedEnemyPos);
										Q->CastOnPosition(EstimatedEnemyPos);
									}
								}

							}
						}
					}


					if (!ESkillToggle() && HarassE->Enabled() && E->IsReady())
					{
						auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
						if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
						{
							if (!Target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(Target))
								|| (Target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(Target))
									&& !Target->HasBuff("LuxIlluminatingFraulein")))
							{
								AdvPredictionOutput outputfam;
								Vec3 EstimatedEnemyPos;
								GPrediction->GetFutureUnitPosition(Target, 0.2f, true, EstimatedEnemyPos);
								E->CastOnPosition(EstimatedEnemyPos);
							}

						}
					}
					if (ESkillToggle() && EMissile != nullptr && CountEnemy(EMissile->GetPosition(), 320) >= 1)
					{
						E->CastOnPlayer();

					}
				}
			}
		}

	}
	void Killsteal()
	{
		for (auto target : GEntityList->GetAllHeros(false, true))
		{
			if (!SupportMode->Enabled())
			{
				auto dmgQ = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
				auto dmgE = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
				auto dmgR = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
				auto psv = GDamage->GetAutoAttackDamage(GEntityList->Player(), target, true);
				if (target != nullptr && !target->IsDead() && target->IsValidTarget())
				{
					if (target->HasBuff("LuxIlluminatingFraulein"))
					{
						if (KSR->Enabled() && R->IsReady() && target->IsValidTarget(GEntityList->Player(), R->Range()) && ((dmgR + psv) >= target->GetHealth()))
						{
							if (KSRmin->Enabled())
							{


								if (target->HasBuffOfType(BUFF_Slow) || target->HasBuffOfType(BUFF_Stun) || target->HasBuffOfType(BUFF_Snare) || target->HasBuffOfType(BUFF_Fear) || target->HasBuffOfType(BUFF_Knockup) || target->HasBuffOfType(BUFF_Knockback) || target->HasBuffOfType(BUFF_Charm) || target->HasBuffOfType(BUFF_Taunt) || target->HasBuffOfType(BUFF_Suppression))
								{
									Vec3 EstimatedEnemyPos;
									GPrediction->GetFutureUnitPosition(target, 0.2f, true, EstimatedEnemyPos);
									R->CastOnPosition(EstimatedEnemyPos);
								}
							}
							if (!KSRmin->Enabled())
							{
								Vec3 EstimatedEnemyPos;
								GPrediction->GetFutureUnitPosition(target, 0.2f, true, EstimatedEnemyPos);
								R->CastOnPosition(EstimatedEnemyPos);
							}
						}
					}
					else if (!target->HasBuff("LuxIlluminatingFraulein"))
					{
						if (KSQ->Enabled() && Q->IsReady() && target->IsValidTarget(GEntityList->Player(), Q->Range()) && ((dmgQ) >= target->GetHealth()))
						{
							AdvPredictionOutput outputfam;
							Q->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
							if (outputfam.HitChance >= kHitChanceHigh)
							{
								Vec3 EstimatedEnemyPos;
								GPrediction->GetFutureUnitPosition(target, 0.2f, true, EstimatedEnemyPos);
								Q->CastOnPosition(EstimatedEnemyPos);
							}
						}
						if (KSR->Enabled() && R->IsReady() && target->IsValidTarget(GEntityList->Player(), R->Range()) && ((dmgR) >= target->GetHealth()))
						{
							if (target->HasBuffOfType(BUFF_Slow) || target->HasBuffOfType(BUFF_Stun) || target->HasBuffOfType(BUFF_Snare) || target->HasBuffOfType(BUFF_Fear) || target->HasBuffOfType(BUFF_Knockup) || target->HasBuffOfType(BUFF_Knockback) || target->HasBuffOfType(BUFF_Charm) || target->HasBuffOfType(BUFF_Taunt) || target->HasBuffOfType(BUFF_Suppression))
							{
								Vec3 EstimatedEnemyPos;
								GPrediction->GetFutureUnitPosition(target, 0.2f, true, EstimatedEnemyPos);
								R->CastOnPosition(EstimatedEnemyPos);
							}
						}

						if (KSE->Enabled() && E->IsReady() && target->IsValidTarget(GEntityList->Player(), E->Range()) && ((dmgE) >= target->GetHealth()))
						{
							AdvPredictionOutput outputfam;
							Vec3 EstimatedEnemyPos;
							GPrediction->GetFutureUnitPosition(target, 0.2f, true, EstimatedEnemyPos);
							E->CastOnPosition(EstimatedEnemyPos);
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
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, Q->Range() + Args.Sender->BoundingRadius())
			&& AntiGapQ->Enabled() && Q->IsReady())
		{
			Q->CastOnTarget(Args.Sender);
		}
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, W->Range() + Args.Sender->BoundingRadius())
			&& AntiGapW->Enabled() && W->IsReady())
		{
			W->CastOnTarget(Args.Sender);
		}
	}


	void Draw() const
	{
		if (DrawQRange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (DrawWRange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }

		if (DrawERange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }

		if (DrawRRange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), 3340); }

		if (DrawKillable->Enabled())
		{
			for (auto hero : GEntityList->GetAllHeros(false, true))
			{
				Vec2 barPos = Vec2();
				if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
				{
					auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
					auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
					auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR);

					auto psv = GDamage->GetAutoAttackDamage(GEntityList->Player(), hero, true);
					
					float percentHealthAfterDamage = max(0, hero->GetHealth() - float(QDamage+EDamage+RDamage)) / hero->GetMaxHealth();
					float yPos = barPos.y + yOffset;
					float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
					float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());
					if (!hero->IsDead() && hero->IsValidTarget())
					{
						float differenceInHP = xPosCurrentHp - xPosDamage;
						float pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

						for (int i = 0; i < differenceInHP; i++)
						{
							GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
						}
						if (QDamage + psv + EDamage + RDamage >= hero->GetHealth())
						{
							GRender->DrawTextW(Vec2(barPos.x + xOffset, barPos.y + yOffset - 15), Color, "Killable");
						}
						if (RDamage >= hero->GetHealth())
						{
							GRender->DrawTextW(Vec2(barPos.x + xOffset, barPos.y + yOffset + 15 ), Color2, "R can Kill");
						}
					}
				}
			}
		}
	}


};