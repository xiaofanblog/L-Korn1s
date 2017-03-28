#pragma once
#include "Menu.h"
#include <string>
class BrandBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Brand - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboQMode = ComboMenu->AddSelection("Q Mode", 1, { "Always", "Only Stun" });
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboW = ComboMenu->CheckBox("Use W in combo", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			ComboMode = ComboMenu->AddSelection("Combo Mode", 1, { "E-Q-W", "E-W-Q", "W-E-Q", "W-Q-E" });
			ComboRmin = ComboMenu->AddInteger("Use R if bounces on", 1, 5, 2);
			ComboRally = ComboMenu->CheckBox("R for Minions Bounce", false);
			ComboRhp = ComboMenu->AddInteger("R if Enemy HP", 10, 100, 50);
			ComboRenable = ComboMenu->CheckBox("R Only if Killable with Combo", true);
			SupportMode = ComboMenu->CheckBox("Support Mode", true);

		}

		HarassMenu = MainMenu->AddMenu("Harass");
		{
			HarassQ = HarassMenu->CheckBox("Use Q in Harass", true);
			HarassQMode = HarassMenu->AddSelection("Q Mode", 1, { "Always", "Only Stun" });
			HarassW = HarassMenu->CheckBox("Use E in Combo", true);
			HarassE = HarassMenu->CheckBox("Use W in combo", true);
			HarassMode = HarassMenu->AddSelection("Harass Mode", 1, { "E-Q-W", "E-W-Q", "W-E-Q", "W-Q-E" });
		}

		KillstealMenu = MainMenu->AddMenu("Killsteal");
		{
			KSQ = KillstealMenu->CheckBox("Killsteal Q", true);
			KSW = KillstealMenu->CheckBox("Killsteal W", true);
			KSE = KillstealMenu->CheckBox("Killsteal E", true);
			KSR = KillstealMenu->CheckBox("Killsteal R", true);
			KSRmin = KillstealMenu->AddInteger("R Only if Bounces on", 1, 5, 2);
		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{

			DrawQRange = DrawingMenu->CheckBox("Draw Q range", true);
			DrawWRange = DrawingMenu->CheckBox("Draw W range", true);
			DrawERange = DrawingMenu->CheckBox("Draw E range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
			DrawDamage = DrawingMenu->CheckBox("Draw Damage", true);
		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithMinions, kCollidesWithYasuoWall));
		W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, true, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall));
		Q->SetOverrideDelay(0.25);
		Q->SetOverrideRadius(50);
		Q->SetOverrideSpeed(100000000000);
		Q->SetOverrideRange(1000);
		W->SetOverrideDelay(0.25);
		W->SetOverrideRadius(50);
		W->SetOverrideSpeed(100000);
		W->SetOverrideRange(900);

	}


	int xOffset = 10;
	int yOffset = 15;
	int Width = 103;
	int Height = 8;
	Vec4 Color = Vec4(105, 198, 5, 255);
	Vec4 FillColor = Vec4(198, 176, 5, 255);

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

	static int CountMinionsNearMe(IUnit* Source, float range)
	{
		auto minion = GEntityList->GetAllMinions(false, true, true);
		auto count = 0;

		for (auto unit : minion)
		{
			if (unit->IsValidTarget() && !unit->IsDead() && (unit->IsCreep() || unit->IsJungleCreep()))
			{
				auto flDistance = (unit->GetPosition() - Enemy->GetPosition()).Length();
				if (flDistance < range)
				{
					count++;
				}
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

	void Harass()
	{
		Enemy = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && Enemy->IsValidTarget())
			{
				if (!Enemy->IsDead())
				{
					if (HarassMode->GetInteger() == 1)
					{

						if (E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && HarassE->Enabled())
						{
							E->CastOnTarget(Enemy);
						}

						if (Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && HarassQ->Enabled())
						{
							if (HarassQMode->GetInteger() == 0)
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
								if (InSpellRange(Q, pred))
									Q->CastOnPosition(pred);
							}
							if (HarassQMode->GetInteger() == 1)
							{
								if (Enemy->HasBuff("brandablaze"))
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
									if (InSpellRange(Q, pred))
										Q->CastOnPosition(pred);
								}
							}
						}

						if (W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && HarassW->Enabled())
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
							W->CastOnPosition(pred);
						}
					}
				}
			}

			if (HarassMode->GetInteger() == 0)
			{
				if (E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && HarassE->Enabled())
				{
					E->CastOnTarget(Enemy);
				}

				if (W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && HarassW->Enabled())
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
					W->CastOnPosition(pred);
				}

				if (Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && HarassQ->Enabled())
				{
					if (HarassQMode->GetInteger() == 0)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						if (InSpellRange(Q, pred))
							Q->CastOnPosition(pred);
					}
					if (HarassQMode->GetInteger() == 1)
					{
						if (Enemy->HasBuff("brandablaze"))
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
							if (InSpellRange(Q, pred))
								Q->CastOnPosition(pred);
						}
					}
				}
			}

			if (HarassMode->GetInteger() == 2)
			{

				if (W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && HarassW->Enabled())
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
					W->CastOnPosition(pred);
				}

				if (E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && HarassE->Enabled())
				{
					E->CastOnTarget(Enemy);
				}

				if (Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && HarassQ->Enabled())
				{
					if (HarassQMode->GetInteger() == 0)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						if (InSpellRange(Q, pred))
							Q->CastOnPosition(pred);
					}
					if (HarassQMode->GetInteger() == 1)
					{
						if (Enemy->HasBuff("brandablaze"))
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
							if (InSpellRange(Q, pred))
								Q->CastOnPosition(pred);
						}
					}
				}
			}

			if (HarassMode->GetInteger() == 3)
			{
				if (W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && HarassW->Enabled())
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
					W->CastOnPosition(pred);
				}

				if (Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && HarassQ->Enabled())
				{
					if (HarassQMode->GetInteger() == 0)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						if (InSpellRange(Q, pred))
							Q->CastOnPosition(pred);
					}
					if (HarassQMode->GetInteger() == 1)
					{
						if (Enemy->HasBuff("brandablaze"))
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
							if (InSpellRange(Q, pred))
								Q->CastOnPosition(pred);
						}
					}
				}

				if (E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && HarassE->Enabled())
				{
					E->CastOnTarget(Enemy);
				}

			}
		}
	}
	
	void Combo()
	{


		Enemy = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		for (auto Enemy : GEntityList->GetAllHeros(false, true));
		if (Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && Enemy->IsValidTarget())
		{
			if (!Enemy->IsDead())
			{
				if (ComboMode->GetInteger() == 1)
				{

					if (E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && ComboE->Enabled())
					{
						E->CastOnTarget(Enemy);
					}

					if (Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && ComboQ->Enabled())
					{
						if (ComboQMode->GetInteger() == 0)
						{

							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
							if (InSpellRange(Q, pred))
								Q->CastOnPosition(pred);
						}
						if (ComboQMode->GetInteger() == 1)
						{
							if (Enemy->HasBuff("brandablaze"))
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
								if (InSpellRange(Q, pred))
									Q->CastOnPosition(pred);
							}
						}
					}

					if (W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && ComboW->Enabled())
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						W->CastOnPosition(pred);
					}
				}

				if (ComboMode->GetInteger() == 0)
				{
					if (E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && ComboE->Enabled())
					{
						E->CastOnTarget(Enemy);
					}
					if (W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && ComboW->Enabled())
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						W->CastOnPosition(pred);
					}


					if (Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && ComboQ->Enabled())
					{
						if (ComboQMode->GetInteger() == 0)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
							if (InSpellRange(Q, pred))
								Q->CastOnPosition(pred);
						}
						if (ComboQMode->GetInteger() == 1)
						{
							if (Enemy->HasBuff("brandablaze"))
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
								if (InSpellRange(Q, pred))
									Q->CastOnPosition(pred);
							}
						}

					}
				}
				if (ComboMode->GetInteger() == 2)
				{
					if (W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && ComboW->Enabled())
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						W->CastOnPosition(pred);
					}

					if (E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && ComboE->Enabled())
					{
						E->CastOnTarget(Enemy);
					}
					if (Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && ComboQ->Enabled())
					{
						if (ComboQMode->GetInteger() == 0)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
							if (InSpellRange(Q, pred))
								Q->CastOnPosition(pred);
						}
						if (ComboQMode->GetInteger() == 1)
						{
							if (Enemy->HasBuff("brandablaze"))
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
								if (InSpellRange(Q, pred))
									Q->CastOnPosition(pred);
							}
						}
					}

				}

				if (ComboMode->GetInteger() == 3)
				{
					if (W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && ComboW->Enabled())
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						W->CastOnPosition(pred);
					}

					if (Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && ComboQ->Enabled())
					{
						if (ComboQMode->GetInteger() == 0)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
							if (InSpellRange(Q, pred))
								Q->CastOnPosition(pred);
						}
						if (ComboQMode->GetInteger() == 1)
						{
							if (Enemy->HasBuff("brandablaze"))
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
								if (InSpellRange(Q, pred))
									Q->CastOnPosition(pred);
							}
						}
					}


					if (E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && ComboE->Enabled())
					{
						E->CastOnTarget(Enemy);
					}

				}

				if (ComboR->Enabled())
				{
					Enemy = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
					for (auto Enemy : GEntityList->GetAllHeros(false, true));
					if (Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && Enemy->IsValidTarget())
					{
						if (R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && !ComboRenable->Enabled())
						{
							if (ComboRhp->GetInteger() >= Enemy->HealthPercent())
							{
								if (GetEnemiesInRange(Enemy, R->Range()) >= ComboRmin->GetInteger() && !ComboRally->Enabled())
								{
									R->CastOnTarget(Enemy);
								}
								if (ComboRally->Enabled())
								{
									if (CountMinionsNearMe(Enemy, R->Range()) >= 1 && GetEnemiesInRange(Enemy, R->Range()) >= ComboRmin->GetInteger())
									{
										R->CastOnTarget(Enemy);
									}
								}
							}
						}
						if (R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && ComboRenable->Enabled())
						{
							auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
							auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotW);
							auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
							auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);
							auto bendrasdmg = QDamage + WDamage + EDamage + RDamage;
							if (Enemy->GetHealth() <= bendrasdmg)
							{
								if (GetEnemiesInRange(Enemy, R->Range()) >= ComboRmin->GetInteger() && !ComboRally->Enabled())
								{
									R->CastOnTarget(Enemy);
								}
								if (ComboRally->Enabled())
								{
									if (CountMinionsNearMe(Enemy, R->Range()) >= 1 && GetEnemiesInRange(Enemy, R->Range()) >= ComboRmin->GetInteger())
									{
										R->CastOnTarget(Enemy);
									}
								}
							}
						}
					}

				}
			}
		}
	}

	void Killsteal()
	{
		if (!SupportMode->Enabled())
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
				auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotW);
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);

				if (Enemy != nullptr && !Enemy->IsDead() && Enemy->IsValidTarget())
				{
					if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
					{

						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						if (InSpellRange(Q, pred))
							Q->CastOnPosition(pred);
					}
					if (KSW->Enabled() && W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && WDamage > Enemy->GetHealth())
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						W->CastOnPosition(pred);
					}
					if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
					{
						E->CastOnTarget(Enemy);
					}
					if (KSRmin->GetInteger() <= GetEnemiesInRange(Enemy, R->Range()))
					{
						if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage > Enemy->GetHealth())
						{
							R->CastOnTarget(Enemy);
						}
					}
				}

			}
		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), R->Range()); }

		if (DrawDamage->Enabled())
		{

			for (auto hero : GEntityList->GetAllHeros(false, true))
			{
				Vec2 barPos = Vec2();
				if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
				{
					auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
					auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotW);
					auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
					auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR);
					auto bendrasdmg = QDamage + WDamage + EDamage + RDamage;
					float percentHealthAfterDamage = max(0, hero->GetHealth() - float(bendrasdmg)) / hero->GetMaxHealth();
					float yPos = barPos.y + yOffset;
					float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
					float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());

					if (RDamage * 3 + EDamage + QDamage > hero->GetHealth())
					{
						GRender->DrawTextW(Vec2(barPos.x + xOffset, barPos.y + yOffset - 13), Color, "Killable");
					}
					if (!hero->IsDead())
					{
						float differenceInHP = xPosCurrentHp - xPosDamage;
						float pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

						for (int i = 0; i < differenceInHP; i++)
						{
							GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
						}
					}
				}
			}
		}
		
	}

};