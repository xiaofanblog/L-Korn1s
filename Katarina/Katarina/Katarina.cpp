#include "PluginSDK.h"
#include <string>

PluginSetup("Katarina")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboMode;
IMenuOption* ComboE;
IMenuOption* SaveE;
IMenuOption* ComboR;
IMenu* ComboRset;
IMenuOption* ComboRkill;
IMenuOption* ComboRalways;
IMenuOption* ComboRhit;
IMenuOption* ComboRstack;

IMenuOption* ComboRcancelks;
IMenuOption* ComboRcancel;
IMenuOption* ComboRcheck;


IMenu* HarassMenu;
IMenuOption* HarassQ; 
IMenuOption* HarassMode;
IMenuOption* HarassE;
IMenuOption* HarassW;



IMenuOption* HarassAuto;
IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;
IMenuOption* DrawDaggers;
IMenuOption* Drawdamage;
IMenuOption* Drawtoggle;


IMenu* FleeMenu;
IMenuOption* FleeW;
IMenuOption* FleeE;
IMenuOption* FleeKey;
IMenuOption* FleeED;
IMenuOption* DrawFleeRad;
IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmEmin;
IMenuOption* FarmW;
IMenuOption* FarmWmin;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSE;
IMenuOption* KSR;
IMenuOption* KSRgap;

IMenu* LastMenu;
IMenuOption* LastQ;

float KeyPres;

bool stuff = false;
double damages = 0;
bool Harassenable = false;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;


int wdelay;

IUnit* enemi;


IUnit* Player;
IUnit* dmga = nullptr;

IInventoryItem* Gunblade;
IInventoryItem* Cutlass;

int hello;

int xOffset = 10;
int yOffset = 20;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(25, 255, 0, 200);

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, (kCollidesWithNothing));

	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, (kCollidesWithNothing));

	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, (kCollidesWithNothing));

	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, (kCollidesWithYasuoWall));
	Gunblade = GPluginSDK->CreateItemForId(3146, 700);
	Cutlass = GPluginSDK->CreateItemForId(3144, 550);
}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Katarina by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboMode = ComboMenu->AddSelection("Combo Mode", 0, { "QE", "EQ" });
		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboW = ComboMenu->CheckBox("Use W", true);
		ComboE = ComboMenu->CheckBox("Use E", true);
		SaveE = ComboMenu->CheckBox("Save E if no Daggers", false);
		ComboRset = ComboMenu->AddMenu("R Settings");
		ComboR = ComboRset->CheckBox("Use R", true);
		ComboRalways = ComboRset->CheckBox("R Always", false);
		ComboRkill = ComboRset->CheckBox("R Only If Killable", true);
		ComboRstack = ComboRset->AddFloat("X R Daggers to check damage", 1, 15, 8);
		ComboRhit = ComboRset->AddFloat("R If Hits", 1, 5, 1);
		ComboRcancel = ComboRset->CheckBox("Cancel R if no enemies", true);
		ComboRcheck = ComboRset->AddFloat("Dont waste R if Enemy HP lower than", 0, 500, 100);
		ComboRcancelks = ComboRset->CheckBox("Cancel R for KS", true);


	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassMode = HarassMenu->AddSelection("Harass Mode", 0, { "QE", "EQ" });
		HarassAuto = HarassMenu->AddKey("Auto Q Toggle", 'T');
		HarassQ = HarassMenu->CheckBox("Use Q", true);
		HarassW = HarassMenu->CheckBox("Use W", true);
		HarassE = HarassMenu->CheckBox("Use E", true);
	}

	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmQ = FarmMenu->CheckBox("Lane Clear with Q", true);
		FarmW = FarmMenu->CheckBox("Lane Clear with W", true);
		FarmWmin = FarmMenu->AddFloat("^- If Hits ", 1, 6, 3);
		FarmE = FarmMenu->CheckBox("Lane Clear with E", true);
		FarmEmin = FarmMenu->AddFloat("^- If Dagger hits", 1, 6, 3);
	}

	LastMenu = MainMenu->AddMenu("Last Hit");
	{
		LastQ = LastMenu->CheckBox("Last hit Q", true);
	}
	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Killsteal with Q", true);
		KSE = KillstealMenu->CheckBox("Killsteal with E", true);
		KSR = KillstealMenu->CheckBox("^- Killsteal with E Dagger", true);
		KSRgap = KillstealMenu->CheckBox("Killsteal with E gap for Q", true);
	}
	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		DrawDaggers = DrawingMenu->CheckBox("Draw Daggers", true);
		Drawdamage = DrawingMenu->CheckBox("Draw Damage", true);
		Drawtoggle = DrawingMenu->CheckBox("Draw Toggle", true);
		DrawFleeRad = DrawingMenu->CheckBox("Draw Radius on Flee", true);
	}
	FleeMenu = MainMenu->AddMenu("Flee");
	{
		FleeKey = FleeMenu->AddKey("Flee", 'G');
		FleeW = FleeMenu->CheckBox("Flee with W", true);
		FleeE = FleeMenu->CheckBox("Flee with E", true);
		FleeED = FleeMenu->CheckBox("^- Use E on Daggers", true);
	}
}

void Flee()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		GGame->IssueOrderEx(GEntityList->Player(), kMoveTo, GGame->CursorPosition(), false);
		if (FleeW->Enabled() && W->IsReady())
		{
			W->CastOnPlayer();
		}
		if (FleeE->Enabled() && E->IsReady())
		{
			for (auto target : GEntityList->GetAllHeros(true, false))
			{
				if (target != GEntityList->Player())
				{
					if (target->IsValidTarget() && target->IsValidTarget(GEntityList->Player(), E->Range()) && !target->IsDead())
					{
						if ((target->GetPosition() - GGame->CursorPosition()).Length2D() < 200)
						{
							E->CastOnUnit(target);
						}
					}
				}
			}
			for (auto Minion : GEntityList->GetAllMinions(true, true, true))
			{
				if (!Minion->IsDead() && Minion != nullptr && Minion->IsValidTarget())
				{
					if ((Minion->GetPosition() - GGame->CursorPosition()).Length2D() < 200)
					{
						E->CastOnUnit(Minion);
					}
				}
			}
		}
		if (FleeED->Enabled())
		{
			for (auto dagger : GEntityList->GetAllUnits())
			{
				if (std::string(dagger->GetObjectName()) == "HiddenMinion")
				{
					if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && dagger != nullptr && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
					{

						if ((dagger->GetPosition() - GGame->CursorPosition()).Length2D() < 200)
						{
							E->CastOnPosition(dagger->GetPosition());
						}
					}
				}
			}
		}

	}
}
void HarasTog()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		if (GUtility->IsKeyDown(HarassAuto->GetInteger()))
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
void psvdmg()
{
	for (auto target : GEntityList->GetAllHeros(false, true))
	{
		for (auto dagger : GEntityList->GetAllUnits())
		{
			if (std::string(dagger->GetObjectName()) == "HiddenMinion")
			{
				if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && dagger != nullptr && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
				{
					if ((target->GetPosition() - dagger->GetPosition()).Length2D() < 450)
					{
						double basepsv = 35;
						double psvlvl = 0;
						if (GEntityList->Player()->GetLevel() >= 1 && GEntityList->Player()->GetLevel() < 6)
						{
							psvlvl = 0.55;
						}
						if (GEntityList->Player()->GetLevel() >= 6 && GEntityList->Player()->GetLevel() < 11)
						{
							psvlvl = 0.7;
						}
						if (GEntityList->Player()->GetLevel() >= 11 && GEntityList->Player()->GetLevel() < 16)
						{
							psvlvl = 0.85;
						}
						if (GEntityList->Player()->GetLevel() >= 16)
						{
							psvlvl = 1;
						}
						auto psv = basepsv + (GEntityList->Player()->GetLevel() * 12);
						auto ap = GEntityList->Player()->TotalMagicDamage();
						auto psvdmg = GEntityList->Player()->TotalMagicDamage()*psvlvl;
						auto psvdmgad = GEntityList->Player()->BonusDamage() * 1;
						auto full = psvdmg + psv + psvdmgad;
						
						damages = GDamage->CalcMagicDamage(GEntityList->Player(), target, full);
					}
				}
				else damages = 0;
			}
		}
	}

}
inline int CountEnemy(Vec3 Location, int range)
{
	int Count = 0;

	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if ((Enemy->GetPosition() - Location).Length2D() < range && Enemy->IsValidTarget() && !Enemy->IsDead())
		{
			Count++;
		}
	}
	return (Count);
}

inline Vec3 Extend(Vec3 from, Vec3 to, float distance)
{
	float realDistance = (from - to).Length() * distance / (from - to).Length2D();
	auto direction = (to - from).VectorNormalize();
	return from + direction * realDistance;
}


void Auto()
{
	float endtime;
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
						if (!GEntityList->Player()->IsCastingImportantSpell(&endtime))
						{
							Q->CastOnTarget(Enemy);
						}
					}
				}

			}
		}
	}
}
void Combo()
{
	float endtime;
	if (!ComboRcancelks->Enabled())
	{
		if (Gunblade->IsOwned() && Gunblade->IsReady() && !GEntityList->Player()->IsDead() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{
			if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Gunblade->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
			{
				Gunblade->CastOnTarget(GTargetSelector->GetFocusedTarget());
			}
			else
			{
				Gunblade->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 700));
			}
		}
		if (Cutlass->IsOwned() && Cutlass->IsReady() && !GEntityList->Player()->IsDead() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{
			if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Cutlass->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
			{
				Cutlass->CastOnTarget(GTargetSelector->GetFocusedTarget());
			}
			else
			{
				Cutlass->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 550));
			}
		}
		if (ComboMode->GetInteger() == 0 && GGame->TickCount() > hello)
		{
			if (ComboQ->Enabled() && Q->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					Q->CastOnTarget(target);
				}
			}
			if (ComboW->Enabled() && W->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 350);
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					W->CastOnPlayer();
				}

			}
			if (ComboE->Enabled() && E->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				IUnit* Dagger = nullptr;
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					for (auto dagger : GEntityList->GetAllUnits())
					{
						if (std::string(dagger->GetObjectName()) == "HiddenMinion")
						{
							if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
							{
								if (dagger != nullptr)
								{
									Dagger = dagger;
									stuff = true;
									if (CountEnemy(dagger->GetPosition(), 450) != 0 && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
									{


										auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
										E->CastOnPosition(ext);
									}


								}

							}
						}
						if (std::string(dagger->GetObjectName()) != "HiddenMinion")
						{
							stuff = false;
						}
					}
					if (!SaveE->Enabled() && Dagger != nullptr && (GEntityList->Player()->GetPosition() - Dagger->GetPosition()).Length2D() > E->Range() && !Q->IsReady())
					{
						E->CastOnTarget(target);
					}
					if (!SaveE->Enabled() && stuff == false && Dagger == nullptr && !Q->IsReady())
					{
						E->CastOnTarget(target);
					}

				}
			}
		}
		if (ComboMode->GetInteger() == 1 && GGame->TickCount() > hello)
		{
			if (ComboE->Enabled() && E->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					IUnit* Dagger = nullptr;
					for (auto dagger : GEntityList->GetAllUnits())
					{

						if (std::string(dagger->GetObjectName()) == "HiddenMinion")
						{
							if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
							{
								if (dagger != nullptr)
								{
									Dagger = dagger;
									stuff = true;
									if (CountEnemy(dagger->GetPosition(), 450) != 0 && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
									{


										auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
										E->CastOnPosition(ext);
									}


								}

							}
						}
						if (std::string(dagger->GetObjectName()) != "HiddenMinion")
						{
							stuff = false;
						}
					}
					if (!SaveE->Enabled() && Dagger != nullptr && (GEntityList->Player()->GetPosition() - Dagger->GetPosition()).Length2D() > E->Range())
					{
						E->CastOnPosition(target->GetPosition().Extend(GEntityList->Player()->GetPosition(), 50));
					}
					if (!SaveE->Enabled() && stuff == false && Dagger == nullptr)
					{
						E->CastOnPosition(target->GetPosition().Extend(GEntityList->Player()->GetPosition(), 50));
					}
				}
			}
			if (ComboW->Enabled() && W->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 350);
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					W->CastOnPlayer();
				}

			}
			if (ComboQ->Enabled() && Q->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime) && !E->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					Q->CastOnTarget(target);
				}
			}
		}
	}
	if (ComboRcancelks->Enabled())
	{
		if (Gunblade->IsOwned() && Gunblade->IsReady() && !GEntityList->Player()->IsDead() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{
			if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Gunblade->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
			{
				Gunblade->CastOnTarget(GTargetSelector->GetFocusedTarget());
			}
			else
			{
				Gunblade->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 700));
			}
		}
		if (Cutlass->IsOwned() && Cutlass->IsReady() && !GEntityList->Player()->IsDead() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{
			if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Cutlass->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
			{
				Cutlass->CastOnTarget(GTargetSelector->GetFocusedTarget());
			}
			else
			{
				Cutlass->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 550));
			}
		}
		if (ComboMode->GetInteger() == 0 && GGame->TickCount() > hello)
		{
			if (ComboQ->Enabled() && Q->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					Q->CastOnTarget(target);
				}
			}
			if (ComboW->Enabled() && W->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 350);
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					W->CastOnPlayer();
				}

			}
			if (ComboE->Enabled() && E->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				IUnit* Dagger = nullptr;
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					for (auto dagger : GEntityList->GetAllUnits())
					{
						if (std::string(dagger->GetObjectName()) == "HiddenMinion")
						{
							if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
							{
								if (dagger != nullptr)
								{
									Dagger = dagger;
									stuff = true;
									if (CountEnemy(dagger->GetPosition(), 450) != 0 && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
									{


										auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
										E->CastOnPosition(ext);
									}


								}

							}
						}
						if (std::string(dagger->GetObjectName()) != "HiddenMinion")
						{
							stuff = false;
						}
					}
					if (!SaveE->Enabled() && Dagger != nullptr && (GEntityList->Player()->GetPosition() - Dagger->GetPosition()).Length2D() > E->Range() && !Q->IsReady())
					{
						E->CastOnTarget(target);
					}
					if (!SaveE->Enabled() && stuff == false && Dagger == nullptr && !Q->IsReady())
					{
						E->CastOnTarget(target);
					}
				}
			}
		}
		if (GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);

				if (Enemy->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					IUnit* Dagger = nullptr;
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
					if (target != nullptr && target->IsValidTarget() && !target->IsDead())
					{
						for (auto dagger : GEntityList->GetAllUnits())
						{
							if (std::string(dagger->GetObjectName()) == "HiddenMinion")
							{
								if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
								{
									if (dagger != nullptr)
									{
										Dagger = dagger;
										stuff = true;
										if (damages > target->GetHealth() && E->IsReady() && (dagger->GetPosition() - target->GetPosition()).Length2D() < 450 && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
										{

											auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
											E->CastOnPosition(ext);
										}


									}

								}
							}
							if (std::string(dagger->GetObjectName()) != "HiddenMinion")
							{
								stuff = false;
							}
						}
						if (!SaveE->Enabled() && Dagger != nullptr && (GEntityList->Player()->GetPosition() - Dagger->GetPosition()).Length2D() > E->Range() && !Q->IsReady())
						{
							E->CastOnTarget(target);
						}
						if (!SaveE->Enabled() && stuff == false && Dagger == nullptr && !Q->IsReady())
						{
							E->CastOnTarget(target);
						}

					}
				}
				if (Q->IsReady() && QDamage >= Enemy->GetHealth() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnTarget(Enemy);
				}
			}
		}
		if (ComboMode->GetInteger() == 1 && GGame->TickCount() > hello)
		{
			if (ComboW->Enabled() && W->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 400);
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					W->CastOnPlayer();
				}

			}
			if (ComboE->Enabled() && E->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					IUnit* Dagger = nullptr;
					for (auto dagger : GEntityList->GetAllUnits())
					{

						if (std::string(dagger->GetObjectName()) == "HiddenMinion")
						{
							if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
							{
								if (dagger != nullptr)
								{
									Dagger = dagger;
									stuff = true;
									if (CountEnemy(dagger->GetPosition(), 450) != 0 && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
									{


										auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
										E->CastOnPosition(ext);
									}

								}


							}
						}
						if (std::string(dagger->GetObjectName()) != "HiddenMinion")
						{
							stuff = false;
						}
					}
					if (!SaveE->Enabled() && Dagger != nullptr && (GEntityList->Player()->GetPosition() - Dagger->GetPosition()).Length2D() > E->Range())
					{
						E->CastOnPosition(target->GetPosition().Extend(GEntityList->Player()->GetPosition(), 50));
					}
					if (!SaveE->Enabled() && stuff == false && Dagger == nullptr)
					{
						E->CastOnPosition(target->GetPosition().Extend(GEntityList->Player()->GetPosition(), 50));
					}
				}
			}

			if (ComboQ->Enabled() && Q->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime) && !E->IsReady())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					Q->CastOnTarget(target);
				}
			}
		}
		if (GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{

			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
				auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);

				if (Enemy->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					IUnit* Dagger = nullptr;
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
					if (target != nullptr && target->IsValidTarget() && !target->IsDead())
					{
						for (auto dagger : GEntityList->GetAllUnits())
						{
							if (std::string(dagger->GetObjectName()) == "HiddenMinion")
							{
								if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
								{
									if (dagger != nullptr)
									{
										Dagger = dagger;
										stuff = true;
										if (damages > target->GetHealth() && E->IsReady() && (dagger->GetPosition() - target->GetPosition()).Length2D() < 450 && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
										{

											auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
											E->CastOnPosition(ext);
										}


									}

								}
							}
							if (std::string(dagger->GetObjectName()) != "HiddenMinion")
							{
								stuff = false;
							}
						}
						if (EDamage >= Enemy->GetHealth() && Dagger != nullptr && (GEntityList->Player()->GetPosition() - Dagger->GetPosition()).Length2D() > E->Range() && !Q->IsReady())
						{
							E->CastOnTarget(target);
						}
						if (EDamage >= Enemy->GetHealth() && stuff == false && Dagger == nullptr && !Q->IsReady())
						{
							E->CastOnTarget(target);
						}

					}
				}
				if (Q->IsReady() && QDamage >= Enemy->GetHealth() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnTarget(Enemy);
				}
			}
		}
	}
	if (ComboR->Enabled() && R->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range() - 100);
		if (target != nullptr && target->IsValidTarget() && !target->IsDead())
		{
			if (ComboRalways->Enabled() && target->GetHealth() > ComboRcheck->GetFloat() && (!W->IsReady() || CountEnemy(GEntityList->Player()->GetPosition(), 350) == 0) && !Q->IsReady())
			{
				R->CastOnPlayer();
				hello = GGame->TickCount() + 100;
			}
			if (ComboRkill->Enabled() && ComboRhit->GetFloat() <= CountEnemy(GEntityList->Player()->GetPosition(), R->Range()) && (!W->IsReady() || CountEnemy(GEntityList->Player()->GetPosition(), 350) == 0) && !Q->IsReady())
			{
				auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotR);
				if ((RDamage * ComboRstack->GetFloat()) >= target->GetHealth() && target->GetHealth() > ComboRcheck->GetFloat())
				{
					R->CastOnPlayer();
					hello = GGame->TickCount() + 100;
				}
			}
		}
	}
	if (GEntityList->Player()->IsCastingImportantSpell(&endtime) && CountEnemy(GEntityList->Player()->GetPosition(), R->Range()) == 0)
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
		if (target != nullptr && target->IsValidTarget() && !target->IsDead())
		{
			IUnit* Dagger = nullptr;
			for (auto dagger : GEntityList->GetAllUnits())
			{

				if (std::string(dagger->GetObjectName()) == "HiddenMinion")
				{
					if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
					{
						if (dagger != nullptr)
						{
							Dagger = dagger;
							stuff = true;
							if (CountEnemy(dagger->GetPosition(), 450) && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
							{

								auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
								E->CastOnPosition(ext);
							}


						}

					}
				}
				if (std::string(dagger->GetObjectName()) != "HiddenMinion")
				{
					stuff = false;
				}
			}
			if (Dagger != nullptr && (GEntityList->Player()->GetPosition() - Dagger->GetPosition()).Length2D() > E->Range())
			{
				E->CastOnTarget(target);
			}
			if (stuff == false && Dagger == nullptr)
			{
				E->CastOnTarget(target);
			}
		}
		else GGame->IssueOrderEx(GEntityList->Player(), kMoveTo, GGame->CursorPosition(), false);
	}
}
void LastHit()
{
	for (auto Minion : GEntityList->GetAllMinions(false, true, true))
	{
		if (!Minion->IsDead() && Minion != nullptr)
		{
			if (LastQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
			{
				Q->CastOnUnit(Minion);
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
		auto passive = damages;

		if (Enemy != nullptr && !Enemy->IsDead() & Enemy->IsValidTarget())
		{
			if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
			{
				E->CastOnTarget(Enemy);
			}
			if (E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && passive > Enemy->GetHealth() && KSE->Enabled())
			{
				IUnit* Dagger = nullptr;
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (target != nullptr && target->IsValidTarget() && !target->IsDead())
				{
					for (auto dagger : GEntityList->GetAllUnits())
					{
						if (std::string(dagger->GetObjectName()) == "HiddenMinion")
						{
							if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
							{
								if (dagger != nullptr)
								{
									Dagger = dagger;
									stuff = true;
									if ((dagger->GetPosition() - target->GetPosition()).Length2D() < 450 && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
									{

										auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
										E->CastOnPosition(ext);
									}


								}

							}
						}

					}
				}
			
			}
			if (KSRgap->Enabled() && Q->IsReady() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()+Q->Range()) && QDamage > Enemy->GetHealth())
			{
				for (auto minionQ : GEntityList->GetAllMinions(false, true, true))
				{
					auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), minionQ, kSlotQ);
					float myDistance = (GEntityList->Player()->GetPosition() - Enemy->GetPosition()).Length2D();
					if (myDistance < E->Range())
					{
						return;
					}
					else if (minionQ->IsValidTarget(GEntityList->Player(), E->Range()) && (Enemy->GetPosition() - minionQ->GetPosition()).Length() < Q->Range())
					{
						if (minionQ != nullptr)
						{
							E->CastOnUnit(minionQ);
							return;
						}
					}
				}
			}
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
			{
				Q->CastOnTarget(Enemy);
			}
		}
	}
}
void Mixed()
{
	float endtime;
	if (HarassMode->GetInteger() == 0)
	{
		if (HarassQ->Enabled() && Q->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				Q->CastOnTarget(target);
			}
		}
		if (HarassW->Enabled() && W->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 350);
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				W->CastOnPlayer();
			}

		}
		if (HarassE->Enabled() && E->IsReady() && !Q->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{
			IUnit* Dagger = nullptr;
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				for (auto dagger : GEntityList->GetAllUnits())
				{
					if (std::string(dagger->GetObjectName()) == "HiddenMinion")
					{
						if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
						{
							if (dagger != nullptr)
							{
								Dagger = dagger;
								stuff = true;
								if (CountEnemy(dagger->GetPosition(), 450) != 0 && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
								{

									auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
									E->CastOnPosition(ext);
								}


							}

						}
					}
					if (std::string(dagger->GetObjectName()) != "HiddenMinion")
					{
						stuff = false;
					}
				}
				if (Dagger != nullptr && (GEntityList->Player()->GetPosition() - Dagger->GetPosition()).Length2D() > E->Range() && !Q->IsReady())
				{
					E->CastOnTarget(target);
				}
				if (stuff == false && Dagger == nullptr && !Q->IsReady())
				{
					E->CastOnTarget(target);
				}
			}
		}
	}
	if (HarassMode->GetInteger() == 1)
	{
		if (HarassE->Enabled() && E->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
			if (target != nullptr && target->IsValidTarget() && !target->IsDead())
			{
				IUnit* Dagger = nullptr;
				for (auto dagger : GEntityList->GetAllUnits())
				{

					if (std::string(dagger->GetObjectName()) == "HiddenMinion")
					{
						if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
						{
							if (dagger != nullptr)
							{
								Dagger = dagger;
								stuff = true;
								if (CountEnemy(dagger->GetPosition(), 450) != 0 && stuff == true && (dagger->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() < E->Range())
								{

									auto ext = Extend(target->GetPosition(), Dagger->GetPosition(), 250);
									E->CastOnPosition(ext);
								}


							}

						}
					}
					if (std::string(dagger->GetObjectName()) != "HiddenMinion")
					{
						stuff = false;
					}
				}
				if (Dagger != nullptr && (GEntityList->Player()->GetPosition() - Dagger->GetPosition()).Length2D() > E->Range() && !Q->IsReady())
				{
					E->CastOnPosition(target->GetPosition().Extend(GEntityList->Player()->GetPosition(), 50));
				}
				if (stuff == false && Dagger == nullptr && !Q->IsReady())
				{
					E->CastOnPosition(target->GetPosition().Extend(GEntityList->Player()->GetPosition(), 50));
				}
			}
		}
	}
	if (HarassQ->Enabled() && Q->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && target->IsValidTarget() && !target->IsDead())
		{
			Q->CastOnTarget(target);
		}
	}
	if (HarassW->Enabled() && W->IsReady() && !GEntityList->Player()->IsCastingImportantSpell(&endtime))
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 350);
		if (target != nullptr && target->IsValidTarget() && !target->IsDead())
		{
			W->CastOnPlayer();
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
	for (auto Minion : GEntityList->GetAllMinions(false, true, true))
	{
		if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
		{
			Q->CastOnUnit(Minion);
		}
		if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), 300) && GetMinionsW(300) >= FarmWmin->GetFloat())
		{
			if (W->CastOnPlayer())
			{
				wdelay = GGame->TickCount() + 1200;
			}
		}
		if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget() && !Minion->IsDead() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
		{

			for (auto dagger : GEntityList->GetAllUnits())
			{
				if (std::string(dagger->GetObjectName()) == "HiddenMinion")
				{
					if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
					{
						if (dagger != nullptr)
						{
							if (wdelay < GGame->TickCount() && CountMinionsNearMe(dagger, 380) >= FarmEmin->GetFloat() && (GEntityList->Player()->GetPosition() - dagger->GetPosition()).Length2D() <= E->Range())
							{
								E->CastOnPosition(dagger->GetPosition());
							}
						}
					}
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
			auto QDamage = 0;
			if (Q->IsReady())
			{
				QDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotQ);
			}

			auto EDamage = 0;
			if (E->IsReady())
			{
				EDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotE);
			}
			auto RDamage = 0;
			if (R->IsReady())
			{
				RDamage = GDamage->GetSpellDamage(GEntityList->Player(), hero, kSlotR);
			}
			auto damage = QDamage + EDamage + (RDamage*ComboRstack->GetFloat()) + damages;
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

PLUGIN_EVENT(void) OnGameUpdate()
{

	Killsteal();
	Auto();
	psvdmg();
	HarasTog();

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
		LastHit();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Mixed();
	}
	if (GUtility->IsKeyDown(FleeKey->GetInteger()))
	{
		Flee();
	}
}



PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()-50); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(145, 255, 255, 255), E->Range()); }
	if (DrawDaggers->Enabled())
	{
		for (auto dagger : GEntityList->GetAllUnits())
		{
			if (std::string(dagger->GetObjectName()) == "HiddenMinion")
			{
				if ((std::string(dagger->GetObjectName()).find("Katarina_Base_Q") || std::string(dagger->GetObjectName()).find("Katarina_Base_Dagger")) && dagger != nullptr && !dagger->IsDead() && dagger->IsValidObject() && dagger->GetTeam() == GEntityList->Player()->GetTeam() && !dagger->IsDead())
				{
					if (CountEnemy(dagger->GetPosition(), 450) != 0)
					{
						GPluginSDK->GetRenderer()->DrawOutlinedCircle(dagger->GetPosition(), Vec4(25, 255, 0, 200), 450);
						GPluginSDK->GetRenderer()->DrawOutlinedCircle(dagger->GetPosition(), Vec4(25, 255, 0, 200), 150);
					}
					if (CountEnemy(dagger->GetPosition(), 450) == 0)
					{
						GPluginSDK->GetRenderer()->DrawOutlinedCircle(dagger->GetPosition(), Vec4(255, 0, 0, 200), 450);
						GPluginSDK->GetRenderer()->DrawOutlinedCircle(dagger->GetPosition(), Vec4(255, 0, 0, 200), 150);
					}
				}
			}
		}
	}
	if (DrawFleeRad->Enabled())
	{
		for (auto hero : GEntityList->GetAllHeros(false, true))
		{
			if (GUtility->IsKeyDown(FleeKey->GetInteger()))
			{
				GRender->DrawOutlinedCircle(GGame->CursorPosition(), Vec4(255, 0, 0, 255), 200);
			}
		}
	}
	if (Drawdamage->Enabled())
	{
		dmgdraw();
	}
	if (Drawtoggle->Enabled())
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

	GGame->PrintChat("<b><font color=\"#FFFFFF\">Katarina<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#7FFF00\"> - Loaded</font></b>");
	GGame->PrintChat("<b><font color=\"#f8a101\">Version: 0.2</font></b>");

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GGame->PrintChat("<b><font color=\"#FFFFFF\">Katarina<b><font color=\"#f8a101\"> by</font></b> Kornis<font color=\"#FF0000\"> - Unloaded</font></b>");
}