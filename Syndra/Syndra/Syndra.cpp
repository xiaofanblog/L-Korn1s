#include "PluginSDK.h"
#include <string>
#include "Template.h"

PluginSetup("Kornis Syndra")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboQE;
IMenuOption* ComboQ2;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboRcheck;
IMenuOption* KSRcheck;
IMenuOption* QEmouse;
IMenuOption* ComboWR;
IMenuOption* ComboRStack;
IMenuOption* ItemsCombo;
IMenuOption* ComboRmin;
IMenu* RBlacklist;
IMenu* AASet;
IMenuOption* ComboAA;
IMenuOption* ComboAAkey;
IMenuOption* QEsettings;
IMenuOption* ComboAALevel;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSW;
IMenuOption* KSE;
IMenuOption* KSR;
IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;
IMenuOption* DrawRkill;
IMenuOption* DrawRfill;
IMenuOption* DrawOrb;
IMenuOption* DrawWRange;
IMenuOption* DrawQE;
IMenuOption* DrawLane;
IMenuOption* DrawHarass;
IMenu* MiscMenu;
IMenuOption* AntiGapE;
IMenuOption* InterruptE;

IMenu* FarmMenu;
IMenuOption* FarmToggle;
IMenuOption* FarmMana;
IMenuOption* FarmQ;
IMenuOption* FarmQLS;
IMenuOption* FarmW;

IMenu* HarassMenu;
IMenuOption* HarassMana;
IMenuOption* HarassQ;
IMenuOption* HarassW;
IMenuOption* HarassE;
IMenuOption* HarassQauto;
IMenuOption* HarassQautom;
ISpell2* Q;
ISpell2* QE;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Ignite;

IUnit* Enemy;

Vec3 EndPos;

IUnit* Player;

int lastw;
int lastq;
int lastwe;
int lastqe;
bool Farmenable = true;
bool Harassenable = false;
float KeyPre; 
float KeyPres;
int lastqE;

int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(25, 255, 0, 200);

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, false, false, (kCollidesWithNothing));
	Q->SetSkillshot(0.f, 60.f, 10000000000000.f, 800.f);
	QE = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, false, kCollidesWithYasuoWall);
	QE->SetSkillshot(0.6f, 80.f, 2100.f, 1200.f);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, true, false, kCollidesWithNothing);
	W->SetSkillshot(0.25f, 50.f, 5000.f, 980.f);

	E = GPluginSDK->CreateSpell2(kSlotE, kConeCast, false, false, kCollidesWithYasuoWall);
	E->SetSkillshot(0.25f, 80.f, 2500.f, 650.f);

	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, (kCollidesWithYasuoWall));
}


void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Syndra");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboMenu = MainMenu->AddMenu("Combo Settings");
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboQE = ComboMenu->CheckBox("Use QE", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		ComboR = ComboMenu->CheckBox("Use R", true);
		ComboRcheck = ComboMenu->AddFloat("Dont waste R if Enemy HP lower than", 0, 500, 100);
		AASet = ComboMenu->AddMenu("AA Settings");
		ComboAALevel = AASet->AddInteger("At what level disable AA", 1, 18, 6);
		ComboAA = AASet->CheckBox("Disable AA", false);
		ComboAAkey = AASet->AddKey("Disable key", 32);
		RBlacklist = MainMenu->AddMenu("R Blacklist");
		for (auto enemy : GEntityList->GetAllHeros(false, true)) {
			RBlacklist->CheckBox(enemy->ChampionName(), false);
		}
		QEmouse = ComboMenu->AddKey("QE Key", 'T');
		QEsettings = ComboMenu->AddSelection("QE on Key Mode", 1, { "Target", "Mouse", "Logic"});

	}
	HarassMenu = MainMenu->AddMenu("Harass Settings");
	{
		HarassMana = HarassMenu->AddFloat("Mana Percent", 10, 100, 50);
		HarassQauto = HarassMenu->CheckBox("Auto Q On Dash", true);
		HarassQautom = HarassMenu->AddKey("Auto Q Always(Toggle)", 'G');
		HarassQ = HarassMenu->CheckBox("Harass with Q", true);
		HarassW = HarassMenu->CheckBox("Harass with W", true);
		HarassE = HarassMenu->CheckBox("Harass with E", true);
	}
	FarmMenu = MainMenu->AddMenu("Farm Settings");
	{
		FarmToggle = FarmMenu->AddKey("Farm Toggle", 4);
		FarmMana = FarmMenu->AddFloat("Mana Percent", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Farm with Q", true);
		FarmW = FarmMenu->CheckBox("Farm with W", true);
	}
	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Killsteal with Q", true);
		KSW = KillstealMenu->CheckBox("Killsteal with W", true);
		KSE = KillstealMenu->CheckBox("Killsteal with E", true);
		KSR = KillstealMenu->CheckBox("Killsteal with Smart R", true);
		KSRcheck = KillstealMenu->AddFloat("Dont waste R if Enemy HP lower than", 0, 500, 100);
	}
	DrawingMenu = MainMenu->AddMenu("Drawings.");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawQE = DrawingMenu->CheckBox("Draw QE Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", false);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", false);
		DrawRkill = DrawingMenu->CheckBox("Draw R killable", true);
		DrawRfill= DrawingMenu->CheckBox("Draw R Damage", true);
		DrawLane = DrawingMenu->CheckBox("Draw Lane Toggle", true);
		DrawHarass = DrawingMenu->CheckBox("Draw Harass Toggle", true);
	}

	MiscMenu = MainMenu->AddMenu("Misc.");
	{
		AntiGapE = MiscMenu->CheckBox("Anti-Gap E", true);
		InterruptE = MiscMenu->CheckBox("Interrupt E", true);
	}
}
inline float GetDistanceVectors(Vec3 from, Vec3 to)
{
	auto x1 = from.x;
	auto x2 = to.x;
	auto y1 = from.y;
	auto y2 = to.y;
	auto z1 = from.z;
	auto z2 = to.z;
	return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0)));
}


static void CastQELogic(IUnit* target)
{
	if ((GEntityList->Player()->GetPosition() - target->GetPosition()).Length2D() < 800)
	{
		auto ppos = GEntityList->Player()->ServerPosition();
		auto startPosition = ppos.Extend(target->ServerPosition(), Q->Range());

		QE->SetRangeCheckFrom(startPosition);
		QE->SetFrom(startPosition);
		AdvPredictionOutput prediction_output;
		QE->RunPrediction(target, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall), &prediction_output);

		if (prediction_output.HitChance >= kHitChanceHigh)
		{
			Q->SetOverrideRange(1200);
			Vec3 pred;
			GPrediction->GetFutureUnitPosition(target, 0.5, true, pred);
			if (!target->IsValidTarget(GEntityList->Player(), E->Range()))
			{
				if (Q->CastOnPosition(pred.Extend(GEntityList->Player()->ServerPosition(), 420)));
				{
					lastqE = GGame->Latency();
					Q->SetOverrideRange(800);
					QE->SetFrom(Vec3(0, 0, 0));
				}
			}
			if (target->IsValidTarget(GEntityList->Player(), E->Range()))
			{
				if (Q->CastOnPosition(pred));
				{
					lastqE = GGame->Latency();
					Q->SetOverrideRange(800);
					QE->SetFrom(Vec3(0, 0, 0));
				}
			}
		}
	}
	if ((GEntityList->Player()->GetPosition() - target->GetPosition()).Length2D() > 800 && (GEntityList->Player()->GetPosition() - target->GetPosition()).Length2D() < 1100)
	{
		auto ppos = GEntityList->Player()->ServerPosition();
		auto startPosition = ppos.Extend(target->ServerPosition(), Q->Range()-100);
		QE->SetOverrideDelay(0.4);
		QE->SetRangeCheckFrom(startPosition);
		QE->SetFrom(startPosition);
		AdvPredictionOutput prediction_output;
		QE->RunPrediction(target, true, static_cast<eCollisionFlags>(kCollidesWithYasuoWall), &prediction_output);

		if (prediction_output.HitChance >= kHitChanceHigh)
		{
			if (Q->CastOnPosition(startPosition))
			{
				QE->SetFrom(Vec3(0, 0, 0));
				lastqE = GGame->Latency();
			}
		}
		/*Vec3 pred;
		GPrediction->GetFutureUnitPosition(target, 0.3, true, pred);
		if (!target->IsValidTarget(GEntityList->Player(), E->Range()))
		{
			if (Q->CastOnPosition(pred.Extend(GEntityList->Player()->ServerPosition(), 420)));
			{
				lastqE = GGame->Latency();
				Q->SetOverrideRange(800);
				QE->SetFrom(Vec3(0, 0, 0));
			}
		}
		if (target->IsValidTarget(GEntityList->Player(), E->Range()))
		{
			if (Q->CastOnPosition(pred));
			{
				lastqE = GGame->Latency();
				Q->SetOverrideRange(800);
				QE->SetFrom(Vec3(0, 0, 0));
			}
		}*/
	}
}
static double GetUltimateDamage(IUnit* target)
{
	if (target == nullptr || !R->IsReady() || !target->IsValidTarget() || target->HasBuffOfType(BUFF_SpellShield) || target->HasBuffOfType(BUFF_SpellImmunity))
		return 0;
	double baseR[3] = { 270.0, 405.0, 540.0 };
	double baseOrbsR[3] = { 90.0, 135.0, 180.0 };
	auto holdMagicDmg = GEntityList->Player()->TotalMagicDamage();
	auto checkBase = baseR[GEntityList->Player()->GetSpellLevel(3) - 1];
	auto checkSoldAdd = baseOrbsR[GEntityList->Player()->GetSpellLevel(3) - 1];
	auto QRealBase = checkBase + (holdMagicDmg * 0.6);
	auto holdmyballslol = checkSoldAdd * (GEntityList->Player()->GetSpellBook()->GetAmmo(kSlotR) - 3);
	auto QRealAdditional = holdmyballslol + (holdMagicDmg * 0.2);
	auto RRealDamage = QRealBase + QRealAdditional;
	auto damage = GDamage->CalcMagicDamage(GEntityList->Player(), target, RRealDamage);
	return damage;

}


void ProjectOn(Vec2 const& Point, Vec2 const& SegmentStart, Vec2 const& SegmentEnd, ProjectionInfo& Out)
{
	auto cx = Point.x;
	auto cy = Point.y;
	auto ax = SegmentStart.x;
	auto ay = SegmentStart.y;
	auto bx = SegmentEnd.x;
	auto by = SegmentEnd.y;
	auto rL = ((cx - ax) * (bx - ax) + (cy - ay) * (by - ay)) /
		(powf(bx - ax, 2) + powf(by - ay, 2));
	auto pointLine = Vec2(ax + rL * (bx - ax), ay + rL * (by - ay));
	float rS;
	if (rL < 0)
	{
		rS = 0;
	}
	else if (rL > 1)
	{
		rS = 1;
	}
	else
	{
		rS = rL;
	}

	auto isOnSegment = (rS == rL);
	auto pointSegment = isOnSegment ? pointLine : Vec2(ax + rS * (bx - ax), ay + rS * (by - ay));

	Out = ProjectionInfo(isOnSegment, pointSegment, pointLine);
}

float Distance(Vec2 point, Vec2 start, Vec2 End, bool onlyOnSegment)
{
	ProjectionInfo projection_info;
	ProjectOn(point, start, End, projection_info);

	if (projection_info.IsOnSegment || onlyOnSegment == false)
	{
		return projection_info.SegmentPoint.Distance(point);
	}

	return FLT_MAX;
}
// iJava <3
static void CastELogic(IUnit* target)
{
	SArray<IUnit*> sOrbs = SArray<IUnit*>(GEntityList->GetAllUnits()).Where([](IUnit* m) {return m != nullptr &&
		m->GetTeam() == GEntityList->Player()->GetTeam() && !m->IsDead() && std::string(m->GetObjectName()) == "Seed"; });

	for (auto ball : sOrbs.ToVector())
	{
		auto orb = ball->ServerPosition();
		if (GEntityList->Player()->ServerPosition().To2D().Distance(orb.To2D()) <= E->Range() && GEntityList->Player()->ServerPosition().To2D().Distance(orb.To2D()) >= 200)
		{
			auto rangeLeft = 100 + (-0.6 * GEntityList->Player()->ServerPosition().To2D().Distance(orb.To2D()) + 900);
			auto start_point = orb.To2D() - (orb.To2D().VectorNormalize() - GEntityList->Player()->ServerPosition().To2D().VectorNormalize()) * 10;
			auto end_point = start_point + (start_point - GEntityList->Player()->ServerPosition().To2D()) * static_cast<float>(rangeLeft);

			QE->SetOverrideDelay(0.1);
			QE->SetOverrideSpeed(10000000000);
			QE->SetRangeCheckFrom(orb);
			QE->SetFrom(orb);
			AdvPredictionOutput prediction_output;
			QE->RunPrediction(target, true, kCollidesWithYasuoWall, &prediction_output);

			if (prediction_output.HitChance >= kHitChanceHigh
				&& Distance(prediction_output.TargetPosition.To2D(), start_point, end_point, false) < QE->Radius() + target->BoundingRadius()-20)
			{
				E->CastOnPosition(orb);
				QE->SetFrom(Vec3(0, 0, 0));
				return;
			}
		}
	}
}

void autorange()
{
	if (!GEntityList->Player()->HasBuff("syndrawtooltip"))
	{
		W->SetOverrideRange(925);
	}
	else W->SetOverrideRange(1100);
}
void Combo()
{

	if (Q->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), Q->Range()) && ComboQ->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			Vec3 pred;
			GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
			Q->CastOnPosition(pred);
		}
	}
	if (W->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && ComboW->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			if (!GEntityList->Player()->HasBuff("syndrawtooltip") && lastq < GGame->TickCount())
			{
				IUnit* obj = nullptr;
				for (auto orbs : GEntityList->GetAllUnits())
				{
					if (orbs != nullptr && !orbs->IsDead() && orbs->IsValidObject() && orbs->GetTeam() == GEntityList->Player()->GetTeam() && !orbs->IsDead() && std::string(orbs->GetObjectName()) == "Seed" && GetDistanceVectors(orbs->GetPosition(), GEntityList->Player()->GetPosition()) <= 925)
					{
						obj = orbs;
						if (obj != nullptr)
						{
							W->CastOnPosition(obj->GetPosition());
							lastw = GGame->TickCount() + GGame->Latency() + 20;
							lastwe = GGame->TickCount() + GGame->Latency() + 150;
						}
					}

				}
				for (auto minion : GEntityList->GetAllMinions(false, true, true))
				{
					if (minion != nullptr && minion->IsValidTarget() && !minion->IsDead() && minion->IsValidTarget(GEntityList->Player(), 925))
					{
						obj = minion;
						if (obj != nullptr)
						{
							W->CastOnPosition(obj->GetPosition());
							lastw = GGame->TickCount() + GGame->Latency() + 20;
							lastwe = GGame->TickCount() + GGame->Latency() + 150;
						}
					}
				}			

			}
		}
	}

	if (GEntityList->Player()->HasBuff("syndrawtooltip"))
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && lastqe < GGame->TickCount() && ComboW->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			if (!target->HasBuff("SyndraEDebuff"))
			{
				Vec3 pred;
				GPrediction->GetFutureUnitPosition(target, 0.25f, true, pred);
				W->CastOnPosition(pred);
				
			}

		}
	}
	if (E->IsReady() && lastwe < GGame->TickCount())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, QE->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), QE->Range()) && ComboE->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			CastELogic(target);

		}
	}
	if (ComboQE->Enabled() && Q->ManaCost() + E->ManaCost() <= GEntityList->Player()->GetMana() && lastwe < GGame->TickCount())
	{
		auto qeTarget = GTargetSelector->FindTarget(QuickestKill, SpellDamage, QE->Range());
		auto wTarget = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
		if (wTarget == nullptr && qeTarget != nullptr && Q->IsReady() && E->IsReady() && GEntityList->Player()->IsValidTarget(qeTarget, QE->Range()))
		{
			CastQELogic(qeTarget);
			lastqe = GGame->TickCount() + GGame->Latency() + 100;
		}
	}
	if (R->IsReady())
	{

		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
		if (target != nullptr && ComboR->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && GetUltimateDamage(target) > target->GetHealth())
		{

			for (auto enemy : GEntityList->GetAllHeros(false, true))
			{
				IMenuOption* temp = RBlacklist->GetOption(enemy->ChampionName());
				if (GetUltimateDamage(enemy) > enemy->GetHealth() && R->IsReady() && enemy->IsValidTarget(GEntityList->Player(), R->Range()))
				{
					if (!temp->Enabled() && enemy->GetHealth() > ComboRcheck->GetFloat())
					{
						R->CastOnTarget(enemy);
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
		auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotW);
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);

		if (Enemy != nullptr && !Enemy->IsDead() & Enemy->IsValidTarget())
		{
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				Vec3 pred;
				GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
				Q->CastOnPosition(pred);
			}
			if (KSW->Enabled() && W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()) && WDamage > Enemy->GetHealth())
			{

				if (!GEntityList->Player()->HasBuff("syndrawtooltip") && lastq < GGame->TickCount())
				{
					IUnit* obj = nullptr;
					for (auto orbs : GEntityList->GetAllUnits())
					{
						if (orbs != nullptr && !orbs->IsDead() && orbs->IsValidObject() && orbs->GetTeam() == GEntityList->Player()->GetTeam() && !orbs->IsDead() && std::string(orbs->GetObjectName()) == "Seed" && GetDistanceVectors(orbs->GetPosition(), GEntityList->Player()->GetPosition()) <= 925)
						{
							obj = orbs;
							if (obj != nullptr)
							{
								W->CastOnPosition(obj->GetPosition());
								lastw = GGame->TickCount() + GGame->Latency() + 20;
								lastwe = GGame->TickCount() + GGame->Latency() + 150;
							}
						}

					}
					for (auto minion : GEntityList->GetAllMinions(false, true, true))
					{
						if (minion != nullptr && minion->IsValidTarget() && !minion->IsDead() && minion->IsValidTarget(GEntityList->Player(), 925))
						{
							obj = minion;
							if (obj != nullptr)
							{
								W->CastOnPosition(obj->GetPosition());
								lastw = GGame->TickCount() + GGame->Latency() + 20;
								lastwe = GGame->TickCount() + GGame->Latency() + 150;
							}
						}
					}
				}
			}


			if(WDamage > Enemy->GetHealth() && W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), W->Range()))
			{
				if (GEntityList->Player()->HasBuff("syndrawtooltip"))
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
					if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && ComboW->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
					{
						if (!target->HasBuff("SyndraEDebuff") && lastqe < GGame->TickCount())
						{
							W->CastOnTarget(target);
							
						}

					}
				}
			}
			if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
			{
				E->CastOnTarget(Enemy);
			}
			if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && GetUltimateDamage(Enemy) > Enemy->GetHealth())
			{

				for (auto enemy : GEntityList->GetAllHeros(false, true))
				{
					IMenuOption* temp = RBlacklist->GetOption(enemy->ChampionName());
					if (GetUltimateDamage(enemy) > enemy->GetHealth() && R->IsReady() && enemy->IsValidTarget(GEntityList->Player(), R->Range()))
					{
						if (!temp->Enabled() && enemy->GetHealth() > KSRcheck->GetFloat())
						{
							R->CastOnTarget(enemy);
						}

					}
				}
			}
		}
	}
}
static int GetMinionsW(float range)
{
	auto minions = GEntityList->GetAllMinions(false, true, false);
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




void FarmTog()
{
	if (GetAsyncKeyState(FarmToggle->GetInteger()))
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
void HarasTog()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		if (GetAsyncKeyState(HarassQautom->GetInteger()))
		{
			if (Harassenable == true && GGame->Time() > KeyPres)
			{
				Harassenable = false;
				KeyPres = GGame->Time() + 0.250;

			}
			if (Harassenable == false && GGame->Time() > KeyPres)
			{
				Harassenable = true;
				KeyPres = GGame->Time() + 0.250;

			}

		}
	}
}
static int GetMinionsQ(float range)
{
	auto minions = GEntityList->GetAllMinions(false, true, false);
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
	if (Farmenable == true)
	{
		if (FarmMana->GetFloat() <= GEntityList->Player()->ManaPercent())
		{
			for (auto Minion : GEntityList->GetAllMinions(false, true, true))
			{
				if (FarmQ->Enabled())
				{
					if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
					{
						Vec3 pos;
						int hit;
						if (GetMinionsQ(Q->Range()) < 7 && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
						{
							GPrediction->FindBestCastPosition(Q->Range(), 100, false, true, false, pos, hit);
							Q->CastOnPosition(pos);
						}
						if (GetMinionsQ(Q->Range()) > 7 && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
						{
							Q->CastOnUnit(Minion);
						}
					}
				}
				if (FarmW->Enabled() && W->IsReady() && GetMinionsW(W->Range()) >= 3)
				{
					if (!GEntityList->Player()->HasBuff("syndrawtooltip"))
					{
						IUnit* obj = nullptr;
						for (auto orbs : GEntityList->GetAllUnits())
						{
							if (orbs != nullptr && !orbs->IsDead() && orbs->IsValidObject() && orbs->GetTeam() == GEntityList->Player()->GetTeam() && !orbs->IsDead() && std::string(orbs->GetObjectName()) == "Seed" && GetDistanceVectors(orbs->GetPosition(), GEntityList->Player()->GetPosition()) <= 925)
							{
								obj = orbs;
								if (obj != nullptr)
								{
									W->CastOnPosition(obj->GetPosition());
									lastw = GGame->TickCount() + GGame->Latency() + 20;
									lastwe = GGame->TickCount() + GGame->Latency() + 150;
								}
							}

						}
						for (auto minion : GEntityList->GetAllMinions(false, true, true))
						{
							if (minion != nullptr && minion->IsValidTarget() && !minion->IsDead() && minion->IsValidTarget(GEntityList->Player(), 925))
							{
								obj = minion;
								if (obj != nullptr)
								{
									W->CastOnPosition(obj->GetPosition());
									lastw = GGame->TickCount() + GGame->Latency() + 20;
									lastwe = GGame->TickCount() + GGame->Latency() + 150;
								}
							}
						}
					}
				}
			}
		}
		if (FarmMana->GetFloat() <= GEntityList->Player()->ManaPercent() && FarmMana->GetFloat()-10 <= GEntityList->Player()->ManaPercent())
		{
			for (auto Minion : GEntityList->GetAllMinions(false, true, true))
			{
				if (GEntityList->Player()->HasBuff("syndrawtooltip") && Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
				{
					Vec3 pos;
					int WHit;
					GPrediction->FindBestCastPosition(W->Range(), 120, false, true, false, pos, WHit);
					if (WHit >= 2)
					{
						W->CastOnPosition(pos);
					}
					if (GetMinionsW(W->Range()) <= 2)
					{
						GPrediction->FindBestCastPosition(W->Range(), W->Radius(), false, true, false, pos, WHit);
						if (WHit == 2)
						{
							W->CastOnPosition(pos);
						}
						if (WHit == 1)
						{
							W->CastOnPosition(pos);
						}
					}
				}
			}
		}
	}
}
void Rrange()
{
	if (GEntityList->Player()->GetSpellLevel(kSlotR) == 3)
	{
		R->SetOverrideRange(750);
	}
}
void autoQ()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (HarassQauto->Enabled())
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
void HarassAuto()
{
	if (Harassenable == true)
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (Enemy != nullptr)
			{
				if (Q->IsReady() && Enemy->IsValidTarget() && !Enemy->IsDead())
				{
					if (Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
						Q->CastOnPosition(pred);
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
		if (Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), Q->Range()) && HarassQ->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				Vec3 pred;
				GPrediction->GetFutureUnitPosition(target, 0.2f, true, pred);
				Q->CastOnPosition(pred);
			}
		}
		if (W->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && HarassW->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				if (!GEntityList->Player()->HasBuff("syndrawtooltip") && lastq < GGame->TickCount())
				{
					IUnit* obj = nullptr;
					for (auto orbs : GEntityList->GetAllUnits())
					{
						if (orbs != nullptr && !orbs->IsDead() && orbs->IsValidObject() && orbs->GetTeam() == GEntityList->Player()->GetTeam() && !orbs->IsDead() && std::string(orbs->GetObjectName()) == "Seed" && GetDistanceVectors(orbs->GetPosition(), GEntityList->Player()->GetPosition()) <= 925)
						{
							obj = orbs;
							if (obj != nullptr)
							{
								W->CastOnPosition(obj->GetPosition());
								lastw = GGame->TickCount() + GGame->Latency() + 20;
								lastwe = GGame->TickCount() + GGame->Latency() + 150;
							}
						}

					}
					for (auto minion : GEntityList->GetAllMinions(false, true, true))
					{
						if (minion != nullptr && minion->IsValidTarget() && !minion->IsDead() && minion->IsValidTarget(GEntityList->Player(), 925))
						{
							obj = minion;
							if (obj != nullptr)
							{
								W->CastOnPosition(obj->GetPosition());
								lastw = GGame->TickCount() + GGame->Latency() + 20;
								lastwe = GGame->TickCount() + GGame->Latency() + 150;
							}
						}
					}
				}
			}
		}

		if (GEntityList->Player()->HasBuff("syndrawtooltip"))
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && HarassW->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				if (!target->HasBuff("SyndraEDebuff") && lastqe < GGame->TickCount())
				{
					W->CastOnTarget(target);
					
				}

			}
		}
		if (E->IsReady() && lastwe < GGame->TickCount())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, QE->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), QE->Range()) && HarassE->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				CastELogic(target);

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
void SemiQE(Vec3 pos)
{
	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		Vec3 Qpos;
		auto playerpos = GEntityList->Player()->GetPosition();
		auto newPos = pos - GEntityList->Player()->GetPosition();
		if (GetDistanceVectors(GEntityList->Player()->GetPosition(), pos) <= 800)
		{
			Qpos = pos;
		}
		else if (GetDistanceVectors(GEntityList->Player()->GetPosition(), pos) > 800)
		{
			Qpos = playerpos + newPos.VectorNormalize() * 800;
		}
		else
		{
			Qpos = playerpos + newPos.VectorNormalize() * GetDistanceVectors(GEntityList->Player()->GetPosition(), pos);
		}
		Q->CastOnPosition(Qpos);
	}
}

static void OnProcessSpellCast(CastedSpell const& Args)
{
	if (Args.Caster_ == GEntityList->Player())
	{
		if (std::string(Args.Name_) == "SyndraQ")
		{
			if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && lastw < GGame->TickCount())
			{
				EndPos = Args.EndPosition_;
				GPluginSDK->DelayFunctionCall(lastqE, []()
				{
					E->CastOnPosition(EndPos);
					lastqE = 0;
				});
			}
			if (GetAsyncKeyState(QEmouse->GetInteger()))
			{
				EndPos = Args.EndPosition_;
				GPluginSDK->DelayFunctionCall(lastqE, []()
				{
					E->CastOnPosition(EndPos);
					lastqE = 0;
				});
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
			auto damage = GetUltimateDamage(hero);
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
					if (damage < hero->GetHealth() && DrawRfill->Enabled())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
					}
					if (damage > hero->GetHealth() && DrawRfill->Enabled())
					{
						GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), Color2);
					}
					if (damage > hero->GetHealth() && DrawRkill->Enabled())
					{
						GRender->DrawTextW(Vec2(barPos.x + xOffset, barPos.y + yOffset - 13), Color, "Killable");
					}
				}
				if (!hero->IsVisible())
				{

				}
			}
		}
	}
}


PLUGIN_EVENT(void) OnGameUpdate()
{
	autorange();
	autoQ();
	Rrange();
	Killsteal();
	FarmTog();
	HarasTog();
	HarassAuto();
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if (GetAsyncKeyState(QEmouse->GetInteger()))
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			if (QEsettings->GetInteger() == 0)
			{
				auto qeTarget = GTargetSelector->FindTarget(QuickestKill, SpellDamage, QE->Range());
				if (qeTarget != nullptr && Q->IsReady() && E->IsReady() && GEntityList->Player()->IsValidTarget(qeTarget, QE->Range()))
				{

					CastQELogic(qeTarget);
				}
			}
			if (QEsettings->GetInteger() == 1)
			{
				SemiQE(GGame->CursorPosition());
			}
			if (QEsettings->GetInteger() == 2)
			{
				if (GetEnemiesInRange(GEntityList->Player(), QE->Range()) == 0)
				{
					SemiQE(GGame->CursorPosition());
				}
				if (GetEnemiesInRange(GEntityList->Player(), QE->Range()) > 0)
				{
					auto qeTarget = GTargetSelector->FindTarget(QuickestKill, SpellDamage, QE->Range());
					if (qeTarget != nullptr && Q->IsReady() && E->IsReady() && GEntityList->Player()->IsValidTarget(qeTarget, QE->Range()))
					{

						CastQELogic(qeTarget);
					}
				}
			}
		}
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		Farm();
	}
	if (GetAsyncKeyState(ComboAAkey->GetInteger()))
	{
		auto level = Player->GetLevel();
		if (ComboAA->Enabled() && level >= ComboAALevel->GetInteger() && Player->GetMana() > 100)
		{
			GOrbwalking->SetAttacksAllowed(false);
		}
	}
	if (!GetAsyncKeyState(ComboAAkey->GetInteger()) || Player->GetMana() < 100 || (!Q->IsReady() && !W->IsReady() && !E->IsReady()))
	{
		{
			GOrbwalking->SetAttacksAllowed(true);
		}
	}

}
PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)

{
	if (InterruptE->Enabled() && (Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length() < E->Range() && E->IsReady() && Args.Target->IsValidTarget())
	{
		E->CastOnTarget(Args.Target);
	}
}
PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{

	if (Args.Sender != GEntityList->Player()
		&& Args.Sender->IsEnemy(GEntityList->Player())
		&& GEntityList->Player()->IsValidTarget(Args.Sender, E->Range() + Args.Sender->BoundingRadius())
		&& AntiGapE->Enabled() && E->IsReady())
	{
		E->CastOnTarget(Args.Sender);
	}
}

PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(145, 255, 255, 255), Q->Range()); }
	if (DrawQE->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), 1200); }
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	dmgdraw();
	if (DrawLane->Enabled())
	{
		static IFont* pFont = nullptr;

		if (pFont == nullptr)
		{
			pFont = GRender->CreateFont("Tahoma", 16.f, kFontWeightNormal);
			pFont->SetOutline(true);
			pFont->SetLocationFlags(kFontLocationNormal);
		}
		Vec2 pos;
		if (GGame->Projection(GEntityList->Player()->GetPosition(), &pos))
		{
			if (Farmenable == true)
			{
				std::string text = std::string("Farm ON");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
			if (Farmenable == false)
			{
				std::string text = std::string("Farm OFF");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
		}
	}
	if (DrawHarass->Enabled())
	{
		static IFont* pFont = nullptr;

		if (pFont == nullptr)
		{
			pFont = GRender->CreateFont("Tahoma", 16.f, kFontWeightNormal);
			pFont->SetOutline(true);
			pFont->SetLocationFlags(kFontLocationCenterVertical);
		}
		Vec2 pos;
		if (GGame->Projection(GEntityList->Player()->GetPosition(), &pos))
		{
			if (Harassenable == true)
			{
				std::string text = std::string("Harass ON");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y - 10, text.c_str());
			}
			if (Harassenable == false)
			{
				std::string text = std::string("Harass OFF");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y - 10, text.c_str());
			}
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
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Syndra<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#7FFF00\"> - Loaded</font></b>");
	GGame->PrintChat("<b><font color=\"#f8a101\">Version: 0.1</font></b>");

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Syndra<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#FF0000\"> - Unloaded</font></b>");
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnProcessSpellCast);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
}