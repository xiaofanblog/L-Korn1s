#include "PluginSDK.h"
#include "Rembrandt.h"
#include <string>
#include "MalachitePred.h"
PluginSetup("Ahri")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenu* RSet;
IMenuOption* ComboRburst;
IMenuOption* ComboRmode;
IMenuOption* ComboFE;
IMenuOption* ComboFERange;
IMenuOption* ComboR;
IMenuOption* ComboRrange;
IMenuOption* ComboRkill;
IMenuOption* ComboRdive;



IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassE;
IMenuOption* HarassW;
IMenuOption* HarassMana;



IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawWRange;
IMenuOption* DrawRRange;
IMenuOption* DrawDamage;
IMenuOption* DrawEF;
IMenuOption* AutoDash;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmQhit;
IMenuOption* FarmMana;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSE;
IMenuOption* KSR;
IMenu* FleeMenu;
IMenuOption* FleeKey;
IMenu* MiscMenu;
IMenuOption* InterruptE;
IMenuOption* AntiGapE;
IMenuOption* AutoE;
IMenu* BurstMenu;
IMenuOption* BurstKey;
IMenuOption* BurstE;



ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* EF;
ISpell2* R;


ISpell* Flash;


IUnit* Player;
int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(25, 255, 0, 200);

float Keypres;
bool Harassenable;


void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Ahri by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		RSet = ComboMenu->AddMenu("R Settings");
		ComboR = RSet->CheckBox("Enable R", true);
		ComboRkill = RSet->CheckBox("R Only if Killable", true);
		ComboRmode = RSet->AddSelection("R Usage:", 0, { "To Side", "To Mouse", "To Target" });
		ComboRburst = RSet->CheckBox("Auto R Burst Logic if Killable", true);
		ComboRdive = RSet->AddKey("R Under-Turret Toggle", 'T');
		ComboRrange = RSet->AddFloat("Dont Jump in X Enemies", 0, 5, 2);
		ComboFE = ComboMenu->AddKey("E - Flash", 'G');
		ComboFERange = ComboMenu->AddFloat("^- Max Range", 950, 1250, 1100);
	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassMana = HarassMenu->AddInteger("Mana Percent for Harass", 10, 100, 50);
		HarassQ = HarassMenu->CheckBox("Use Q", true);
		HarassW = HarassMenu->CheckBox("Use W", true);
		HarassE = HarassMenu->CheckBox("Use E", true);
	}

	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", false);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", false);
		DrawEF = DrawingMenu->CheckBox("Draw E-Flash Range", true);
		DrawDamage = DrawingMenu->CheckBox("Draw Damage", true);
	}
	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Killsteal with Q", true);
		KSR = KillstealMenu->CheckBox("Killsteal with W", true);
		KSE = KillstealMenu->CheckBox("Killsteal with E", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmMana = FarmMenu->AddInteger("Mana Percent for LaneClear", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Lane Clear with Q", true);
		FarmQhit = FarmMenu->AddFloat("^- if Hits", 0, 6, 2);
	}

	MiscMenu = MainMenu->AddMenu("Misc.");
	{
		InterruptE = MiscMenu->CheckBox("Interrupt with E", true);
		AntiGapE = MiscMenu->CheckBox("Anti GapClose with E", true);
		AutoE = MiscMenu->CheckBox("Auto E on CC", true);
	}
	FleeMenu = MainMenu->AddMenu("Flee");
	{
		FleeKey = FleeMenu->AddKey("Flee with Q", 'Z');
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

void AutoEs()
{
	if (E->IsReady() && AutoE->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
		if (target != nullptr && target->IsValidTarget() && !target->IsDead())
		{
			if (CanMove(target))
			{
				MalachiteCast(E, target, kHitChanceHigh);
			}

		}
	}
}
void LoadSpells()
{
	if (GEntityList->Player()->GetSpellSlot("SummonerFlash") != kSlotUnknown)
		Flash = GPluginSDK->CreateSpell(GEntityList->Player()->GetSpellSlot("SummonerFlash"), 425);
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall));
	Q->SetSkillshot(0.25f, 80.f, 2000.f, 800.f);

	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, true, true, (kCollidesWithYasuoWall));
	W->SetSkillshot(0.f, 0.f, 900.f, 650.f);

	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, (kCollidesWithYasuoWall | kCollidesWithMinions));
	E->SetSkillshot(0.25f, 60.f, 2050.f, 900.f);
	EF = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, (kCollidesWithYasuoWall | kCollidesWithMinions));
	EF->SetSkillshot(0.25f, 60.f, 2050.f, 950+350);

	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, (kCollidesWithNothing));
	R->SetSkillshot(0.f, 600.f, 2200.f, 450.f);
}
PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (Args.Source != nullptr && InterruptE->Enabled() && (Args.Source->GetPosition() - GEntityList->Player()->GetPosition()).Length() < E->Range() && E->IsReady() && Args.Source->IsValidTarget() && Args.Source->IsTargetable())
	{
		E->CastOnTarget(Args.Source);
	
	}
}

PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{
	if (Args.Source != nullptr && Args.Source != GEntityList->Player()
		&& Args.Source->IsEnemy(GEntityList->Player())
		&& AntiGapE->Enabled() && E->IsReady() && Args.Source->IsValidTarget() && (Args.Source->ChampionName() != "Zed" || Args.Source->ChampionName() != "Master Yi"))
	{
		if (Distance(GEntityList->Player(), Args.EndPosition) <= E->Range())
		{
			E->CastOnPosition(Args.EndPosition);
			
		}

	}
}
void HarasTog()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		if (GUtility->IsKeyDown(ComboRdive->GetInteger()))
		{
			if (Harassenable == true && GGame->Time() > Keypres)
			{
				Harassenable = false;
				Keypres = GGame->Time() + 0.250;

			}
			if (Harassenable == false && GGame->Time() > Keypres)
			{
				Harassenable = true;
				Keypres = GGame->Time() + 0.250;

			}

		}
	}
}
void Harass()
{
	if (HarassQ->Enabled() && Q->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && target->IsValidTarget() && !target->IsDead())
		{
			Vec3 pred;
			GPrediction->GetFutureUnitPosition(target, 0.45, false, pred);
			if ((pred - GEntityList->Player()->GetPosition()).Length2D() < Q->Range())
			{
				Q->CastOnPosition(pred);
			}
		}
	}

	if (HarassW->Enabled() && W->IsReady())
	{

		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr && target->IsValidTarget() && !target->IsDead())
		{
			{
				W->CastOnPlayer();
			}
		}
	}
	if (HarassE->Enabled() && E->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
		if (target != nullptr && target->IsValidTarget() && !target->IsDead())
		{
			/*AdvPredictionOutput outputfam;
			E->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
			if (outputfam.HitChance >= kHitChanceHigh &&  outputfam.HitChance != kHitChanceCollision)
			{
			Vec3 pred;
			GPrediction->GetFutureUnitPosition(target, 0.45f, false, pred);
			if ((pred - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
			{
			E->CastOnPosition(pred);
			}
			}*/
			{
				MalachiteCast(E, target, kHitChanceHigh);
			}
		}
	}
}
static double GetW(IUnit* target)
{
	int qdmg = 0;
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 1)
	{
		qdmg = 40;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 2)
	{
		qdmg = 65;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 3)
	{
		qdmg = 90;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 4)
	{
		qdmg = 115;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotW) == 5)
	{
		qdmg = 140;
	}
	auto calc = GEntityList->Player()->TotalMagicDamage() * 0.35;
	auto full = calc + qdmg;
	auto damage = GDamage->CalcMagicDamage(GEntityList->Player(), target, full);
	return damage;

}
void Killsteal()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
		auto WDamage = GetW(Enemy);
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);

		if (Enemy != nullptr && !Enemy->IsDead() & Enemy->IsValidTarget())
		{
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				Vec3 pred;
				GPrediction->GetFutureUnitPosition(Enemy, 0.45, false, pred);
				if ((pred - GEntityList->Player()->GetPosition()).Length2D() < Q->Range())
				{
					Q->CastOnPosition(pred);
				}
			}
			if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
			{
				MalachiteCast(E, Enemy, kHitChanceHigh);

			}
			if (KSR->Enabled() && W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && WDamage > Enemy->GetHealth())
			{
				W->CastOnPlayer();
			}
		}
	}
}
void dmgdraw()
{
	for (auto hero : GEntityList->GetAllHeros(false, true))
	{
		Vec2 barPos = Vec2();
		if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
		{
			auto QDamage = 0;

			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotQ) > 0)
			{
				QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
			}


			auto WDamage = 0;
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotW) > 0)
			{
				WDamage = GetW(hero);
			}
			auto EDamage = 0;
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotE) > 0)
			{
				EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
			}
			auto RDamage = 0;
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotR) > 0)
			{
				RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR);
			}
			auto damage = QDamage + EDamage + RDamage*3 + WDamage;
			float percentHealthAfterDamage = max(0, hero->GetHealth() - float(damage)) / hero->GetMaxHealth();
			float yPos = barPos.y + yOffset;
			float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
			float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());
			if (!hero->IsDead() && hero->IsValidTarget())
			{
				float differenceInHP = xPosCurrentHp - xPosDamage;
				float pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

				for (int i = 0; i < differenceInHP; i++)
				{
					if (damage < hero->GetHealth())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
					}
					if (damage > hero->GetHealth())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), Color2);
					}

				}
				if (!hero->IsVisible())
				{

				}
			}
		}
	}
}
void Combo()
{
	{

		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				Vec3 pred;
				GPrediction->GetFutureUnitPosition(target, 0.45, false, pred);
				if ((pred - GEntityList->Player()->GetPosition()).Length2D() < Q->Range())
				{
					Q->CastOnPosition(pred);
				}
			}
		}

		if (ComboW->Enabled() && W->IsReady())
		{

			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				{
					W->CastOnPlayer();
				}
			}
		}
		if (ComboE->Enabled() && E->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				/*AdvPredictionOutput outputfam;
				E->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
				if (outputfam.HitChance >= kHitChanceHigh &&  outputfam.HitChance != kHitChanceCollision)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(target, 0.45f, false, pred);
					if ((pred - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
					{
						E->CastOnPosition(pred);
					}
				}*/
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
				auto WDamage = GetW(target);
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
				auto damage = QDamage + WDamage + EDamage + RDamage;
				if (ComboRburst->Enabled() && (!R->IsReady() || target->GetHealth() > damage))
				{

					{
						MalachiteCast(E, target, kHitChanceHigh);
					}
				}
				if (!ComboRburst->Enabled())
				{

					{
						MalachiteCast(E, target, kHitChanceHigh);
					}
				}
			}
		}
		if (ComboR->Enabled() && R->IsReady() && E->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				if (ComboRburst->Enabled())
				{
					auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
					auto WDamage = GetW(target);
					auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
					auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
					auto damage = QDamage + WDamage + EDamage + RDamage;
					if (target->GetHealth() < damage)
					{
						if (ComboRrange->GetFloat() > CountEnemiesInRange(target->GetPosition(), 600))
						{
							if (MalachiteCast(E, target, kHitChanceHigh))
							{
								R->CastOnPosition(target->GetPosition());
							}
							if (GEntityList->Player()->HasBuff("AhriTumble"))
							{
								if (ComboRmode->GetInteger() == 0)
								{
									if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 450)
									{

										SArray<Vec3> Poses = GetCircleCircleIntersections(GEntityList->Player()->GetPosition(), target->GetPosition(), 450, 450);
										Poses.OrderBy<float>([&](Vec3 x) {return Distance(x, GGame->CursorPosition()); });
										for (Vec3 position : Poses.ToVector())
										{
											if (Distance(target, position) <= GetAutoAttackRange(target) && !GNavMesh->IsPointWall(position))
											{
												R->CastOnPosition(position.Extend(target->GetPosition(), 150));
											}
										}
									}
									if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 450)
									{
										R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
									}
								}
								if (ComboRmode->GetInteger() == 1)
								{

									if (Harassenable == true)
									{


										if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
										{
											R->CastOnPosition(GGame->CursorPosition());
										}
									}

									if (Harassenable == false)
									{


										R->CastOnPosition(GGame->CursorPosition());


									}
								}
							}

							if (ComboRmode->GetInteger() == 2)
							{

								if (Harassenable == true)
								{

									if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
									{
										R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
									}


								}
								if (Harassenable == false)
								{


									R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));

								}

							}
						}
					}
				}
			}
		}
		if (ComboR->Enabled() && R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range()+500);
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				if (ComboRmode->GetInteger() == 0)
				{
					if (ComboRkill->Enabled())
					{
						auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
						auto WDamage = GetW(target);
						auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
						auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
						auto damage = QDamage + WDamage + EDamage + RDamage * 3;
						if (target->GetHealth() < damage)
						{
							auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range() + 500);
							if (target != nullptr && target->IsValidTarget() && !target->IsDead())
							{
								if (ComboRrange->GetFloat() > CountEnemiesInRange(target->GetPosition(), 600))
								{
									if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 450)
									{

										SArray<Vec3> Poses = GetCircleCircleIntersections(GEntityList->Player()->GetPosition(), target->GetPosition(), 450, 450);
										Poses.OrderBy<float>([&](Vec3 x) {return Distance(x, GGame->CursorPosition()); });
										for (Vec3 position : Poses.ToVector())
										{
											if (Distance(target, position) <= GetAutoAttackRange(target) && !GNavMesh->IsPointWall(position))
											{
												R->CastOnPosition(position.Extend(target->GetPosition(), 150));
											}
										}
									}
									if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 450)
									{
										R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
									}
								}
							}
						}
					}
					if (!ComboRkill->Enabled())
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range()+500);
						if (target != nullptr && target->IsValidTarget() && !target->IsDead())
						{
							if (ComboRrange->GetFloat() > CountEnemiesInRange(target->GetPosition(), 600))
							{
								if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 450)
								{

									SArray<Vec3> Poses = GetCircleCircleIntersections(GEntityList->Player()->GetPosition(), target->GetPosition(), 450, 450);
									Poses.OrderBy<float>([&](Vec3 x) {return Distance(x, GGame->CursorPosition()); });
									for (Vec3 position : Poses.ToVector())
									{
										if (Distance(target, position) <= GetAutoAttackRange(target) && !GNavMesh->IsPointWall(position))
										{
											R->CastOnPosition(position.Extend(GEntityList->Player()->GetPosition(), 150));
										}
									}
								}
								if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > 450)
								{
									R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
								}
							}
						}
					}
				}
				if (ComboRmode->GetInteger() == 1)
				{
					if (ComboRkill->Enabled())
					{
						auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
						auto WDamage = GetW(target);
						auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
						auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
						auto damage = QDamage + WDamage + EDamage + RDamage * 3;
						if (target->GetHealth() < damage)
						{
							if (ComboRrange->GetFloat() > CountEnemiesInRange(target->GetPosition(), 600))
							{
								if (Harassenable == true)
								{
									if (!GEntityList->Player()->HasBuff("AhriTumble"))
									{
										if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
										{
											R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
										}
									}
									if (GEntityList->Player()->HasBuff("AhriTumble"))
									{
										if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
										{
											R->CastOnPosition(GGame->CursorPosition());
										}
									}
								}
								if (Harassenable == false)
								{
									if (!GEntityList->Player()->HasBuff("AhriTumble"))
									{

										R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));

									}
									if (GEntityList->Player()->HasBuff("AhriTumble"))
									{

										R->CastOnPosition(GGame->CursorPosition());

									}

								}
							}
						}
					}
					if (!ComboRkill->Enabled())
					{
						if (Harassenable == true)
						{
							if (ComboRrange->GetFloat() > CountEnemiesInRange(target->GetPosition(), 600))
							{
								if (!GEntityList->Player()->HasBuff("AhriTumble"))
								{
									if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
									{
										R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
									}
								}
								if (GEntityList->Player()->HasBuff("AhriTumble"))
								{
									if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
									{
										R->CastOnPosition(GGame->CursorPosition());
									}

								}
							}
						}
						if (Harassenable == false)
						{
							if (ComboRrange->GetFloat() > CountEnemiesInRange(target->GetPosition(), 600))
							{
								if (!GEntityList->Player()->HasBuff("AhriTumble"))
								{

									R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));

								}
								if (GEntityList->Player()->HasBuff("AhriTumble"))
								{

									R->CastOnPosition(GGame->CursorPosition());

								}
							}
						}
					}

				}
				if (ComboRmode->GetInteger() == 2)
				{
					if (ComboRkill->Enabled())
					{
						auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
						auto WDamage = GetW(target);
						auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
						auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
						auto damage = QDamage + WDamage + EDamage + RDamage * 3;
						if (target->GetHealth() < damage)
						{
							if (ComboRrange->GetFloat() > CountEnemiesInRange(target->GetPosition(), 600))
							{
								if (Harassenable == true)
								{
									if (!GEntityList->Player()->HasBuff("AhriTumble"))
									{
										if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
										{
											
											R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
										}
									}
									if (GEntityList->Player()->HasBuff("AhriTumble"))
									{
										if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
										{
											R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
										}
									}
								}
								if (Harassenable == false)
								{
									if (!GEntityList->Player()->HasBuff("AhriTumble"))
									{
										R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));

									}
									if (GEntityList->Player()->HasBuff("AhriTumble"))
									{

										R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
									}
								}
							}
						}
					}
					if (!ComboRkill->Enabled())
					{
						if (Harassenable == true)
						{
							if (ComboRrange->GetFloat() > CountEnemiesInRange(target->GetPosition(), 600))
							{
								if (!GEntityList->Player()->HasBuff("AhriTumble"))
								{
									if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
									{
										R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
									}
								}
								if (GEntityList->Player()->HasBuff("AhriTumble"))
								{
									if (!GUtility->IsPositionUnderTurret(target->GetPosition(), false, true))
									{
										R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
									}

								}
							}
						}
						if (Harassenable == false)
						{
							if (ComboRrange->GetFloat() > CountEnemiesInRange(target->GetPosition(), 600))
							{
								if (GEntityList->Player()->HasBuff("AhriTumble"))
								{
									R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));

								}
								if (!GEntityList->Player()->HasBuff("AhriTumble"))
								{

									R->CastOnPosition(target->ServerPosition().Extend(GEntityList->Player()->GetPosition(), 200));
								
								}
							}
						}
					}
				}
			}
		}

	}
}
void EFlash()
{
	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		GGame->IssueOrderEx(GEntityList->Player(), kMoveTo, GGame->CursorPosition(), false);
		if (E->IsReady() && Flash != nullptr && Flash->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, ComboFERange->GetFloat());
			if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{

				if (MalachiteCast(EF, target, kHitChanceHigh))
				{
					GPluginSDK->DelayFunctionCall(200, [=]()
					{
						Flash->CastOnPosition(target->ServerPosition());

					});
					
				}
			}
		}
	}
}
void Farm()
{

	if (FarmMana->GetInteger() <= GEntityList->Player()->ManaPercent())
	{
		for (auto Minion : GEntityList->GetAllMinions(false, true, false))
		{
			if (FarmQ->Enabled())
			{
				if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep()))
				{
					std::vector<Vec3> CastPos;
					CastPos.push_back(GEntityList->Player()->GetPosition());
					FarmLocation Farmlocation;
					Rembrandt::FindBestLineCastPosition(CastPos, Q->Range(), Q->Range(), 150, false, true, true, Farmlocation);
					if (FarmQ->Enabled())
					{
						if (Farmlocation.HitCount > FarmQhit->GetFloat())
						{
							Q->CastOnPosition(Farmlocation.CastPosition);
						}
					}
				}
			}
		}
		for (auto Miniona : GEntityList->GetAllMinions(false, false, true))
		{
			if (FarmQ->Enabled())
			{
				if (Miniona->IsEnemy(GEntityList->Player()) && !Miniona->IsDead() && Miniona->IsValidTarget() && Miniona->IsJungleCreep())
				{
					std::vector<Vec3> CastPos;
					CastPos.push_back(GEntityList->Player()->GetPosition());
					FarmLocation Farmlocation;
					Rembrandt::FindBestLineCastPosition(CastPos, Q->Range(), Q->Range(), 150, true, false, true, Farmlocation);
					if (FarmQ->Enabled())
					{
						if (Farmlocation.HitCount > 0)
						{

							Q->CastOnPosition(Farmlocation.CastPosition);

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
		if (Q->IsReady())
		{
			Q->CastOnPosition(GEntityList->Player()->GetPosition().Extend(GGame->CursorPosition(), -500));
		}

	}
}
PLUGIN_EVENT(void) OnGameUpdate()
{
	Killsteal();
	AutoEs();
	HarasTog();
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if (GUtility->IsKeyDown(ComboFE->GetInteger()))
	{
		EFlash();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		Farm();
	}
	if (GUtility->IsKeyDown(FleeKey->GetInteger()))
	{
		Flee();
	}
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 50, 50, 255),  Q->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), R->Range()); }
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), W->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), E->Range()); }
	if (DrawEF->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 50, 50, 255), ComboFERange->GetFloat()); }
	if (DrawDamage->Enabled())
	{
		dmgdraw();
	}
	static IFont* pFont = nullptr;

	if (pFont == nullptr)
	{
		pFont = GRender->CreateFont("Arial", 15.f, kFontWeightHeavy);
		pFont->SetOutline(true);
		pFont->SetLocationFlags(kFontLocationNormal);
	}
	Vec2 pos;
	if (GGame->Projection(GEntityList->Player()->GetPosition(), &pos))
	{
		if (Harassenable == true)
		{
			std::string text = std::string("R Under Turret: OFF");
			Vec4 clr = Vec4(200, 50, 60,255);
			pFont->SetColor(clr);
			pFont->Render(pos.x+20, pos.y, text.c_str());
		}
		if (Harassenable == false)
		{
			std::string text = std::string("R Under Turret: ON");
			Vec4 clr = Vec4(200, 50, 60, 255);
			pFont->SetColor(clr);
			pFont->Render(pos.x+20, pos.y, text.c_str());
		}
	}
}


PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	Player = GEntityList->Player();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Ahri<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#7FFF00\"> - Loaded</font></b>");
	GGame->PrintChat("<b><font color=\"#f8a101\">Version: <b><font color=\"#FFFFFF\">0.1</font></b>");

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Ahri<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#FF0000\"> - Unloaded</font></b>");
}
