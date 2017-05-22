#include "PluginSDK.h"
#include <string>
#include "MalachitePred.h"
PluginSetup("Xerath by Kornis")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenu* ComboR;
IMenuOption* ComboRAut;
IMenuOption* ComboRTap;
IMenuOption* ComboRradi;
IMenuOption* ComboRrad;
IMenuOption* ComboRTapa;
IMenu* RCustom;
IMenuOption* DelEn;
IMenuOption* R1;
IMenuOption* R2;
IMenuOption* R3;
IMenuOption* R4;
IMenuOption* R5;
IMenuOption* SemiE;
IMenuOption* OrbUse;


IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassE;
IMenuOption* HarassMana;
IMenu* Tip;
IMenuOption* ForceQ;
IMenuOption* SemiQkey;



IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;
IMenuOption* DrawWRange;
IMenuOption* DrawDamage;

IMenu* MiscMenu;
IMenuOption* AntiGap;
IMenuOption* IntSpells;
IMenuOption* AutoELogica;
IMenu* FarmMenu;
IMenuOption* FarmMana;
IMenuOption* FarmQ;
IMenuOption* FarmW;
IMenuOption* FarmQmin;
IMenuOption* FarmWmin;

IMenuOption* pingonks;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

IUnit* Player;

IUnit* rTargetLast;
IUnit* Enemy;
IUnit* enemy;
IUnit* minion;
IUnit* target;
IInventoryItem* orb;

int lastq;
int laste;
int lastwcast = 0;
int lastw = 0;
int lastqcast;
int LastPingTime2 = 0.f;

int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(25, 255, 0, 200);

int helalmoney;

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, false, kCollidesWithNothing);
	Q->SetSkillshot(0.6f, 100.f, 1000000000000000000.f, 1550.f);
	Q->SetCharged(700, 1440, 1.5);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, true, false, kCollidesWithNothing);
	W->SetSkillshot(0.2f, 100, FLT_MAX, 1100);
	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall | kCollidesWithMinions));
	E->SetSkillshot(0.25f, 70, 2000, 1050);
	R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, false, kCollidesWithNothing);
	R->SetOverrideDelay(0.4);
	R->SetOverrideRadius(130);
	R->SetOverrideSpeed(std::numeric_limits<float>::infinity());
}
void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Xerath by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		ComboR = MainMenu->AddMenu("R Settings");
		ComboRAut = ComboR->CheckBox("Auto use charges", true);
		ComboRTap = ComboR->CheckBox("Use charges on Tap", false);
		ComboRTapa = ComboR->AddKey("Tap key:", 'T');
		ComboRrad = ComboR->CheckBox("Aim in Circle", true);
		ComboRradi = ComboR->AddFloat("Circle Size", 0, 1500, 500);
		pingonks = ComboR->CheckBox("Ping on Killable with R", true);
		RCustom = ComboR->AddMenu("R Custom Delays");
		DelEn = RCustom->CheckBox("Enable Custom Delays", false);
		R1 = RCustom->AddFloat("R1 Delay: ", 0, 1500, 300);
		R2 = RCustom->AddFloat("R2 Delay: ", 0, 1500, 300);
		R3 = RCustom->AddFloat("R3 Delay: ", 0, 1500, 300);
		R4 = RCustom->AddFloat("R4 Delays: ", 0, 1500, 300);
		R5 = RCustom->AddFloat("R5 Delay: ", 0, 1500, 300);



	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQ = HarassMenu->CheckBox("Use Q", true);
		HarassMana = HarassMenu->AddInteger("Mana Percent for harass", 10, 100, 50);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawDamage = DrawingMenu->CheckBox("Draw R Damage", true);
	}

	MiscMenu = MainMenu->AddMenu("Misc.");
	{
		AntiGap = MiscMenu->CheckBox("AntiGapclose E", true);
		IntSpells = MiscMenu->CheckBox("Interrupt spells with E", true);
		AutoELogica = MiscMenu->CheckBox("Use E on CC", true);
		SemiE = MiscMenu->AddKey("Semi E", 'Z');

	}
	FarmMenu = MainMenu->AddMenu("Farm.");
	{
		FarmMana = FarmMenu->AddInteger("Mana Percent", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Use Q ", true);
		FarmW = FarmMenu->CheckBox("Use W", true);
		FarmWmin = FarmMenu->AddInteger("Min minions for W", 1, 6, 3);

	}
	Tip = MainMenu->AddMenu("Semi Q");
	{
		SemiQkey = Tip->AddKey("Semi Q Key", 'G');
		ForceQ = Tip->CheckBox("Use Semi Q", true);
	}
}
void Semi()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (!Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && !Enemy->IsDead())
			{
				AdvPredictionOutput outputfam;
				E->RunPrediction(Enemy, false, kCollidesWithMinions, &outputfam);
				if (outputfam.HitChance >= kHitChanceHigh)
				{
					E->CastOnTarget(Enemy);
				}
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
			auto RDamage = 0;
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotR) == 1)
			{
				RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR) * 3;
			}
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotR) == 2)
			{
				RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR) * 4;
			}
			if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotR) == 3)
			{
				RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR) * 5;
			}
			auto damage = RDamage;
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
void PingKS()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (!Enemy->IsDead() && Enemy->IsValidTarget())
		{
			if (pingonks->Enabled())
			{
				auto RDamage = 0;
				if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotR) == 1)
				{
					RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR) * 3;
				}
				if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotR) == 2)
				{
					RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR) * 4;
				}
				if (GEntityList->Player()->GetSpellBook()->GetLevel(kSlotR) == 3)
				{
					RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR) * 5;
				}
				if (R->IsReady())
				{
					if (Enemy->GetHealth() < RDamage)
					{
						if (GGame->Time() - LastPingTime2 >= 3)
						{
							GGame->ShowPing(2, Enemy->GetPosition(), true);
							LastPingTime2 = GGame->Time();

						}


					}
				}
			}
		}
	}
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
	for (auto Minion : GEntityList->GetAllMinions(false, true, false))
	{
		if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
		{
			Vec3 pos;
			int hit;
			if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget())
			{
				if (Player->HasBuff("XerathArcanopulseChargeUp"))
				{
					if (Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						if (Q->GetChargePercent() > 30)
						{
							if ((GetMinionsQ(800) <= 7))
							{
								GPrediction->FindBestCastPosition(Q->Range(), 150, true, true, false, pos, hit);
								{

									Q->CastOnPosition(pos);

								}
							}
							if ((GetMinionsQ(800) >= 7))
							{
								Q->CastOnTarget(Minion);
							}

						}
					}
				}
				if (FarmMana->GetInteger() <= GEntityList->Player()->ManaPercent())
				{
					if (!Player->HasBuff("XerathArcanopulseChargeUp"))
					{
						if (Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
						{
							if (GetMinionsQ(Q->Range()) > 1)
							{
								if ((GetMinionsQ(800) <= 7))
								{
									GPrediction->FindBestCastPosition(Q->Range(), 150, true, true, false, pos, hit);
									{

										Q->StartCharging();

									}
								}
								if ((GetMinionsQ(800) >= 7))
								{
									Q->StartCharging();
								}
							}
						}
					}
					if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
					{

						Vec3 pos;
						int hit;
						if (GetMinionsQ(W->Range()) < 7 && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
						{
							GPrediction->FindBestCastPosition(W->Range(), 200, false, true, false, pos, hit);
							W->CastOnPosition(pos);
						}
						if (GetMinionsQ(W->Range()) > 7 && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
						{
							W->CastOnUnit(Minion);
						}

					}
				}

			}
		}
	}
}
int GetEnemiesInRange(float range)
{
	auto enemies = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto enemy : enemies)
	{
		if (enemy != nullptr && enemy->GetTeam() != GEntityList->Player()->GetTeam())
		{
			auto flDistance = (enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D();
			if (flDistance < range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}


bool CastingR()
{
	if (GEntityList->Player()->HasBuff("XerathLocusOfPower2") || GEntityList->Player()->HasBuff("XerathLocusPulse"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void RCastAuto()
{
	if (!DelEn->Enabled())
	{
		if (ComboRAut->Enabled()) {

			if (!ComboRrad->Enabled())
			{
				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 1)
				{
					if (CastingR() == true)
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							MalachiteCast(R, target, kHitChanceHigh);


						}
					}
				}


				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 2)
				{

					if (CastingR() == true)
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 4400);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							MalachiteCast(R, target, kHitChanceHigh);
						}
					}
				}
				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
				{
					if (CastingR() == true)
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 5600);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							MalachiteCast(R, target, kHitChanceHigh);
							
						}
						
					}
				}
			}
			if (ComboRrad->Enabled())
			{
				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 1)
				{
					if (CastingR() == true)
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{


							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}


						}
					}
				}


				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 2)
				{

					if (CastingR() == true)
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 4400);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
	
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							
							}
						}
					}
				}
				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
				{
					if (CastingR() == true)
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 5600);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{

							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
								
							}
						}
					}
				}
			}
		}
	}
	if (DelEn->Enabled())
	{
		if (ComboRAut->Enabled())
		{
			if (!ComboRrad->Enabled())
			{
				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 1)
				{
					if (CastingR() == true)
					{
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 3)
						{
							if (GGame->TickCount() - helalmoney > R1->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);

								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 2)
						{
							if (GGame->TickCount() - helalmoney > R2->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 1)
						{
							if (GGame->TickCount() - helalmoney > R3->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);


								}

							}
						}
					}
				}


				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 2)
				{

					if (CastingR() == true)
					{
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 4)
						{
							if (GGame->TickCount() - helalmoney > R1->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);

								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 3)
						{
							if (GGame->TickCount() - helalmoney > R2->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 2)
						{
							if (GGame->TickCount() - helalmoney > R3->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);

								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 1)
						{
							if (GGame->TickCount() - helalmoney > R4->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);


								}

							}
						}
					}
				}
				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
				{
					if (CastingR() == true)
					{
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 5)
						{
							if (GGame->TickCount() - helalmoney > R1->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 4)
						{
							if (GGame->TickCount() - helalmoney > R2->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 3)
						{
							if (GGame->TickCount() - helalmoney > R3->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 2)
						{
							if (GGame->TickCount() - helalmoney > R4->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 1)
						{
							if (GGame->TickCount() - helalmoney > R5->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									MalachiteCast(R, target, kHitChanceHigh);

								}

							}
						}
					}
				}
			}
			if (ComboRrad->Enabled())
			{
				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 1)
				{
					if (CastingR() == true)
					{
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 3)
						{
							if (GGame->TickCount() - helalmoney > R1->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 2)
						{
							if (GGame->TickCount() - helalmoney > R2->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 1)
						{
							if (GGame->TickCount() - helalmoney > R3->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
					}
				}


				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 2)
				{

					if (CastingR() == true)
					{
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 4)
						{
							if (GGame->TickCount() - helalmoney > R1->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 3)
						{
							if (GGame->TickCount() - helalmoney > R2->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 2)
						{
							if (GGame->TickCount() - helalmoney > R3->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 1)
						{
							if (GGame->TickCount() - helalmoney > R4->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}

								}

							}
						}
					}
				}
				if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
				{
					if (CastingR() == true)
					{
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 5)
						{
							if (GGame->TickCount() - helalmoney > R1->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 4)
						{
							if (GGame->TickCount() - helalmoney > R2->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 3)
						{
							if (GGame->TickCount() - helalmoney > R3->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 2)
						{
							if (GGame->TickCount() - helalmoney > R4->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
									}


								}

							}
						}
						if (GEntityList->Player()->GetBuffCount("xerathrshots") == 1)
						{
							if (GGame->TickCount() - helalmoney > R5->GetFloat())
							{

								auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
								if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
								{
									if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
									{
										MalachiteCast(R, target, kHitChanceHigh);
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
void RCastTapo()
{
	if (!DelEn->Enabled())
	{
		if (!ComboRrad->Enabled())
		{
			if (GEntityList->Player()->GetSpellLevel(kSlotR) == 1)
			{
				if (CastingR() == true)
				{
					if (ComboRTap->Enabled())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							MalachiteCast(R, target, kHitChanceHigh);
						}
					}
				}
			}
			if (GEntityList->Player()->GetSpellLevel(kSlotR) == 2)
			{
				if (CastingR() == true)
				{
					if (ComboRTap->Enabled())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 4400);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							MalachiteCast(R, target, kHitChanceHigh);
						}
					}
				}
			}
			if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
			{
				if (CastingR() == true)
				{
					if (ComboRTap->Enabled())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 5600);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							MalachiteCast(R, target, kHitChanceHigh);
						}
					}
				}
			}

		}
		if (ComboRrad->Enabled())
		{
			if (GEntityList->Player()->GetSpellLevel(kSlotR) == 1)
			{
				if (CastingR() == true)
				{
					if (ComboRTap->Enabled())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}
						}
					}
				}
			}
			if (GEntityList->Player()->GetSpellLevel(kSlotR) == 2)
			{
				if (CastingR() == true)
				{
					if (ComboRTap->Enabled())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 4400);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}
						}
					}
				}
			}
			if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
			{
				if (CastingR() == true)
				{
					if (ComboRTap->Enabled())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 5600);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}
						}
					}
				}
			}

		}
	}
	if (DelEn->Enabled())
	{
		if (GEntityList->Player()->GetSpellLevel(kSlotR) == 1)
		{
			if (CastingR() == true)
			{
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 3)
				{
					if (GGame->TickCount() - helalmoney > R1->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}


						}

					}
				}
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 2)
				{
					if (GGame->TickCount() - helalmoney > R2->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}


						}

					}
				}
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 1)
				{
					if (GGame->TickCount() - helalmoney > R3->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}


						}

					}
				}
			}
		}


		if (GEntityList->Player()->GetSpellLevel(kSlotR) == 2)
		{

			if (CastingR() == true)
			{
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 4)
				{
					if (GGame->TickCount() - helalmoney > R1->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}


						}

					}
				}
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 3)
				{
					if (GGame->TickCount() - helalmoney > R2->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}


						}

					}
				}
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 2)
				{
					if (GGame->TickCount() - helalmoney > R3->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}


						}

					}
				}
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 1)
				{
					if (GGame->TickCount() - helalmoney > R4->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}


						}

					}
				}
			}
		}
		if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
		{
			if (CastingR() == true)
			{
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 5)
				{
					if (GGame->TickCount() - helalmoney > R1->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}

						}

					}
				}
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 4)
				{
					if (GGame->TickCount() - helalmoney > R2->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							MalachiteCast(R, target, kHitChanceHigh);


						}

					}
				}
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 3)
				{
					if (GGame->TickCount() - helalmoney > R3->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}

						}

					}
				}
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 2)
				{
					if (GGame->TickCount() - helalmoney > R4->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}


						}

					}
				}
				if (GEntityList->Player()->GetBuffCount("xerathrshots") == 1)
				{
					if (GGame->TickCount() - helalmoney > R5->GetFloat())
					{

						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 3200);
						if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
						{
							if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < ComboRradi->GetFloat())
							{
								MalachiteCast(R, target, kHitChanceHigh);
							}

						}

					}
				}
			}
		}
	}
}


void ForcingQ()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		if (ForceQ->Enabled())
		{
		GGame->IssueOrderEx(Player, kMoveTo, GGame->CursorPosition(), false);
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead() && !target->IsDashing())
			{
				if (Player->HasBuff("XerathArcanopulseChargeUp"))
				{
					if (target->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
							Vec3 EstimatedEnemyPos;
							GPrediction->GetFutureUnitPosition(target, 0.15f, true, EstimatedEnemyPos);
							Q->CastOnPosition(EstimatedEnemyPos);
							
					}
				}
				if (!Player->HasBuff("XerathArcanopulseChargeUp"))
				{
					if (Q->IsReady())
					{
						Q->StartCharging();
					}
				}
			}
		}
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

void Combo()
{

	/*GGame->PrintChat("Delay");
	std::string s = std::to_string(E->GetDelay());
	char const *pchar = s.c_str();
	GGame->PrintChat(pchar);
	GGame->PrintChat("Speed");
	std::string sa = std::to_string(E->Speed());
	char const *pchara = sa.c_str();
	GGame->PrintChat(pchara);
	GGame->PrintChat("Radius");
	std::string sb = std::to_string(E->Radius());
	char const *pcharb = sb.c_str();
	GGame->PrintChat(pcharb);
	GGame->PrintChat("Range");
	*/
	if (ComboQ->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (!CanMove(GEntityList->Player()) && target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead() && !target->IsDashing())
		{
			if (Player->HasBuff("XerathArcanopulseChargeUp"))
			{
				lastqcast = 0;
				if (target->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Vec3 EstimatedEnemyPos;
					GPrediction->GetFutureUnitPosition(target, 0.15f, true, EstimatedEnemyPos);
					Q->CastOnPosition(EstimatedEnemyPos);
					lastqcast = 0;
				}

			}
			if (!Player->HasBuff("XerathArcanopulseChargeUp"))
			{
				if (target->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->StartCharging();
				}
			}
		}
	}
	if (ComboE->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			if (E->IsReady())
			{
				AdvPredictionOutput outputfam;
				E->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
				if (outputfam.HitChance >= kHitChanceHigh)
				{
					E->CastOnTarget(target, kHitChanceHigh);
				}
			}
				//Vec3 EstimatedEnemyPos;
				//GPrediction->GetFutureUnitPosition(target, 0.2f, true, EstimatedEnemyPos);
				//GPrediction->SimulateMissile(GEntityList->Player()->ServerPosition(), target, 100000000000, 60.f, 1050, 0.25f, kCollidesWithMinions, EstimatedEnemyPos);
				//E->CastOnPosition(EstimatedEnemyPos);

			
		}
	}
	if (ComboW->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			if (W->IsReady() && W->Range())
			{
				Vec3 EstimatedEnemyPos;
				GPrediction->GetFutureUnitPosition(target, 0.13f, true, EstimatedEnemyPos);
				W->CastOnPosition(EstimatedEnemyPos);

			}
		}
	}
		
}

void Mixed()
{
	if (HarassQ->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			if (Player->HasBuff("XerathArcanopulseChargeUp"))
			{
				if (target->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
						Vec3 EstimatedEnemyPos;
						GPrediction->GetFutureUnitPosition(target, 0.15f, true, EstimatedEnemyPos);
						Q->CastOnPosition(EstimatedEnemyPos);
				}
			}
			if (!Player->HasBuff("XerathArcanopulseChargeUp"))
			{
				if (Q->IsReady())
				{
					Q->StartCharging();
				}
			}
		}
	}
}


int GetMinionsInRange(float range)
{
	auto minions = GEntityList->GetAllMinions(false, true, false);
	auto minionsinrange = 0;

	for (auto minion : minions)
	{
		if (minion != nullptr && minion->GetTeam() != GEntityList->Player()->GetTeam())
		{
			auto flDistance = (minion->GetPosition() - GEntityList->Player()->GetPosition()).Length();
			if (flDistance < range)
			{
				minionsinrange++;
			}
		}
	}
	return minionsinrange;
}

void AutoELogic()
{
	if (E->IsReady() && AutoELogica->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
		if (target != nullptr)
		{
			if (CanMove(target))
			{
				E->CastOnTarget(target, kHitChanceVeryHigh);
			}


		}
	}

}

PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{
	if (Args.Source != nullptr && Args.Source != GEntityList->Player()
		&& Args.Source->IsEnemy(GEntityList->Player())
		&& AntiGap->Enabled() && E->IsReady() && Args.Source->IsValidTarget())
	{
		if ((Args.EndPosition - GEntityList->Player()->GetPosition()).Length() < E->Range())
		{
			E->CastOnPosition(Args.EndPosition);
		}

	}
}

PLUGIN_EVENT(void) OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		Farm();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
	{
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Mixed();
	}
	if (GUtility->IsKeyDown(ComboRTapa->GetInteger()))
	{
		RCastTapo();
	}
	if (GUtility->IsKeyDown(SemiQkey->GetInteger()))
	{
		ForcingQ();
	}
	if (GUtility->IsKeyDown(SemiE->GetInteger()))
	{
		Semi();
	}
	RCastAuto();
	AutoELogic();
	PingKS();
}


PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)
{
	if (Args.Source != nullptr && IntSpells->Enabled() && (Args.Source->GetPosition() - GEntityList->Player()->GetPosition()).Length() < E->Range() && E->IsReady() && Args.Source->IsValidTarget())
	{
		E->CastOnTarget(Args.Source);
	}
}

PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (GEntityList->Player()->GetSpellLevel(kSlotR) == 1)
	{
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), 3200); }
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotR) == 2)
	{
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), 4400); }
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
	{
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), 5600); }
	}
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	if (DrawDamage->Enabled())
	{
		dmgdraw();
	}
	if (GEntityList->Player()->HasBuff("XerathLocusOfPower2") || GEntityList->Player()->HasBuff("XerathLocusPulse"))
	{
		GRender->DrawOutlinedCircle(GGame->CursorPosition(), Vec4(255, 0, 0, 255), ComboRradi->GetFloat());

	}
}

PLUGIN_EVENT(void) OnSpellCast(CastedSpell const& Args)
{
	if (Args.Caster_ == GEntityList->Player())
	{

		if (std::string(Args.Name_) == "XerathRMissileWrapper")
		{
			helalmoney = GGame->TickCount();

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
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Xerath<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#7FFF00\"> - Loaded</font></b>");
	GGame->PrintChat("<b><font color=\"#f8a101\">Version: <b><font color=\"#FFFFFF\">0.2</font></b>");
	GEventManager->AddEventHandler(kEventOnSpellCast, OnSpellCast);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnSpellCast);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Xerath<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#FF0000\"> - Unloaded</font></b>");
}