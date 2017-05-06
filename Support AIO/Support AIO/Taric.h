#pragma once
#include "Menu.h"

class TaricBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Taric - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboEally = ComboMenu->CheckBox("Use E from Ally", true);
			ComboEprio = ComboMenu->CheckBox("Priority E on myself", false);

		}
		WSet = MainMenu->AddMenu("Shielding");
		{
			ShieldE = WSet->CheckBox("Auto W", true);
			ShieldHP = WSet->AddFloat("Auto Shield if X HP", 1, 100, 50);
			ShieldHPinc = WSet->AddFloat("Auto Shield if Inc. Damage Percent", 1, 100, 10);
			ShieldInc = WSet->CheckBox("Auto Shield only Incoming Damage(sometimes not accurate)", true);
			ShieldKill = WSet->CheckBox("Auto Shield if inc. damage will kill", true);
			ShieldAA = WSet->CheckBox("Priority Allies over me", true);
		}
		QSet = MainMenu->AddMenu("Healing");
		{
			ManaTear = QSet->AddFloat("Mana Percent", 1, 100, 20);
			ComboQ = QSet->CheckBox("Auto Q", true);
			HealQ = QSet->AddFloat("Q if Ally Health Percent <", 10, 100, 80);
			HealQme = QSet->AddFloat("Q if My Health Percent <", 10, 100, 60);
			HealQally = QSet->CheckBox("Q on Ally", true);
		}
			DrawingMenu = MainMenu->AddMenu("Drawings");
		
			{
				DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
				DrawWRangemax = DrawingMenu->CheckBox("Draw W Max Range", true);
				DrawWRangemin = DrawingMenu->CheckBox("Draw W Range", true);
				DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
				DrawQFRange = DrawingMenu->CheckBox("Draw Ranges on Ally if W", true);


			}

		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			InterruptE = MiscMenu->CheckBox("E Interrupt", true);
			AntiGapE = MiscMenu->CheckBox("E AntiGap", true);
			AutoE = MiscMenu->CheckBox("Auto E on CCd", true);
		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, (kCollidesWithNothing));
		Q->SetSkillshot(0.25f, 325.f, 3200.f, 325.f);

		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, (kCollidesWithNothing));
		W->SetSkillshot(0.25f, FLT_MAX, 3200.f, 800.f);

		E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, false, false, (kCollidesWithNothing));
		E->SetSkillshot(0.25f, 100.f, 100000000000.f, 620.f);


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

	void AutoELogic()
	{
		if (E->IsReady() && AutoE->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 1280 + E->Range());
			if (target != nullptr)
			{
				if (CanMove(target))
				{
					if (target->IsValidTarget() && !target->IsDead() && target != nullptr)
					{

						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
						{
							if (target != nullptr && target->IsValidTarget() && !target->IsDead())
							{
								E->CastOnTarget(target);
							}
						}


						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > E->Range())
						{
							for (auto Ally : GEntityList->GetAllHeros(true, false))
							{
								if (Ally->HasBuff("taricwallybuff") && !Ally->IsDead() && Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), 1280) && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 1280 + E->Range())
								{
									if (target->IsValidTarget(Ally, E->Range()))
									{
										if (target != nullptr && target->IsValidTarget() && !target->IsDead())
										{
											E->CastOnPosition(target->ServerPosition());
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
	void QHeal()
	{
		if (ManaTear->GetFloat() < GEntityList->Player()->ManaPercent())
		{
			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{
				if (ComboQ->Enabled() && Q->IsReady())
				{
					if (!HealQally->Enabled())
					{
						if (Ally == GEntityList->Player())
						{
							if (HealQme->GetFloat() > GEntityList->Player()->HealthPercent() && !GEntityList->Player()->IsRecalling() && !GEntityList->Player()->IsDead())
							{
								Q->CastOnPlayer();
							}
						}
					}
					if (HealQally->Enabled())
					{
						if (!Ally->HasBuff("taricwallybuff"))
						{
							if (Ally != GEntityList->Player())
							{
								if (HealQ->GetFloat() >= Ally->HealthPercent() && !GEntityList->Player()->IsRecalling() && !GEntityList->Player()->IsDead() && !Ally->IsRecalling() && !Ally->IsDead() && Ally->IsValidTarget(GEntityList->Player(), Q->Range()))
								{
									Q->CastOnPlayer();
								}
							}
						}
						if (Ally->HasBuff("taricwallybuff"))
						{
							if (Ally != GEntityList->Player())
							{
								if (HealQ->GetFloat() >= Ally->HealthPercent() && !GEntityList->Player()->IsRecalling() && !GEntityList->Player()->IsDead() && !Ally->IsRecalling() && !Ally->IsDead() && Ally->IsValidTarget(Ally, Q->Range()) && GetAlliesInRange(GEntityList->Player(), 1280) > 0)
								{
									Q->CastOnPlayer();
								}
							}
						}
						if (HealQme->GetFloat() >= GEntityList->Player()->HealthPercent() && !GEntityList->Player()->IsRecalling() && !GEntityList->Player()->IsDead())
						{
							if (Ally == GEntityList->Player())
							{
								Q->CastOnPlayer();
							}
						}
					}

				}
			}
		}
	}
	void Combo()
	{
		if (ComboE->Enabled() && E->IsReady())
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				if (ComboEprio->Enabled())
				{

					if (Enemy->IsValidTarget() && !Enemy->IsDead() && Enemy != nullptr)
					{

						if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
						{

							E->CastOnTarget(Enemy);

						}


						if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > E->Range())
						{
							for (auto Ally : GEntityList->GetAllHeros(true, false))
							{
								if (Ally->HasBuff("taricwallybuff") && !Ally->IsDead() && Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), 1280) && (Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 1280 + E->Range())
								{
									if (Enemy->IsValidTarget(Ally, E->Range()))
									{

										E->CastOnPosition(Enemy->ServerPosition());

									}
								}
							}
						}
					}
				}

				if (!ComboEally->Enabled())
				{
					if (Enemy->IsValidTarget() && Enemy->IsValidTarget(GEntityList->Player(), 1280 + E->Range()) && !Enemy->IsDead() && Enemy != nullptr)
					{

						if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
						{
							if (Enemy != nullptr && Enemy->IsValidTarget() && !Enemy->IsDead())
							{
								E->CastOnTarget(Enemy);
							}
						}
					}
				}

				if (ComboEally->Enabled())
				{
					for (auto Ally : GEntityList->GetAllHeros(true, false))
					{
						if (Enemy->IsValidTarget() && !Enemy->IsDead() && Enemy != nullptr &&  Enemy->IsValidTarget(GEntityList->Player(), 1280 + E->Range()))
						{
							if (GetAlliesInRange(GEntityList->Player(), 1280 + E->Range() == 0) || !Ally->HasBuff("taricwallybuff"))
							{
								if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
								{
									if (Enemy != nullptr && Enemy->IsValidTarget() && !Enemy->IsDead())
									{
										E->CastOnTarget(Enemy);
									}
								}
							}
							if (Ally->IsValidTarget(GEntityList->Player(), 1280) && !Enemy->IsValidTarget(Ally, E->Range()))
							{



								if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
								{
									if (Enemy != nullptr && Enemy->IsValidTarget() && !Enemy->IsDead())
									{
										E->CastOnTarget(Enemy);
									}
								}
							}

							if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > E->Range())
							{
								if (Ally->HasBuff("taricwallybuff") && !Ally->IsDead() && Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), 1280) && (Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 1280 + E->Range())
								{
									if (Enemy->IsValidTarget(Ally, E->Range()))
									{
										if (Enemy != nullptr && Enemy->IsValidTarget() && !Enemy->IsDead())
										{
											E->CastOnPosition(Enemy->ServerPosition());
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
										if (tracker.getIncomingDamage(ally) >= ally->GetHealth() && GEntityList->Player()->IsValidTarget(ally, W->Range()))
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
								if (tracker.getIncomingDamage(ally) >= GEntityList->Player()->GetHealth())
								{
									W->CastOnUnit(ally);
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
							if (tracker.getIncomingDamage(ally) >= GEntityList->Player()->GetHealth() && GEntityList->Player()->IsValidTarget(ally, W->Range()))
							{
								W->CastOnUnit(ally);
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
	void Interrupt(InterruptibleSpell const& Args)
	{
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, E->Range() + 1280) && InterruptE->Enabled() && E->IsReady())
		{
			if (Args.Target->IsValidTarget() && !Args.Target->IsDead() && Args.Target != nullptr)
			{

				if ((Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
				{
					if (Args.Target != nullptr && Args.Target->IsValidTarget() && !Args.Target->IsDead())
					{
						E->CastOnTarget(Args.Target);
					}
				}


				if ((Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > E->Range())
				{
					for (auto Ally : GEntityList->GetAllHeros(true, false))
					{
						if (Ally->HasBuff("taricwallybuff") && !Ally->IsDead() && Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), 1280) && (Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 1280 + E->Range())
						{
							if (Args.Target->IsValidTarget(Ally, E->Range()))
							{
								if (Args.Target != nullptr && Args.Target->IsValidTarget() && !Args.Target->IsDead())
								{
									E->CastOnPosition(Args.Target->ServerPosition());
								}
							}
						}
					}
				}
			}
		}


	}
	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, E->Range() + 1280)
			&& AntiGapE->Enabled() && E->IsReady())
		{
			if (Args.Sender->IsValidTarget() && !Args.Sender->IsDead() && Args.Sender != nullptr)
			{

				if ((Args.Sender->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
				{
					if (Args.Sender != nullptr && Args.Sender->IsValidTarget() && !Args.Sender->IsDead())
					{
						E->CastOnPosition(Args.EndPosition);
					}
				}


				if ((Args.Sender->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > E->Range())
				{
					for (auto Ally : GEntityList->GetAllHeros(true, false))
					{
						if (Ally->HasBuff("taricwallybuff") && !Ally->IsDead() && Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), 1280) && (Args.Sender->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 1280 + E->Range())
						{
							if (Args.Sender->IsValidTarget(Ally, E->Range()))
							{
								if (Args.Sender != nullptr && Args.Sender->IsValidTarget() && !Args.Sender->IsDead())
								{
									E->CastOnPosition(Args.EndPosition);
								}
							}
						}
					}
				}
			}
		}
		
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRangemin->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(145, 255, 255, 255), W->Range()); }
		if (DrawWRangemax->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(145, 255, 255, 255), 1280); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
		if (DrawQFRange->Enabled())
		{
			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{
				if (Ally->IsValidTarget() && Ally != nullptr && !Ally->IsDead() && Ally->IsValidTarget(GEntityList->Player(), 1280))
				{
					if (Ally->HasBuff("taricwallybuff"))
					{
						if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(Ally->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
						if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(Ally->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
					}
				}
			}
		}
	}
};