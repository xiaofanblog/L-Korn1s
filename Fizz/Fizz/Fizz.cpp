#include "PluginSDK.h"
#include <string>

PluginSetup("Kornis's Fizz")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenu* ComboQ;
IMenuOption* ComboQenable;
IMenuOption* ComboQsave;
IMenu* ComboWen;
IMenuOption* ComboW;
IMenuOption* SaveE;
IMenuOption* ComboWproc;
IMenu* ComboE;
IMenuOption* ComboEenable;
IMenuOption* ComboE1;
IMenuOption* ComboRalways;

IMenu* ComboR;
IMenuOption* ComboRhp;
IMenuOption* ComboRmode;
IMenuOption* ComboRenable;
IMenuOption* ComboRkill;
IMenuOption* ComboSemiManual;



IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassW;
IMenuOption* HarassE;
IMenuOption* HarassWproc;
IMenuOption* HarassMode;
IMenuOption* HarassMana;


IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange; 
IMenuOption* DrawFill; 
IMenuOption* DrawDamage;
IMenuOption* DrawEFlash;


IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmEmin;
IMenuOption* FarmW;
IMenuOption* FarmMana;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSR;



ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell* Flash;
int Wproc = 0;

std::vector<std::string> ComboRType = { "Gapclose", "Dash", "After Dash" };
std::vector<std::string> HarassType = { "WQAEE", "EEWQ" };

IUnit* Player;

int xOffset = 10;
int yOffset = 15;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);


void LoadSpells()
{

	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithYasuoWall);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, false, kCollidesWithYasuoWall);
	R->SetSkillshot(0.25f, 80.f, 5000.f, 1100.f);
	if (GEntityList->Player()->GetSpellSlot("SummonerFlash") != kSlotUnknown)
		Flash = GPluginSDK->CreateSpell(GEntityList->Player()->GetSpellSlot("SummonerFlash"), 425);

}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Fizz by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{

		ComboQ = ComboMenu->AddMenu("Q Settings");
		ComboQenable = ComboQ->CheckBox("Use Q", true);
		/*ComboQgap = ComboQ->CheckBox("Use Q to gap if kilable with R combo", true);*/
		ComboQsave = ComboQ->CheckBox("Smart Q", true);
		ComboWen = ComboMenu->AddMenu("W Settings");
		ComboW = ComboWen->CheckBox("Use W", true);
		ComboWproc = ComboWen->CheckBox("Wait for W proc", true);
		ComboE = ComboMenu->AddMenu("E Settings");
		ComboEenable = ComboE->CheckBox("Use E", true);
		SaveE = ComboE->CheckBox("Save E", true);
		ComboE1 = ComboE->AddKey("E Flash Combo ", 'T');
		ComboR = ComboMenu->AddMenu("R Settings");
		ComboRenable = ComboR->CheckBox("Use R", true);
		ComboRhp = ComboR->AddInteger("Use R if enemy HP <", 1, 100, 35);
		ComboRkill = ComboR->CheckBox("Use R only if killable with combo", false);
		ComboRalways = ComboR->CheckBox("Use R always", true);
		ComboRmode = ComboR->AddSelection("R: ", 1, ComboRType);
		ComboSemiManual = ComboR->AddKey("Semi-Manual R", 'R');

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassMana = HarassMenu->AddInteger("Mana percent for harass", 10, 100, 50);
		HarassQ = HarassMenu->CheckBox("Use Q to harass", true);
		HarassW = HarassMenu->CheckBox("Use W to harass", true);
		HarassE = HarassMenu->CheckBox("Use E to harass", true);
		HarassMode = HarassMenu->AddSelection("Harass Type", 0, HarassType);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawDamage = DrawingMenu->CheckBox("Draw Killable", true);
		DrawFill = DrawingMenu->CheckBox("Fill damage", true);
		DrawEFlash = DrawingMenu->CheckBox("Draw E Flash Range", true);
	}
	FarmMenu = MainMenu->AddMenu("Jungle/Lane Clear");
	{
		FarmMana = FarmMenu->AddInteger("Mana percent for clear", 10, 100, 50);
		FarmQ = FarmMenu->CheckBox("Clear with Q", true);
		FarmW = FarmMenu->CheckBox("Clear with W", true);
		FarmE = FarmMenu->CheckBox("Clear with E", true);
		FarmEmin = FarmMenu->AddInteger("E hit X minions", 1, 6, 2);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSR = KillstealMenu->CheckBox("Use R", true);
	}
}

void EFlash()
{
	GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Flash != nullptr)
		{
			if (Enemy->IsValidTarget() && Enemy != nullptr && ((Player->GetPosition() - Enemy->GetPosition()).Length()) < (E->Range() * 2) + 350 && !Enemy->IsDead())
			{

				if (E->IsReady() && Player->IsTargetable() && ((Player->GetPosition() - Enemy->GetPosition()).Length()) > (E->Range() * 2) - 20)
				{
					E->CastOnPosition(Enemy->ServerPosition());
				}
				if (!Player->IsTargetable() && Flash->IsReady())
				{
					Flash->CastOnPosition(Enemy->ServerPosition());
				}
				if (R->IsReady() && !Flash->IsReady() && !Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()))
				{
					R->CastOnTarget(Enemy);
				}
				if (W->IsReady() && !R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), 500))
				{
					W->CastOnPlayer();
				}
				if (Q->IsReady() && !W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnTarget(Enemy);
				}
			}
		}
	}
}


float GetDistance(IUnit* source, IUnit* target)
{
	auto x1 = source->GetPosition().x;
	auto x2 = target->GetPosition().x;
	auto y1 = source->GetPosition().y;
	auto y2 = target->GetPosition().y;
	auto z1 = source->GetPosition().z;
	auto z2 = target->GetPosition().z;
	return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0)));
}

void CastE(IUnit* hit)
{
	auto time = 0.25f + GetDistance(Player, hit) / 1550;
	Vec3 futurePos;
	GPrediction->GetFutureUnitPosition(hit, time, true, futurePos);
	Vec2 hitpos = futurePos.To2D().Extend(Player->GetPosition().To2D(), -40);
	Vec3 Hithere;
	Hithere.x = hitpos.x;
	Hithere.y = futurePos.y;
	Hithere.z = hitpos.y;
	E->CastOnPosition(Hithere);
}
void CastR(IUnit* hit)
{
	auto time = 0.25f + GetDistance(Player, hit) / 1550;
	Vec3 futurePos;
	GPrediction->GetFutureUnitPosition(hit, time, true, futurePos);
	Vec2 hitpos = futurePos.To2D().Extend(Player->GetPosition().To2D(), -40);
	Vec3 Hithere;
	Hithere.x = hitpos.x;
	Hithere.y = futurePos.y;
	Hithere.z = hitpos.y;
	R->CastOnPosition(Hithere);
}


void dmgdraw()
{
	if (!DrawDamage->Enabled())
		return;
	for (auto hero : GEntityList->GetAllHeros(false, true))
	{
		Vec2 barPos = Vec2();
		if (hero->GetHPBarPosition(barPos) && !hero->IsDead())
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
			auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
			auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR);
			auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotW);
			float percentHealthAfterDamage = max(0, hero->GetHealth() - float(WDamage + RDamage + EDamage + QDamage)) / hero->GetMaxHealth();
			float yPos = barPos.y + yOffset;
			float xPosDamage = (barPos.x + xOffset) + Width * percentHealthAfterDamage;
			float xPosCurrentHp = barPos.x + xOffset + Width * (hero->GetHealth() / hero->GetMaxHealth());

			if (WDamage + RDamage + EDamage + QDamage > hero->GetHealth())
			{
				GRender->DrawTextW(Vec2(barPos.x + xOffset, barPos.y + yOffset - 13), Color, "Killable");
			}
			if (DrawFill->Enabled() && !hero->IsDead() && hero->IsValidTarget())
			{
				float differenceInHP = xPosCurrentHp - xPosDamage;
				float pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

				for (int i = 0; i < differenceInHP; i++)
				{
					GRender->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + Height), FillColor);
				}
				if (!hero->IsVisible())
				{

				}
			}
		}
	}
}

/*void ComboGap(IUnit* Enemy)
{
	GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
	for (auto target : GEntityList->GetAllMinions(false, true, true))
	{
		if ((Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() < Q->Range())
		{
			return;
		}
		else if (target->IsValidTarget(GEntityList->Player(), Q->Range()) && (Enemy->GetPosition() - target->GetPosition()).Length() < R->Range()-250)
		{
			Q->CastOnTarget(target);
		}

	}
	
}*/

static void OnCreateObject(IUnit* Source)
{

	if (strstr(Source->GetObjectName(), "Fizz_Base_W_DmgMarkerMaintain.troy"))
	{
		Wproc = 1;
	}
}

static void OnDeleteObject(IUnit* Source)
{
	if (strstr(Source->GetObjectName(), "Fizz_Base_W_DmgMarkerDecay.troy") || strstr(Source->GetObjectName(), "Fizz_Base_W_DmgMarker_champion.troy"))
	{
		Wproc = 0;
	}
}

void Combo()
{
	/*for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
		auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);
		auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);
		auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotW);
		if (QDamage + RDamage + EDamage + WDamage > Enemy->GetHealth() && ComboQgap->Enabled())
		{

			auto Etarget = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, E->Range());
			if (R->IsReady() && Q->IsReady() && (Enemy->GetPosition() - Player->GetPosition()).Length() < ((R->Range() + Q->Range()) - 200) && (Enemy->GetPosition() - Player->GetPosition()).Length() > Q->Range())
			{
				ComboGap(Enemy);
			}
			if (R->IsReady() && !Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range() - 150))
			{
				R->CastOnTarget(Enemy);
			}
			if (!R->IsReady() && E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(Enemy, E->Range()) && E->IsReady())
			{
				E->CastOnTarget(Etarget);
			}
			if (!Player->IsTargetable() && E->IsReady() && GEntityList->Player()->IsValidTarget(Enemy, E->Range()))
			{
				E->CastOnTarget(Etarget);
			}
			if (!E->IsReady() && W->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), 500))
			{
				W->CastOnPlayer();
			}
		}
	}*/

	// What a mess >.>

	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
	if (target != nullptr && !target->IsInvulnerable() && !target->IsDead())
	{

		if (R->IsReady())
		{
			auto targetE = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range() * 2);
			if ((target->GetPosition() - Player->GetPosition()).Length() > Q->Range() && target->IsValidTarget(GEntityList->Player(), R->Range() - 150))
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
				auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
				if (QDamage + EDamage + RDamage + WDamage > target->GetHealth())
				{
					R->CastOnTarget(target, kHitChanceHigh);
				}
			}
			if (!Q->IsReady() && target->IsValidTarget(GEntityList->Player(), R->Range()))
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
				auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
				if (QDamage + EDamage + RDamage + WDamage > target->GetHealth())
				{
					R->CastOnTarget(target, kHitChanceHigh);
				}
			}
			if (ComboRmode->GetInteger() == 0)
			{
				if (ComboRenable->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && ComboRhp->GetInteger() > target->HealthPercent() && !ComboRkill->Enabled())
				{
					R->CastOnTarget(target, kHitChanceHigh);
				}
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
				auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);

				if (ComboRenable->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && ComboRkill->Enabled())
				{
					if (QDamage + EDamage + RDamage + WDamage > target->GetHealth())
					{
						R->CastOnTarget(target, kHitChanceHigh);
					}
				}
				if (ComboRenable->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && !ComboRkill->Enabled() && ComboRalways->Enabled() && !ComboRkill->Enabled())
				{
					R->CastOnTarget(target, kHitChanceHigh);
				}

				if (ComboQenable->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnTarget(target);
				}
				if (!ComboWproc->Enabled() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), 500))
				{
					W->CastOnPlayer();
				}
				if (ComboWproc->Enabled() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), 350) && Wproc == 1)
				{
					if (W->CastOnPlayer())
					{
						Wproc = 0;
					}
				}
				auto targetEa = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 800);
				if (targetEa != nullptr)
				{
					if (SaveE->Enabled())
					{
						if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, Q->Range()-10) && ComboEenable->Enabled())
						{
							E->CastOnPosition(targetEa->ServerPosition());
						}
					}
					if (!SaveE->Enabled())
					{
						if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, 800) && ComboEenable->Enabled())
						{
							E->CastOnPosition(targetEa->ServerPosition());
						}
					}

					if (!Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, E->Range()) && ComboEenable->Enabled())
					{

						CastE(targetEa);
					}
				}

			}
			if (ComboRmode->GetInteger() == 1)
			{
				if (ComboQenable->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					if (Q->CastOnTarget(target))
					{
						if (ComboRenable->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && ComboRhp->GetInteger() > target->HealthPercent() && !ComboRkill->Enabled())
						{
							R->CastOnTarget(target);
						}
						auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
						auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
						auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
						auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);

						if (ComboRenable->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && ComboRkill->Enabled())
						{
							if (QDamage + EDamage + RDamage + WDamage > target->GetHealth())
							{
								R->CastOnTarget(target);
							}
						}
						if (ComboRenable->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && !ComboRkill->Enabled() && ComboRalways->Enabled() && !ComboRkill->Enabled())
						{
							R->CastOnTarget(target, kHitChanceHigh);
						}
					}
				}
				if (!ComboWproc->Enabled() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), 500))
				{
					W->CastOnPlayer();
				}
				if (ComboWproc->Enabled() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), 350) && Wproc == 1)
				{
					if (W->CastOnPlayer())
					{
						Wproc = 0;
					}
				}

				auto targetEa = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 800);
				if (targetEa != nullptr)
				{
					if (SaveE->Enabled())
					{
						if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, Q->Range() - 10) && ComboEenable->Enabled())
						{
							E->CastOnPosition(targetEa->ServerPosition());
						}
					}
					if (!SaveE->Enabled())
					{
						if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, 800) && ComboEenable->Enabled())
						{
							E->CastOnPosition(targetEa->ServerPosition());
						}
					}

					if (!Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, E->Range()) && ComboEenable->Enabled())
					{

						CastE(targetEa);
					}
				}
			}
			if (ComboRmode->GetInteger() == 2)
			{
				if (ComboWproc->Enabled() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), 350) && Wproc == 1)
				{
					if (W->CastOnPlayer())
					{
						Wproc = 0;
					}
				}
				if (!ComboWproc->Enabled() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range() + 20))
				{
					W->CastOnPlayer();
				}
				if (ComboQenable->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					if (Q->CastOnTarget(target))
					{
						if (ComboRenable->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && ComboRhp->GetInteger() > target->HealthPercent() && !ComboRkill->Enabled())
						{
							R->CastOnTarget(target, kHitChanceHigh);
						}
						auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
						auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE);
						auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
						auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);

						if (ComboRenable->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range()) && ComboRkill->Enabled())
						{
							if (QDamage + EDamage + RDamage + WDamage > target->GetHealth())
							{
								R->CastOnTarget(target, kHitChanceHigh);
							}
						}
						if (ComboRenable->Enabled() && target->IsValidTarget(GEntityList->Player(), R->Range() - 150) && !ComboRkill->Enabled() && ComboRalways->Enabled() && !ComboRkill->Enabled())
						{
							R->CastOnTarget(target, kHitChanceHigh);
						}
					}
				}
				auto targetEa = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 800);
				if (targetEa != nullptr)
				{
					if (SaveE->Enabled())
					{
						if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, Q->Range() - 10) && ComboEenable->Enabled())
						{
							E->CastOnPosition(targetEa->ServerPosition());
						}
					}
					if (!SaveE->Enabled())
					{
						if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, 800) && ComboEenable->Enabled())
						{
							E->CastOnPosition(targetEa->ServerPosition());
						}
					}

					if (!Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, E->Range()) && ComboEenable->Enabled())
					{

						CastE(targetEa);
					}
				}
			}
		}
	}


	if (!R->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && !target->IsInvulnerable() && !target->IsDead())
		{
			if (Q->IsReady() && !ComboQsave->Enabled() && ComboQenable->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				Q->CastOnTarget(target);
			}
			if (Q->IsReady() && ComboQsave->Enabled() && ComboQenable->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);
				auto WDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotW);
				if (W->IsReady() && QDamage + WDamage >= target->GetHealth())
				{
					if (W->CastOnPlayer())
					{
						Q->CastOnTarget(target);
					}

				}
				if (!W->IsReady() && QDamage >= target->GetHealth())
				{
					Q->CastOnTarget(target);
				}
				if ((target->GetPosition() - Player->GetPosition()).Length() > 260)
				{
					Q->CastOnTarget(target);
				}
				if (target->IsDashing())
				{
					Q->CastOnTarget(target);
				}
				if (15 > target->HealthPercent())
				{
					Q->CastOnTarget(target);
				}
			}
			if (!ComboWproc->Enabled())
			{
				if (ComboW->Enabled() && W->IsReady() && target->IsValidTarget(GEntityList->Player(), 500))
				{
					W->CastOnPlayer();
				}
			}
			if (ComboWproc->Enabled() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), 350) && Wproc == 1)
			{
				if (W->CastOnPlayer())
				{
					Wproc = 0;
				}
			}

		}
		auto targetEa = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 800);
		if (targetEa != nullptr)
		{
			if (SaveE->Enabled())
			{
				if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, Q->Range() - 10) && ComboEenable->Enabled())
				{
					E->CastOnPosition(targetEa->ServerPosition());
				}
			}
			if (!SaveE->Enabled())
			{
				if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, 800) && ComboEenable->Enabled())
				{
					E->CastOnPosition(targetEa->ServerPosition());
				}
			}

			if (!Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, E->Range()) && ComboEenable->Enabled())
			{

				CastE(targetEa);
			}
		}
	}
}


void Mixed()
{
	if (HarassMana->GetInteger() < Player->ManaPercent())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && !target->IsInvulnerable() && !target->IsDead())
		{
			if (HarassMode->GetInteger() == 0)
			{
				if (HarassW->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range() + 10))
				{
					if (W->CastOnPlayer())
					{
						Q->CastOnTarget(target);
					}
				}
				auto targetEa = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 800);
				if (targetEa != nullptr)
				{
					if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, 800) && HarassE->Enabled())
					{
						E->CastOnPosition(targetEa->ServerPosition());
					}

					if (!Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, E->Range()) && HarassE->Enabled())
					{

						CastE(targetEa);
					}
				}
			}
		}

		if (HarassMode->GetInteger() == 1)
		{
			auto targetEa = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 800);
			if (targetEa != nullptr)
			{
				if (E->IsReady() && Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, 800) && HarassE->Enabled())
				{
					E->CastOnPosition(targetEa->ServerPosition());
				}

				if (!Player->IsTargetable() && GEntityList->Player()->IsValidTarget(targetEa, E->Range()) && HarassE->Enabled())
				{
					E->CastOnTarget(targetEa, kHitChanceHigh);

				}
			}
			if (!E->IsReady() && HarassE->Enabled())
			{
				if (HarassW->Enabled() && targetEa->IsValidTarget(GEntityList->Player(), Q->Range() + 10))
				{
					W->CastOnPlayer();
				}
				if (HarassQ->Enabled() && targetEa->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnTarget(targetEa);
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
		auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);

		if (Enemy != nullptr && !Enemy->IsDead())
		{
			if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()-150) && RDamage > Enemy->GetHealth())
			{
				R->CastOnTarget(Enemy, kHitChanceHigh);
			}
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				Q->CastOnTarget(Enemy);
			}

		}

	}
}
void Semi()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (!Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), R->Range() - 150) && R->Range() - 150 && !Enemy->IsDead())
		{
			CastR(Enemy);
		}
	}
}


void Farm()
{
	if (Player->ManaPercent() > FarmMana->GetInteger())
	{
		for (auto Minion : GEntityList->GetAllMinions(false, true, true))
		{
			if (!Minion->IsDead() && Minion != nullptr)
			{
				if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnUnit(Minion);
				}
				if (Player->IsTargetable() && E->IsReady() && FarmE->Enabled() && GEntityList->Player()->IsValidTarget(Minion, E->Range()+100))
					{
					E->CastOnPosition(Minion->ServerPosition());
					}
				if (!Player->IsTargetable() && GEntityList->Player()->IsValidTarget(Minion, E->Range()) && FarmE->Enabled())
				{
					E->CastOnUnit(Minion);
				}
				if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 300))
				{
					if (W->CastOnPlayer())
					{
						GOrbwalking->SetOverrideTarget(Minion);
						return;
					}
				}

			}
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
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Mixed();
	}
	if (GetAsyncKeyState(ComboE1->GetInteger()) & 0x8000)
	{
		EFlash();
	}
	if (GetAsyncKeyState(ComboSemiManual->GetInteger()) & 0x8000)
	{
		Semi();
	}
	Killsteal();
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawEFlash->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), (E->Range()*2)+350); }
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
	GEventManager->AddEventHandler(kEventOnCreateObject, OnCreateObject);
	GEventManager->AddEventHandler(kEventOnDestroyObject, OnDeleteObject);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnCreateObject, OnCreateObject);
	GEventManager->RemoveEventHandler(kEventOnDestroyObject, OnDeleteObject);
}