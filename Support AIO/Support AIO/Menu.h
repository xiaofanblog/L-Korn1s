#pragma once
#include "PluginSDK.h"
#include <map>
#include "Extension.h"
#include "EventManager.h"
#include "MalachitePred.h"
#include "Geometry.h"
IMenu* MainMenu;


IMenu* QSet;
IMenu* WSet;
IMenu* ESet;
IMenu* RSet;
int xOffset = 10;
int yOffset = 15;
int Width = 103;
int Height = 8;
Vec4 Color = Vec4(105, 198, 5, 255);
Vec4 FillColor = Vec4(198, 176, 5, 255);
Vec4 Color2 = Vec4(238, 18, 0, 72);


float DragonDmg = 0;
double DragonTime = 0;

IMenu* ComboMenu;
IMenuOption* Survival;
IMenuOption* Chase;
IMenuOption* ComboQ;
IMenuOption* ComboQ2;
IMenuOption* ComboQflash;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* QWQMouse;
IMenuOption* ComboElanded;
IMenuOption* ComboEQ;
IMenuOption* DrawPred;
IMenuOption* QColor;
IMenuOption* WColor;
IMenuOption* EColor;
IMenuOption* RColor;
IMenuOption* ComboQMode;
IMenuOption* ComboMode;
IMenuOption* HarassQMode;
IMenuOption* HarassMode;
IMenuOption* ComboEmin;
IMenuOption* ComboR;
IMenuOption* ComboRinterrupt;
IMenuOption* ComboRmin;
IMenuOption* ComboQmin;
IMenuOption* ComboQmax;
IMenuOption* InterruptE;
IMenuOption* ComboRhp;
IMenuOption* ComboQinterrupt;

IMenuOption* HealRsemi;
IMenuOption* ComboRenable;
IUnit* EMissile;
int lastQcast;


IMenuOption* ComboEgapclose;
IMenuOption* ComboQenable;
IMenuOption* ComboQcc;
IMenuOption* ComboWenable;
IMenuOption* ComboWQreset;
IMenuOption* ComboEslow;
IMenuOption* ComboEenable;
IMenuOption* ComboEflee;
IMenuOption* ComboEinterrupt;

IMenu* ESettings;

IMenuOption* LastHitMana;
IMenuOption* TearQ;
IMenuOption* TearW;
IMenuOption* ManaTear;

IMenu* FarmMenu;
IMenuOption* FarmW;
IMenuOption* FarmWmin;
IMenuOption* FarmMana;
IMenuOption* FarmQmin;
IMenuOption* FarmE;
IMenuOption* FarmEmin;
IMenuOption* AA;
IMenu* LastHitMenu;

IMenu* HookMenu;
IMenuOption* GrabQ;
IMenuOption* GrabQA;
IMenuOption* GrabQB;
IMenuOption* AutoQ;
IMenuOption* LastW;
IMenuOption* LastE;
IMenu* BlacklistMenu;
IMenu* BlacklistMenuR; 
IMenu* BlacklistMenuW;

IMenu* MiscMenu;
IMenuOption* InterruptQ;
IMenuOption* InterruptR;
IMenuOption* AntigapQ;
IMenuOption* HealW;
IMenuOption* HealWally;
IMenuOption* HealWmy;
IMenuOption* HealR;
IMenuOption* UseE;
IMenuOption* HealEAlly;
IMenuOption* HealRhp;
IMenuOption* MiscMana;
IMenuOption* HealEMe;
IMenuOption* InterruptW;
IMenuOption* DrawKillable;
IMenuOption* AAlevel;
IMenuOption* AntiGapQ;
IMenuOption* AntiGapW;
IMenuOption* HealWenemy;
IMenuOption* HealWmyself;
IMenuOption* ComboRally;
IMenuOption* ForceR;


IMenuOption* WHeal1;
IMenuOption* WHeal2;
IMenuOption* WHeal3;
IMenuOption* WHeal4;
IMenuOption* WHeal5;
IMenuOption* EPriority1;
IMenuOption* EPriority2;
IMenuOption* EPriority3;
IMenuOption* EPriority4; 
IMenuOption* EPriority5;

IMenu* HarassMenu;
IMenu* HealMenu;
IMenuOption* HarassMana;
IMenuOption* HarassQ;
IMenuOption* HarassQ2;
IMenuOption* HarassE;
IMenuOption* HarassW;
IMenu* WMenu;
IMenu* EMenu;
IMenuOption* priorityEtoggle;
IMenuOption* priorityESelector;
IMenuOption* priorityWtoggle;
IMenuOption* priorityWSelector;
IMenu* DrawingMenu;
IMenuOption* DrawQRange;
IMenuOption* DrawQFRange;
IMenuOption* DrawRRange;
IMenuOption* DrawERange;
IMenuOption* DrawQmin;
IMenuOption* AntiGapR;
IMenuOption* DrawEmin;
IMenuOption* DrawWRange;
IMenuOption* FarmQ;
IMenuOption* ComboR1vs1;
IMenu* RSettings;
IMenuOption* SaveW;
IMenuOption* SaveKey;
IMenuOption* SaveE;
IMenuOption* AntiGapE;
IMenuOption* DrawDamage;
IMenuOption* ComboPred;
IMenuOption* LastQ;

IMenu* ShieldMenu;
IMenuOption* ShieldE;
IMenuOption* ShieldKill;
IMenuOption* ShieldHP;

IUnit* minion;
std::vector<IUnit*> minions;

int lastw;

std::map<int, IMenuOption*> ChampionUse;

IMenu* KillstealMenu;
IMenuOption* KSE;
IMenuOption* KSR;
IMenuOption* AAkey;
IMenuOption* KSQ;
IMenuOption* ComboQdash;
IMenuOption* KSW;
IMenuOption* SupportMode;
IMenuOption* HarassAA;
IMenuOption* KSRmin;
IMenuOption* ShieldAA;
IMenuOption* ShieldInc;
IMenuOption* ShieldHPinc;

IMenu* JungleKS;
IMenuOption* AutoRjungle;
IMenuOption*  Rdragon;
IMenuOption* Rbaron;
IMenuOption* Rred;
IMenuOption* Rblue;
IMenuOption* Rally;
ISpell2* Q;
ISpell2* Q2;
ISpell2* Q3;
ISpell2* W;
ISpell2* E;
ISpell2* R;
IMenuOption* Rmin;

IUnit* Enemy;
IUnit* VelkozQMissile;

ISpell* Flash;

int lastwcast;
int LastQcast;

IEventManager* eventManager = nullptr;


IUnit* Player;
IUnit* WHeal01;
IUnit* WHeal02;
IUnit* WHeal03;
IUnit* WHeal04;
IUnit* WHeal05;
IUnit* EPriority01;
IUnit* EPriority02;
IUnit* EPriority03;
IUnit* EPriority04;
IUnit* EPriority05;