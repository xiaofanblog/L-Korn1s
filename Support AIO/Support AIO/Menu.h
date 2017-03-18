#pragma once
#include "PluginSDK.h"
#include <map>

IMenu* MainMenu;


IMenu* QSet;
IMenu* WSet;
IMenu* ESet;
IMenu* RSet;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboQflash;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboElanded;
IMenuOption* ComboEQ;
IMenuOption* ComboEmin;
IMenuOption* ComboR;
IMenuOption* ComboRinterrupt;
IMenuOption* ComboRmin;
IMenuOption* ComboQmin;
IMenuOption* ComboQmax;
IMenuOption* ComboRhp;
IMenuOption* ComboQinterrupt;
IMenuOption* ComboRenable;


IMenuOption* ComboEgapclose;
IMenuOption* ComboQenable;
IMenuOption* ComboQcc;
IMenuOption* ComboWenable;
IMenuOption* ComboWQreset;
IMenuOption* ComboEslow;
IMenuOption* ComboEenable;
IMenuOption* ComboEflee;
IMenuOption* ComboEinterrupt;





IMenu* HookMenu;
IMenuOption* GrabQ;
IMenuOption* GrabQA;
IMenuOption* GrabQB;
IMenuOption* AutoQ;

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
IMenuOption* DrawEmin;
IMenuOption* DrawWRange;


std::map<int, IMenuOption*> ChampionUse;

IMenu* KillstealMenu;
IMenuOption* KSE;
IMenuOption* KSR;
IMenuOption* KSQ;
IMenuOption* KSW;
IMenuOption* HarassAA;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

ISpell* Flash;


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