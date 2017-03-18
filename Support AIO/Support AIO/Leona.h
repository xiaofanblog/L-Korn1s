#pragma once
#include "Menu.h"
#include <string>
class LeonaBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Leona by Kornis");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboEQ = ComboMenu->CheckBox("Use Q only if E hits", true);
			ComboW = ComboMenu->CheckBox("Use W in combo", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			ComboRmin = ComboMenu->AddInteger("Use R if enemies X >", 1, 5, 2);

		}
		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			AntigapQ = MiscMenu->CheckBox("Anti-Gapclose Q", true);
			InterruptR = MiscMenu->CheckBox("Interrupt R", true);
			InterruptQ = MiscMenu->CheckBox("Interrupt Q", true);
		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawERange = DrawingMenu->CheckBox("Draw E range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		}

		BlacklistMenu = MainMenu->AddMenu("Blacklist");

		for (auto enemy : GEntityList->GetAllHeros(false, true)) {
			BlacklistMenu->CheckBox(enemy->ChampionName(), false);
		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, true, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E->SetOverrideDelay(0.25);
		E->SetOverrideRadius(50);
		E->SetOverrideSpeed(5000);
		E->SetOverrideRange(860);
		R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R->SetOverrideDelay(0.25);
		R->SetOverrideRadius(50);
		R->SetOverrideSpeed(1000);
		R->SetOverrideRange(1150);
	}



	int GetEnemiesInRange(float range)
	{
		auto enemies = GEntityList->GetAllHeros(false, true);
		auto enemiesInRange = 0;

		for (auto enemy : enemies)
		{
			if (enemy != nullptr && enemy->GetTeam() != GEntityList->Player()->GetTeam())
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
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			IMenuOption* temp = BlacklistMenu->GetOption(Enemy->ChampionName());
			if (!Enemy->IsDead())
			{
				if (ComboE->Enabled() && E->Range())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
					if (E->IsReady())
						if (!temp->Enabled())
						{
							{
								if (!ComboEQ->Enabled())
								{
									if (target != nullptr)
									{
										if (E->CastOnTarget(target))
										{
											Q->CastOnPlayer();
										}
									}
								}

								if (ComboEQ->Enabled())
								{
									if (Q->IsReady())
									{
										if (target != nullptr)
										{
											E->CastOnTarget(target);
											if (target->HasBuff("leonazenithblade"))
											{
												Q->CastOnPlayer();
											}

										}
									}
									else if (!Q->IsReady())
									{
										if (target != nullptr)
										{
											{
												E->CastOnTarget(target);
											}
										}
									}
								}
							}
							if (!E->IsReady())
							{
								if (target != nullptr)
								{
									if (target->IsValidTarget(GEntityList->Player(), 300))
									{
										Q->CastOnPlayer();
									}
								}
							}
						}
				}


				if (ComboQ->Enabled() && Q->IsReady() && W->Range())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
					if (target != nullptr && target->IsValidTarget(GEntityList->Player(), 400))
					{
						Q->CastOnPlayer();
					}
				}
				if (ComboW->Enabled() && W->IsReady() && W->Range())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
					if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						W->CastOnPlayer();
					}
				}
				if (ComboR->Enabled() && R->IsReady() && R->Range())
				{
					if (ComboRmin->GetInteger() >= 2)
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
						if (target != nullptr && !target->IsDead() && !target->IsInvulnerable())
						{
							Vec3 pos;
							int hit;
							GPrediction->FindBestCastPosition(R->Range(), 250, false, false, true, pos, hit);
							if (hit >= ComboRmin->GetInteger())
								R->CastOnPosition(pos);

						}
					}
					if (ComboRmin->GetInteger() == 1)
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
						if (target != nullptr && !target->IsDead() && !target->IsInvulnerable())
						{
							R->CastOnTarget(target);
						}
					}
				}
			}

		}
	}

	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player() && Args.Sender->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Sender, 200 + Args.Sender->BoundingRadius()) && AntigapQ->Enabled() && Q->IsReady())
		{
			if (Q->CastOnPlayer())
			{
				GOrbwalking->SetOverrideTarget(Args.Sender);
			}
		}
	}


	void Interrupt(InterruptibleSpell const& Args)
	{
		float endtime;
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, 200) && InterruptQ->Enabled() && Q->IsReady())
		{
			Q->CastOnPlayer();
			GOrbwalking->SetOverrideTarget(Args.Target);
		}
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, R->Range()) && InterruptR->Enabled() && R->IsReady())
		{
			if (Args.Target->IsCastingImportantSpell(&endtime))
			{
				if (!Q->IsReady() || !GEntityList->Player()->IsValidTarget(Args.Target, 200))
					R->CastOnTarget(Args.Target, kHitChanceHigh);
			}

		}
	}

	void Draw() const
	{
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), R->Range()); }
	}
};