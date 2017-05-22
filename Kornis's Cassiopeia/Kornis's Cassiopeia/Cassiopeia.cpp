#include "PluginSDK.h"
#include <string>
#include "SimpleLib.h"

PluginSetup("Kornis Cassiopeia")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboWmax;
IMenuOption* ComboE;
IMenuOption* HarassLast;
IMenuOption* ComboWstart;
IMenu* RMenu;
IMenuOption* ComboQnotpoison;
IMenuOption* ComboRkillable;
IMenuOption* ComboRHealth;
IMenuOption* ComboRRANGE;
IMenuOption* ComboEonlyPoison;
IMenuOption* ComboRMin;
IMenuOption* ComboREnable; 
IMenuOption* ComboRflash;
IMenu* QSettings;
IMenuOption* ComboRylais;
IMenu* WSettings;
IMenu* ESettings;
IMenuOption* SemiR;
IMenuOption* ComboRFacing;
IMenuOption* LastEDelay;

int fixmaybe;

IMenu* HarassMenu;
IMenuOption* HarassMana;
IMenuOption* HarassAuto;
IMenuOption* HarassQ;
IMenuOption* HarassE;
IMenuOption* ComboRcheck;
IMenuOption* KSRCheck;
IMenuOption* HarassEonlyPoison;

IMenu* MiscMenu;
IMenuOption* ComboAA;
IMenuOption* ComboAAkey;
IMenuOption* ComboAALevel;
IMenuOption* DrawFarm;
IMenuOption* DrawPred;

IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* PredMethod;
IMenuOption* DrawWRange;
IMenuOption* DrawERange;
IMenuOption* ComboQdash;
IMenuOption* DrawRRange;
IMenuOption* DrawDamage;
IMenuOption* DrawFill;
IMenuOption* DrawRFlash;
IMenuOption* DrawEkill;
IMenuOption* StackTear;
IMenuOption* StackTearQ;
IMenuOption* StackTearW;

IMenu* FarmMenu;
IMenuOption* FarmManaP;
IMenuOption* FarmManaPA;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmEallow;
IMenuOption* FarmEsmooth;
IMenuOption* FarmELH;
IMenuOption* FarmEpoison;
IMenuOption* FarmQhit;
IMenuOption* FarmETypeChoose;
IMenuOption* Jungle;

IMenu* LastHitMenu;
IMenuOption* LastE;

IMenuOption* FarmKey;
IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSE;
IMenuOption* KSR;
IMenu* FarmPush;
IMenu* FarmPassive;

IInventoryItem* Tear;
IInventoryItem* Staff;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell* Flash;
int delaystuff;

//std::vector<std::string> FarmEType = { "Block AA(Smooth)", "With AA" };

IUnit* Player;
bool Farmenable = true;
float KeyPre;


std::vector<IUnit*> minions;
int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(25, 255, 0, 200);
float delay;

void LoadSpells()
{
	if (GEntityList->Player()->GetSpellSlot("SummonerFlash") != kSlotUnknown)
		Flash = GPluginSDK->CreateSpell(GEntityList->Player()->GetSpellSlot("SummonerFlash"), 425);
	Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, false, false, kCollidesWithNothing);
	Q->SetSkillshot(0.6, 40.f, FLT_MAX, 850.f);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, kCollidesWithNothing);
	W->SetSkillshot(0.25, 75.f, FLT_MAX, 800.f);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithYasuoWall);
	R = GPluginSDK->CreateSpell2(kSlotR, kConeCast, false, true, kCollidesWithNothing);
	Tear = GPluginSDK->CreateItemForId(3070, 0);
	Staff = GPluginSDK->CreateItemForId(3003, 0);

}


void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Cassiopeia");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		QSettings = ComboMenu->AddMenu("Q Settings");
		ComboQ = QSettings->CheckBox("Use Q in Combo", true);
		ComboQnotpoison = QSettings->CheckBox("Only Q if NOT POISONED", false);
		ComboQdash = QSettings->CheckBox("Auto Q on Dash", true);
		WSettings = ComboMenu->AddMenu("W Settings");
		ComboW = WSettings->CheckBox("Use W in Combo", true);
		ComboWstart = WSettings->CheckBox("Start combo with W", true);
		ComboWmax = WSettings->AddFloat("W Max Range", 400, 800, 800);
		ESettings = ComboMenu->AddMenu("E Settings");
		ComboE = ESettings->CheckBox("Use E in Combo", true);
		ComboEonlyPoison = ESettings->CheckBox("Only E if POISONED", false);
		RMenu = ComboMenu->AddMenu("R Settings");
		ComboREnable = RMenu->CheckBox("Use R in Combo", true);
		ComboRkillable = RMenu->CheckBox("^- Only R if Killabe with Combo", true);
		ComboRcheck = RMenu->AddFloat("Dont waste R if Enemy HP lower than", 0, 500, 100);
		ComboRMin = RMenu->AddInteger("R if X enemies", 1, 5, 1);
		ComboRHealth = RMenu->AddInteger("R if Target has Health Percent ", 10, 100, 60);
		ComboRRANGE = RMenu->AddInteger("R Range for usage", 125, 825, 750);
		ComboRFacing = RMenu->CheckBox("Only R if FACING", true);
		ComboRflash = ComboMenu->AddKey("R Flash", 'T');
		ComboRylais = ComboMenu->CheckBox("Rylais Combo(Starts with E)", false);
		SemiR = ComboMenu->AddKey("Semi-Manual R", 'G');
		

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassLast = HarassMenu->CheckBox("Use E to Lasthit", true);
		HarassMana = HarassMenu->AddInteger("Min Mana. for Harass", 10, 100, 50);
		HarassAuto = HarassMenu->CheckBox("Use AUTO Harass", false);
		HarassQ = HarassMenu->CheckBox("Use Q to harass", true);
		HarassE = HarassMenu->CheckBox("Use E to harass", true);
		HarassEonlyPoison = HarassMenu->CheckBox("Only E if POISONED", false);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawDamage = DrawingMenu->CheckBox("Draw killable", true);
		DrawFill = DrawingMenu->CheckBox("Draw damage", true);
		DrawRFlash = DrawingMenu->CheckBox("Draw RFlash range", true);
		DrawEkill = DrawingMenu->CheckBox("Draw Minions Killable With Q", true);
		DrawPred = DrawingMenu->CheckBox("Draw Prediction", true);
		DrawFarm = DrawingMenu->CheckBox("Draw Farm Toggle", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmKey = FarmMenu->AddKey("Mode Change:", 'Z');
		FarmPush = FarmMenu->AddMenu("Pushing Settings");
		FarmManaP = FarmPush->AddInteger("Mana For Push", 10, 100, 20);
		FarmQ = FarmPush->CheckBox("Use Q", true);
		FarmE = FarmPush->CheckBox("Use E", true);
		FarmEpoison = FarmPush->CheckBox("^- Only If Poison", false);
		FarmPassive = FarmMenu->AddMenu("Passive Settings");
		FarmManaPA = FarmPassive->AddInteger("Mana For Passive Farm", 10, 100, 50);
		FarmELH = FarmPassive->CheckBox("Use E", true);
		FarmEsmooth = FarmPassive->CheckBox("^- Block AA(Smooth)", true);
		LastEDelay = FarmPassive->AddFloat("Last Hit E Delay", 0, 1500, 0);
		//Jungle = FarmMenu->CheckBox("Use in Jungle", true);
	}

	LastHitMenu = MainMenu->AddMenu("Last Hit");
	{
		LastE = LastHitMenu->CheckBox("Use E for Last Hit", true);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSE = KillstealMenu->CheckBox("Use E", true);
		KSR = KillstealMenu->CheckBox("Use R", true);
		KSRCheck = KillstealMenu->AddFloat("^- Dont waste R if Enemy HP lower than", 0, 500, 100);

	}
	MiscMenu = MainMenu->AddMenu("Misc.");
	{
		ComboAALevel = MiscMenu->AddInteger("At what level disable AA", 1, 18, 6);
		ComboAA = MiscMenu->CheckBox("Disable AA", true);
		ComboAAkey = MiscMenu->AddKey("Disable key", 32);
		StackTear = MiscMenu->CheckBox("Stack Tear", true);
		StackTearQ = MiscMenu->CheckBox("^- use Q", true);
		StackTearW = MiscMenu->CheckBox("^- use W", true);
		PredMethod = MiscMenu->AddSelection("Pred Method", 1, { "Old", "New" });
	}

}
void FarmTog()
{
	if (GUtility->IsKeyDown(FarmKey->GetInteger()))
	{
		if (Farmenable == true && GGame->Time() > KeyPre)
		{
			Farmenable = false;
			KeyPre = GGame->Time() + 0.250;

		}
		if (Farmenable == false && GGame->Time() > KeyPre)
		{
			Farmenable = true;
			KeyPre = GGame->Time() + 0.250;

		}

	}
}
void Semi()
{

	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (!Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), ComboRRANGE->GetInteger()) && !Enemy->IsDead())
			{
				Vec3 pos;
				int hit;
				GPrediction->FindBestCastPosition(ComboRRANGE->GetInteger(), 290, false, false, true, pos, hit);
				if (hit > 0)
				{
					R->CastOnPosition(pos);
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
			auto flDistance = (enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length();
			if (flDistance < range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}

void dmgdraw()
{
	if (!DrawDamage->Enabled())
		return;
	for (auto hero : GEntityList->GetAllHeros(false, true))
	{
		Vec2 barPos = Vec2();
		if (hero->GetHPBarPosition(barPos) && !hero->IsDead() && hero->IsValidTarget())
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
			auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
			auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR);
			float percentHealthAfterDamage = max(0, hero->GetHealth() - float(RDamage + EDamage*4 + QDamage)) / hero->GetMaxHealth();
			float yPos = barPos.y + yOffset;
			float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
			float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());

			if (RDamage + EDamage*4 + QDamage > hero->GetHealth())
			{
				GRender->DrawTextW(Vec2(barPos.x + xOffset, barPos.y + yOffset - 13), Color, "Killable");
			}
			if (DrawFill->Enabled() && !hero->IsDead())
			{
				float differenceInHP = xPosCurrentHp - xPosDamage;
				float pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);


				for (int i = 0; i < differenceInHP; i++)
				{
					if (RDamage + EDamage * 4 + QDamage < hero->GetHealth())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
					}
					if (RDamage + EDamage * 4 + QDamage > hero->GetHealth())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), Color2);
					}

				}
			}
		}
	}
}

void Combo()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (!Enemy->IsDead())
		{
			auto Qtarget = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			auto Wtarget = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (!ComboWstart->Enabled() && !ComboRylais->Enabled())
			{
				if (!ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
				{
					if (Qtarget != nullptr && Qtarget->IsValidTarget())
					{
						{
							if (PredMethod->GetInteger() == 1)
							{
								AdvPredictionOutput outputfam;
								Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
								if (outputfam.HitChance >= kHitChanceHigh)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
									Q->CastOnPosition(pred);
								}
								else if (outputfam.HitChance == kHitChanceDashing)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
									Q->CastOnTarget(Qtarget, kHitChanceDashing);
								}

							}
							if (PredMethod->GetInteger() == 0)
							{
								AdvPredictionOutput outputfam;
								Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
								if (outputfam.HitChance >= kHitChanceHigh)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
									Q->CastOnPosition(pred);
								}
								else if (outputfam.HitChance == kHitChanceDashing)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
									Q->CastOnTarget(Qtarget, kHitChanceDashing);
								}

							}
						}
					}
				}
				if (ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
				{
					if (Qtarget != nullptr && Qtarget->IsValidTarget())
					{
						if (!Qtarget->HasBuffOfType(BUFF_Poison))
						{
							if (PredMethod->GetInteger() == 1)
							{
								AdvPredictionOutput outputfam;
								Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
								if (outputfam.HitChance >= kHitChanceHigh)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
									Q->CastOnPosition(pred);
								}
								else if (outputfam.HitChance == kHitChanceDashing)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
									Q->CastOnTarget(Qtarget, kHitChanceDashing);
								}

							}
							if (PredMethod->GetInteger() == 0)
							{
								AdvPredictionOutput outputfam;
								Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
								if (outputfam.HitChance >= kHitChanceHigh)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
									Q->CastOnPosition(pred);
								}
								else if (outputfam.HitChance == kHitChanceDashing)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
									Q->CastOnTarget(Qtarget, kHitChanceDashing);
								}

							}
						}
					}
				}
				if (ComboW->Enabled() && W->IsReady() && W->Range())
				{
					if (Wtarget != nullptr && Wtarget->IsValidTarget())
					{
						auto distance = (Player->GetPosition() - Wtarget->GetPosition()).Length();
						if (distance >= 400 && distance <= ComboWmax->GetFloat())
						{
							W->CastOnTarget(Wtarget, kHitChanceHigh);
						}
					}
				}

				if (ComboE->Enabled())
				{
					if (E->IsReady())
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
						if (target != nullptr && target->IsValidTarget())
						{
							if (!(ComboEonlyPoison->Enabled()) || target->HasBuffOfType(BUFF_Poison))
								E->CastOnTarget(target);
						}
					}
				}
			}
			if (ComboWstart->Enabled() && !ComboRylais->Enabled())
			{
				if (ComboW->Enabled() && W->IsReady() && W->Range())
				{
					if (Wtarget != nullptr && Wtarget->IsValidTarget())
					{
						auto distance = (Player->GetPosition() - Wtarget->GetPosition()).Length();
						if (distance >= 400 && distance <= ComboWmax->GetFloat())
						{
							if (W->CastOnTarget(Wtarget, kHitChanceHigh))
							{
								fixmaybe = GGame->TickCount() + 50;
							}
						}
					}
				}

				if (fixmaybe < GGame->TickCount())
				{
					if (!ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
					{
						if (Qtarget != nullptr && Qtarget->IsValidTarget())
						{
							{
								if (PredMethod->GetInteger() == 1)
								{
									AdvPredictionOutput outputfam;
									Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
									if (outputfam.HitChance >= kHitChanceHigh)
									{
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
										Q->CastOnPosition(pred);
									}
									else if (outputfam.HitChance == kHitChanceDashing)
									{
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
										Q->CastOnTarget(Qtarget, kHitChanceDashing);
									}

								}
								if (PredMethod->GetInteger() == 0)
								{
									AdvPredictionOutput outputfam;
									Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
									if (outputfam.HitChance >= kHitChanceHigh)
									{
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
										Q->CastOnPosition(pred);
									}
									else if (outputfam.HitChance == kHitChanceDashing)
									{
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
										Q->CastOnTarget(Qtarget, kHitChanceDashing);
									}

								}
							}
						}
					}
					if (ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
					{
						if (Qtarget != nullptr && Qtarget->IsValidTarget())
						{
							if (!Qtarget->HasBuffOfType(BUFF_Poison))
							{
								if (PredMethod->GetInteger() == 1)
								{
									AdvPredictionOutput outputfam;
									Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
									if (outputfam.HitChance >= kHitChanceHigh)
									{
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
										Q->CastOnPosition(pred);
									}
									else if (outputfam.HitChance == kHitChanceDashing)
									{
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
										Q->CastOnTarget(Qtarget, kHitChanceDashing);
									}

								}
								if (PredMethod->GetInteger() == 0)
								{
									AdvPredictionOutput outputfam;
									Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
									if (outputfam.HitChance >= kHitChanceHigh)
									{
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
										Q->CastOnPosition(pred);
									}
									else if (outputfam.HitChance == kHitChanceDashing)
									{
										Vec3 pred;
										GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
										Q->CastOnTarget(Qtarget, kHitChanceDashing);
									}

								}
							}
						}
					}
				}
				if (ComboE->Enabled())
				{
					if (E->IsReady())
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
						if (target != nullptr && target->IsValidTarget())
						{
							if (!(ComboEonlyPoison->Enabled()) || target->HasBuffOfType(BUFF_Poison))
								E->CastOnTarget(target);
						}
					}
				}
			}
			if (ComboRylais->Enabled())
			{
				if (ComboE->Enabled())
				{
					if (E->IsReady())
					{
						auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
						if (target != nullptr && target->IsValidTarget())
						{
							if (!(ComboEonlyPoison->Enabled()) || target->HasBuffOfType(BUFF_Poison))
								E->CastOnTarget(target);
						}
					}
				}

				if (!ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
				{
					if (Qtarget != nullptr && Qtarget->IsValidTarget())
					{
						{
							if (PredMethod->GetInteger() == 1)
							{
								AdvPredictionOutput outputfam;
								Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
								if (outputfam.HitChance >= kHitChanceHigh)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
									Q->CastOnPosition(pred);
								}
								else if (outputfam.HitChance == kHitChanceDashing)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
									Q->CastOnTarget(Qtarget, kHitChanceDashing);
								}

							}
							if (PredMethod->GetInteger() == 0)
							{
								AdvPredictionOutput outputfam;
								Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
								if (outputfam.HitChance >= kHitChanceHigh)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
									Q->CastOnPosition(pred);
								}
								else if (outputfam.HitChance == kHitChanceDashing)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
									Q->CastOnTarget(Qtarget, kHitChanceDashing);
								}

							}
						}
					}
				}
				if (ComboQnotpoison->Enabled() && ComboQ->Enabled() && Q->IsReady() && Q->Range())
				{
					if (Qtarget != nullptr && Qtarget->IsValidTarget())
					{
						if (!Qtarget->HasBuffOfType(BUFF_Poison))
						{
							if (PredMethod->GetInteger() == 1)
							{
								AdvPredictionOutput outputfam;
								Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
								if (outputfam.HitChance >= kHitChanceHigh)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
									Q->CastOnPosition(pred);
								}
								else if (outputfam.HitChance == kHitChanceDashing)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, false, pred);
									Q->CastOnTarget(Qtarget, kHitChanceDashing);
								}

							}
							if (PredMethod->GetInteger() == 0)
							{
								AdvPredictionOutput outputfam;
								Q->RunPrediction(Qtarget, true, kCollidesWithNothing, &outputfam);
								if (outputfam.HitChance >= kHitChanceHigh)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
									Q->CastOnPosition(pred);
								}
								else if (outputfam.HitChance == kHitChanceDashing)
								{
									Vec3 pred;
									GPrediction->GetFutureUnitPosition(Qtarget, 0.35f, true, pred);
									Q->CastOnTarget(Qtarget, kHitChanceDashing);
								}

							}
						}
					}
				}
				if (ComboW->Enabled() && W->IsReady() && W->Range())
				{
					if (Wtarget != nullptr && Wtarget->IsValidTarget())
					{
						auto distance = (Player->GetPosition() - Wtarget->GetPosition()).Length();
						if (distance >= 400 && distance <= ComboWmax->GetFloat())
						{
							W->CastOnTarget(Wtarget, kHitChanceHigh);
						}
					}
				}
			}


		}
		if (ComboRkillable->Enabled())
		{
			if (ComboREnable->Enabled() && R->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, float(ComboRRANGE->GetInteger()));
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
					auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
					auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);

					if (target->GetHealth() > ComboRcheck->GetFloat())
					{

						if (QDamage + EDamage * 4 + RDamage > target->GetHealth())
						{
							if (target != nullptr && target->IsValidTarget())
							{
								R->CastOnTarget(target);
							}

						}
					}
				}
			}
		}
		if (!ComboRkillable->Enabled())
		{
			if (ComboREnable->Enabled() && R->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, float(ComboRRANGE->GetInteger()));
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					if (target->GetHealth() > ComboRcheck->GetFloat())
					{
						if (target->HealthPercent() <= ComboRHealth->GetInteger())
						{
							Vec3 pos;
							int hit;
							GPrediction->FindBestCastPosition(ComboRRANGE->GetInteger(), 290, false, false, true, pos, hit);
							if (hit >= ComboRMin->GetInteger())
							{
								if (target != nullptr && ComboRFacing->Enabled() && target->IsFacing(Player) && target->IsValidTarget())
								{
									R->CastOnPosition(pos);
								}
								if (target != nullptr && !ComboRFacing->Enabled() && target->IsValidTarget())
								{
									R->CastOnPosition(pos);
								}
							}
						}
					}
				}
			}
		}
	}
}

void AutoQdash()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (ComboQdash->Enabled())
		{
			if (Enemy != nullptr)
			{
				if (Q->IsReady() && Enemy->IsValidTarget())
				{
					if (Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						AdvPredictionOutput outputfam;
						Q->RunPrediction(Enemy, false, kCollidesWithNothing, &outputfam);
						if (outputfam.HitChance == kHitChanceDashing)
						{
							Q->CastOnTarget(Enemy, kHitChanceDashing);
						}
						else return;
					}
				}

			}
		}
	}
}

void Rflash()
{
	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		GGame->IssueOrderEx(GEntityList->Player(), kMoveTo, GGame->CursorPosition(), false);
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (R->IsReady() && Flash != nullptr && Flash->IsReady() && ComboREnable->Enabled())
			{
				if ((Player->GetPosition() - Enemy->GetPosition()).Length() > R->Range() && Enemy->IsValidTarget() && Enemy != nullptr && ((Player->GetPosition() - Enemy->GetPosition()).Length()) < R->Range() + 410 && !Enemy->IsDead())
				{

					if (R->CastOnPosition(Enemy->ServerPosition()))
					{
						Flash->CastOnPosition(Enemy->ServerPosition());
					}
				}
			}
			if (Enemy->IsValidTarget() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && !Flash->IsReady() && !R->IsReady())
			{
				if (Enemy != nullptr)
				{

					W->CastOnTarget(Enemy);
				}
			}

		}
	}
}


void LastHit()
{
	if (!E->IsReady() || !LastE->Enabled())
		return;
	for (auto minion : GEntityList->GetAllMinions(false, true, true))
	{
		if (minion != nullptr && !minion->IsDead())
		{
			if (GEntityList->Player()->IsValidTarget(minion, E->Range()))
			{
				if (GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotE) > GHealthPrediction->GetPredictedHealth(minion, kLastHitPrediction, static_cast<int>(((minion->ServerPosition() - GEntityList->Player()->GetPosition()).Length2D() * 1000) / E->Speed())-125, LastEDelay->GetFloat()))
				{
					E->CastOnUnit(minion);
				}
			}
		}
	}
}

void Mixed()
{
	{
		if (GEntityList->Player()->ManaPercent() > HarassMana->GetInteger())
		{
			if (HarassQ->Enabled() && Q->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr && !target->IsDashing())
				{
					if (PredMethod->GetInteger() == 1)
					{
						AdvPredictionOutput outputfam;
						Q->RunPrediction(target, true, kCollidesWithNothing, &outputfam);
						if (outputfam.HitChance >= kHitChanceHigh)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(target, 0.35f, false, pred);
							Q->CastOnPosition(pred);
						}
						else if (outputfam.HitChance == kHitChanceDashing)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(target, 0.35f, false, pred);
							Q->CastOnTarget(target, kHitChanceDashing);
						}

					}
					if (PredMethod->GetInteger() == 0)
					{
						AdvPredictionOutput outputfam;
						Q->RunPrediction(target, true, kCollidesWithNothing, &outputfam);
						if (outputfam.HitChance >= kHitChanceHigh)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(target, 0.35f, true, pred);
							Q->CastOnPosition(pred);
						}
						else if (outputfam.HitChance == kHitChanceDashing)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(target, 0.35f, true, pred);
							Q->CastOnTarget(target, kHitChanceDashing);
						}

					}
				}
			}
			if (HarassE->Enabled())
			{
				if (E->IsReady())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
					if (target != nullptr)
					{
						if (!(HarassEonlyPoison->Enabled()) || target->HasBuffOfType(BUFF_Poison))
							E->CastOnTarget(target);
					}
				}
			}
		}
		if (HarassLast->Enabled())
		{
			for (auto minion : GEntityList->GetAllMinions(false, true, true))
			{
				if (minion != nullptr && !minion->IsDead())
				{
					if (GEntityList->Player()->IsValidTarget(minion, E->Range()))
					{
						if (GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotE) > GHealthPrediction->GetPredictedHealth(minion, kLastHitPrediction, static_cast<int>(((minion->ServerPosition() - GEntityList->Player()->GetPosition()).Length2D() * 1000) / E->Speed()) - 125,LastEDelay->GetFloat())) {
							E->CastOnUnit(minion);
						}
					}
				}
			}
		}
	}
}

void Killsteal()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
		auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);

		if (Enemy != nullptr && !Enemy->IsDead() && Enemy->IsValidTarget())
		{
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				if (PredMethod->GetInteger() == 1)
				{
					AdvPredictionOutput outputfam;
					Q->RunPrediction(Enemy, true, kCollidesWithNothing, &outputfam);
					if (outputfam.HitChance >= kHitChanceHigh)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.35f, false, pred);
						Q->CastOnPosition(pred);
					}
					else if (outputfam.HitChance == kHitChanceDashing)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.35f, false, pred);
						Q->CastOnTarget(Enemy, kHitChanceDashing);
					}

				}
				if (PredMethod->GetInteger() == 0)
				{
					AdvPredictionOutput outputfam;
					Q->RunPrediction(Enemy, true, kCollidesWithNothing, &outputfam);
					if (outputfam.HitChance >= kHitChanceHigh)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.35f, true, pred);
						Q->CastOnPosition(pred);
					}
					else if (outputfam.HitChance == kHitChanceDashing)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.35f, true, pred);
						Q->CastOnTarget(Enemy, kHitChanceDashing);
					}

				}
			}
			if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
			{
				E->CastOnTarget(Enemy);
			}
			if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage > Enemy->GetHealth() && Enemy->GetHealth() > 100)
			{
				if (Enemy->GetHealth() > KSRCheck->GetFloat())
				{
					Vec3 pos;
					int hit;
					GPrediction->FindBestCastPosition(ComboRRANGE->GetInteger(), 290, false, false, true, pos, hit);
					if (hit > 0)
					{
						R->CastOnPosition(pos);
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

	minions = GEntityList->GetAllMinions(false, true, true);
	for (IUnit* minion : minions)
	{
		if (Farmenable == true)
		{
			if (FarmELH->Enabled())
			{
				if (FarmManaPA->GetFloat() < GEntityList->Player()->ManaPercent())
				{
					if (GEntityList->Player()->IsValidTarget(minion, E->Range()))
					{
						if (GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotE) > GHealthPrediction->GetPredictedHealth(minion, kLastHitPrediction, static_cast<int>(((minion->ServerPosition() - GEntityList->Player()->GetPosition()).Length2D() * 1000) / E->Speed()) - 125, LastEDelay->GetFloat())) {
							E->CastOnUnit(minion);
						}
					}
				}
			}
		}
		if (Farmenable == false)
		{
			if (GEntityList->Player()->ManaPercent() > FarmManaP->GetInteger())
			{
				if (FarmE->Enabled())
				{
					if (!FarmEpoison->Enabled())
					{
						if (E->AttackMinions())
							return;
					}

					else
					{
						if (minion != nullptr)
						{
							if (!minion->IsDead() && minion->HasBuffOfType(BUFF_Poison) && SimpleLib::SimpleLib::GetDistance(Player, minion) <= E->Range())
							{
								E->CastOnUnit(minion);
							}
						}
					}
				}


				if (FarmQ->Enabled() && Q->IsReady() && (minion->IsCreep() || minion->IsJungleCreep()))
				{
					Vec3 pos;
					int hit;
					if (GetMinionsQ(Q->Range()) < 7 && minion->IsEnemy(GEntityList->Player()) && !minion->IsDead() && minion->IsValidTarget() && minion->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						GPrediction->FindBestCastPosition(Q->Range(), 100, false, true, false, pos, hit);
						Q->CastOnPosition(pos);
					}
					if (GetMinionsQ(Q->Range()) > 7 && minion->IsEnemy(GEntityList->Player()) && !minion->IsDead() && minion->IsValidTarget() && minion->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						Q->CastOnUnit(minion);
					}
				}

			}
		}
	}
}
void Jungles()
{
	for (auto Minion : GEntityList->GetAllMinions(false, false, true))
	{
		if (E->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
		{
			E->CastOnUnit(Minion);
		}
	}
}
void _OnOrbwalkPreAttack(IUnit* minion)
{
	if (Farmenable == true)
	{
		for (auto minion : GEntityList->GetAllMinions(false, true, false))
		{
			if (minion->IsCreep())
			{
				if (FarmEsmooth->Enabled())
				{
					if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
					{
						if (GEntityList->Player()->GetSpellLevel(kSlotE) > 0)
						{
							if (minion->IsValidTarget(GEntityList->Player(), E->Range()))
							{
								auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotE);
								if (minion->GetHealth() < EDamage + 100 && Player->GetMana() > E->ManaCost())
								{
									if (minion != nullptr && minion->IsCreep())
									{
										GOrbwalking->DisableNextAttack();
									}
								}
							}
						}
					}
				}
				if (GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
				{
					if (GEntityList->Player()->GetSpellLevel(kSlotE) > 0)
					{
						if (minion->IsValidTarget(GEntityList->Player(), E->Range()))
						{
							auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotE);
							if (minion->GetHealth() < EDamage + 100 && Player->GetMana() > E->ManaCost())
							{
								if (minion != nullptr && minion->IsCreep())
								{
									GOrbwalking->DisableNextAttack();
								}
							}
						}
					}
				}
			}
			if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
			{
				if (HarassLast->Enabled() && GEntityList->Player()->GetSpellLevel(kSlotE) > 0)
				{
					if (minion->IsValidTarget(GEntityList->Player(), E->Range()))
					{
						auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), minion, kSlotE);
						if (minion->GetHealth() < EDamage + 100 && Player->GetMana() > E->ManaCost())
						{
							if (minion != nullptr && minion->IsCreep())
							{
								GOrbwalking->DisableNextAttack();
							}
						}
					}
				}
			}
		}
	}


}
void Auto()
{
	if (HarassAuto->Enabled())
	{
		if (GEntityList->Player()->ManaPercent() > HarassMana->GetInteger())
		{
			if (HarassQ->Enabled())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{

					if (Q->IsReady())
					{
						if (PredMethod->GetInteger() == 1)
						{
							AdvPredictionOutput outputfam;
							Q->RunPrediction(target, true, kCollidesWithNothing, &outputfam);
							if (outputfam.HitChance >= kHitChanceHigh)
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(target, 0.35f, false, pred);
								Q->CastOnPosition(pred);
							}

						}
						if (PredMethod->GetInteger() == 0)
						{
							AdvPredictionOutput outputfam;
							Q->RunPrediction(target, true, kCollidesWithNothing, &outputfam);
							if (outputfam.HitChance >= kHitChanceHigh)
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(target, 0.35f, true, pred);
								Q->CastOnPosition(pred);
							}

						}
					}
				}

			}
		}
	}
}
void stacking()
{
	if (StackTear->Enabled() && GUtility->IsPositionInFountain(GEntityList->Player()->ServerPosition(), true, false))
	{
		if (Tear->IsOwned() || Staff->IsOwned())
		{
			if (StackTearQ->Enabled())
			{
				Q->CastOnPosition(GEntityList->Player()->GetPosition());
			}
			if (StackTearW->Enabled())
			{
				W->CastOnPosition(GEntityList->Player()->GetPosition());
			}
		}
	}
}

PLUGIN_EVENT(void) OnGameUpdate()
{
	FarmTog();
	stacking();

	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		Farm();
		Jungles();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
	{
		LastHit();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Mixed();
	}
	if (GUtility->IsKeyDown(ComboAAkey->GetInteger()))
	{
		auto level = Player->GetLevel();
		if (ComboAA->Enabled() && level >= ComboAALevel->GetInteger() && Player->GetMana() > 100)
		{
			GOrbwalking->SetAttacksAllowed(false);
		}
	}
	if (!GUtility->IsKeyDown(ComboAAkey->GetInteger()) || Player->GetMana() < 100)
	{
		{
			GOrbwalking->SetAttacksAllowed(true);
		}
	}
	if (GUtility->IsKeyDown(ComboRflash->GetInteger()))
	{
		Rflash();
	}
	if (GUtility->IsKeyDown(SemiR->GetInteger()))
	{
		Semi();
	}
	Auto();
	Killsteal();
	AutoQdash();

}

void drawpred()
{
	if (DrawPred->Enabled())
	{
		for (auto hero : GEntityList->GetAllHeros(false, true))
		{
			if (!hero->IsDead() && hero->IsValidTarget())
			{
				if (PredMethod->GetInteger() == 0)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(hero, 0.35f, true, pred);
					GRender->DrawOutlinedCircle(pred, Vec4(255, 255, 255, 255), Q->Radius());
				}
				if (PredMethod->GetInteger() == 1)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(hero, 0.35f, false, pred);
					GRender->DrawOutlinedCircle(pred, Vec4(255, 255, 255, 255), Q->Radius());
				}
			}
		}
	}
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(211, 255, 0, 255), float(ComboRRANGE->GetInteger())); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(120, 255, 0, 255), ComboWmax->GetFloat()); }
	if (DrawRFlash->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 100, 255), ComboRRANGE->GetInteger() + 410); }
	dmgdraw();
	if (DrawEkill->Enabled())
	{
		for (auto minionE : GEntityList->GetAllMinions(false, true, false))
		{
			if (minionE->IsValidTarget(GEntityList->Player(), E->Range()))
			{
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), minionE, kSlotE);
				{

					if (GDamage->GetSpellDamage(GEntityList->Player(), minionE, kSlotE) > GHealthPrediction->GetPredictedHealth(minionE, kLastHitPrediction, static_cast<int>(((minionE->ServerPosition() - GEntityList->Player()->GetPosition()).Length2D() * 1000) / E->Speed()) - 125, LastEDelay->GetFloat()) && minionE != nullptr && !minionE->IsDead())
					{
						GRender->DrawOutlinedCircle(minionE->GetPosition(), Vec4(255, 255, 0, 255), 80.f);
					}
				}
			}

		}
	}
	drawpred();
	if (DrawFarm->Enabled())
	{
		static IFont* pFont = nullptr;

		if (pFont == nullptr)
		{
			pFont = GRender->CreateFont("Arial", 15.f, kFontWeightBold);
			pFont->SetOutline(true);
			pFont->SetLocationFlags(kFontLocationNormal);
		}
		Vec2 pos;
		if (GGame->Projection(GEntityList->Player()->GetPosition(), &pos))
		{
			if (Farmenable == true)
			{
				std::string text = std::string("Farm: Passive");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
			if (Farmenable == false)
			{
				std::string text = std::string("Farm: Pushing");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
		}
	}
}

PLUGIN_EVENTD(void) OnOrbwalkPreAttack(IUnit* Args1)
{
	_OnOrbwalkPreAttack(Args1);
}



PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	Player = GEntityList->Player();

	GEventManager->AddEventHandler(kEventOrbwalkBeforeAttack, OnOrbwalkPreAttack);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOrbwalkBeforeAttack, OnOrbwalkPreAttack);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
}