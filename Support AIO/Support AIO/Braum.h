#pragma once
#include "Menu.h"
#include <string>
class BraumBase
{
public:

	void DrawMenu()
	{
		MainMenu = GPluginSDK->AddMenu("Braum :: Support AIO");
		ComboMenu = MainMenu->AddMenu("Combo Menu");
		FarmMenu = MainMenu->AddMenu("Lane Clear Menu");
		DrawingMenu = MainMenu->AddMenu("Drawings");
		MiscMenu = MainMenu->AddMenu("Misc.");


		ComboQ = ComboMenu->CheckBox("Use Q", true);
		ComboR = ComboMenu->CheckBox("Use R", true);
		ComboRmin = ComboMenu->AddInteger("Min enemies for R", 1, 5, 2);
		SaveW = ComboMenu->CheckBox("Use W", true);
		SaveE = ComboMenu->CheckBox("Use E", true);
		SaveKey = ComboMenu->AddKey("Save Ally(W/E) key", 'G');
		ForceR = ComboMenu->AddKey("Force R", 'T');
		FarmQ = FarmMenu->CheckBox("Q push lane", false);
		LastQ = FarmMenu->CheckBox("Q last hit", false);
		FarmMana = FarmMenu->AddFloat("Save spells if mana% is", 1, 100, 50);
		DrawQRange = DrawingMenu->CheckBox("Draw Q", true);
		DrawWRange = DrawingMenu->CheckBox("Draw W", false);
		DrawRRange = DrawingMenu->CheckBox("Draw R", true);
		AntiGapW = MiscMenu->CheckBox("Auto W Gapclose", true);
		AntiGapE = MiscMenu->CheckBox("Auto E Gapclose", true);
		InterruptR = MiscMenu->CheckBox("R to interrupt", true);
		ComboEenable = MiscMenu->CheckBox("Auto E for targeted spells", true);
	}

	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, true, kCollidesWithMinions);
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
		R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, true, static_cast<eCollisionFlags>(kCollidesWithNothing));
		Q->SetSkillshot(0.25f, 50.f, 1400.f, 1000.f);
		W->SetOverrideRange(650);
		E->SetOverrideRange(25000);
		R->SetSkillshot(0.25f, 160, 1000.f, 1200.f);

	}

	void AutoE(CastedSpell const& Args)
	{
		for (auto Ally : GEntityList->GetAllHeros(true, false))
		{
			if (ComboEenable->Enabled() && E->IsReady()) {
				if (Args.Target_ == GEntityList->Player() && Args.Caster_->IsHero() && Args.Caster_->IsEnemy(Ally)) {

					auto data = Args.Data_;
					auto target = GSpellData->GetTarget(data);
					std::string spellName = Args.Name_;
					if (spellName.find("BasicAttack") != std::string::npos) {
						return;
					}
					if (Args.Caster_->IsValidTarget(GEntityList->Player(), Q->Range()))
					{

						E->CastOnPosition(Args.Caster_->ServerPosition());
					}
				}
			}
		}

	}

	void Semi()
	{
		if (!GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
		{
			for (auto Enemy : GEntityList->GetAllHeros(false, true))
			{
				if (!Enemy->IsInvulnerable() && Enemy->IsValidTarget(GEntityList->Player(), R->Range() - 100) && !Enemy->IsDead())
				{
					R->CastOnTarget(Enemy, kHitChanceHigh);
				}
			}
		}
	}

	void ComboSave()
	{
		GGame->IssueOrder(GEntityList->Player(), kMoveTo, GGame->CursorPosition());

		if (SaveW->Enabled() && W->IsReady())
		{
			for (auto Ally : GEntityList->GetAllHeros(true, false))
			{
				if (Ally != nullptr)
				{
					if (Ally != GEntityList->Player() && GEntityList->Player()->IsValidTarget(Ally, W->Range()))
					{
						W->CastOnTarget(Ally);
						lastw = 0;
					}
				}
			}
		}
		if (lastw == 0)
		{
			lastw = GGame->TickCount() + 100;
		}
		if (lastw != 0 && GGame->TickCount() > lastw)
		{
			if (SaveE->Enabled() && E->IsReady())
			{
				for (auto Ally : GEntityList->GetAllHeros(true, false))
				{
					for (auto target : GEntityList->GetAllHeros(false, true))
					{
							auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
							if (target != nullptr && Ally != nullptr)
							{
								if (Ally->IsValidTarget(GEntityList->Player(), 650) && target->IsValidTarget(GEntityList->Player(), Q->Range()) && !W->IsReady())
								{
									E->CastOnPosition(target->ServerPosition());
									lastw = 0;
								}
							}
						
					}
				}
			}

		}
	}
	void Push()
	{
		minions = GEntityList->GetAllMinions(false, true, true);
		for (IUnit* minion : minions)
			if (GEntityList->Player()->ManaPercent() >= FarmMana->GetFloat())
			{
				if (FarmQ->Enabled() && Q->IsReady() && minion->IsValidTarget(minion, Q->Range()) && GEntityList->Player()->IsValidTarget(minion, Q->Range()) && (!(minion->IsDead())))

				{
					if (minion != nullptr)
					{
						Q->CastOnUnit(minion);
					}
				}
			}
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
	void Combo()
	{
		for (auto target : GEntityList->GetAllHeros(false, true))
		{
			if (ComboQ->Enabled() && Q->IsReady() && target != nullptr)
			{
				auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
				if (GEntityList->Player()->IsValidTarget(target, Q->Range()))
				{
					Q->CastOnTarget(target, kHitChanceHigh);
				}
			}
			if (SaveW->Enabled() && W->IsReady())
			{
				for (auto Ally : GEntityList->GetAllHeros(true, false))
				{
					if (Ally != GEntityList->Player())
					{
						if (Ally->IsValidTarget(target, 500) && (GEntityList->Player()->GetPosition() - Ally->GetPosition()).Length2D() > 200 && (GEntityList->Player()->GetPosition() - Ally->GetPosition()).Length2D() < 650 && (GEntityList->Player()->GetPosition() - target->GetPosition()).Length2D() > 300)
						{
							if (Ally != nullptr)
							{
								W->CastOnTarget(Ally);
							}

						}
					}
				}
			}
			if (SaveE->Enabled() && E->IsReady())
			{
				for (auto Ally : GEntityList->GetAllHeros(true, false))
				{
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
					if (Ally->HasBuff("BraumW") || GEntityList->Player()->HasBuffOfType(BUFF_Damage))
					{
						if (target != nullptr && Ally != nullptr)
						{
							if (Ally->IsValidTarget(GEntityList->Player(), E->Range()) && target->IsValidTarget(GEntityList->Player(), W->Range()))
							{
								E->CastOnTarget(target);

							}
						}
					}
				}
			}
			if (ComboR->Enabled() && R->IsReady())// && GEntityList->Player()->IsValidTarget(target, R->Range()) && target != nullptr)
			{
				///int enemies;

				///Vec3 pos;
				{
					auto RPlugin = ComboRmin->GetInteger();
					///GPrediction->FindBestCastPosition(1200, 40, true, false, true, pos, enemis);
					auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 1200);
					if (GPlugin(GPluginSDK, 1200) >= RPlugin && target->IsValidTarget(GEntityList->Player(), 1200))
					{
						if (target != nullptr)
						{
							R->CastOnTargetAoE(target, RPlugin, kHitChanceMedium);
						}

					}
				}
			}
		}


	}
	static int GSave(IPluginSDK *sdk, float range)
	{
		static auto entityList = sdk->GetEntityList();
		static auto player = entityList->Player();

		auto count = 0;

		for (auto unit : entityList->GetAllHeros(true, false))
		{
			if (player->IsValidTarget(unit, range))
			{
				count++;
			}
		}

		return count;
	}
	void OnInterruptible(InterruptibleSpell const& Args)
	{
		{
			if (Args.DangerLevel == kHighDanger && InterruptR->Enabled() && R->IsReady())
				R->CastOnTarget(Args.Target);
		}
		{
			if (Args.DangerLevel == kHighDanger && SaveW->Enabled() && W->IsReady())

			{
				auto Target = GTargetSelector->FindTarget(ClosestPriority, SpellDamage, W->Range());
				if (GSave(GPluginSDK, W->Range()) >= 1 && GEntityList->Player()->IsValidTarget(Target, W->Range()))
				{
					for (auto Ally : GEntityList->GetAllHeros(true, false))
					{
						W->CastOnTarget(Ally);
					}
				}
			}
		}
		{
			if (Args.DangerLevel == kHighDanger && SaveE->Enabled() && !W->IsReady() && E->IsReady())

			{
				auto Target = GTargetSelector->FindTarget(ClosestPriority, SpellDamage, 500);
				if (GPlugin(GPluginSDK, 500) >= 1 && GEntityList->Player()->IsValidTarget(Target, 500))
				{
					for (auto target : GEntityList->GetAllHeros(false, true))
						if (target != nullptr)
						{
							E->CastOnTarget(target);
						}
				}
			}
		}
	}
	static int CountEnemiesNearMe(IPluginSDK *sdk, float range)
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
	static int CountAllyMinionsNearMe(IPluginSDK *sdk, float range)
	{
		static auto entityList = sdk->GetEntityList();
		static auto player = entityList->Player();

		auto count = 0;

		for (auto unit : entityList->GetAllMinions(true, false, false))
		{
			if (player->IsValidTarget(unit, range))
			{
				count++;
			}
		}

		return count;
	}

	/*int GetMinionType(IUnit* minion)
	{
	if (minion->GetType() != FL_CREEP)
	return MT_Unknown;

	std::string szBaseSkinName = std::string(minion->SkinName());

	if (std::find(NormalMinionList.begin(), NormalMinionList.end(), szBaseSkinName) != NormalMinionList.end())
	return MT_Normal | (minion->IsMelee() ? MT_Melee : MT_Ranged);

	if (std::find(SiegeMinionList.begin(), SiegeMinionList.end(), szBaseSkinName) != SiegeMinionList.end())
	return MT_Siege | MT_Ranged;

	if (std::find(SuperMinionList.begin(), SuperMinionList.end(), szBaseSkinName) != SuperMinionList.end())
	return MT_Super | MT_Melee;

	if (minion->IsWard())
	return MT_Ward;

	return MT_Unknown;
	}*/

	void LastHit()
	{
		for (auto Minion : GEntityList->GetAllMinions(false, true, true))
		{
			if (!Minion->IsDead() && Minion != nullptr)
			{
				if (LastQ->Enabled() && Q->IsReady() && Minion->IsValidTarget(GEntityList->Player(), Q->Range()) && GDamage->GetSpellDamage(GEntityList->Player(), Minion, kSlotQ) >= Minion->GetHealth())
				{
					Q->LastHitMinion();
				}
			}
		}
	}
	void AntiGapclose(GapCloserSpell const& Args)
	{
		minions = GEntityList->GetAllMinions(true, false, false);
		for (IUnit* minion : minions)
			if (Args.Sender != GEntityList->Player()
				&& Args.Sender->IsEnemy(GEntityList->Player())
				&& GEntityList->Player()->IsValidTarget(Args.Sender, W->Range() + Args.Sender->BoundingRadius())
				&& AntiGapW->Enabled() && W->IsReady() && CountAllyMinionsNearMe(GPluginSDK, W->Range() >= 1) && minion != nullptr && !minion->IsDead())
			{
				W->CastOnUnit(minion);
			}
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->IsEnemy(GEntityList->Player())
			&& GEntityList->Player()->IsValidTarget(Args.Sender, W->Range() + Args.Sender->BoundingRadius())
			&& AntiGapE->Enabled() && E->IsReady())
		{
			E->CastOnPosition(Args.Sender->ServerPosition());
		}
	}

	void Draw() const
	{
		if (DrawQRange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (DrawWRange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }


		if (DrawRRange->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), 1150); }
	}
};