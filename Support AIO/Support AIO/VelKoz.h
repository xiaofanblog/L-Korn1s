#pragma once
#include "Menu.h"
#include "MalachitePred.h"
#include <string>
#include "Geometry.h"
class VelKozBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("VelKoz - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboQ2 = ComboMenu->CheckBox("Use Q2 in Combo", true);
			ComboW = ComboMenu->CheckBox("Use W in Combo", true);
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			ComboRhp = ComboMenu->CheckBox("Include passive for R damage", true);
			ComboRally = ComboMenu->CheckBox("R only if Enemy in Mouse Radius", true);
			SupportMode = ComboMenu->CheckBox("Support Mode", true);

		}
		HarassMenu = MainMenu->AddMenu("Harass");
		{
			HarassMana = HarassMenu->AddFloat("Mana Percent", 10, 100, 50);
			HarassQ = HarassMenu->CheckBox("Harass Q", true);
			HarassQ2 = HarassMenu->CheckBox("Harass Q2", true);
			HarassW = HarassMenu->CheckBox("Harass W", true);
			HarassE = HarassMenu->CheckBox("Harass E", true);
		}
		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			AntiGapE = MiscMenu->CheckBox("Anti-Gapclose E", true);
			InterruptE = MiscMenu->CheckBox("Interrupt E", true);
			AutoQ = MiscMenu->CheckBox("Auto E on Dash", true);
		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q range", true);
			DrawWRange = DrawingMenu->CheckBox("Draw W range", true);
			DrawERange = DrawingMenu->CheckBox("Draw E range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
			DrawKillable = DrawingMenu->CheckBox("Draw R Killable text", true);
			DrawEmin = DrawingMenu->CheckBox("Draw R Damage", true);
			DrawQFRange = DrawingMenu->CheckBox("Draw Radius", true);

		}

		FarmMenu = MainMenu->AddMenu("Farming");
		{
			FarmMana = FarmMenu->AddFloat("Mana Percent", 10, 100, 50);
			FarmW = FarmMenu->CheckBox("Farm W", true);
			FarmE = FarmMenu->CheckBox("Farm E", true);

		}

		KillstealMenu = MainMenu->AddMenu("Killsteal");
		{
			KSQ = KillstealMenu->CheckBox("Killsteal with Q", true);
			KSW = KillstealMenu->CheckBox("Killsteal with W", true);
			KSE = KillstealMenu->CheckBox("Killsteal with E", true);
		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, false, kCollidesWithYasuoWall | kCollidesWithMinions | kCollidesWithHeroes);
		Q->SetSkillshot(0.25f, 55, 1300, 950);
		Q2 = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, false, kCollidesWithYasuoWall | kCollidesWithMinions | kCollidesWithHeroes);
		Q2->SetSkillshot(0.25f, 55, 2100, 1300);
		Q3 = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, false, kCollidesWithYasuoWall | kCollidesWithMinions | kCollidesWithHeroes);
		Q3->SetSkillshot(0.f, 55, numeric_limits<float>::infinity(), 1300);
		W = GPluginSDK->CreateSpell2(kSlotW, kLineCast, true, false, kCollidesWithYasuoWall);
		W->SetSkillshot(0.25f, 110, 100000000, 950);
		E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, (kCollidesWithNothing));
		E->SetSkillshot(0.2f, 110.f, 1000000.f, 750);
		R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, false, kCollidesWithYasuoWall);
		R->SetOverrideRange(1550);

	}
	PLUGIN_EVENT(void) VelkozOnCreateObject(IUnit* Source)
	{
		if (Source == nullptr)
			return;
		if (Source->IsMissile() && GMissileData->GetCaster(Source)->GetNetworkId() == GEntityList->Player()->GetNetworkId())
		{
			auto data = GMissileData->GetName(Source);
			if (Equals(data, "VelkozQMissile"))
				VelkozQMissile = Source;
		}
	}
	PLUGIN_EVENT(void) VelkozOnDestroyObject(IUnit* Source)
	{
		if (Source == nullptr)
			return;
		if (Source->IsMissile() && GMissileData->GetCaster(Source)->GetNetworkId() == GEntityList->Player()->GetNetworkId())
		{
			auto data = GMissileData->GetName(Source);
			if (Equals(data, "VelkozQMissile"))
				VelkozQMissile = nullptr;
		}
	}
	void setQ()
	{
		if (VelkozQMissile != nullptr)
		{
			Q2->SetRangeCheckFrom(Extend(VelkozQMissile->GetPosition(), GEntityList->Player()->GetPosition(), -Q->Speed()* GGame->Latency() / 1000));
			Q3->SetRangeCheckFrom(Extend(VelkozQMissile->GetPosition(), GEntityList->Player()->GetPosition(), -Q->Speed()* GGame->Latency() / 1000));
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
			auto target = GOrbwalking->GetLastTarget();
			if (SupportMode->Enabled() && GetAlliesInRange(GEntityList->Player(), 1000) >= 1)
			{
				if (target != nullptr && target->IsValidTarget() && (target->IsCreep() || target->IsJungleCreep()))
				{
					GOrbwalking->SetAttacksAllowed(false);
				}
			}
			else if (target != nullptr && target->IsValidTarget() && target->IsHero())
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


	void autoQ()
	{
		if (VelkozQMissile != nullptr)
		{
			for (IUnit* hero : ValidEnemies(Q2->Range(), VelkozQMissile->GetPosition()).ToVector())
			{
				// :^)
				/*AdvPredictionOutput outputfam;
				Q2->RunPrediction(hero, false, kCollidesWithNothing, &outputfam);
				AdvPredictionOutput outputfam2;
				Q3->RunPrediction(hero, false, kCollidesWithNothing, &outputfam2);
				if (outputfam2.HitChance >= kHitChanceHigh  && abs(AngleBetween(outputfam.CastPosition, Q2->GetRangeCheckFrom(), GMissileData->GetStartPosition(VelkozQMissile)) - 90) <= 3)
				{
				
				*Vec3 pred;
					GPrediction->GetFutureUnitPosition(hero, 0.2f, true, pred);
					if (InSpellRange(Q2, pred))
						Q2->CastOnPlayer();
				}*/
				auto pred = MalachitePredGetPrediction(Q2, hero);
				auto pred1 = MalachitePredGetPrediction(Q3, hero);
				if (pred1.HitChance >= kHitChanceHigh  && abs(AngleBetween(pred.CastPosition, Q2->GetRangeCheckFrom(), GMissileData->GetStartPosition(VelkozQMissile)) - 90) <= 3)
				{
					Q2->CastOnPlayer();
				}
			}
		}
	}

	static double VelkozGetRDamage(IUnit* target)
	{
		if (target == nullptr || !R->IsReady() || !target->IsValidTarget() || target->HasBuffOfType(BUFF_SpellShield) || target->HasBuffOfType(BUFF_SpellImmunity))
			return 0;
		if (ComboRhp->Enabled())
		{
			double basepsv = 33;
			double baseR[3] = { 450, 625, 800 };
			auto psv = basepsv + (GEntityList->Player()->GetLevel()*7.5);
			auto ap = GEntityList->Player()->TotalMagicDamage();
			auto psvdmg = GEntityList->Player()->TotalMagicDamage()*0.5;
			auto checkBase = baseR[GEntityList->Player()->GetSpellLevel(3) - 1];
			auto fulldmg = checkBase + (ap * 1.25) + (psv + psvdmg);
			auto RRealDamage = fulldmg;
			auto damage = GDamage->CalcMagicDamage(GEntityList->Player(), target, RRealDamage);
			return damage;
		}
		if (!ComboRhp->Enabled())
		{
			double baseR[3] = { 450, 625, 800 };
			auto ap = GEntityList->Player()->TotalMagicDamage();
			auto checkBase = baseR[GEntityList->Player()->GetSpellLevel(3) - 1];
			auto fulldmg = checkBase + (ap * 1.25);
			auto RRealDamage = fulldmg;
			auto damage = GDamage->CalcMagicDamage(GEntityList->Player(), target, RRealDamage);
			return damage;
			//441わ
			//8335
			//1924
		}
	}

	void AutoE()
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (AutoQ->Enabled())
			{
				if (Enemy != nullptr)
				{
					if (E->IsReady() && Enemy->IsValidTarget())
					{
						if (Enemy->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							AdvPredictionOutput outputfam;
							E->RunPrediction(Enemy, false, kCollidesWithNothing, &outputfam);
							if (outputfam.HitChance == kHitChanceDashing)
							{
								E->CastOnTarget(Enemy, kHitChanceDashing);
							}
							else return;
						}
					}

				}
			}
		}
	}
	// Q logic by Malachite <3
	void Combo()
	{
		if (ComboW->Enabled() && W->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
			{
				if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Target, 0.1f, true, pred);
					if (InSpellRange(W, pred))
					{
						W->CastOnPosition(pred);
					}
				}
			}
		}
		if (ComboE->Enabled() && E->IsReady())
		{
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
			{
				if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
				{
					E->CastOnTarget(Target);
				}
			}
		}
		if (ComboQ->Enabled() && Q->IsReady() && VelkozQMissile == nullptr)
		{
			auto target = SelectTarget(SpellDamage, Q->Range());
			if (target != nullptr)
			{
				Q->CastOnTarget(target);
			}
			if (target != nullptr && ComboQ2->Enabled())
			{
				Vec3 giua = (target->GetPosition() + GEntityList->Player()->GetPosition()) / 2;
				for (int i = 90; i >= -90; i = i - 5)
				{
					Vec3 explode = RotateAround(target->GetPosition(), giua, i);
					if (Distance(explode, GEntityList->Player()->GetPosition()) <= 1100 && Distance(explode, target->GetPosition()) <= Q2->Range()
						&& Distance(explode, GEntityList->Player()->GetPosition()) >= 300
						&& MalachitePredGetCollisions(GEntityList->Player()->GetPosition(), explode, Q->GetCollisionFlags(), target, Q).size() == 0
						&& MalachitePredGetCollisions(explode, target->GetPosition(), Q3->GetCollisionFlags(), target, Q3).size() == 0)

					{
						Q->CastOnPosition(explode);
						break;
					}
				}
			}
		}
		if (ComboQ2->Enabled() && VelkozQMissile != nullptr && Q->IsReady())
		{
			for (IUnit* hero : ValidEnemies(Q2->Range(), VelkozQMissile->GetPosition()).ToVector())
			{
				auto pred = MalachitePredGetPrediction(Q2, hero);
				auto pred1 = MalachitePredGetPrediction(Q3, hero);
				if (pred1.HitChance >= kHitChanceHigh  && abs(AngleBetween(pred.CastPosition, Q2->GetRangeCheckFrom(), GMissileData->GetStartPosition(VelkozQMissile)) - 90) <= 3)
				{
					Q2->CastOnPlayer();
				}
			}
		}

		if (ComboR->Enabled() && R->IsReady())
		{
			auto target = GTargetSelector->FindTargetEx(QuickestKill, SpellDamage, R->Range(), nullptr, true,
				&ValidEnemies().Where([&](IUnit* i) {return Distance(i, GGame->CursorPosition()) > 400; }).ToVector());
			if (target != nullptr && target->IsValidTarget() && target->IsHero())
			{
				if (GEntityList->Player()->IsValidTarget(target, R->Range()-50))
				{
					if (VelkozGetRDamage(target) >= target->GetHealth())
					{
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 300)
						{
								R->CastOnTarget(target);
						
						}

					}
				}
			}
		}
	}
	
	void Harass()
	{
		if (HarassMana->GetFloat() <= GEntityList->Player()->ManaPercent())
		{
			if (HarassW->Enabled() && W->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, W->Range()))
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Target, 0.2f, true, pred);
						if (InSpellRange(W, pred))
						{
							W->CastOnPosition(pred);
						}
					}
				}
			}
			if (HarassE->Enabled() && E->IsReady())
			{
				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero())
				{
					if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Target, 0.15f, true, pred);
						if (InSpellRange(E, pred))
						{
							E->CastOnPosition(pred);
						}
					}
				}
			}
			if (HarassQ->Enabled() && Q->IsReady() && VelkozQMissile == nullptr)
			{
				auto target = SelectTarget(SpellDamage, Q->Range());
				if (target != nullptr)
				{
					Q->CastOnTarget(target);
				}
				if (target != nullptr && ComboQ2->Enabled())
				{
					Vec3 giua = (target->GetPosition() + GEntityList->Player()->GetPosition()) / 2;
					for (int i = 90; i >= -90; i = i - 5)
					{
						Vec3 explode = RotateAround(target->GetPosition(), giua, i);
						if (Distance(explode, GEntityList->Player()->GetPosition()) <= 1100 && Distance(explode, target->GetPosition()) <= Q2->Range()
							&& Distance(explode, GEntityList->Player()->GetPosition()) >= 300
							&& MalachitePredGetCollisions(GEntityList->Player()->GetPosition(), explode, Q->GetCollisionFlags(), target, Q).size() == 0
							&& MalachitePredGetCollisions(explode, target->GetPosition(), Q3->GetCollisionFlags(), target, Q3).size() == 0)

						{
							Q->CastOnPosition(explode);
							break;
						}
					}
				}
			}
			if (HarassQ2->Enabled() && VelkozQMissile != nullptr && Q->IsReady())
			{
				for (IUnit* hero : ValidEnemies(Q2->Range(), VelkozQMissile->GetPosition()).ToVector())
				{
					auto pred = MalachitePredGetPrediction(Q2, hero);
					auto pred1 = MalachitePredGetPrediction(Q3, hero);
					if (pred1.HitChance >= kHitChanceHigh  && abs(AngleBetween(pred.CastPosition, Q2->GetRangeCheckFrom(), GMissileData->GetStartPosition(VelkozQMissile)) - 90) <= 3)
					{

						Q2->CastOnPlayer();
					}
				}
			}
		}
	}

	void Farm()
	{
		minions = GEntityList->GetAllMinions(false, true, true);
		for (IUnit* minion : minions)
			if (GEntityList->Player()->ManaPercent() >= FarmMana->GetFloat())
			{
				{
					if (FarmE->Enabled() && E->IsReady() && GEntityList->Player()->IsValidTarget(minion, E->Range()) && (!(minion->IsDead())))
					{
						if (minion != nullptr)
						{
							Vec3 pos;
							int hit;
							GPrediction->FindBestCastPosition(E->Range(), E->Radius(), false, true, false, pos, hit);
							if (hit >= 2)
								E->CastOnPosition(pos);
						}
					}

					if (FarmW->Enabled() && W->IsReady() && GEntityList->Player()->IsValidTarget(minion, W->Range()) && (!(minion->IsDead())))
					{
						if (minion != nullptr)
						{
							if (W->CastOnUnit(minion)) { return; }
						}
					}

				}
			}
	}


	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, 200 + Args.Source->BoundingRadius()) && AntiGapE->Enabled() && E->IsReady())
		{
			E->CastOnTarget(Args.Source, kHitChanceDashing);
		}
	}


	void Interrupt(InterruptibleSpell const& Args)
	{
		float endtime;
		if (Args.Source != GEntityList->Player() && Args.Source->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Source, E->Range()) && InterruptE->Enabled() && E->IsReady())
		{
			if (Args.Source->IsCastingImportantSpell(&endtime))
			{
				E->CastOnTarget(Args.Source, kHitChanceHigh);
			}

		}
	}
	PLUGIN_EVENT(void) OnUpdateChargedSpell(int Slot, Vec3* Position, bool* ReleaseCast, bool* TriggerEvent)
	{
		for (auto target : GEntityList->GetAllHeros(false, true))
		{
			if (target != nullptr && !target->IsDead() && target->IsValidTarget(GEntityList->Player(), R->Range()))
			{
				Vec3 pos = target->ServerPosition();
				Vec3* prePos = &pos;

				if (GEntityList->Player()->GetSpellBook()->IsChanneling())
				{
				
					//GGame->PrintChat("hello");
					Slot = kSlotR;
					Position = prePos;
					ReleaseCast = false;
					TriggerEvent = false;

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
				auto dmgW = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
				if (target != nullptr && !target->IsDead() && target->IsValidTarget())
				{
					if (KSQ->Enabled() && Q->IsReady() && target->IsValidTarget(GEntityList->Player(), Q->Range()) && dmgQ >= target->GetHealth())
					{
						if (VelkozQMissile == nullptr)
						{
							auto target = SelectTarget(SpellDamage, Q->Range());
							if (target != nullptr)
							{
								Q->CastOnTarget(target);
							}
							if (target != nullptr && ComboQ2->Enabled())
							{
								Vec3 giua = (target->GetPosition() + GEntityList->Player()->GetPosition()) / 2;
								for (int i = 90; i >= -90; i = i - 5)
								{
									Vec3 explode = RotateAround(target->GetPosition(), giua, i);
									if (Distance(explode, GEntityList->Player()->GetPosition()) <= 1100 && Distance(explode, target->GetPosition()) <= Q2->Range()
										&& Distance(explode, GEntityList->Player()->GetPosition()) >= 300
										&& MalachitePredGetCollisions(GEntityList->Player()->GetPosition(), explode, Q->GetCollisionFlags(), target, Q).size() == 0
										&& MalachitePredGetCollisions(explode, target->GetPosition(), Q3->GetCollisionFlags(), target, Q3).size() == 0)

									{
										Q->CastOnPosition(explode);
										break;
									}
								}
							}

						}
					}

					if (KSW->Enabled() && W->IsReady() && target->IsValidTarget(GEntityList->Player(), W->Range()) && dmgW >= target->GetHealth())
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
						if (InSpellRange(W, pred))
						{
							W->CastOnPosition(pred);
						}
					}
					if (KSE->Enabled() && E->IsReady() && target->IsValidTarget(GEntityList->Player(), E->Range()) && dmgE >= target->GetHealth())
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(target, 0.15f, true, pred);
						if (InSpellRange(E, pred))
						{
							E->CastOnPosition(pred);
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
		if (DrawQFRange->Enabled())
		{
			for (auto hero : GEntityList->GetAllHeros(false, true))
			{
				if (hero->IsValidTarget(GEntityList->Player(), R->Range()) && !hero->IsDead() && hero != nullptr)
				{
					if (VelkozGetRDamage(hero) >= hero->GetHealth())
					{
						GRender->DrawOutlinedCircle(GGame->CursorPosition(), Red(), 400);
					}
				}
			}
		}
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), R->Range()); }
		for (auto hero : GEntityList->GetAllHeros(false, true))
		{
			Vec2 barPos = Vec2();
			if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
			{
				//1483
				auto damage = VelkozGetRDamage(hero);
				float percentHealthAfterDamage = max(0, hero->GetHealth() - float(damage)) / hero->GetMaxHealth();
				float yPos = barPos.y + yOffset;
				float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
				float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());
				if (!hero->IsDead() && hero->IsValidTarget())
				{
					float differenceInHP = xPosCurrentHp - xPosDamage;
					float pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

					if (DrawEmin->Enabled())
					{
						for (int i = 0; i < differenceInHP; i++)
						{
							GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
						}
					}
					if (DrawKillable->Enabled())
					{
						if (damage >= hero->GetHealth())
						{
							GRender->DrawTextW(Vec2(barPos.x + xOffset - 30, barPos.y + yOffset + 20), Color2, "R CAN KILL");
						}
					}
				}
			}
		}
	}
};
