#include "PluginSDK.h"
#include <string>

PluginSetup("Kornis Rengar")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboWheal;
IMenuOption* ComboWcc;
IMenuOption* ComboE;
IMenuOption* PriorityEcombo;
IMenuOption* ComboItems;
IMenuOption* ComboModeChange; 
IMenuOption* ComboSmite;
IMenuOption* SmiteSave;


IMenu* HarassMenu;
IMenuOption* HarassE;


IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawWRange;
IMenuOption* DrawERange;
IMenuOption* DrawPriority;

IMenu* SmiteMenu;
IMenuOption* SmiteUse;
IMenuOption* SmiteKey;
IMenuOption* Smitedraw;


IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmW;
IMenuOption* FarmE;
IMenuOption* FarmWhp;
IMenuOption* SaveStacks;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSE;

IMenu* SkinMenu;
IMenuOption* SkinChange;
IMenuOption* SkinChangeEnable;


IInventoryItem* Youmuus;
IInventoryItem* Tiamat;
IInventoryItem* Titanic_Hydra;
IInventoryItem* Ravenous_Hydra;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Smite;


float KeyPre;
int ComboMode = 1;
short keystate;
bool smiteKeyWasDown = false;

IUnit* Player;
IUnit* Enemy;


void LoadSpells()
{
	auto PlayerSum1 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot1);
	auto PlayerSum2 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot2);
	if (strstr(PlayerSum1, "SummonerSmite")) { Smite = GPluginSDK->CreateSpell2(kSummonerSlot1, kTargetCast, false, false, kCollidesWithNothing); }
	if (strstr(PlayerSum2, "SummonerSmite")) { Smite = GPluginSDK->CreateSpell2(kSummonerSlot2, kTargetCast, false, false, kCollidesWithNothing); }
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall, kCollidesWithMinions));
	Q->SetOverrideRange(550);

	Youmuus = GPluginSDK->CreateItemForId(3142, 0);
	Titanic_Hydra = GPluginSDK->CreateItemForId(3748, 385);
	Ravenous_Hydra = GPluginSDK->CreateItemForId(3074, 385);
	Tiamat = GPluginSDK->CreateItemForId(3077, 385);

}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Rengar");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
		ComboW = ComboMenu->CheckBox("Use W in Combo", true);
		ComboWcc = ComboMenu->CheckBox("Use W 4 stacks on CC", true);
		/*ComboWheal = ComboMenu->AddInteger("Use W if HP <", 5, 30, 20);*/
		ComboE = ComboMenu->CheckBox("Use E in Combo", true);
		PriorityEcombo = ComboMenu->CheckBox("Priority E if out of Q range", true);
		ComboModeChange = ComboMenu->AddKey("Priority change", 'T');
		ComboItems = ComboMenu->CheckBox("Use Items", true);
		ComboSmite = ComboMenu->CheckBox("Use Smite in combo", true);


	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassE = HarassMenu->CheckBox("Use E in harass", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawPriority = DrawingMenu->CheckBox("Draw Priority", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		SaveStacks = FarmMenu->CheckBox("Save stacks", true);
		FarmQ = FarmMenu->CheckBox("Clear with Q", true);
		FarmW = FarmMenu->CheckBox("Clear with W", true);
		FarmE = FarmMenu->CheckBox("Clear with E", true);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSE = KillstealMenu->CheckBox("Use E", true);
	}
	SmiteMenu = MainMenu->AddMenu("Smite menu");
	{
		SmiteUse = SmiteMenu->CheckBox("Use Smite", true);
		SmiteSave = SmiteMenu->CheckBox("Save Smite 1 stack", true);
		SmiteKey = SmiteMenu->AddKey("Smite toggle", 'M');
		Smitedraw = SmiteMenu->CheckBox("Use Draw", true);
	}

	SkinMenu = MainMenu->AddMenu("Skin Changer");
	{
		SkinChangeEnable = SkinMenu->CheckBox("Use Skin Changer", true);
		SkinChange = SkinMenu->AddInteger("Skins", 1, 3, 3);
	}
}

void AutoSmite() // AUTO SMITE PRO BY REMBRANDT
{
	if (Smite != nullptr && Smite->IsReady() && SmiteUse->Enabled()) {
		auto minions = GEntityList->GetAllMinions(false, false, true);
		for (IUnit* minion : minions)
		{
			if ((minion->GetPosition() - GEntityList->Player()->GetPosition()).Length() <= 570)
			{
				if (strstr(minion->GetObjectName(), "Red") || strstr(minion->GetObjectName(), "Blue") || strstr(minion->GetObjectName(), "Dragon") || strstr(minion->GetObjectName(), "Rift") || strstr(minion->GetObjectName(), "Baron"))
				{
					if (minion != nullptr && !minion->IsDead() && minion->GetHealth() <= GDamage->GetSummonerSpellDamage(GEntityList->Player(), minion, kSummonerSpellSmite))
					{
						Smite->CastOnUnit(minion);
					}
				}
			}
		}
	}
}


void ChangePriority()
{
	if (GetAsyncKeyState(ComboModeChange->GetInteger()) && !GGame->IsChatOpen() && GGame->Time() > KeyPre)
	{
		if (ComboMode == 1)
		{
			ComboMode = 2;
			KeyPre = GGame->Time() + 0.250;
		}
		else if (ComboMode == 2)
		{
			ComboMode = 3;
			KeyPre = GGame->Time() + 0.250;
		}
		else
		{
			ComboMode = 1;
			KeyPre = GGame->Time() + 0.250;
		}
	}
}
static void SkinChanger()
{
	if (SkinChangeEnable->Enabled())
	{
		if (GEntityList->Player()->GetSkinId() != SkinChange->GetInteger())
		{
			GEntityList->Player()->SetSkinId(SkinChange->GetInteger());
		}
	}
	else
	{
		GEntityList->Player()->SetSkinId(GEntityList->Player()->GetSkinId());
	}
}

int EnemiesInRange(IUnit* Source, float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto target : Targets)
	{
		if (target != nullptr && !target->IsDead())
		{
			auto flDistance = (target->GetPosition() - Source->GetPosition()).Length();
			if (flDistance < range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}

void Combo()
{
	if (!GEntityList->Player()->HasBuff("RengarR"))
	{
		if (Smite != nullptr && Smite->IsReady() && ComboSmite->Enabled()) // AUTO SMITE PRO BY REMBRANDT
		{

			if (!SmiteSave->Enabled())
			{
				if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && (GTargetSelector->GetFocusedTarget()->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 500)
				{

					Smite->CastOnTarget(GTargetSelector->GetFocusedTarget());
				}
				else
				{
					Smite->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 700));
				}
			}
			if (SmiteSave->Enabled())
			{
				if (GEntityList->Player()->GetSpellBook()->GetAmmo(Smite->GetSlot()) == 2)
				{
					if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && (GTargetSelector->GetFocusedTarget()->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 500)
					{

						Smite->CastOnTarget(GTargetSelector->GetFocusedTarget());
					}
					else
					{
						Smite->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 700));
					}
				}
			}
		}
		if (Youmuus->IsOwned() && Youmuus->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
		{
			if (EnemiesInRange(Player, 500) > 0)
			{
				Youmuus->CastOnPlayer();
			}
		}
		if (Ravenous_Hydra->IsOwned() && Ravenous_Hydra->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
		{
			if (EnemiesInRange(Player, 385) > 0)
			{
				Ravenous_Hydra->CastOnPlayer();
			}
		}
		if (Tiamat->IsOwned() && Tiamat->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
		{
			if (EnemiesInRange(Player, 385) > 0)
			{
				Tiamat->CastOnPlayer();
			}
		}
		if (Titanic_Hydra->IsOwned() && Titanic_Hydra->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
		{
			if (EnemiesInRange(Player, 385) > 0)
			{
				Titanic_Hydra->CastOnPlayer();
			}
		}
		Enemy = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
		for (auto Enemy : GEntityList->GetAllHeros(false, true));
		if (Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && Enemy->IsValidTarget())
		{

			if (Player->GetMana() == 4 || Player->GetMana() < 4)
			{
				if (PriorityEcombo->Enabled() && E->IsReady())
				{
					if (Enemy->IsValidTarget(GEntityList->Player(), E->Range()))
					{
						if ((Enemy->GetPosition() - Player->GetPosition()).Length() > Q->Range())
						{
							E->CastOnTarget(Enemy);
						}
					}
				}
			}
			if (Player->GetMana() == 4)
			{
				if (ComboMode == 1)
				{
					if (Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						Q->CastOnTarget(Enemy);
					}

				}
				if (ComboMode == 2)
				{
					if (Enemy->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						W->CastOnTarget(Enemy);
					}

				}
				if (ComboMode == 3)
				{
					if (Enemy->IsValidTarget(GEntityList->Player(), E->Range()))
					{
						E->CastOnTarget(Enemy);
					}

				}
				if (ComboWcc->Enabled() && W->IsReady())
				{
					if (Player->HasBuffOfType(BUFF_Stun) || Player->HasBuffOfType(BUFF_Silence) || Player->HasBuffOfType(BUFF_Taunt) || Player->HasBuffOfType(BUFF_Polymorph) || Player->HasBuffOfType(BUFF_Snare) || Player->HasBuffOfType(BUFF_Charm) || Player->HasBuffOfType(BUFF_Knockback))
					{
						W->CastOnPlayer();
					}
				}
			}
			if (Player->GetMana() < 4)
			{
				if (ComboQ->Enabled())
				{
					if (Enemy->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						Q->CastOnTarget(Enemy);
					}
				}
				if (ComboW->Enabled())
				{
					if (Enemy->IsValidTarget(GEntityList->Player(), W->Range()))
					{
						W->CastOnTarget(Enemy);
					}
				}
				if (ComboE->Enabled())
				{
					if (Enemy->IsValidTarget(GEntityList->Player(), E->Range()))
					{
						E->CastOnTarget(Enemy);
					}
				}
			}
		}
	}
}



void Mixed()
{
	Enemy = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
	for (auto Enemy : GEntityList->GetAllHeros(false, true));
	if (Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && Enemy->IsValidTarget())
	{

		if (Player->GetMana() < 4)
		{
			if (HarassE->Enabled() && E->IsReady())
			{
				if (Enemy != nullptr)
				{
					E->CastOnTarget(Enemy);
				}

			}
		}
	}
}

void Killsteal()
{
	if (!GEntityList->Player()->HasBuff("RengarR"))
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
			auto EDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotE);

			if (Enemy != nullptr && !Enemy->IsDead())
			{
				if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), 550) && QDamage > Enemy->GetHealth())
				{
					Q->CastOnTarget(Enemy);
				}
				if (KSE->Enabled() && E->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), E->Range()) && EDamage > Enemy->GetHealth())
				{
					E->CastOnTarget(Enemy);
				}


			}
		}
	}
}


void Farm()
{
	if (SaveStacks->Enabled()) 
	{
		if (Player->GetMana() < 4)
		{
			for (auto Minion : GEntityList->GetAllMinions(false, true, true))
			{
				if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
				{
					if (Tiamat->IsOwned() && Tiamat->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
					{
						if (Minion->IsValidTarget(GEntityList->Player(), 385))
						{
							Tiamat->CastOnPlayer();
						}
					}
					if (Titanic_Hydra->IsOwned() && Titanic_Hydra->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
					{
						if (Minion->IsValidTarget(GEntityList->Player(), 385))
						{
							Titanic_Hydra->CastOnPlayer();
						}
					}
					if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 400))
					{
						Vec3 vecCastPosition;
						int iMinionsHit = 0;

						Q->FindBestCastPosition(true, false, vecCastPosition, iMinionsHit);

						if (Q->IsReady() && iMinionsHit >= 1) {
							Q->CastOnPosition(vecCastPosition);
						}
					}
					if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
					{
						E->CastOnTarget(Minion);
					}
					if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), W->Range() - 10))
					{
						W->CastOnTarget(Minion);
					}

				}
				/*if (ComboWheal->GetInteger() > Player->HealthPercent())
				{
					W->CastOnPlayer();
				}*/
			}
		}
	}
	if (!SaveStacks->Enabled())
	{
		if (Player->GetMana() <= 4)
		{

			for (auto Minion : GEntityList->GetAllMinions(false, true, true))
			{
				if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && (Minion->IsCreep() || Minion->IsJungleCreep()))
				{
					if (Tiamat->IsOwned() && Tiamat->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
					{
						if (Minion->IsValidTarget(GEntityList->Player(), 385))
						{
							Tiamat->CastOnPlayer();
						}
					}
					if (Titanic_Hydra->IsOwned() && Titanic_Hydra->IsReady() && ComboItems->Enabled() && !(Player->IsDead()))
					{
						if (Minion->IsValidTarget(GEntityList->Player(), 385))
						{
							Titanic_Hydra->CastOnPlayer();
						}
					}
					if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), 400))
					{
						Vec3 vecCastPosition;
						int iMinionsHit = 0;

						Q->FindBestCastPosition(true, false, vecCastPosition, iMinionsHit);

						if (Q->IsReady() && iMinionsHit >= 1) {
							Q->CastOnPosition(vecCastPosition);
						}
					}
					if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
					{
						E->CastOnTarget(Minion);
					}
					if (FarmW->Enabled() && W->IsReady() && Minion->IsValidTarget(GEntityList->Player(), W->Range() - 10))
					{
						W->CastOnTarget(Minion);
					}

				}

			}
		}

	}
}

void CheckKeyPresses()
{
	keystate = GetAsyncKeyState(SmiteKey->GetInteger()); //Rembrandt

	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		if (keystate < 0) // If most-significant bit is set...
		{
			// key is down . . .
			if (smiteKeyWasDown == false)
			{
				//toggle smite
				if (SmiteUse->GetInteger() == 0) { SmiteUse->UpdateInteger(1); }
				else { SmiteUse->UpdateInteger(0); }
				smiteKeyWasDown = true;
			}
		}
		else
		{
			// key is up . . .
			smiteKeyWasDown = false;
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
	
	AutoSmite();
	Killsteal();
	SkinChanger();
	CheckKeyPresses();
	ChangePriority();
}

PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), 500); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(50, 200, 100, 255), E->Range()); }
	if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(50, 200, 100, 255), W->Range()); }
	if (DrawPriority->Enabled())
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
			if (ComboMode == 1)
			{
				std::string text = std::string("Priority: Q");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
			if (ComboMode == 2)
			{
				std::string text = std::string("Priority: W");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
			if (ComboMode == 3)
			{
				std::string text = std::string("Priority: E");
				Vec4 clr = Vec4(188, 255, 50, 255);
				pFont->SetColor(clr);
				pFont->Render(pos.x, pos.y, text.c_str());
			}
		}
	}
	if (Smite != nullptr && Smitedraw->Enabled())
	{

		static IFont* pFont = nullptr;

		if (pFont == nullptr)
		{
			pFont = GRender->CreateFont("Tahoma", 20.f, kFontWeightNormal);
			pFont->SetOutline(true);
			pFont->SetLocationFlags(kFontLocationNormal);
		}
		Vec2 pos;
		if (GGame->Projection(GEntityList->Player()->GetPosition(), &pos))
		{
			if (SmiteUse->Enabled())
			{

				std::string text = std::string("AUTOSMITE ON");
				Vec4 clr = Vec4(25, 255, 0, 200);
				pFont->SetColor(clr);
				pFont->Render(pos.x + 30, pos.y + 70, text.c_str());
			}
			else
			{
				std::string text = std::string("AUTOSMITE OFF");
				Vec4 clr = Vec4(255, 0, 0, 200);
				pFont->SetColor(clr);
				pFont->Render(pos.x + 30, pos.y + 70, text.c_str());
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
	GGame->PrintChat("<b><font color=\"#f8a101\">Rengar<b><font color=\"#FFFFFF\"> by</font></b> Kornis<font color=\"#7FFF00\"> - Loaded</font></b>");
}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
}