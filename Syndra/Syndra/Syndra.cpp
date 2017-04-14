#include "PluginSDK.h"
#include <string>
#include "OrbManager.h"

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
IMenuOption* QEmouse;
IMenuOption* ComboWR;
IMenuOption* ComboRStack;
IMenuOption* ItemsCombo;
IMenuOption* ComboRmin;
IMenu* RBlacklist;
IMenu* AASet;
IMenuOption* ComboAA;
IMenuOption* ComboAAkey;
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

IMenu* MiscMenu;
IMenuOption* AntiGapE;
IMenuOption* InterruptE;

IMenu* FarmMenu;
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
ISpell2* Q;
ISpell2* QE;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Ignite;

IUnit* Enemy;
IUnit* Player;

int lastw;
int lastwe;

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
	QE->SetSkillshot(0.6f, 80.f, 2100.f, 1100.f);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, true, false, kCollidesWithNothing);
	W->SetSkillshot(0.25f, 50.f, 5000.f, 925.f);

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
		ComboRcheck = ComboMenu->CheckBox("Dont waste R if Q or W can kill", true);
		AASet = ComboMenu->AddMenu("AA Settings");
		ComboAALevel = AASet->AddInteger("At what level disable AA", 1, 18, 6);
		ComboAA = AASet->CheckBox("Disable AA", false);
		ComboAAkey = AASet->AddKey("Disable key", 32);
		RBlacklist = MainMenu->AddMenu("R Blacklist");
		for (auto enemy : GEntityList->GetAllHeros(false, true)) {
			RBlacklist->CheckBox(enemy->ChampionName(), false);
		}
		QEmouse = ComboMenu->AddKey("QE to Mouse", 'T');

	}
	HarassMenu = MainMenu->AddMenu("Harass Settings");
	{
		HarassMana = HarassMenu->AddFloat("Mana Percent", 10, 100, 50);
		HarassQauto = HarassMenu->CheckBox("Auto Q on dash", true);
		HarassQ = HarassMenu->CheckBox("Harass with Q", true);
		HarassW = HarassMenu->CheckBox("Harass with W", true);
		HarassE = HarassMenu->CheckBox("Harass with E", true);
	}
	FarmMenu = MainMenu->AddMenu("Farm Settings");
	{
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
	}
	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawQE = DrawingMenu->CheckBox("Draw QE Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawRkill = DrawingMenu->CheckBox("Draw R killable", true);
		DrawRfill= DrawingMenu->CheckBox("Draw R Damage", true);

	}

	MiscMenu = MainMenu->AddMenu("Misc.");
	{
		AntiGapE = MiscMenu->CheckBox("Anti-Gap E", true);
		InterruptE = MiscMenu->CheckBox("Interrupt E", true);
	}
}
static Vec3 GetGrabbableObjectPosition(bool onlyOrbs)
{
	auto GrabRange = 925.f;
	if (onlyOrbs)
	{
		return OrbManager::GetOrbToGrab(static_cast<int>(GrabRange));
	}

	for (auto minion : GEntityList->GetAllMinions(false, true, true))
	{
		if (minion != nullptr && minion->IsValidTarget(GEntityList->Player(), GrabRange))
		{
			return minion->ServerPosition();
		}
	}

	return OrbManager::GetOrbToGrab(static_cast<int>(GrabRange));
}


static void CastQELogic(IUnit* target)
{
	auto ppos = GEntityList->Player()->ServerPosition();
	auto startPosition = ppos.Extend(target->ServerPosition(), Q->Range());
	QE->SetRangeCheckFrom(startPosition);
	QE->SetFrom(startPosition);
	AdvPredictionOutput prediction_output;
	QE->RunPrediction(target, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall), &prediction_output);

	if (prediction_output.HitChance >= kHitChanceHigh)
	{
		Q->SetOverrideRange(1250);
		Vec3 pred;
		GPrediction->GetFutureUnitPosition(target, 0.6f, true, pred);
		if (Q->CastOnPosition(pred.Extend(GEntityList->Player()->ServerPosition(), 500)));
		{
			Q->SetOverrideRange(800);
			QE->SetFrom(Vec3(0, 0, 0));
		}
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

//Dewblack <3
static void CastELogic(IUnit* target)
{
	for (auto orb : OrbManager::GetOrbs(true))
	{
		if (GEntityList->Player()->ServerPosition().To2D().Distance(orb.To2D()) <= E->Range())
		{
			auto rangeLeft = 100 + (-0.6 * GEntityList->Player()->ServerPosition().To2D().Distance(orb.To2D()) + 950);
			auto start_point = orb.To2D() - (orb.To2D().VectorNormalize() - GEntityList->Player()->ServerPosition().To2D().VectorNormalize()) * 10;
			auto end_point = start_point + (start_point - GEntityList->Player()->ServerPosition().To2D()) * static_cast<float>(rangeLeft);

			QE->SetOverrideDelay(E->GetDelay() * 1000.f + GEntityList->Player()->ServerPosition().To2D().Distance(orb.To2D()) / E->Speed() + target->ServerPosition().To2D().Distance(orb.To2D()) / QE->Speed());
			QE->SetRangeCheckFrom(orb);
			QE->SetFrom(orb);
			AdvPredictionOutput prediction_output;
			QE->RunPrediction(target, true, kCollidesWithYasuoWall, &prediction_output);

			if (prediction_output.HitChance >= kHitChanceHigh
				&& OrbManager::Distance(prediction_output.TargetPosition.To2D(), start_point, end_point, false) < QE->Radius() + target->BoundingRadius())
			{
				E->CastOnPosition(orb);
				QE->SetFrom(Vec3(0, 0, 0));
				return;
			}
		}
	}
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
			if (!GEntityList->Player()->HasBuff("syndrawtooltip"))
			{
				auto object = GetGrabbableObjectPosition(false);
				if (object != Vec3(0, 0, 0))
				{
					W->CastOnPosition(object);
					lastw = GGame->TickCount() + GGame->Latency() + 20;
					lastwe = GGame->TickCount() + GGame->Latency() + 150;

				}
			}
		}
	}

	if (GEntityList->Player()->HasBuff("syndrawtooltip"))
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && ComboW->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
		{
			if (!target->HasBuff("SyndraEDebuff"))
			{
				W->CastOnTarget(target);
				
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
		}
	}
	if (R->IsReady())
	{

		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
		if (target != nullptr && ComboR->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && GetUltimateDamage(target) > target->GetHealth())
		{
			if (!ComboRcheck->Enabled())
			{
				IMenuOption* temp = RBlacklist->GetOption(target->ChampionName());
				if (!temp->Enabled())
				{
					R->CastOnTarget(target);
				}
			}
			if (ComboRcheck->Enabled())
			{
				for (auto enemy : GEntityList->GetAllHeros(false, true))
				{
					IMenuOption* temp = RBlacklist->GetOption(enemy->ChampionName());
					if (Q->IsReady() && GDamage->GetSpellDamage(GEntityList->Player(), enemy, kSlotQ) > enemy->GetHealth())
						continue;
					if ((GEntityList->Player()->HasBuff("syndrawtooltip") || W->IsReady()) && GDamage->GetSpellDamage(GEntityList->Player(), enemy, kSlotW) > enemy->GetHealth())
						continue;

					if (GetUltimateDamage(enemy) > enemy->GetHealth() && R->IsReady() && enemy->IsValidTarget(GEntityList->Player(), R->Range()))
					{
						if (!temp->Enabled())
						{
							R->CastOnTarget(enemy);
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

				if (!GEntityList->Player()->HasBuff("syndrawtooltip"))
				{
					auto object = GetGrabbableObjectPosition(false);
					if (object != Vec3(0, 0, 0))
					{
						W->CastOnPosition(object);
						lastw = GGame->TickCount() + GGame->Latency() + 20;
						lastwe = GGame->TickCount() + GGame->Latency() + 150;

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
						if (!target->HasBuff("SyndraEDebuff"))
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
					if (Q->IsReady() && QDamage> enemy->GetHealth())
						continue;
					if ((GEntityList->Player()->HasBuff("syndrawtooltip") || W->IsReady()) && WDamage > enemy->GetHealth())
						continue;
					if (GetUltimateDamage(enemy) > enemy->GetHealth() && R->IsReady() && enemy->IsValidTarget(GEntityList->Player(), R->Range()))
					{
						if (!temp->Enabled())
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



void Farm()
{
	if (FarmMana->GetFloat() <= GEntityList->Player()->ManaPercent())
	{
		for (auto Minion : GEntityList->GetAllMinions(false, true, true))
		{
			if (FarmQ->Enabled())
			{
				if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
				{
					if (Q->IsReady() && GEntityList->Player()->IsValidTarget(Minion, Q->Range()))
					{
						Q->CastOnUnit(Minion);
					}
				}
			}
			if (FarmW->Enabled() && W->IsReady() && GetMinionsW(W->Range()) >= 3)
			{
				if (!GEntityList->Player()->HasBuff("syndrawtooltip"))
				{
					auto obj = GetGrabbableObjectPosition(false);
					if (obj != Vec3(0, 0, 0) && GEntityList->Player()->ServerPosition().To2D().Distance(obj.To2D()) < 925.f && W->IsReady())
					{
						W->CastOnPosition(obj);

					}
				}
			}
		}
	}
	if (GEntityList->Player()->HasBuff("syndrawtooltip"))
	{
		Vec3 pos;
		int WHit;
		GPrediction->FindBestCastPosition(W->Range(), W->Radius(), false, true, false, pos, WHit);
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
				if (!GEntityList->Player()->HasBuff("syndrawtooltip"))
				{
					auto object = GetGrabbableObjectPosition(false);
					if (object != Vec3(0, 0, 0))
					{
						W->CastOnPosition(object);
						lastw = GGame->TickCount() + GGame->Latency() + 20;
						lastwe = GGame->TickCount() + GGame->Latency() + 150;

					}
				}
			}
		}

		if (GEntityList->Player()->HasBuff("syndrawtooltip"))
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			if (target != nullptr && target->IsValidTarget(GEntityList->Player(), W->Range()) && HarassW->Enabled() && target->IsValidTarget() && target->IsHero() && !target->IsDead())
			{
				if (!target->HasBuff("SyndraEDebuff"))
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
inline float GetDistanceVectors(Vec3 from, Vec3 to)
{
	float x1 = from.x;
	float x2 = to.x;
	float y1 = from.y;
	float y2 = to.y;
	float z1 = from.z;
	float z2 = to.z;
	return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0)));
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
	OrbManager::OnProcessSpellCast(Args);
	if (Args.Caster_ == GEntityList->Player())
	{
		if (std::string(Args.Name_) == "SyndraQ")
		{
			if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && lastw < GGame->TickCount())
			{
				E->CastOnPosition(Args.EndPosition_);
			}
			if (GetAsyncKeyState(QEmouse->GetInteger()))
			{
				E->CastOnPosition(Args.EndPosition_);
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
	autoQ();
	Rrange();
	Killsteal();
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if (GetAsyncKeyState(QEmouse->GetInteger()))
	{
		SemiQE(GGame->CursorPosition());
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
	if (DrawQE->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), QE->Range()); }
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	dmgdraw();
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