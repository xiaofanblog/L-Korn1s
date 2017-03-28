#pragma once
#include "Menu.h"

class ZileanBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Zilean - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			QSet = ComboMenu->AddMenu("Q settings");
			ComboQenable = QSet->CheckBox("Use Q", true);
			ComboQcc = QSet->CheckBox("Auto Q on CC", true);
			ComboQinterrupt = QSet->CheckBox("Use QWQ to Interrupt", true);
			WSet = ComboMenu->AddMenu("W settings");
			ComboWenable = WSet->CheckBox("Use W", true);
			ComboWQreset = WSet->CheckBox("Use W to reset Q", true);
			ESet = ComboMenu->AddMenu("E settings");
			ComboEenable = ESet->CheckBox("Use E", true);
			ComboEslow = ESet->CheckBox("Use auto E on slowed Ally", true);
			ComboEflee = ESet->AddKey("Flee with E", 'G');
			RSet = ComboMenu->AddMenu("R settings");
			ComboRenable = RSet->CheckBox("Use R", true);
			//ComboRkills = RSet->CheckBox("Auto R if damage kills Ally", true);
			ComboRhp = RSet->AddInteger("Use R if HP <", 5, 30, 15);
			QWQMouse = ComboMenu->AddKey("QWQ to Mouse", 'T');
			SupportMode = ComboMenu->CheckBox("Support Mode", true);

		}
		HarassMenu = MainMenu->AddMenu("Harass");
		{
			HarassQ = HarassMenu->CheckBox("Use Q", true);
			HarassW = HarassMenu->CheckBox("Use W to reset", true);
		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
			DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
		}

		KillstealMenu = MainMenu->AddMenu("Killsteal");
		{
			KSQ = KillstealMenu->CheckBox("Use Q", true);
			KSW = KillstealMenu->CheckBox("Use QWQ", true);
		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, true, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		Q->SetOverrideDelay(0.25);
		Q->SetOverrideRadius(80);
		Q->SetOverrideSpeed(3800);
		Q->SetOverrideRange(810);
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);


	}
	int GetAlliesInRange(IUnit* Source, float range)
	{
		auto allies = GEntityList->GetAllHeros(true, false);
		auto AlliesInRange = 0;

		for (auto target : allies)
		{
			if (target != GEntityList->Player())
			{
				if (target != nullptr && !target->IsDead())
				{
					auto flDistance = (target->GetPosition() - Source->GetPosition()).Length();
					if (flDistance <= range)
					{
						AlliesInRange++;
					}
				}
			}
		}
		return AlliesInRange;
	}

	void AAdisable()
	{
		if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
		{
			if (SupportMode->Enabled() && GetAlliesInRange(GEntityList->Player(), 1000) >= 1)
			{
				GOrbwalking->SetAttacksAllowed(false);
			}
			else if (!SupportMode->Enabled() || GetAlliesInRange(GEntityList->Player(), 1000) == 0)
			{
				GOrbwalking->SetAttacksAllowed(true);
			}
		}
		if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		{
			if (SupportMode->Enabled() && GetAlliesInRange(GEntityList->Player(), 1000) >= 1)
			{
				GOrbwalking->SetAttacksAllowed(false);
			}
			else if (!SupportMode->Enabled() || GetAlliesInRange(GEntityList->Player(), 1000) == 0)
			{
				GOrbwalking->SetAttacksAllowed(true);
			}
		}
		else if (GOrbwalking->GetOrbwalkingMode() != kModeLaneClear && GOrbwalking->GetOrbwalkingMode() != kModeMixed)
		{
			GOrbwalking->SetAttacksAllowed(true);

		}
	}

	void Combo()
	{

		if (GEntityList->Player()->HasIncomingDamage())
		{
			GGame->PrintChat("lul");
		}
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && !target->IsInvulnerable() && !target->IsDead())
			if (ComboEenable->Enabled() && E->IsReady() && E->Range() && !target->HasBuffOfType(BUFF_Slow))
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (target != nullptr)
				{
					E->CastOnTarget(target);
				}
			}
		if (ComboQenable->Enabled() && Q->IsReady() && Q->Range())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				Q->CastOnTarget(target, kHitChanceHigh);
			}
		}
		if (ComboWenable->Enabled() && W->IsReady() && Q->Range())
		{
			if (!Q->IsReady() && GEntityList->Player()->GetMana() > W->ManaCost() + Q->ManaCost())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (target != nullptr)
				{
					W->CastOnPlayer();
					Q->CastOnTarget(target, kHitChanceHigh);
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
			if (HarassW->Enabled() && W->IsReady() && Q->Range())
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (!Q->IsReady())
					if (target != nullptr)
					{
						W->CastOnPlayer();
						Q->CastOnTarget(target);
					}

			}

		}
	}
	void Killsteal()
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (target != nullptr && !target->IsInvulnerable() && !target->IsDead())
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotQ);


			if (KSQ->Enabled() && Q->IsReady() && target->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > target->GetHealth())
			{
				Q->CastOnTarget(target, kHitChanceHigh);
			}
			if (!Q->IsReady())
			{
				if (KSQ->Enabled() && KSW->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > target->GetHealth())
				{
					W->CastOnPlayer();
					Q->CastOnTarget(target, kHitChanceHigh);
				}
			}
			if (KSQ->Enabled() && KSW->Enabled() && target->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage * 2 > target->GetHealth())
			{

				if (!Q->IsReady())
				{
					W->CastOnPlayer();
					Q->CastOnTarget(target);
				}

			}
		}
	}

	void QWQ()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			if (Q->IsReady())
			{
				Q->CastOnPosition(GGame->CursorPosition());
			}
			if (W->IsReady() && !Q->IsReady())
			{
				W->CastOnPlayer();
			}
			if (Q->IsReady())
			{
				Q->CastOnPosition(GGame->CursorPosition());
			}
		}
	}
	


	void Auto()
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (Enemy != nullptr && !Enemy->IsDead())
			{
				if (ComboQcc->Enabled())
				{
					if (Enemy->IsValidTarget(Player, Q->Range()) && Q->Range())
					{
						if (Enemy->HasBuffOfType(BUFF_Stun) || Enemy->HasBuffOfType(BUFF_Charm) || Enemy->HasBuffOfType(BUFF_Fear) || Enemy->HasBuffOfType(BUFF_Taunt) || Enemy->HasBuffOfType(BUFF_Suppression) || Enemy->HasBuffOfType(BUFF_Snare))
						{
							Q->CastOnTarget(Enemy);
						}

					}
				}
			}
		}
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && Ally->HasBuffOfType(BUFF_Slow))
			{
				if (ComboEslow->Enabled() && E->Range() && E->IsReady())
					E->CastOnTarget(Ally);
			}
		}
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), R->Range()))
			{
				if (ComboRenable->Enabled() && R->Range() && R->IsReady())
				{
					/*if (Ally->HasIncomingDamage() > Ally->GetHealth())
					{
					
						GGame->PrintChat("omg");
						R->CastOnTarget(Ally);
					}*/
				}
				if (ComboRhp->GetInteger() > Ally->HealthPercent() && R->Range() && R->IsReady())
				{
					R->CastOnTarget(Ally);
				}
			}
		}
		if (ComboRhp->GetInteger() > GEntityList->Player()->HealthPercent() && R->Range() && R->IsReady())
		{
			R->CastOnPlayer();
		}
		/*if (GEntityList->Player()->HasIncomingDamage() > Player->GetHealth() && ComboRkills->Enabled())
		{
		R->CastOnPlayer();
		}*/
	}
	void Flee()
	{
		if (!GGame->IsChatOpen())
		{
			GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
			if (E->IsReady())
			{
				E->CastOnPlayer();
			}

		}
	}

	void Interrupt(InterruptibleSpell const& Args)
	{
		if (ComboQinterrupt->Enabled() && (Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length() < Q->Range() && Q->IsReady() && Args.Target->IsValidTarget())
		{
			Q->CastOnTarget(Args.Target);
		}
		if (!Q->IsReady())
		{
			if (ComboQinterrupt->Enabled() && (Args.Target->GetPosition() - GEntityList->Player()->GetPosition()).Length() < Q->Range() && Args.Target->IsValidTarget())
			{
				W->CastOnPlayer();
				Q->CastOnTarget(Args.Target);
			}
		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	}
};