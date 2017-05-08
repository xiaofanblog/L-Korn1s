#include "PluginSDK.h"

PluginSetup("Kornis's Evelynn")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboRmin;
IMenuOption* ItemsCombo;


IMenu* HarassMenu;
IMenuOption* HarassQ;


IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawRRange;

IMenu* SmiteMenu;
IMenuOption* SmiteUse;
IMenuOption* SmiteE;
IMenuOption* SmiteM;
IMenuOption* SmiteKey;
IMenuOption* SmiteDraw;
IMenuOption* SmiteSave;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmMana;

IMenu* KillstealMenu;
IMenuOption* KSQ;
IMenuOption* KSR;
IMenuOption* KSE;
IMenuOption* Semikey;

IInventoryItem* Gunblade;
IInventoryItem* Cutlass;
IInventoryItem* Protobelt;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Smite;


IUnit* Player;
short keystate;

bool smiteKeyWasDown = false;

void LoadSpells()
{
	auto PlayerSum1 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot1);
	auto PlayerSum2 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot2);
	if (strstr(PlayerSum1, "SummonerSmite")) { Smite = GPluginSDK->CreateSpell2(kSummonerSlot1, kTargetCast, false, false, kCollidesWithNothing); }
	if (strstr(PlayerSum2, "SummonerSmite")) { Smite = GPluginSDK->CreateSpell2(kSummonerSlot2, kTargetCast, false, false, kCollidesWithNothing); }
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithYasuoWall);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, kCollidesWithNothing);
	Gunblade = GPluginSDK->CreateItemForId(3146, 700);
	Cutlass = GPluginSDK->CreateItemForId(3144, 550);
	Protobelt = GPluginSDK->CreateItemForId(3152, 500);

}

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Kornis Evelynn");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
		ComboW = ComboMenu->CheckBox("Use W in Combo", true);
		ComboE = ComboMenu->CheckBox("Use E in Combo", true);
		ComboR = ComboMenu->CheckBox("Use R in Combo", true);
		ComboRmin = ComboMenu->AddInteger("Use R if enemies X >",1,5,2);
		ItemsCombo = ComboMenu->CheckBox("Use Items", true);
		Semikey = ComboMenu->AddKey("Semi R", 'T');
	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassQ = HarassMenu->CheckBox("Use Q to harass", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	{
		FarmMana = FarmMenu->AddInteger("Mana percent for clear", 0, 100, 50);
		FarmQ = FarmMenu->CheckBox("Lane Clear with Q", true);
		FarmE = FarmMenu->CheckBox("Lane Clear with E", true);
	}

	KillstealMenu = MainMenu->AddMenu("Killsteal");
	{
		KSQ = KillstealMenu->CheckBox("Use Q", true);
		KSE = KillstealMenu->CheckBox("Use E", true);
		KSR = KillstealMenu->CheckBox("Use R", true);
	}
	SmiteMenu = MainMenu->AddMenu("Smite menu");
	{
		SmiteUse = SmiteMenu->CheckBox("Use Smite", true);
		SmiteE = SmiteMenu->CheckBox("Smite Enemies", true);
		SmiteM = SmiteMenu->CheckBox("Smite Jungle", true);
		SmiteSave = SmiteMenu->CheckBox("Save Smite 1 stack", true);
		SmiteKey = SmiteMenu->AddKey("Smite toggle", 'M');
		SmiteDraw = SmiteMenu->CheckBox("Use Draw", true);
	}
}
void CheckKeyPresses()
{
	keystate = GetAsyncKeyState(SmiteKey->GetInteger()); //Rembrandt

	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		if (keystate < 0)
		{
			
			if (smiteKeyWasDown == false)
			{
				
				if (SmiteUse->GetInteger() == 0) { SmiteUse->UpdateInteger(1); }
				else { SmiteUse->UpdateInteger(0); }
				smiteKeyWasDown = true;
			}
		}
		else
		{
			
			smiteKeyWasDown = false;
		}
	}
}
void AutoSmite() //REMBRANDT
{
	if (SmiteM->Enabled())
	{
		if (Smite != nullptr && Smite->IsReady() && SmiteUse->Enabled()) {
			auto minions = GEntityList->GetAllMinions(false, false, true);
			for (IUnit* minion : minions)
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

void Combo()
{
	if (SmiteE->Enabled())
	{
		if (Smite != nullptr && Smite->IsReady() && SmiteUse->Enabled())
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
	}
	if (Cutlass->IsOwned() && Cutlass->IsReady() && ItemsCombo->Enabled() && !(Player->IsDead()))
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
	if (Gunblade->IsOwned() && Gunblade->IsReady() && ItemsCombo->Enabled() && !(Player->IsDead()))
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
	if (Protobelt->IsOwned() && Protobelt->IsReady() && ItemsCombo->Enabled() && !(Player->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Protobelt->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			Protobelt->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			Protobelt->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 500));
		}
	}
	if (ComboQ->Enabled() && Q->IsReady() && Q->Range())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr)
		{
			Q->CastOnTarget(target);
		}
	}
	if (ComboE->Enabled() && E->IsReady() && E->Range() + 70)
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range() + 70);
		if (target != nullptr)
		{
			E->CastOnTarget(target);
		}
	}
	if (ComboW->Enabled() && W->IsReady() && 600)
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 600);
		if (target != nullptr)
		{
			W->CastOnPlayer();
		}

	}
	if (ComboR->Enabled() && R->IsReady() && R->Range())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
		if (target != nullptr && !target->IsDead() && !target->IsInvulnerable() && GetEnemiesInRange(R->Range()) >= ComboRmin->GetInteger() && GEntityList->Player()->IsValidTarget(target, R->Range()))
			{
				R->CastOnTarget(target);
		}
	}
}

void Semi()
{
	if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (!Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && !Enemy->IsDead())
			{
				R->CastOnTarget(Enemy);
			}
		}
	}
}



void Mixed()
{
	{
		if (HarassQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				Q->CastOnTarget(target);
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

		if (Enemy != nullptr && !Enemy->IsDead())
		{
			if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage > Enemy->GetHealth())
			{
				R->CastOnTarget(Enemy);
			}
			if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
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
				if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnUnit(Minion);
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
	if (GetAsyncKeyState(Semikey->GetInteger()))
	{
		Semi();
	}
	AutoSmite();
	CheckKeyPresses();
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range() + 70); }
	if (Smite != nullptr && SmiteDraw->Enabled())
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

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
}