#pragma once
#include "Menu.h"

class AlistarBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Alistar - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboQflash = ComboMenu->AddKey("Q Flash", 'T');
			ComboW = ComboMenu->CheckBox("Use W in Combo", true);
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			ComboRhp = ComboMenu->AddInteger("Use R if HP < X", 10, 100, 25);
			ComboRmin = ComboMenu->AddInteger("Enemies near for R", 1, 5, 1);

		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
			QColor = DrawingMenu->AddColor("Q Range Color", 0.f, 0.f, 255.f, 255.f);
			DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
			WColor = DrawingMenu->AddColor("W Range Color", 0.f, 0.f, 255.f, 255.f);
			DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
			EColor = DrawingMenu->AddColor("E Range Color", 0.f, 0.f, 255.f, 255.f);
			DrawQFRange = DrawingMenu->CheckBox("Draw Q Flash Range", true);
			RColor = DrawingMenu->AddColor("QFlash Range Color", 0.f, 0.f, 255.f, 255.f);

		}

		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			InterruptQ = MiscMenu->CheckBox("Q Interrupt", true);
			InterruptW = MiscMenu->CheckBox("W Interrupt", true);
			AntiGapQ = MiscMenu->CheckBox("Q AntiGap", true);
			AntiGapW = MiscMenu->CheckBox("W AntiGap", true);
		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		Q->SetOverrideRange(350);
		if (GEntityList->Player()->GetSpellSlot("SummonerFlash") != kSlotUnknown)
			Flash = GPluginSDK->CreateSpell(GEntityList->Player()->GetSpellSlot("SummonerFlash"), 425);


	}


	int GetEnemiesInRange(IUnit* Source, float range)
	{
		auto enemies = GEntityList->GetAllHeros(false, true);
		auto enemiesInRange = 0;

		for (auto enemy : enemies)
		{
			if (enemy->IsValidTarget() && !enemy->IsDead() && enemy != nullptr && enemy->GetTeam() != GEntityList->Player()->GetTeam())
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
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr && target->IsValidTarget() && target->IsHero() && target->IsValidTarget())
		{
			if (ComboW->Enabled() & GEntityList->Player()->IsValidTarget(target, W->Range()) && W->IsReady() && Q->IsReady() && (GEntityList->Player()->GetMana() > Q->ManaCost()+W->ManaCost()))
			{
				W->CastOnTarget(target);
			}
			if (ComboQ->Enabled() && Q->IsReady() && GEntityList->Player()->IsValidTarget(target, Q->Range()))
			{
				Q->CastOnTarget(target);
			}
			if (ComboE->Enabled() && E->IsReady() && GEntityList->Player()->IsValidTarget(target, E->Range()))
			{
				E->CastOnTarget(target);
			}
			if (ComboR->Enabled() && R->IsReady() && GetEnemiesInRange(GEntityList->Player(), W->Range()) >= ComboRmin->GetInteger())
			{
				if (GEntityList->Player()->HealthPercent() <= ComboRhp->GetInteger())
				{
					R->CastOnPlayer();
				}
			}
		}
	}

	void FlashQ()
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());
		if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				if (Q->IsReady() && Flash != nullptr && Flash->IsReady())
				{
					if ((GEntityList->Player()->GetPosition() - Enemy->GetPosition()).Length2D() > Q->Range() && Enemy->IsValidTarget() && Enemy != nullptr && ((GEntityList->Player()->GetPosition() - Enemy->GetPosition()).Length2D()) < Q->Range() + 380 && !Enemy->IsDead())
					{
						if (Q->CastOnPlayer())
						{
							Flash->CastOnPosition(Enemy->ServerPosition());
						}
					}
				}

			}
		}
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

	void Interrupt(InterruptibleSpell const& Args)
	{
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, Q->Range()) && InterruptQ->Enabled() && Q->IsReady())
		{
			Q->CastOnTarget(Args.Target);
		}
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, W->Range()) && InterruptW->Enabled() && W->IsReady() && (!Q->IsReady() || !GEntityList->Player()->IsValidTarget(Args.Target, Q->Range())))
		{
			W->CastOnTarget(Args.Target);
		}
	}
	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, Q->Range() + Args.Sender->BoundingRadius())
			&& AntiGapQ->Enabled() && E->IsReady())
		{
			Q->CastOnTarget(Args.Sender);
		}
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, W->Range() + Args.Sender->BoundingRadius())
			&& AntiGapW->Enabled() && W->IsReady() && (!Q->IsReady() || !GEntityList->Player()->IsValidTarget(Args.Sender, Q->Range())))
		{
			W->CastOnTarget(Args.Sender);
		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled())
		{
			Vec4 color;
			QColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, Q->Range());
		}
		if (DrawWRange->Enabled())
		{
			Vec4 color;
			WColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, W->Range());
		}
		if (DrawERange->Enabled())
		{
			Vec4 color;
			EColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, E->Range());
		}
		if (DrawQRange->Enabled())
		{
			Vec4 color;
			QColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, Q->Range());
		}
		if (DrawWRange->Enabled())
		{
			Vec4 color;
			WColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, W->Range());
		}
		if (DrawERange->Enabled())
		{
			Vec4 color;
			EColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, E->Range());
		}
		if (DrawRRange->Enabled())
		{
			Vec4 color;
			RColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, R->Range());
		}
		if (DrawQFRange->Enabled())
		{
			Vec4 color;
			RColor->GetColor(&color);

			GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), color, Q->Range()+380);
		}
	
	}
};