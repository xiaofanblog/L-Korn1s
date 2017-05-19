#pragma once
#include "Menu.h"
#include "EventManager.h"
#include "DamageTracker.h"
class RakanBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Rakan - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboW = ComboMenu->CheckBox("Use W in Combo", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			ComboRmin = ComboMenu->AddInteger("Enemies near for R", 1, 5, 2);
			ComboRhp = ComboMenu->AddFloat("If Enemies HP Percent", 10, 100, 50);
			ComboEgapclose = ComboMenu->AddKey("Engage E>W Combo ( Select Target )", 'T');

		}
		HarassMenu = MainMenu->AddMenu("Harass");
		{
			HarassMana = HarassMenu->AddFloat("Mana Percent", 10, 100, 50);
			HarassQ = HarassMenu->CheckBox("Use Q in Harass", true);
			HarassE = HarassMenu->CheckBox("E>W>E Harass Logic", true);

		}

		ShieldMenu = MainMenu->AddMenu("Shielding");
		{
			ShieldE = ShieldMenu->CheckBox("Auto E", true);
			ShieldHP = ShieldMenu->AddFloat("Auto Shield if X HP", 1, 100, 50);
			ShieldHPinc = ShieldMenu->AddFloat("Auto Shield if Inc. Damage Percent", 1, 100, 10);
			ShieldInc = ShieldMenu->CheckBox("Auto Shield only Incoming Damage(sometimes not accurate)", true);
			ShieldKill = ShieldMenu->CheckBox("Auto Shield if inc. damage will kill", true);
		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw Flee Radius", true);
		DrawQFRange = DrawingMenu->CheckBox("Draw Engage Range", true);

		}

		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			InterruptW = MiscMenu->CheckBox("W Interrupt", true);
			AntiGapW = MiscMenu->CheckBox("W AntiGap", true);
		}
		FleeMenu = MainMenu->AddMenu("Flee");
		{
			FleeKey = FleeMenu->AddKey("Flee Key", 'G');
			FleeR = FleeMenu->CheckBox("Flee with R", false);
			FleeE = FleeMenu->CheckBox("Flee with E", true);
		}
	}


	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithMinions));
		Q->SetSkillshot(0.25f, 60, 5000, 850);
		W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, true, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		W->SetSkillshot(0.25f, 50, FLT_MAX, 650);
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));

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

	void Engage()
	{
		if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
		{
			GGame->IssueOrderEx(GEntityList->Player(), kMoveTo, GGame->CursorPosition(), false);

			auto target = GTargetSelector->GetFocusedTarget();
			if (target != nullptr && target->IsValidTarget() && target->IsHero())
			{
				for (auto Ally : GEntityList->GetAllHeros(true, false))
				{
					if (Ally != GEntityList->Player())
					{

						if (Ally != nullptr && Ally->IsValidTarget() && Ally->IsValidTarget(GEntityList->Player(), E->Range() + W->Range()))
						{
							if (target->IsValidTarget(GEntityList->Player(), W->Range() + E->Range()))
							{
								if ((GEntityList->Player()->GetPosition() - Ally->GetPosition()).Length2D() < E->Range())
								{
									if (E->ManaCost() + W->ManaCost() < GEntityList->Player()->GetMana())
									{
										if (!GEntityList->Player()->HasBuff("rakanerecast"))
										{
											E->CastOnTarget(Ally);
										}
										if (GEntityList->Player()->HasBuff("rakanerecast"))
										{
											W->CastOnTarget(target);
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
	void Flee()
	{

		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			GGame->IssueOrderEx(GEntityList->Player(), kMoveTo, GGame->CursorPosition(), false);
			if (FleeE->Enabled() && E->IsReady())
			{
				for (auto target : GEntityList->GetAllHeros(true, false))
				{
					if (target != GEntityList->Player())
					{
						if (target->IsValidTarget() && target->IsValidTarget(GEntityList->Player(), E->Range()) && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < 200)
							{
								E->CastOnUnit(target);
							}
						}
					}
				}
			}
			if (FleeR->Enabled() && R->IsReady())
			{
				if (!E->IsReady())
				{
					R->CastOnPlayer();
				}
				if (E->IsReady() && GetAlliesInRange(GEntityList->Player(), 1000) == 0)
				{ 
					R->CastOnPlayer();
				}
			}
		}
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

				}
				if (ShieldInc->Enabled())
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
					}
				}

			}
		}
	}

	void Combo()
	{
		if (ComboQ->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 50)
				{
					AdvPredictionOutput outputfam;
					Q->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
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
		}
		if (ComboW->Enabled())
		{

			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{
				for (auto Enemy : GEntityList->GetAllHeros(false, true))
				{
					if (Ally != GEntityList->Player())
					{

						if (Ally != nullptr && Ally->IsValidTarget() && Ally->IsValidTarget(GEntityList->Player(), E->Range() + W->Range()))
						{
							if (Enemy->IsValidTarget() && Enemy != nullptr && !Enemy->IsDead() && Enemy->IsValidTarget(Ally, 580))
							{
								if ((GEntityList->Player()->GetPosition() - Ally->GetPosition()).Length2D() < E->Range())
								{
									if (E->ManaCost() + W->ManaCost() < GEntityList->Player()->GetMana())
									{
										if (Ally->IsValidTarget(GEntityList->Player(), E->Range()))
										{
											if (!GEntityList->Player()->HasBuff("rakanerecast") && delay < GGame->TickCount())
											{
												if ((Ally->GetPosition() - Enemy->GetPosition()).Length2D() < (GEntityList->Player()->GetPosition() - Enemy->GetPosition()).Length2D())
												{
													E->CastOnTarget(Ally);
												}
											}
										}
										if (GEntityList->Player()->HasBuff("rakanerecast"))
										{
											Vec3 pred;
											GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
											if (InSpellRange(W, pred))
											{
												if (W->CastOnPosition(pred))
												{
													delay = GGame->TickCount() + 1500;
												}
											}
										}
									}
								}
								if ((GEntityList->Player()->GetPosition() - Ally->GetPosition()).Length2D() > E->Range())
								{
									if (E->ManaCost() + W->ManaCost() < GEntityList->Player()->GetMana())
									{
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
										if (InSpellRange(W, pred))
										{
											if (W->CastOnPosition(pred))
											{
												delay = GGame->TickCount() + 1500;
											}
										}
										if (Ally->IsValidTarget(GEntityList->Player(), E->Range()))
										{
											if (!GEntityList->Player()->HasBuff("rakanerecast") && delay < GGame->TickCount())
											{
												if ((Ally->GetPosition() - Enemy->GetPosition()).Length2D() < (GEntityList->Player()->GetPosition() - Enemy->GetPosition()).Length2D())
												{
													E->CastOnTarget(Ally);
												}
											}
										}
									}
								}
							}
							else if ((Enemy->GetPosition() - Ally->GetPosition()).Length2D() > 580)
							{
								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
								if (target != nullptr && target->IsValidTarget() && !target->IsDead())
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
									if (InSpellRange(W, pred))
									{
										if (W->CastOnPosition(pred))
										{
											delay = GGame->TickCount() + 1500;
										}
									}
								}

							}
						}
						else if ((Enemy->GetPosition() - Ally->GetPosition()).Length2D() > W->Range()+E->Range())
						{
							auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
							if (target != nullptr && target->IsValidTarget() && !target->IsDead())
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
								if (InSpellRange(W, pred))
								{
									if (W->CastOnPosition(pred))
									{
										delay = GGame->TickCount() + 1500;
									}
								}
							}

						}
					}
				}
			}
			if (!E->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
					if (InSpellRange(W, pred))
					{
						if (W->CastOnPosition(pred))
						{
							delay = GGame->TickCount() + 1500;
						}
					}

				}
			}
		}
		if (ComboR->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 1000);
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				if (GetEnemiesInRange(GEntityList->Player(), 1000) > ComboRmin->GetInteger())
				{
					if (target->HealthPercent() < ComboRhp->GetFloat())
					{
						R->CastOnPlayer();
					}
				}
			}
		}
	}

	void Harass()
	{
		if (HarassMana->GetFloat() < GEntityList->Player()->ManaPercent())
		{
			if (HarassQ->Enabled())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{


					AdvPredictionOutput outputfam;
					Q->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
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
			if (HarassE->Enabled())
			{

				for (auto Ally : GEntityList->GetAllHeros(true, false))
				{
					for (auto Enemy : GEntityList->GetAllHeros(false, true))
					{
						if (Ally != GEntityList->Player())
						{

							if (Ally != nullptr && Ally->IsValidTarget() && Ally->IsValidTarget(GEntityList->Player(), E->Range()+W->Range())) 
							{
								if (Enemy->IsValidTarget() && Enemy != nullptr && !Enemy->IsDead() && Enemy->IsValidTarget(Ally, 580))
								{
									if ((GEntityList->Player()->GetPosition() - Ally->GetPosition()).Length2D() < E->Range())
									{
										if (E->ManaCost() + W->ManaCost() < GEntityList->Player()->GetMana())
										{
											if (Ally->IsValidTarget(GEntityList->Player(), E->Range()))
											{
												E->CastOnTarget(Ally);
											}
											if (GEntityList->Player()->HasBuff("rakanerecast"))
											{
												W->CastOnTarget(Enemy);
											}
											if (Ally->IsValidTarget(GEntityList->Player(), E->Range()))
											{

												if (Ally->HasBuff("RakanEShield"))
												{
													E->CastOnTarget(Ally);
												}
											}
										}
									}									
									if ((GEntityList->Player()->GetPosition() - Ally->GetPosition()).Length2D() > E->Range())
									{
										if (E->ManaCost() + W->ManaCost() < GEntityList->Player()->GetMana())
										{
											W->CastOnTarget(Enemy);
											if (Ally->IsValidTarget(GEntityList->Player(), E->Range()))
											{
												E->CastOnTarget(Ally);

												if (Ally->HasBuff("RakanEShield"))
												{
													E->CastOnTarget(Ally);
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
		}
	}

	void Interrupt(InterruptibleSpell const& Args)
	{

		if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, W->Range()) && InterruptW->Enabled() && W->IsReady())
		{
			W->CastOnTarget(Args.Source);
		}
	}
	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Source != GEntityList->Player()
			&& Args.Source->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Source, W->Range() + Args.Source->BoundingRadius())
			&& AntiGapW->Enabled() && W->IsReady())
		{
			W->CastOnPosition(Args.EndPosition);
		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(145, 255, 255, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
		if (DrawQFRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()+W->Range()); }
		if (DrawRRange->Enabled())
		{
			for (auto hero : GEntityList->GetAllHeros(false, true))
			{
				if (GetAsyncKeyState(FleeKey->GetInteger()))
				{
					GRender->DrawOutlinedCircle(GGame->CursorPosition(), Vec4(255, 0, 0, 255), 200);
				}
			}
		}
	}
};