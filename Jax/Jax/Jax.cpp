#include "PluginSDK.h"
#include <string>

PluginSetup("Jax by Kornis")


IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboQAA;
IMenuOption* ComboW;
IMenuOption* ComboWAA;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* ComboR1v1;
IMenu* FleeMenu;
IMenuOption* FleeKey;
IMenuOption* ComboRMin;
IMenuOption* ComboQMode;
IMenuOption* ComboRmin;
IMenuOption* ComboEDelay;
IMenuOption* ComboEDelayadd;

IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassModeQ;
IMenuOption* HarassE;
IMenuOption* HarassWAA;

IMenuOption* FarmMana;

IMenu* QSettings;
IMenu* WSettings;

IMenu* ESettings;
IMenu* RSettings;


IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawERange;
IMenuOption* DrawQmin;
IMenuOption* DrawWard;

IMenu* FarmMenu;
IMenuOption* FarmQ;
IMenuOption* FarmE;
IMenuOption* FarmW;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Wards;

IUnit* Player;
IUnit* target;

int laste; 
int lastedel;

ISpell2* jump;


boolean ButtonDown = false;
boolean doJump = false;
boolean letward = false;
Vec3 location;

IInventoryItem* Ward1;
IInventoryItem* Ward2;
IInventoryItem* Ward3;
IInventoryItem* Ward4;
IInventoryItem* Ward5;
IInventoryItem* Ward6;
IInventoryItem* Ward7;
IInventoryItem* Ward8;
IInventoryItem* Ward9;
IInventoryItem* Ward10;
IInventoryItem* Ward11;
IInventoryItem* Ward12;
IInventoryItem* Ward13;
IInventoryItem* Ward14;
IInventoryItem* Ward15;
IInventoryItem* Ward16;
IInventoryItem* Ward17;



std::vector<std::string> ComboMode = { "Q>E", "E>Q"};
std::vector<std::string> HarassMode = { "Q>E", "E>Q"};
std::vector<std::string> ComboEMode = { "Instant", "Delay" };


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
	Wards = GPluginSDK->CreateSpell2(kSlotTrinket, kCircleCast, false, false, kCollidesWithWalls);
	Ward1 = GPluginSDK->CreateItemForId(2045, 600);
	Ward2 = GPluginSDK->CreateItemForId(2049, 600);
	Ward3 = GPluginSDK->CreateItemForId(2050, 600);
	Ward4 = GPluginSDK->CreateItemForId(2301, 600);
	Ward5 = GPluginSDK->CreateItemForId(2302, 600);
	Ward6 = GPluginSDK->CreateItemForId(2303, 600);
	Ward7 = GPluginSDK->CreateItemForId(3340, 600);
	Ward8 = GPluginSDK->CreateItemForId(3361, 600);
	Ward9 = GPluginSDK->CreateItemForId(3362, 600);
	Ward10 = GPluginSDK->CreateItemForId(3711, 600);
	Ward11 = GPluginSDK->CreateItemForId(1408, 600);
	Ward12 = GPluginSDK->CreateItemForId(1409, 600);
	Ward13 = GPluginSDK->CreateItemForId(1410, 600);
	Ward14 = GPluginSDK->CreateItemForId(1411, 600);
	Ward15 = GPluginSDK->CreateItemForId(2043, 600);
	Ward16 = GPluginSDK->CreateItemForId(2055, 600);
	Ward17 = GPluginSDK->CreateItemForId(1418, 600);
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
//JumpPos from Federal
static void DoWardJump(Vec3 pos)
{
	Vec3 JumpPos;
	auto basePos = GEntityList->Player()->GetPosition();
	auto newPos = pos - GEntityList->Player()->GetPosition();
	if (!jump->IsReady()) {
		return;
	}

	if (GetDistanceVectors(GEntityList->Player()->GetPosition(), pos) <= 590)
	{
		JumpPos = pos;
	}
	else if (GetDistanceVectors(GEntityList->Player()->GetPosition(), pos) > 590)
	{
		JumpPos = basePos + newPos.VectorNormalize() * 590;
	}
	else
	{
		JumpPos = basePos + newPos.VectorNormalize() * GetDistanceVectors(GEntityList->Player()->GetPosition(), pos);
	}

	if (Ward1->IsReady() && Ward1->IsOwned())
	{
		Ward1->CastOnPosition(JumpPos);
	}
	else if (Ward2->IsReady() && Ward2->IsOwned())
	{
		Ward2->CastOnPosition(JumpPos);
	}
	else if (Ward3->IsReady() && Ward3->IsOwned())
	{
		Ward3->CastOnPosition(JumpPos);
	}
	else if (Ward4->IsReady() && Ward4->IsOwned())
	{
		Ward4->CastOnPosition(JumpPos);
	}
	else if (Ward5->IsReady() && Ward5->IsOwned())
	{
		Ward5->CastOnPosition(JumpPos);
	}
	else if (Ward6->IsReady() && Ward6->IsOwned())
	{
		Ward6->CastOnPosition(JumpPos);
	}
	else if (Ward7->IsReady() && Ward7->IsOwned())
	{
		Ward7->CastOnPosition(JumpPos);
	}
	else if (Ward8->IsReady() && Ward8->IsOwned())
	{
		Ward8->CastOnPosition(JumpPos);
	}
	else if (Ward9->IsReady() && Ward9->IsOwned())
	{
		Ward9->CastOnPosition(JumpPos);
	}
	else if (Ward10->IsReady() && Ward10->IsOwned())
	{
		Ward10->CastOnPosition(JumpPos);
	}
	else if (Ward11->IsReady() && Ward11->IsOwned())
	{
		Ward11->CastOnPosition(JumpPos);
	}
	else if (Ward12->IsReady() && Ward12->IsOwned())
	{
		Ward12->CastOnPosition(JumpPos);
	}
	else if (Ward13->IsReady() && Ward13->IsOwned())
	{
		Ward13->CastOnPosition(JumpPos);
	}
	else if (Ward14->IsReady() && Ward14->IsOwned())
	{
		Ward14->CastOnPosition(JumpPos);
	}
	else if (Ward15->IsReady() && Ward15->IsOwned())
	{
		Ward15->CastOnPosition(JumpPos);
	}
	else if (Ward16->IsReady() && Ward16->IsOwned())
	{
		Ward16->CastOnPosition(JumpPos);
	}
	else if (Ward17->IsReady() && Ward17->IsOwned())
	{
		Ward17->CastOnPosition(JumpPos);
	}


	doJump = true;

}


static bool IsKeyDown(IMenuOption *menuOption)
{
	return GetAsyncKeyState(menuOption->GetInteger()) & 0x8000;
}


void KeyPressEvent() {

	if (IsKeyDown(FleeKey) && ButtonDown == false) {
		ButtonDown = true;
		DoWardJump(GGame->CursorPosition());

	}

	else if (IsKeyDown(FleeKey)) {
		return;
	}
	else {
		ButtonDown = false;
	}

}


void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Jax by Kornis");
	ComboMenu = MainMenu->AddMenu("Combo");
	{
		QSettings = ComboMenu->AddMenu("Q Settings");
		ComboQMode = QSettings->AddSelection("Combo Mode", 0, ComboMode);
		ComboQ = QSettings->CheckBox("Use Q in Combo", true);
		ComboQAA = QSettings->AddInteger("Q Min. Range", 10, 400, 300);
		WSettings = ComboMenu->AddMenu("W Settings");
		ComboW = WSettings->CheckBox("Use W always", false);
		ComboWAA = WSettings->CheckBox("Use W AA reset", true);
		ESettings = ComboMenu->AddMenu("E Settings");
		ComboE = ESettings->CheckBox("Use E in Combo", true);
		ComboEDelay = ESettings->AddSelection("E mode:", 0, ComboEMode);
		ComboEDelayadd = ESettings->AddInteger("E Delay", 100, 400, 250);
		RSettings = ComboMenu->AddMenu("R Settings");
		ComboR = RSettings->CheckBox("Use R in Combo", true);
		ComboRMin = RSettings->AddInteger("Min. enemies for R: ", 1, 5, 2);
		ComboRmin = RSettings->AddInteger("Min HP for R: ", 10, 100, 30);

	}
	HarassMenu = MainMenu->AddMenu("Harass");
	{
		HarassModeQ = HarassMenu->AddSelection("Harass Mode: ", 0, HarassMode);
		HarassQ = HarassMenu->CheckBox("Use Q to harass", true);
		HarassWAA = HarassMenu->CheckBox("Use W AA reset", true);
		HarassE = HarassMenu->CheckBox("Use E to harass", true);
	}


	DrawingMenu = MainMenu->AddMenu("Drawings");
	{
		DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
		DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
		DrawQmin = DrawingMenu->CheckBox("Draw Q min Range", true);
		DrawWard = DrawingMenu->CheckBox("Draw Wardjump Range", true);
	}
	FarmMenu = MainMenu->AddMenu("Farming");
	FarmMana = FarmMenu->AddInteger("Mana percent for Clear", 10, 100, 50);
	FarmQ = FarmMenu->CheckBox("Farm Q", true);
	FarmW = FarmMenu->CheckBox("Farm W", true);
	FarmE = FarmMenu->CheckBox("Farm E", true);

	FleeMenu = MainMenu->AddMenu("Wardjump");
	FleeKey = FleeMenu->AddKey("Wardjump", 'G');
}

int GetEnemiesInRange(float range)
{
	auto enemies = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 1;

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

	if (ComboQMode->GetInteger() == 0)
	{
		
		target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
		for (auto target : GEntityList->GetAllHeros(false, true));
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);

			if (Q->IsReady() && ComboQ->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > ComboQAA->GetInteger())
				{
					Q->CastOnTarget(target);
				}
				if (target->IsDashing())
				{
					Q->CastOnTarget(target);
				}
				if (target->GetHealth() < QDamage)
				{
					Q->CastOnTarget(target);
				}

			}
			if (ComboEDelay->GetInteger() == 0)
			{

				if (E->IsReady() && ComboE->Enabled() && E->Range() && !Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnPlayer();
				}
				else if (ComboE->Enabled() && Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnTarget(target);
				}
			}
			if (ComboEDelay->GetInteger() == 1)
			{
				if (E->IsReady() && ComboE->Enabled() && E->Range() && !Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnPlayer();
					lastedel = GGame->CurrentTick();
				}
			
				else if ((GGame->CurrentTick() - lastedel > ComboEDelayadd->GetInteger()) && ComboE->Enabled() && Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnTarget(target);
				}
			}
			if (W->IsReady() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), 300) && !ComboWAA->Enabled())
			{
				W->CastOnPlayer();
			}
			if (R->IsReady() && ComboR->Enabled())
			{
				if (GetEnemiesInRange(650) >= ComboRMin->GetInteger() && ComboRmin->GetInteger() >= GEntityList->Player()->HealthPercent())
				{
					R->CastOnPlayer();
				}
			}
		}
	}
	if (ComboQMode->GetInteger() == 1)
	{
		target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range()+100);
		for (auto target : GEntityList->GetAllHeros(false, true));
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
		{

			if (ComboEDelay->GetInteger() == 0)
			{

				if (E->IsReady() && ComboE->Enabled() && E->Range() && !Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), Q->Range()+100))
				{
					E->CastOnPlayer();
					laste = GGame->CurrentTick();
				}
				else if (ComboE->Enabled() && Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), Q->Range()+100))
				{
					E->CastOnTarget(target);
				}
				if (GGame->CurrentTick() - laste > 100)
				{
					if (Q->IsReady() && ComboQ->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > ComboQAA->GetInteger())
						{
							Q->CastOnTarget(target);
						}
					}

				}

				if (W->IsReady() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), 300) && !ComboWAA->Enabled())
				{
					W->CastOnPlayer();
				}
				if (R->IsReady() && ComboR->Enabled())
				{
					if (GetEnemiesInRange(650) >= ComboRMin->GetInteger() && ComboRmin->GetInteger() >= GEntityList->Player()->HealthPercent())
					{
						R->CastOnPlayer();
					}
				}
			}
			if (ComboEDelay->GetInteger() == 1)
			{
				if (E->IsReady() && ComboE->Enabled() && E->Range() && !Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), Q->Range()+100))
				{
					E->CastOnPlayer();
					laste = GGame->CurrentTick();
					lastedel = GGame->CurrentTick();
				}

				else if ((GGame->CurrentTick() - lastedel > ComboEDelayadd->GetInteger()) && ComboE->Enabled() && Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), E->Range()))
				{
					E->CastOnTarget(target);
				}
				if (GGame->CurrentTick() - laste > 100)
				{
					if (Q->IsReady() && ComboQ->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
					{
						if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > ComboQAA->GetInteger())
						{
							Q->CastOnTarget(target);
						}
					}

				}

				if (W->IsReady() && ComboW->Enabled() && target->IsValidTarget(GEntityList->Player(), 300) && !ComboWAA->Enabled())
				{
					W->CastOnPlayer();
				}
				if (R->IsReady() && ComboR->Enabled())
				{
					if (GetEnemiesInRange(650) >= ComboRMin->GetInteger() && ComboRmin->GetInteger() >= GEntityList->Player()->HealthPercent())
					{
						R->CastOnPlayer();
					}
				}
			}
		}
	}
}

void LastHit()
{

}

void Mixed()
{
	if (ComboQMode->GetInteger() == 0)
	{

		target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
		for (auto target : GEntityList->GetAllHeros(false, true));
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);

			if (Q->IsReady() && HarassQ->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
			{
				if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > ComboQAA->GetInteger())
				{
					Q->CastOnTarget(target);
				}
				if (target->IsDashing())
				{
					Q->CastOnTarget(target);
				}
				if (target->GetHealth() < QDamage)
				{
					Q->CastOnTarget(target);
				}
			}

			if (E->IsReady() && HarassE->Enabled() && E->Range() && !Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), E->Range()))
			{
				E->CastOnPlayer();
			}
			else if (HarassE->Enabled() && Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), E->Range()))
			{
				E->CastOnTarget(target);
			}
		}
		
	}
	if (HarassModeQ->GetInteger() == 1)
	{
		target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range() + 100);
		for (auto target : GEntityList->GetAllHeros(false, true));
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
		{

			if (E->IsReady() && HarassE->Enabled() && E->Range() && !Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), Q->Range() + 100))
			{
				E->CastOnPlayer();
				laste = GGame->CurrentTick();
			}
			else if (HarassE->Enabled() && Player->HasBuff("JaxCounterStrike") && target->IsValidTarget(GEntityList->Player(), Q->Range() + 100))
			{
				E->CastOnTarget(target);
			}
			if (GGame->CurrentTick() - laste > 100)
			{
				if (Q->IsReady() && HarassQ->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length2D() > ComboQAA->GetInteger())
					{
						Q->CastOnTarget(target);
					}
				}
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
			if (Minion->IsEnemy(GEntityList->Player()) && !Minion->IsDead() && Minion->IsValidTarget() && Minion->IsCreep() || Minion->IsJungleCreep())
			{
				if (FarmQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Q->CastOnUnit(Minion);
				}
				if (FarmE->Enabled() && E->IsReady() && Minion->IsValidTarget(GEntityList->Player(), E->Range() + 100))
				{
					E->CastOnUnit(Minion);
				}
				if (Player->HasBuff("JaxCounterStrike"))
				{
					E->CastOnUnit(Minion);
				}

			}
		}
	}
}



PLUGIN_EVENT(void) OnCreateObject(IUnit* Source)
{


	if (!doJump) {
		return;
	}

	if (Source == nullptr) {
		return;
	}

	//GGame->PrintChat("Created Object1");
	float flDistance = (Source->GetPosition() - location).Length();
	//char array[30];
	//sprintf_s(array, "%f", flDistance);
	//GGame->PrintChat(array);

	if (flDistance < 50000) {

		std::string itemName = Source->GetObjectName();
		if (itemName.find("Ward") != std::string::npos || itemName.find("ward") != std::string::npos || itemName.find("JammerDevice") != std::string::npos) {
			jump->CastOnUnit(Source);
			doJump = false;
		}
	}



}

boolean InitSpells()
{

	auto champion = GEntityList->Player();

	if (champion == nullptr) {
		return false;
	}

	auto name = champion->ChampionName();
	//GGame->PrintChat(name);


	if (strcmp(name, "LeeSin") == 0) {
		GGame->PrintChat("Loaded Lee Jump!");

		jump = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);

	}
	else if ((strcmp(name, "Jax")) == 0) {
		GGame->PrintChat("Loaded Jax Jump!");
		jump = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	}



	return true;



}

PLUGIN_EVENT(void) OnGameUpdate()
{
	KeyPressEvent();
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
}


PLUGIN_EVENT(void) OnAfterAttack(IUnit* source, IUnit* target)
{
	if (source != Player || target == nullptr)
		return;

	switch (GOrbwalking->GetOrbwalkingMode())
	{
	case kModeCombo:
		for (auto hero : GEntityList->GetAllHeros(false, true)) {
			if (ComboWAA->Enabled() && W->IsReady() && (hero->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 300)
			{
				W->CastOnPlayer();
			}
		}
		break;
	case kModeMixed:
		for (auto hero : GEntityList->GetAllHeros(false, true)) {
			if (HarassWAA->Enabled() && W->IsReady() && (hero->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 300)
			{
				W->CastOnPlayer();
			}
		}
		break;
	case kModeLaneClear:
		for (auto minion : GEntityList->GetAllMinions(false, true, true)) {
			if (FarmW->Enabled() && W->IsReady() && (minion->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 300)
			{
				W->CastOnPlayer();
			}
		}
		break;
	}
}


PLUGIN_EVENT(void) OnRender()
{
	if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	if (DrawQmin->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), ComboQAA->GetInteger()); }
	if (DrawWard->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), 630); }
}


PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	Player = GEntityList->Player();
	if (!InitSpells()) {
		return;
	}

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
	GEventManager->AddEventHandler(kEventOnCreateObject, OnCreateObject);

}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
	GEventManager->RemoveEventHandler(kEventOnCreateObject, OnCreateObject);
}