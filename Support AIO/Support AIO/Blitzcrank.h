#pragma once
#include "Menu.h"

class BlitzcrankBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Blitzcrank - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q in Combo", true);
			ComboE = ComboMenu->CheckBox("Use E in Combo", true);
			ComboElanded = ComboMenu->CheckBox("Use E only if Q landed", true);
			ComboR = ComboMenu->CheckBox("Use R in Combo", true);
			ComboRinterrupt = ComboMenu->CheckBox("Use R to Interrupt", true);
			ComboRmin = ComboMenu->AddInteger("Use R if enemies X >", 1, 5, 2);

		}
		HookMenu = MainMenu->AddMenu("Q settings");
		{
			GrabQ = HookMenu->AddKey("Grab Q", 'T');
			AutoQ = HookMenu->CheckBox("Use auto Q", false);
			ComboQmin = HookMenu->AddInteger("Min Q range", 10, 400, 300);
			ComboQmax = HookMenu->AddInteger("Max Q range", 600, 900, 900);
		}


		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q max", true);
			DrawQmin = DrawingMenu->CheckBox("Draw Q minimum", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);
			DrawPred = DrawingMenu->CheckBox("Draw Prediction", true);
		}

		KillstealMenu = MainMenu->AddMenu("Killsteal");
		{
			KSQ = KillstealMenu->CheckBox("Use Q", true);
			KSR = KillstealMenu->CheckBox("Use R", true);
		}

		BlacklistMenu = MainMenu->AddMenu("Blacklist");

		for (auto enemy : GEntityList->GetAllHeros(false, true)) {
			BlacklistMenu->CheckBox(enemy->ChampionName(), false);
		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall, kCollidesWithMinions));
		Q->SetOverrideDelay(0.25);
		Q->SetOverrideRadius(50);
		Q->SetOverrideSpeed(2000);
		Q->SetOverrideRange(950);
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
		R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, static_cast<eCollisionFlags>(kCollidesWithNothing));


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


		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			IMenuOption * temp = BlacklistMenu->GetOption(Enemy->ChampionName());
			if (ComboQ->Enabled() && Q->IsReady() && Q->Range() && !temp->Enabled() && Enemy->IsValidTarget())
			{
				if ((!Enemy->HasBuffOfType(BUFF_SpellShield) || !Enemy->HasBuffOfType(BUFF_SpellImmunity)) && Enemy != nullptr && (Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() <= ComboQmax->GetInteger() && (Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() >= ComboQmin->GetInteger())
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
					if (InSpellRange(Q, pred))
						Q->CastOnPosition(pred);

				}

			}
			if (ComboE->Enabled() && E->IsReady() && Q->Range())
			{
				if (!ComboElanded->Enabled())
				{
					if (Enemy != nullptr && (Enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 300)
					{
						if (E->CastOnPlayer())
						{
							GOrbwalking->SetOverrideTarget(Enemy);
						}
					}
				}
				if (ComboElanded->Enabled() && E->IsReady())
				{
					if (Enemy->HasBuff("rocketgrab2") && (Enemy != nullptr))
					{
						if (E->CastOnPlayer())
						{
							GOrbwalking->SetOverrideTarget(Enemy);
						}
					}
				}
			}
			if (ComboR->Enabled() && R->IsReady() && R->Range())
			{
				if (Enemy != nullptr && !Enemy->IsDead() && !Enemy->IsInvulnerable() && GetEnemiesInRange(R->Range()) >= ComboRmin->GetInteger())
				{
					R->CastOnPlayer();
				}
			}
		}
	}

	void Hook()
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (AutoQ->Enabled())
			{
				IMenuOption * temp = BlacklistMenu->GetOption(Enemy->ChampionName());
				if (Q->IsReady() && !temp->Enabled() && Q->Range())
				{
					if (Enemy != nullptr)
					{

						Vec3 pred;
						GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
						if (InSpellRange(Q, pred))
							Q->CastOnPosition(pred);
					}
				}
			}
		}
	}


	void ManQ()
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			IMenuOption * temp = BlacklistMenu->GetOption(Enemy->ChampionName());
			if (Q->IsReady() && !temp->Enabled() && Q->Range())
			{
				if (Enemy != nullptr)
				{

					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
					if (InSpellRange(Q, pred))
						Q->CastOnPosition(pred);
				}
			}
		}
	}

	void Killsteal()
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			auto QDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotQ);
			auto RDamage = GDamage->GetSpellDamage(GEntityList->Player(), Enemy, kSlotR);

			if (Enemy != nullptr && !Enemy->IsDead())
			{
				if (KSR->Enabled() && R->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), R->Range()) && RDamage > Enemy->GetHealth())
				{
					R->CastOnPlayer();
				}
				if (KSQ->Enabled() && Q->IsReady() && Enemy->IsValidTarget(GEntityList->Player(), Q->Range()) && QDamage > Enemy->GetHealth())
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(Enemy, 0.2f, true, pred);
					if (InSpellRange(Q, pred))
						Q->CastOnPosition(pred);
				}

			}
		}
	}

	void Interrupt(InterruptibleSpell const& Args)
	{
		if (Args.Target != GEntityList->Player() && Args.Target->IsEnemy(GEntityList->Player()) && GEntityList->Player()->IsValidTarget(Args.Target, R->Range()) && ComboRinterrupt->Enabled() && R->IsReady())
		{
			R->CastOnTarget(Args.Target);
		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), ComboQmax->GetInteger()); }
		if (DrawQmin->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), ComboQmin->GetInteger()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), R->Range()); }
		if (DrawPred->Enabled())
		{
			for (auto hero : GEntityList->GetAllHeros(false, true))
			{
				if (!hero->IsDead() && hero->IsValidTarget() && hero->IsValidTarget(GEntityList->Player(), Q->Range()))
				{
					Vec3 pred;
					GPrediction->GetFutureUnitPosition(hero, 0.2f, true, pred);
					Vec2 vecMyPosition;
					Vec2 vecProjectedPosition;
					if (GGame->Projection(GEntityList->Player()->GetPosition(), &vecMyPosition))
					{
						GGame->Projection(pred, &vecProjectedPosition);
						GRender->DrawOutlinedCircle(pred, Vec4(25, 255, 0, 200), Q->Radius());
						if (GNavMesh->IsPointWall(pred))
							GRender->DrawLine(vecMyPosition, vecProjectedPosition, Vec4(25, 255, 0, 200));
						else
							GRender->DrawLine(vecMyPosition, vecProjectedPosition, Vec4(25, 255, 0, 200));
					}
				}
				
			}
		}
	}
};