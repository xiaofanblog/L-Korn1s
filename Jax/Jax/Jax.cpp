#include "PluginSDK.h"

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

IUnit* Player;
IUnit* target;

int laste; 
int lastedel;

ISpell2* jump;


boolean ButtonDown = false;
boolean doJump = false;
Vec3 location;

IInventoryItem *sight_stone;
IInventoryItem *red_stone;
IInventoryItem *pink_ward;
IInventoryItem *yellow_trinket;
IInventoryItem *t_knife;
IInventoryItem *t_knifeA;
IInventoryItem *t_knifeB;
IInventoryItem *t_knifeC;
IInventoryItem *t_knifeD;

std::vector<std::string> ComboMode = { "Q>E", "E>Q"};
std::vector<std::string> HarassMode = { "Q>E", "E>Q"};
std::vector<std::string> ComboEMode = { "Instant", "Delay" };


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
}

IInventoryItem* GetAvaliableItem() {

	if (sight_stone->IsOwned() && sight_stone->IsReady()) {
		return sight_stone;
	}
	else if (t_knife->IsOwned() && t_knife->IsReady()) {
		return t_knife;
	}
	else if (t_knifeA->IsOwned() && t_knifeA->IsReady()) {
		return t_knifeA;
	}
	else if (t_knifeB->IsOwned() && t_knifeB->IsReady()) {
		return t_knifeB;
	}
	else if (t_knifeC->IsOwned() && t_knifeC->IsReady()) {
		return t_knifeC;
	}
	else if (t_knifeD->IsOwned() && t_knifeD->IsReady()) {
		return t_knifeD;
	}
	else if (red_stone->IsOwned() && red_stone->IsReady()) {
		return red_stone;
	}
	else if (yellow_trinket->IsOwned() && yellow_trinket->IsReady()) {
		return yellow_trinket;
	}

	else if (pink_ward->IsOwned() && pink_ward->IsReady()) {
		return pink_ward;
	}



}

void DoWardJump() {

	if (!jump->IsReady()) {
		return;
	}

	auto item = GetAvaliableItem();
	if (item == nullptr) {
		GGame->PrintChat("No wards");
		return;
	}

	location = GGame->CursorPosition();
	item->CastOnPosition(location);
	doJump = true;
}


static bool IsKeyDown(IMenuOption *menuOption)
{
	return GetAsyncKeyState(menuOption->GetInteger()) & 0x8000;
}


void KeyPressEvent() {

	if (IsKeyDown(FleeKey) && ButtonDown == false) {
		ButtonDown = true;
		DoWardJump();

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

	if (flDistance < 100) {

		std::string itemName = Source->GetObjectName();
		if (itemName.find("Ward") != std::string::npos || itemName.find("ward") != std::string::npos) {
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

	sight_stone = GPluginSDK->CreateItemForId(2049, 625);
	red_stone = GPluginSDK->CreateItemForId(2045, 625);
	yellow_trinket = GPluginSDK->CreateItemForId(3340, 625);
	pink_ward = GPluginSDK->CreateItemForId(2055, 625);
	t_knife = GPluginSDK->CreateItemForId(3711, 625);
	t_knifeA = GPluginSDK->CreateItemForId(1408, 625);
	t_knifeB = GPluginSDK->CreateItemForId(1409, 625);
	t_knifeC = GPluginSDK->CreateItemForId(1410, 625);
	t_knifeD = GPluginSDK->CreateItemForId(1418, 625);


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