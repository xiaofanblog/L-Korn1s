#include "PluginSDK.h"
#include "Rembrandt.h"
#include <string>

PluginSetup("Jayce")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenu* Qset;
IMenu* Wset;
IMenu* Eset;
IMenuOption* ComboQM;
IMenuOption* ComboWWait;
IMenuOption* ComboQAOE;
IMenuOption* ComboWM;
IMenuOption* ComboEM;
IMenuOption* ComboEKill;
IMenuOption* ComboEKillcombo;
IMenuOption* ComboModeE;
IMenuOption* ComboQEo;
IMenuOption* ComboESemi;
IMenuOption* ComboQR;
IMenuOption* ComboWR;
IMenuOption* ComboSemiQE;
IMenuOption* ComboER;
IMenuOption* Insec;
IMenuOption* ComboR;


IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassQE;
IMenuOption* HarassMana;


IMenu* DrawingMenu;
IMenuOption* DrawQMRange;
IMenuOption* DrawEMRange;
IMenuOption* DrawWMRange;
IMenuOption* DrawRMRange;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawWRange;
IMenuOption* DrawRRange;
IMenuOption* DrawQERange;
IMenuOption* DrawDamage;
IMenuOption* DrawTime;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmW;
IMenuOption* FarmWR;
IMenuOption* FarmMana;

IMenu* JFarmMenu;
IMenuOption* JFarmQR;
IMenuOption* JFarmQM;
IMenuOption* JFarmEM;
IMenuOption* JFarmWM;
IMenuOption* JFarmWR;
IMenuOption* JFarmMana;

IMenu* KillstealMenu;
IMenuOption* KSQR;
IMenuOption* KSEM;
IMenuOption* KSQE;
IMenuOption* KSQM;

IMenu* MiscMenu;
IMenuOption* AntiGapE;
IMenuOption* InterruptE;
IMenu* InsecMenu;
IMenuOption* InsecQ;
IMenuOption* InsecQproc;

IMenu* FleeMenu;
IMenuOption* FleeE; 
IMenuOption* FleeR;
IMenuOption* FleeKey;

Vec3 EndPos;


ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Q2;
ISpell2* QE;
ISpell2* W2;
ISpell2* E2;
ISpell2* R2;
ISpell* Flash;

int qdelay;

int delay;

IUnit* Player;
int helalmoney = 0;


int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(25, 255, 0, 200);
void LoadSpells()
{

	if (GEntityList->Player()->GetSpellSlot("SummonerFlash") != kSlotUnknown)
		Flash = GPluginSDK->CreateSpell(GEntityList->Player()->GetSpellSlot("SummonerFlash"), 425);
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	Q->SetSkillshot(0.25f, 550.f, 2800.f, 550.f);

	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, true, kCollidesWithNothing);
	W->SetSkillshot(0.25f, 285.f, 3200.f, 260.f);

	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	E->SetSkillshot(0.25f, 240.f, 3200.f, 290.f);

	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);

	//Ranged Form
	Q2 = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall| kCollidesWithMinions));
	Q2->SetSkillshot(0.25f, 70.f, 1450.f, 1000.f);

	QE = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall | kCollidesWithMinions));
	QE->SetSkillshot(0.25f, 100.f, 2000.f, 1550.f);

	W2 = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithYasuoWall);
	W2->SetSkillshot(0.25f, 500.f, 3200.f, 550.f);

	E2 = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags> (kCollidesWithNothing));
	E2->SetSkillshot(0.1f, 120.f, FLT_MAX, 650.f);

	R2 = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Jayce by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		Qset = ComboMenu->AddMenu("Q Settings");
		ComboQM = Qset->CheckBox("Use Q Melee", true);
		ComboQR = Qset->CheckBox("Use Q Ranged", true);
		ComboQEo = Qset->CheckBox("QE only if out of Q Range", false);
		//ComboQAOE = Qset->CheckBox("Use Q AOE on Minions", true);
		Wset = ComboMenu->AddMenu("W Settings");
		ComboWM = Wset->CheckBox("Use W Melee", true);
		ComboWR = Wset->CheckBox("Use W Ranged", true);
		ComboWWait= Wset->CheckBox("Wait for Ranged W to end", true);
		Eset = ComboMenu->AddMenu("E Settings");
		ComboEM = Eset->CheckBox("Use E Melee", true);
		ComboER = Eset->CheckBox("Use E Ranged", true);
		ComboModeE = Eset->AddSelection("E Mode", 2, { "Use E Melee only for KS", "Use E Melee only if Killable Combo", "Always" });
		//ComboEKill = Eset->CheckBox("Use E Melee only for KS", true);
		//ComboEKillcombo = Eset->CheckBox("Use E Melee only if Killable Combo", true);
		ComboESemi = Eset->CheckBox("Use E on Q", true);
		ComboR = ComboMenu->CheckBox("Use R Switch", true);
		ComboSemiQE = ComboMenu->AddKey("QE To Mouse", 'T');
	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassMana = HarassMenu->AddFloat("Mana Percent", 10, 100, 50);
		HarassQ = HarassMenu->CheckBox("Use Ranged Q", true);
		HarassQE = HarassMenu->CheckBox("Use QE", true);
	}

	MiscMenu = MainMenu->AddMenu("Misc.");
	{
		InterruptE = MiscMenu->CheckBox("Interupt E", true);
		AntiGapE = MiscMenu->CheckBox("AntiGap E", true);
	}
	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQMRange = DrawingMenu->CheckBox("Draw Q Melee", true);
		DrawWMRange = DrawingMenu->CheckBox("Draw W Melee", true);
		DrawEMRange = DrawingMenu->CheckBox("Draw E Melee", true);
		DrawQRange = DrawingMenu->CheckBox("Draw Q Ranged", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Ranged", true);
		DrawQERange = DrawingMenu->CheckBox("Draw Q+E Range", true);
		DrawDamage = DrawingMenu->CheckBox("Draw Damage", true);
		DrawTime = DrawingMenu->CheckBox("Draw Q Timer", true);
	}
	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQR = KillstealMenu->CheckBox("Killsteal with Q Ranged", true);
		KSQM = KillstealMenu->CheckBox("Killsteal with Q Melee", true);
		KSQE = KillstealMenu->CheckBox("Killsteal with QE", true);
		KSEM = KillstealMenu->CheckBox("Killsteal with E Melee", true);
	}
	InsecMenu = MainMenu->AddMenu("Insec");
	{
		Insec = InsecMenu->AddKey("Insec", 17);
		InsecQ = InsecMenu->CheckBox("Use Q to Gap", true);
		InsecQproc = InsecMenu->CheckBox("Wait for Q proc damage", false);
	}

	JFarmMenu = MainMenu->AddMenu("JungeClear");
	{
		JFarmMana = JFarmMenu->AddFloat("Mana Percent", 10, 100, 50);
		JFarmQM = JFarmMenu->CheckBox("Use Q Melee", true);
		JFarmQR = JFarmMenu->CheckBox("Use Q Ranged", true);
		JFarmWR = JFarmMenu->CheckBox("Use W Ranged", true);
		JFarmWM = JFarmMenu->CheckBox("Use W Melee", true);
		JFarmEM = JFarmMenu->CheckBox("Use E Melee", true);
	}
	
	FarmMenu = MainMenu->AddMenu("LaneClear");
	{
		FarmMana = FarmMenu->AddFloat("Mana Percent", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Use Q Melee", true);
		FarmW = FarmMenu->AddFloat("^- if Hits", 1, 6, 3);
		FarmE = FarmMenu->CheckBox("Use W Melee", true);
		FarmWR = FarmMenu->AddFloat("^- if in W Range", 1, 6, 3);
	}
	FleeMenu = MainMenu->AddMenu("Flee");
	{
		FleeE = FleeMenu->CheckBox("Use E to Flee", true);
		FleeR = FleeMenu->CheckBox("Use R to Flee", true);
		FleeKey = FleeMenu->AddKey("Flee Key: ", 'G');
	}
}
void SemiQ()
{
	if (GEntityList->Player()->HasBuff("jaycestancehammer"))
	{
		R->CastOnPlayer();
	}
	if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
			if (E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(GGame->CursorPosition(), 150)))
			{
				QE->CastOnPosition(GGame->CursorPosition());
			}

		}
	}
}

static int CountMinionsNearMe(IUnit* Source, float range)
{
	auto minion = GEntityList->GetAllMinions(false, true, true);
	auto count = 0;

	for (auto unit : minion)
	{
		if (unit->IsValidTarget() && !unit->IsDead() && (unit->IsCreep() || unit->IsJungleCreep()))
		{
			auto flDistance = (unit->GetPosition() - Source->GetPosition()).Length();
			if (flDistance < range)
			{
				count++;
			}
		}
	}

	return count;
}
void Farm()
{
	for (auto Minion : GEntityList->GetAllMinions(false, true, false))
	{
		if (FarmMana->GetFloat() < GEntityList->Player()->ManaPercent())
		{
			if (GEntityList->Player()->HasBuff("jaycestancehammer"))
			{
				if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					if (CountMinionsNearMe(GEntityList->Player(), Q->Range()) >= FarmW->GetFloat())
					{
						if (Q->CastOnTarget(Minion))
						{
							W->CastOnPlayer();
						}
						qdelay = GGame->Time() + GEntityList->Player()->GetSpellBook()->GetTotalCooldown(kSlotQ);
					}
				}

				if (FarmE->Enabled() && W->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), W->Range()) && CountMinionsNearMe(GEntityList->Player(), W->Range()) >= FarmWR->GetFloat())
				{
					W->CastOnPlayer();
				}
			}

		}
	}
}
static double GetQRangedDamage(IUnit* target)
{
	int qdmg = 0;
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 1)
	{
		qdmg = 70;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 2)
	{
		qdmg = 120;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 3)
	{
		qdmg = 170;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 4)
	{
		qdmg = 220;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 5)
	{
		qdmg = 270;

	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 6)
	{
		qdmg = 320;
	}
	auto calc = GEntityList->Player()->BonusDamage() * 1.2;
	auto full = calc + qdmg;
	auto damage = GDamage->CalcPhysicalDamage(GEntityList->Player(), target, full);
	return damage;

}
static double GetQE(IUnit* target)
{
	int qedmg = 0;
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 1)
	{
		qedmg = 98;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 2)
	{
		qedmg = 168;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 3)
	{
		qedmg = 238;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 4)
	{
		qedmg = 308;
	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 5)
	{
		qedmg = 374;

	}
	if (GEntityList->Player()->GetSpellLevel(kSlotQ) == 6)
	{
		qedmg = 448;
	}
	auto calc = GEntityList->Player()->BonusDamage() * 1.68;
	auto full = calc + qedmg;
	auto damage = GDamage->CalcPhysicalDamage(GEntityList->Player(), target, full);
	return damage;

}
void Flee()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
		if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
		{

			if (FleeE->Enabled())
			{
				E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(GGame->CursorPosition(), 200));
			}
		}
		if (FleeR->Enabled())
		{
			R->CastOnPlayer();
		}
	} 

}

void Jungle()
{
	for (auto Minion : GEntityList->GetAllMinions(false, false, true))
	{
		if (JFarmMana->GetFloat() < GEntityList->Player()->ManaPercent())
		{
			if (GEntityList->Player()->HasBuff("jaycestancehammer"))
			{
				if (JFarmQM->Enabled() && Q->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnTarget(Minion);
					qdelay = GGame->Time() + GEntityList->Player()->GetSpellBook()->GetTotalCooldown(kSlotQ);
				}

				if (JFarmWM->Enabled() && W->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					W->CastOnPlayer();
				}
				if (JFarmEM->Enabled() && E->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnTarget(Minion);
				}
			}
			if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
			{
				if (JFarmQR->Enabled() && Q->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), Q2->Range()))
				{
					Q2->CastOnTarget(Minion);
				}

				if (JFarmWR->Enabled() && W->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), W->Range()))
				{
					W2->CastOnPlayer();
				}
			}
			if (!GEntityList->Player()->HasBuff("jaycestancehammer") && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				if (!Q->IsReady() && !E->IsReady())
				{
					R->CastOnPlayer();
				}
			}
			if (GEntityList->Player()->HasBuff("jaycestancehammer") && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				if (!Q2->IsReady() && !W2->IsReady() && GEntityList->Player()->HasBuff("JayceHyperCharge"))
				{
					R->CastOnPlayer();
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
void insec()
{
	if (GEntityList->Player()->HasBuff("jaycestancehammer"))
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				if (Enemy->IsValidTarget() && Enemy != nullptr  && !Enemy->IsDead())
				{
					if (((Player->GetPosition() - Enemy->GetPosition()).Length()) < 380)
					{
						if (Flash != nullptr && Flash->IsReady() && E->IsReady())
						{
							for (auto Ally : GEntityList->GetAllHeros(true, false))
							{
								if (Ally != GEntityList->Player() && Ally != nullptr && !Ally->IsDead() && Ally->IsValidTarget())
								{

									if (((Player->GetPosition() - Ally->GetPosition()).Length2D()) < 1000)
									{
										if (helalmoney < GGame->TickCount())
										{
											if (Flash->CastOnPosition(Enemy->GetPosition().Extend(Ally->GetPosition(), -100)))
											{
												E->CastOnTarget(Enemy);
												helalmoney = 0;

											}
										}
									}

								}
							}
							if (GetAlliesInRange(GEntityList->Player(), 1000) == 0)
							{
								if (helalmoney < GGame->TickCount())
								{
									if (Flash->CastOnPosition(Enemy->GetPosition().Extend(GEntityList->Player()->GetPosition(), -100)))
									{
										E->CastOnTarget(Enemy);
										helalmoney = 0;

									}
								}
							}



						}
					}
					
					if (InsecQ->Enabled())
					{
						if (InsecQproc->Enabled())
						{
								if (((Player->GetPosition() - Enemy->GetPosition()).Length()) > 380 && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
								{
									if (Q->CastOnTarget(Enemy))
									{
										helalmoney = GGame->TickCount() + 600;
										qdelay = GGame->Time() + GEntityList->Player()->GetSpellBook()->GetTotalCooldown(kSlotQ);
									}
								}
						}
						if (!InsecQproc->Enabled())
						{
							if (((Player->GetPosition() - Enemy->GetPosition()).Length()) > 380 && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
							{
								if (Q->CastOnTarget(Enemy))
								{
									helalmoney = 0;
									qdelay = GGame->Time() + GEntityList->Player()->GetSpellBook()->GetTotalCooldown(kSlotQ);
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
	if (!ComboQEo->Enabled())
	{
		if (Q2->IsReady() && E2->IsReady() && Q2->ManaCost() + E2->ManaCost() < GEntityList->Player()->GetMana())
		{
			if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
			{

				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, QE->Range());
				if (target != nullptr && target->IsValidTarget(GEntityList->Player(), QE->Range()) && ComboQR->Enabled() && ComboESemi->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
				{
					AdvPredictionOutput outputfam;
					QE->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
					if (outputfam.HitChance >= kHitChanceHigh)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(target, 0.3f, true, pred);
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > Q2->Range())
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(target, 0.3f, true, pred);
							if (E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(pred, 150)))
							{
								QE->CastOnPosition(pred);
							}
						}
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < Q2->Range())
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(target, 0.5f, true, pred);
							if (E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(pred, 150)))
							{
								QE->CastOnPosition(pred);
							}
						}


					}
				}
			}
		}
	}
	if (ComboQEo->Enabled())
	{
		if (Q2->IsReady() && E2->IsReady() && Q2->ManaCost() + E2->ManaCost() < GEntityList->Player()->GetMana())
		{
			if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
			{

				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, QE->Range());
				if (target != nullptr && target->IsValidTarget(GEntityList->Player(), QE->Range()) && ComboESemi->Enabled() && ComboQR->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
				{
					if (!target->IsValidTarget(GEntityList->Player(), Q->Range()))
					{

						AdvPredictionOutput outputfam;
						QE->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
						if (outputfam.HitChance >= kHitChanceHigh)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(target, 0.3f, true, pred);
							if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > Q2->Range())
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(target, 0.3f, true, pred);
								if (E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(pred, 150)))
								{
									QE->CastOnPosition(pred);
								}
							}
							if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < Q2->Range())
							{
								Vec3 pred;
								GPrediction->GetFutureUnitPosition(target, 0.5f, true, pred);
								if (E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(pred, 150)))
								{
									QE->CastOnPosition(pred);
								}
							}


						}
					}
				}
				
			}
		}
	}
	if (Q2->IsReady() && ComboQEo->Enabled())
	{
		if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
		{

			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q2->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), Q2->Range()) && ComboQR->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{

				AdvPredictionOutput outputfam;
				Q2->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
				if (outputfam.HitChance >= kHitChanceHigh)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(target, 0.3f, true, pred);
					Q2->CastOnPosition(pred);

				}
			}
			
		}
	}
	if ((Q2->IsReady() && !E2->IsReady() && !ComboQEo->Enabled()) || (Q2->IsReady() && !ComboQEo->Enabled() && E2->ManaCost() + Q2->ManaCost() > GEntityList->Player()->GetMana()) || (Q2->IsReady() && !ComboQEo->Enabled() && !ComboESemi->Enabled()))
	{
		if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
		{

			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q2->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), Q2->Range()) && ComboQR->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{

				AdvPredictionOutput outputfam;
				Q2->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
				if (outputfam.HitChance >= kHitChanceHigh)
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(target, 0.3f, true, pred);
					Q2->CastOnPosition(pred);

				}
			}
		}
	}
	if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
	{
		if (W2->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W2->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W2->Range()) && ComboWR->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				W2->CastOnPlayer();
			}
		}
	}
	if (R->IsReady())
	{
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 1200);

			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), 1200) && ComboR->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				if (target->IsValidTarget(GEntityList->Player(), Q->Range()) && !GEntityList->Player()->HasBuff("jaycestancehammer"))
				{
					if (ComboWWait->Enabled())
					{
						if (!W->IsReady() && !GEntityList->Player()->HasBuff("JayceHyperCharge"))
						{

							if (qdelay - GGame->Time() < 1)
							{
								R->CastOnPlayer();
							}
							if (qdelay - GGame->Time() > 1 && target->IsValidTarget(GEntityList->Player(), 200))
							{
								R->CastOnPlayer();
							}

						}
					}
					if (!ComboWWait->Enabled() && GEntityList->Player()->HasBuff("JayceHyperCharge"))
					{
						if (qdelay - GGame->Time() < 1)
						{
							R->CastOnPlayer();
						}
						if (qdelay - GGame->Time() > 1 && target->IsValidTarget(GEntityList->Player(), 200))
						{
							R->CastOnPlayer();
						}
					}
				}
				if (GEntityList->Player()->HasBuff("jaycestancehammer") && !Q->IsReady() && !W->IsReady())
				{
					R->CastOnPlayer();
				}
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
				auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
				if (GEntityList->Player()->HasBuff("jaycestancehammer") && QDamage + WDamage > target->GetHealth() && target->IsValidTarget(GEntityList->Player(), QE->Range()))
				{
					R->CastOnPlayer();
				}

				if (GEntityList->Player()->HasBuff("jaycestancehammer") && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > Q->Range() + 200 && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < 1200)
				{
					AdvPredictionOutput outputfam;
					QE->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
					if (outputfam.HitChance >= kHitChanceHigh)
					{
						R->CastOnPlayer();
					}
				}
			}
		}
	}
	
	if (ComboModeE->GetInteger() == 0)
	{
		if (GEntityList->Player()->HasBuff("jaycestancehammer"))
		{
			if (E->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (target != nullptr && target->IsValidTarget(GEntityList->Player(), E->Range()) && ComboEM->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
				{
					auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
					if (EDamage > target->GetHealth())
					{
						E->CastOnTarget(target);
					}
				}
			}
		}
	}
	if (ComboModeE->GetInteger() == 1)
	{
		if (GEntityList->Player()->HasBuff("jaycestancehammer"))
		{
			if (E->IsReady())
			{

				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (target != nullptr && target->IsValidTarget(GEntityList->Player(), E->Range()) && ComboEM->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
				{
					auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
					auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
					if ((GetQE(target) + EDamage > target->GetHealth() && GEntityList->Player()->GetSpellRemainingCooldown(kSlotR) < 4) || (EDamage > target->GetHealth()))
					{
						E->CastOnTarget(target);
					}
				}
			}
		}
	}
	if (ComboModeE->GetInteger() == 2)
	{
		if (E->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), E->Range()) && ComboEM->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				if (GEntityList->Player()->GetSpellRemainingCooldown(kSlotR) < 0.5)
				{
					E->CastOnTarget(target);
				}
			}
		}
	}
	if (GEntityList->Player()->HasBuff("jaycestancehammer"))
	{
		if (Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), Q->Range()) && ComboQM->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				Q->CastOnTarget(target);
				qdelay = GGame->Time() + GEntityList->Player()->GetSpellBook()->GetTotalCooldown(kSlotQ);
			}
		}
	}
	if (GEntityList->Player()->HasBuff("jaycestancehammer"))
	{
		if (W->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && ComboWM->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				W->CastOnPlayer();
			}
		}
	}
}
//

void dmgdraw()
{
	for (auto hero : GEntityList->GetAllHeros(false, true))
	{
		Vec2 barPos = Vec2();
		if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
		{
			auto QDamage = 0;
			if (Q->IsReady())
			{
				QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
			}
			auto WDamage = 0;
			if (W->IsReady())
			{
				WDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotW);
			}
			auto EDamage = 0;
			if (E->IsReady())
			{
				EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
			}
			auto QEDamage = 0;
			if (Q2->IsReady() && E2->IsReady())
			{
				QEDamage = GetQE(hero);
			}
			auto QRangDamage = 0;
			if (Q2->IsReady())
			{
				QRangDamage = GetQRangedDamage(hero);
			}
			if (GEntityList->Player()->HasBuff("jaycestancehammer"))
			{
				auto damage = QDamage + EDamage + WDamage;
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
			if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
			{
				auto damage = QEDamage + EDamage;
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
}

void Killsteal()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto QMDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
		auto QEdmg = GetQE(Enemy);
		auto qranged = GetQRangedDamage(Enemy);
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);

		if (Enemy != nullptr && !Enemy->IsDead() & Enemy->IsValidTarget())
		{
			if ((QMDamage || QEdmg || qranged || EDamage) > Enemy->GetHealth())
			{
				if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
				{
					if (QMDamage > (qranged || QEdmg) || EDamage > (qranged || QEdmg))
					{
						R->CastOnPlayer();
					}
				}
				if (GEntityList->Player()->HasBuff("jaycestancehammer"))
				{
					if (QMDamage < (qranged || QEdmg) || EDamage < (qranged || QEdmg))

					{
						R->CastOnPlayer();
					}
				}
			}
			if (GEntityList->Player()->HasBuff("jaycestancehammer"))
			{
				if (KSQM->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QMDamage > Enemy->GetHealth())
				{
					Q->CastOnTarget(Enemy);
					qdelay = GGame->Time() + GEntityList->Player()->GetSpellBook()->GetTotalCooldown(kSlotQ);
				}
				if (KSEM->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
				{
					E->CastOnTarget(Enemy);
				}
			}
			if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
			{
				if (KSQR->Enabled() && Q2->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q2->Range()) && qranged > Enemy->GetHealth())
				{

					AdvPredictionOutput outputfam;
					Q2->RunPrediction(Enemy, false, kCollidesWithMinions, &outputfam);
					if (outputfam.HitChance >= kHitChanceHigh)
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
						Q2->CastOnPosition(pred);

					}
				}
				if (KSQE->Enabled() && E2->IsReady() && Q2->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), QE->Range()) && QEdmg > Enemy->GetHealth() && E2->ManaCost() + Q2->ManaCost() < GEntityList->Player()->GetMana())
				{
					AdvPredictionOutput outputfam;
					QE->RunPrediction(Enemy, false, kCollidesWithMinions, &outputfam);
					if (outputfam.HitChance >= kHitChanceHigh)
					{
						if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > Q2->Range())
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.3f, true, pred);
							if (E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(pred, 150)))
							{
								QE->CastOnPosition(pred);
							}
						}
						if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < Q2->Range())
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(Enemy, 0.5f, true, pred);
							if (E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(pred, 150)))
							{
								QE->CastOnPosition(pred);
							}
						}


					}
				}
			}
		}
	}
}

PLUGIN_EVENT(void) OnProcessSpellCast(CastedSpell const& Args)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo || GOrbwalking->GetOrbwalkingMode() == kModeMixed || GetAsyncKeyState(ComboSemiQE->GetInteger()))
		return;
	if (Args.Caster_ == GEntityList->Player())
	{
		
		if (std::string(Args.Name_) == "JayceShockBlast")
		{
			
			/*if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && ComboER->Enabled())
			{
				EndPos = Args.EndPosition_;
				delay = GGame->TickCount() + 130;
			}*/
			if (ComboESemi->Enabled())
			{
				EndPos = Args.EndPosition_;
				delay = GGame->TickCount() + 130;
			}

		}
	}
}

void hello()
{
	if (delay < GGame->TickCount() && delay != 0)
	{
		E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(EndPos, 150));
		delay = 0;
	}
}
void Harass()
{
	if (GEntityList->Player()->ManaPercent() > HarassMana->GetFloat())
	{
		if (HarassQ->Enabled())
		{
			if (Q2->IsReady())
			{
				if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
				{

					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q2->Range());
					if (target != nullptr && target->IsValidTarget(GEntityList->Player(), Q2->Range()) && target->IsValidTarget() && target->IsHero() && !target->IsDead())
					{

						AdvPredictionOutput outputfam;
						Q2->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
						if (outputfam.HitChance >= kHitChanceHigh)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(target, 0.3f, true, pred);
							Q2->CastOnPosition(pred);

						}
					}
				}
			}
		}
		if (HarassQE->Enabled())
		{
			if (Q2->IsReady() && E2->IsReady() && Q2->ManaCost() + E2->ManaCost() < GEntityList->Player()->GetMana())
			{
				if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
				{

					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, QE->Range());
					if (target != nullptr && target->IsValidTarget(GEntityList->Player(), QE->Range()) && target->IsValidTarget() && target->IsHero() && !target->IsDead())
					{

						AdvPredictionOutput outputfam;
						QE->RunPrediction(target, false, kCollidesWithMinions, &outputfam);
						if (outputfam.HitChance >= kHitChanceHigh)
						{
							Vec3 pred;
							GPrediction->GetFutureUnitPosition(target, 0.3f, true, pred);
							if (E2->CastOnPosition(GEntityList->Player()->ServerPosition().Extend(pred, 150)))
							{
								QE->CastOnPosition(pred);
							}


						}
					}
				}
			}
		}
	}
}

PLUGIN_EVENT(void) OnGameUpdate()
{

	Killsteal();
	hello();
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		Farm();
		Jungle();
	}
	if (GetAsyncKeyState(FleeKey->GetInteger()) & 0x8000)
	{
		Flee();
	}
	if (GetAsyncKeyState(ComboSemiQE->GetInteger()) & 0x8000)
	{
		SemiQ();
	}
	if (GetAsyncKeyState(Insec->GetInteger()) & 0x8000)
	{
		insec();
	}
}


PLUGIN_EVENT(void) OnRender()
{
	if (GEntityList->Player()->HasBuff("jaycestancehammer"))
	{
		if (DrawQMRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawEMRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
		if (DrawWMRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	}
	if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q2->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E2->Range()); }
		if (DrawQERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), QE->Range()); }
	}
	if (DrawDamage->Enabled())
	{
		dmgdraw();
	}
	if (DrawTime->Enabled() && (qdelay - GGame->Time()) > 0)
	{

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
			if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
			{
				std::string text1 = std::string("Q Melee CD: ");
				Vec4 clr1 = Vec4(255, 255, 255, 255);
				pFont->SetColor(clr1);
				pFont->Render(pos.x + -50, pos.y + 70, text1.c_str());
				int smth = qdelay - GGame->Time();
				std::string text = std::to_string(smth);
				Vec4 clr = Vec4(255, 255, 255, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x + 30, pos.y + 70, text.c_str());
				
			}

		}
	}
}

PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (Args.Source != nullptr && InterruptE->Enabled() && (Args.Source->GetPosition() - GEntityList->Player()->GetPosition()).Length() < E->Range() && E->IsReady() && Args.Source->IsValidTarget())
	{
		if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
		{
			R->CastOnPlayer();

		}
		if (GEntityList->Player()->HasBuff("jaycestancehammer"))
		{
			E->CastOnTarget(Args.Source);

		}
	}
}

PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{
	if (Args.Source != nullptr && Args.Source != GEntityList->Player()
		&& Args.Source->IsEnemy(GEntityList->Player())
		&& AntiGapE->Enabled() && E->IsReady() && Args.Source->IsValidTarget(GEntityList->Player(), E->Range()))
	{
		if (!GEntityList->Player()->HasBuff("jaycestancehammer"))
		{
			R->CastOnPlayer();

		}
		if (GEntityList->Player()->HasBuff("jaycestancehammer"))
		{
			E->CastOnTarget(Args.Source);

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
	GEventManager->AddEventHandler(kEventOnSpellCast, OnProcessSpellCast);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Jayce<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#7FFF00\"> - Loaded</font></b>");
	GGame->PrintChat("<b><font color=\"#f8a101\">Version: 0.1</font></b>");
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);


}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnProcessSpellCast);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Jayce<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#FF0000\"> - Unloaded</font></b>");

}