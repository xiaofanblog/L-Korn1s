#pragma once
#include "Menu.h"
#include <string>
class SonaBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Sona - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			ComboRmin = ComboMenu->AddInteger("Use R if enemies X >", 1, 5, 2);
			ForceR = ComboMenu->AddKey("Force R", 'T');
			SupportMode = ComboMenu->CheckBox("Support Mode", true);

		}
		MiscMenu = MainMenu->AddMenu("Misc.");
		{
			HarassQ = MiscMenu->AddKey("Auto Q Toggle", 'T');
			AntiGapE = MiscMenu->CheckBox("Anti-Gapclose E", true);
			ComboEslow = MiscMenu->CheckBox("Auto E if slowed", true);
			InterruptR = MiscMenu->CheckBox("Interrupt R", true);
		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q range", true);
			DrawWRange = DrawingMenu->CheckBox("Draw W range", true);
			DrawERange = DrawingMenu->CheckBox("Draw E range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
			DrawDamage = DrawingMenu->CheckBox("Draw Toggle", true);

		}

		HealMenu = MainMenu->AddMenu("Healing");
		{
			HealW = HealMenu->CheckBox("Use auto W", true);
			HealWally = HealMenu->AddInteger("Ally HP percent(if lower than)", 10, 100, 50);
			HealWmyself = HealMenu->AddInteger("My HP percent(if lower than)", 10, 100, 50);
			HealWenemy = HealMenu->AddInteger("Heal if X Allies =>", 1, 5, 1);
		}
	}

	void HarasTog()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			if (GetAsyncKeyState(HarassQ->GetInteger()))
			{
				if (Harassenable == true && GGame->Time() > Keypres)
				{
					Harassenable = false;
					Keypres = GGame->Time() + 0.250;

				}
				if (Harassenable == false && GGame->Time() > Keypres)
				{
					Harassenable = true;
					Keypres = GGame->Time() + 0.250;

				}

			}
		}
	}
	void AAdisable()
	{
		if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
		{
			auto target = GOrbwalking->GetLastTarget();
			if (SupportMode->Enabled() && GetAlliesInRange(GEntityList->Player(), 1000) >= 1)
			{
				if (target != nullptr && target->IsValidTarget() && (target->IsCreep() || target->IsJungleCreep()))
				{
					GOrbwalking->SetAttacksAllowed(false);
				}
			}
			else if (target != nullptr && target->IsValidTarget() && target->IsHero())
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

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, true, (kCollidesWithYasuoWall));
		R->SetSkillshot(0.25f, 140.f, 2400.f, 1000.f);
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

	void Semi()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				if (!Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), 860) && Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && !Enemy->IsDead())
				{
					R->CastOnTarget(Enemy, kHitChanceHigh);
				}
			}
		}
	}

	void Auto()
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (Ally != nullptr && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && Ally->HasBuffOfType(BUFF_Slow))
			{
				if (ComboEslow->Enabled() && E->Range() && E->IsReady())
				{
					E->CastOnTarget(Ally);
				}
			}
		}
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

							Q->CastOnTarget(Enemy);

						}
					}

				}
			}
		}
	}


	void Healing()
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (HealW->Enabled() && W->IsReady())
			{
				if (!GEntityList->Player()->IsRecalling() && !Ally->IsRecalling() && Ally != nullptr && Ally->HealthPercent() <= HealWally->GetInteger() &&  Ally != GEntityList->Player() && !Ally->IsDead() && GetAlliesInRange(GEntityList->Player(), W->Range()) >= HealWenemy->GetInteger())
				{
					W->CastOnTarget(Ally);
				}
				else if (Ally != nullptr && Ally == GEntityList->Player() && !GEntityList->Player()->IsDead())
				{
					if (!GEntityList->Player()->IsRecalling() && GEntityList->Player()->HealthPercent() <= HealWmyself->GetInteger() && !GUtility->IsPositionInFountain(GEntityList->Player()->GetPosition(), true, false))
					{
						W->CastOnPlayer();
					}
				}
			}
		}
	}

	void Combo()
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true)) {
			if (Enemy != nullptr && Enemy->IsValidTarget() && Enemy->IsHero() && !Enemy->IsDead())
			{
				if (ComboQ->Enabled() && Q->IsReady())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
					if (GEntityList->Player()->IsValidTarget(target, Q->Range()))
					{
						Q->CastOnTarget(target);
					}
				}
				if (ComboE->Enabled() && E->IsReady())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
					if (GEntityList->Player()->IsValidTarget(target, E->Range()))
					{
						E->CastOnTarget(target);
					}
				}
				if (ComboR->Enabled() && R->IsReady())
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
					if (GEntityList->Player()->IsValidTarget(target, R->Range()))
					{
						R->CastOnTargetAoE(target, ComboRmin->GetInteger(), kHitChanceHigh);
					}
				}
			}
		}
	}

	void AntiGapclose(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player() && Args.Sender->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Sender, 200 + Args.Sender->BoundingRadius()) && AntiGapE->Enabled() && E->IsReady())
		{
			E->CastOnPlayer();
		}
	}


	void Interrupt(InterruptibleSpell const& Args)
	{
		float endtime;
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, 860) && InterruptR->Enabled() && R->IsReady())
		{
			if (Args.Target->IsCastingImportantSpell(&endtime))
			{
				R->CastOnTarget(Args.Target, kHitChanceHigh);
			}

		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), R->Range()); }
		if (DrawDamage->Enabled())
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

};