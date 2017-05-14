#pragma once
#pragma once
#include "Menu.h"

class ZyraBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Zyra - Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo");
		{
			ComboQ = ComboMenu->CheckBox("Use Q", true);
			ComboW = ComboMenu->CheckBox("Use W", true);
			ComboE = ComboMenu->CheckBox("Use E", true);
			ComboR = ComboMenu->CheckBox("Use R", true);
			ComboRmin = ComboMenu->AddFloat("^- if Hits", 1, 5, 2);
			SupportMode = ComboMenu->CheckBox("Support Mode", true);

		}
		FarmMenu = MainMenu->AddMenu("Farming");
		{
			FarmMana = FarmMenu->AddFloat("Mana Manager", 0, 100, 50);
			FarmQ = FarmMenu->CheckBox("Use Q", true);
			FarmQmin = FarmMenu->AddFloat("^- if Hits", 1, 6, 2);
			FarmE = FarmMenu->CheckBox("Use E", true);
			FarmEmin = FarmMenu->AddFloat("^- if Hits", 1, 6, 2);
		}
		DrawingMenu = MainMenu->AddMenu("Drawings");
		{
			DrawQRange = DrawingMenu->CheckBox("Draw Q Range", true);
			DrawWRange = DrawingMenu->CheckBox("Draw W Range", true);
			DrawERange = DrawingMenu->CheckBox("Draw E Range", true);
			DrawRRange = DrawingMenu->CheckBox("Draw R Range", true);

		}
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithNothing));
		Q->SetSkillshot(0.25f, 100.f, 3000.f, 800.f);

		W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, (kCollidesWithNothing));
		W->SetSkillshot(0.25f, 250.f, 1000.f, 850.f);

		E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, (kCollidesWithYasuoWall));
		E->SetSkillshot(0.25f, 60.f, 2500.f, 1150.f);

		R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, (kCollidesWithNothing));


	}
	static int GPlugin(IPluginSDK *sdk, float range)
	{
		static auto entityList = sdk->GetEntityList();
		static auto player = entityList->Player();

		auto count = 0;

		for (auto unit : entityList->GetAllHeros(false, true))
		{
			if (player->IsValidTarget(unit, range))
			{
				count++;
			}
		}

		return count;
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
		for (auto target : GEntityList->GetAllHeros(false, true))

		{

			if (ComboQ->Enabled() && Q->IsReady())
			{
				if (GPlugin(GPluginSDK, Q->Range()) == 1)
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()) && Target != nullptr && Target->IsValidTarget() && Target->IsHero() && !Target->IsDead())
					{
						Q->CastOnTarget(Target, kHitChanceHigh);
					}
				}
				if (GPlugin(GPluginSDK, Q->Range()) >= 2)
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
					Q->CastOnTargetAoE(Target, 2, kHitChanceHigh);
				}
			}
			if (ComboW->Enabled() && W->IsReady() && target != nullptr)
			{
				if (Q->IsReady() && ComboQ->Enabled())
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()))
					{
						if (Target != nullptr && Target->IsValidTarget() && Target->IsHero() && !Target->IsDead())

						{
							Vec3 vecPoint = GEntityList->Player()->ServerPosition() + (Target->ServerPosition() - GEntityList->Player()->ServerPosition()).VectorNormalize() * 800;

							W->CastOnPosition(vecPoint);
						}
					}
				}
				if (E->IsReady() && ComboE->Enabled())
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
					if (GEntityList->Player()->IsValidTarget(Target, E->Range()))
					{
						if (Target != nullptr && Target->IsValidTarget() && Target->IsHero() && !Target->IsDead())
						{
							Vec3 vecPoint = GEntityList->Player()->ServerPosition() + (Target->ServerPosition() - GEntityList->Player()->ServerPosition()).VectorNormalize() * 800;

							W->CastOnPosition(vecPoint);
						}
					}
				}
			}
			if (ComboE->Enabled() && E->IsReady() && target != nullptr)
			{

				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (GEntityList->Player()->IsValidTarget(Target, E->Range()) && Target != nullptr && Target->IsValidTarget() && Target->IsHero() && !Target->IsDead())
				{
					E->CastOnTarget(Target, kHitChanceHigh);
				}
			}
		}
		if (ComboR->Enabled() && R->IsReady())
		{
			auto RPlugin = ComboRmin->GetFloat();
			auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
			if (Target != nullptr && Target->IsValidTarget() && Target->IsHero() && !Target->IsDead())
			{
				if (RPlugin == 1 && GEntityList->Player()->IsValidTarget(Target, 1200))

				{

					R->CastOnTarget(Target, kHitChanceMedium);

				}



				if (GPlugin(GPluginSDK, R->Range()) >= RPlugin)
				{
					int enemies;
					Vec3 pos;
					R->FindBestCastPosition(false, true, pos, enemies);
					if (enemies >= RPlugin)
					{
						R->CastOnPosition(pos);

					}
				}
			}
		}
	}
	static int CountMinionsNearMe(IPluginSDK *sdk, float range)
	{
		static auto entityList = sdk->GetEntityList();
		static auto player = entityList->Player();

		auto count = 0;

		for (auto unit : entityList->GetAllMinions(false, true, true))
		{
			if (player->IsValidTarget(unit, range))
			{
				count++;
			}
		}

		return count;
	}
	void Push()
	{
		minions = GEntityList->GetAllMinions(false, true, true);
		for (IUnit* minion : minions)
			if (GEntityList->Player()->ManaPercent() >= FarmMana->GetFloat())
			{
				if (FarmQ->Enabled() && Q->IsReady())
				{

					if (minion != nullptr)
					{
						if (!minion->IsDead() && minion->IsValidTarget() && GEntityList->Player()->IsValidTarget(minion, Q->Range()))
						{
							Vec3 bla;
							int blabla;
							GPrediction->FindBestCastPosition(Q->Range(), Q->Radius(), false, true, false, bla, blabla);
							if (blabla >= FarmQmin->GetFloat() && FarmQmin->GetFloat() >= 2)
							{
								Q->CastOnPosition(bla);
							}
							if (FarmQmin->GetFloat() == 1)
							{
								Q->CastOnUnit(minion);
							}

						}
					}


				}


				if (FarmE->Enabled() && E->IsReady())
				{
					if (minion != nullptr)
					{
						if (!(minion->IsDead()) && minion->IsValidTarget() &&  GEntityList->Player()->IsValidTarget(minion, E->Range()) && minion != nullptr && (CountMinionsNearMe(GPluginSDK, 1100) >= 2))
						{

							Vec3 bla;
							int blabla;
							GPrediction->FindBestCastPosition(E->Range(), 70, true, true, false, bla, blabla);
							if (blabla >= FarmEmin->GetFloat())
								E->CastOnPosition(bla);

						}
					}
				}

			}


	}
	void Poke()

	{

		for (auto target : GEntityList->GetAllHeros(false, true))

		{

			if (ComboQ->Enabled() && Q->IsReady())
			{
				if (GPlugin(GPluginSDK, Q->Range()) == 1)
				{
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

					if (GEntityList->Player()->IsValidTarget(Target, Q->Range()) && Target != nullptr && Target->IsValidTarget() && Target->IsHero() && !Target->IsDead())
					{
						Q->CastOnTarget(Target, kHitChanceHigh);
					}
				}
				if (GPlugin(GPluginSDK, Q->Range()) >= 2)
				{
					Q->CastOnTargetAoE(target, 2, kHitChanceHigh);
				}
			}
			if (ComboW->Enabled() && W->IsReady() && target != nullptr)
			{
				auto Target = GTargetSelector->FindTarget(ClosestPriority, SpellDamage, 1500);
				if (Target != nullptr && Target->IsValidTarget() && Target->IsHero() && !Target->IsDead())
					if (target->IsChasing(Target))
					{
						{ Vec3 vecPoint = GEntityList->Player()->ServerPosition() + (Target->ServerPosition() - GEntityList->Player()->ServerPosition()).VectorNormalize() * 800;

						W->CastOnPosition(vecPoint); }
					}
					else if (Q->IsReady() || E->IsReady())
					{
						if (target->IsValidTarget(GEntityList->Player(), Q->Range()) || target->IsValidTarget(GEntityList->Player(), E->Range()))
							auto Target = GTargetSelector->FindTarget(ClosestPriority, SpellDamage, Q->Range());
						if (!target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(target))
							|| (target->IsValidTarget(GEntityList->Player(), GEntityList->Player()->GetRealAutoAttackRange(target))
								&& (target->HasBuffOfType(BUFF_Knockup) || target->HasBuffOfType(BUFF_Stun))))
							W->CastOnTarget(Target);
					}
			}
			if (ComboE->Enabled() && E->IsReady() && target != nullptr)
			{

				auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
				if (GEntityList->Player()->IsValidTarget(Target, E->Range()) && Target != nullptr && Target->IsValidTarget() && Target->IsHero() && !Target->IsDead())
				{
					E->CastOnTarget(Target, kHitChanceHigh);
				}
			}
		}
	}
	void Plants()
	{
		for (auto plants : GEntityList->GetAllUnits())
		{
			if (std::string(plants->GetObjectName()) == "Seed")
			{
				if ((std::string(plants->GetObjectName()).find("Zyra_Base_W") || std::string(plants->GetObjectName()).find("Zyra_Base_P")) && plants != nullptr && !plants->IsDead() && plants->IsValidObject() && plants->GetTeam() == GEntityList->Player()->GetTeam() && !plants->IsDead())
				{
					///GPluginSDK->GetRenderer()->DrawOutlinedCircle(plants->GetPosition(), Vec4(255, 255, 0, 255), 100);
					auto Target = GTargetSelector->FindTarget(ClosestPriority, SpellDamage, 1000);
					if (Target != nullptr && Target->IsHero() && !Target->IsDead())
					{
						if (Q->IsReady() && Target->IsValidTarget(plants, 500) && Target->IsValidTarget(GEntityList->Player(), 1000))
						{
							///if (plants->IsValidTarget(Target, 100))
							Q->CastOnUnit(plants);
						}
					}
				}
			}
		}
	}
	void Draw() const
	{
		if (DrawQRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
		if (DrawWRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
		if (DrawRRange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
		if (DrawERange->Enabled()) { GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	}
};