#pragma once
#include "Menu.h"
#include <unordered_map>
#include "EventManager.h"
#include "DamageTracker.h"
#include "lpputils.h"

class JannaBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Janna - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboW = ComboMenu->CheckBox("Use W in Combo", true);
			SupportMode = ComboMenu->CheckBox("Support Mode", true);

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
		}	

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
			AntiGapQ = MiscMenu->CheckBox("Anti Gap Q", true);
			AntiGapW = MiscMenu->CheckBox("Anti Gap W", true);
		}

	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall));
		Q->SetSkillshot(0.25f, 100.f, 10000000.f, 800.f);
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
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

								if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat())
								{
									E->CastOnPlayer();
								}
								if (GetEnemiesInRange(GEntityList->Player(), E->Range()) >= 1 && GEntityList->Player()->HealthPercent() <= ShieldHP->GetFloat())
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

											if (tracker.getIncomingDamagePercent(GEntityList->Player()) >= ShieldHPinc->GetFloat())
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

	void Combo()
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
		{
			if (ComboQ->Enabled() && GEntityList->Player()->IsValidTarget(target, Q->Range()) && Q->IsReady())
			{
				Q->CastOnTarget(target, kHitChanceHigh);

			}
			if (ComboW->Enabled() && GEntityList->Player()->IsValidTarget(target, W->Range()) && W->IsReady())
			{
				W->CastOnTarget(target);

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
			Q->CastOnTarget(Args.Source);
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
		if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, Q->Range()) && InterruptQ->Enabled() && Q->IsReady())
		{
			Q->CastOnTarget(Args.Source, kHitChanceHigh);

			LastQcast = GGame->TickCount() + 1500;


		}
		if (LastQcast < GGame->TickCount())
		{
			if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, R->Range()) && InterruptR->Enabled() && R->IsReady())
			{
				if (Args.DangerLevel == kHighDanger && !Q->IsReady())
				{
					R->CastOnPlayer();

				}
			}

		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(125, 155, 255, 255), E->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	}
};