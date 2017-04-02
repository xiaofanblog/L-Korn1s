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
			ShieldHP = ShieldMenu->AddFloat("Auto Shield if > X HP", 1, 100, 50);
			ShieldKill = ShieldMenu->CheckBox("Auto Shield if inc. damage will kill", true);
			ShieldAA = ShieldMenu->CheckBox("Priority Allies than me", true);
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
		}

	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall));
		Q->SetSkillshot(0.f, 120.f, 900.f, 1700.f);
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
	}



	void AutoE()
	{

		static DamageTracker tracker = DamageTracker(GPluginSDK, true);
		for (auto ally : GEntityList->GetAllHeros(true, false))
		{
			if (ShieldE->Enabled())
			{
				if (ally != GEntityList->Player())
				{
					std::string s = std::to_string(tracker.hasIncomingDamage(GEntityList->Player()));
					char const *pchar = s.c_str();
					GGame->PrintChat(pchar);
					if (ally != nullptr && !ally->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(ally, E->Range()))
					{
						float extraDamage;
						if (ally != nullptr && !ally->IsDead() && (GEntityList->Player()->GetPosition() - ally->GetPosition()).Length2D() <= E->Range()
							&& ally->HealthPercent() <= 15.f && tracker.hasIncomingLethalDamage(ally, extraDamage))
						{
							E->CastOnUnit(ally);
						}

						if (!tracker.hasIncomingDamage(ally))
							return;
						if (ShieldKill->Enabled())
						{
							if ((tracker.getIncomingDamage(ally) >= ally->GetHealth()) || ((ally->HealthPercent() - tracker.getIncomingDamagePercent(ally)) <= 15.f && (GEntityList->Player()->GetPosition() - ally->GetPosition()).Length2D() <= E->Range()))
							{
								E->CastOnUnit(ally);
							}
						}
						if ((ally->HealthPercent() - tracker.getIncomingDamagePercent(ally)) <= ShieldHP->GetFloat() && (GEntityList->Player()->GetPosition() - ally->GetPosition()).Length2D() <= E->Range())
						{
							E->CastOnUnit(ally);
						}
					}
				}
				if (ShieldAA->Enabled())
				{
					if (ally == GEntityList->Player())
					{
						if (ally->HealthPercent() <= GEntityList->Player()->HealthPercent())
						{
							if (ally != nullptr && !GEntityList->Player()->IsDead())
							{
								float extraDamage;
								if (tracker.hasIncomingDamage(GEntityList->Player()) && GEntityList->Player()->HealthPercent() <= 15.f && tracker.hasIncomingLethalDamage(GEntityList->Player(), extraDamage))
								{
									GGame->PrintChat("1");
									E->CastOnPlayer();
								}
							}
							if (!tracker.hasIncomingDamage(GEntityList->Player()))
								return;
							if (ShieldKill->Enabled())
							{
								if (tracker.hasIncomingDamage(GEntityList->Player()) && (tracker.getIncomingDamage(GEntityList->Player()) >= GEntityList->Player()->GetHealth()) || ((GEntityList->Player()->HealthPercent() - tracker.getIncomingDamagePercent(GEntityList->Player())) <= 15.f))
								{
									GGame->PrintChat("2");
									E->CastOnPlayer();
								}
							}
							if (tracker.hasIncomingDamage(GEntityList->Player()) && (GEntityList->Player()->HealthPercent() - tracker.getIncomingDamagePercent(GEntityList->Player())) <= ShieldHP->GetFloat())
							{
								GGame->PrintChat("3");
								E->CastOnPlayer();
							}
						}
					}
				}
				if (!ShieldAA->Enabled())
				{
					if (ally == GEntityList->Player())
					{
						if (ally != nullptr && !GEntityList->Player()->IsDead())
						{
							float extraDamage;
							if (tracker.hasIncomingDamage(GEntityList->Player())&& GEntityList->Player()->HealthPercent() <= 15.f && tracker.hasIncomingLethalDamage(GEntityList->Player(), extraDamage))
							{
								GGame->PrintChat("1");
								E->CastOnPlayer();
							}
						}
						if (!tracker.hasIncomingDamage(GEntityList->Player()))
							return;
						if (ShieldKill->Enabled())
						{
							if (tracker.hasIncomingDamage(GEntityList->Player()) && (tracker.getIncomingDamage(GEntityList->Player()) >= GEntityList->Player()->GetHealth()) || ((GEntityList->Player()->HealthPercent() - tracker.getIncomingDamagePercent(GEntityList->Player())) <= 15.f))
							{
								GGame->PrintChat("2");
								E->CastOnPlayer();
							}
						}
						if (tracker.hasIncomingDamage(GEntityList->Player()) && (GEntityList->Player()->HealthPercent() - tracker.getIncomingDamagePercent(GEntityList->Player())) <= ShieldHP->GetFloat())
						{
							GGame->PrintChat("3");
							E->CastOnPlayer();
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


	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	}
};