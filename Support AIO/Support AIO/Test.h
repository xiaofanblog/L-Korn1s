#pragma once
#include "Menu.h"

class TestBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Test");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);

		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall, kCollidesWithMinions));
		Q->SetOverrideDelay(0.25);
		Q->SetOverrideRadius(60);
		Q->SetOverrideSpeed(2000);
		Q->SetOverrideRange(1150);
	}




	void Combo()
	{
		if (ComboQ->Enabled() && Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				Q->CastOnTarget(target);
			}
		}

	}
};