#pragma once
#include <functional>
#include "PluginData.h"
#include "PluginSDK.h"

typedef unsigned int event_id_t;
constexpr event_id_t invalid_event = -1;

namespace eventmanager {
	class OrbwalkerEventManager
	{
	public:
		static event_id_t RegisterBeforeAttackEvent(const std::function<void(event_id_t, IUnit *)> &func);
		static event_id_t RegisterAttackEvent(const std::function<void(event_id_t, IUnit *, IUnit *)> &func);
		static event_id_t RegisterAfterAttackEvent(const std::function<void(event_id_t, IUnit *, IUnit *)> &func);
		static event_id_t RegisterNewTargetEvent(const std::function<void(event_id_t, IUnit *, IUnit *)> &func);
		static event_id_t RegisterNonKillableMinionEvent(const std::function<void(event_id_t, IUnit *)> &func);
		static event_id_t RegisterFindTargetEvent(const std::function<IUnit *(event_id_t)> &func);

		static void UnregisterBeforeAttackEvent(event_id_t id);
		static void UnregisterAttackEvent(event_id_t id);
		static void UnregisterAfterAttackEvent(event_id_t id);
		static void UnregisterNewTargetEvent(event_id_t id);
		static void UnregisterNonKillableMinionEvent(event_id_t id);
		static void UnregisterFindTargetEvent(event_id_t id);

	private:
		OrbwalkerEventManager() {}
	};

	class GameEventManager
	{
	public:
		static event_id_t RegisterUpdateEvent(const std::function<void(event_id_t)> &func);
		static event_id_t RegisterEndEvent(const std::function<void(event_id_t)> &func);
		static event_id_t RegisterWndProcEvent(const std::function<bool(event_id_t, HWND Wnd, UINT Message, WPARAM wParam, LPARAM lParam)> &func);
		static event_id_t RegisterJungleNotifyEvent(const std::function<void(event_id_t, JungleNotifyData *)> &func);

		static void UnregisterUpdateEvent(event_id_t id);
		static void UnregisterEndEvent(event_id_t id);
		static void UnregisterWndProcEvent(event_id_t id);
		static void UnregisterJungleNotifyEvent(event_id_t id);

	private:
		GameEventManager() {}
	};

	class DrawEventManager
	{
	public:
		static event_id_t RegisterRenderEvent(const std::function<void(event_id_t)> &func);
		static event_id_t RegisterRenderBehindHudEvent(const std::function<void(event_id_t)> &func);
		static event_id_t RegisterD3DPresentEvent(const std::function<void(event_id_t)> &func);
		static event_id_t RegisterD3DPreResetEvent(const std::function<void(event_id_t)> &func);
		static event_id_t RegisterD3DPostResetEvent(const std::function<void(event_id_t)> &func);

		static void UnregisterRenderEvent(event_id_t id);
		static void UnregisterRenderBehindHudEvent(event_id_t id);
		static void UnregisterD3DPresentEvent(event_id_t id);
		static void UnregisterD3DPreResetEvent(event_id_t id);
		static void UnregisterD3DPostResetEvent(event_id_t id);

	private:
		DrawEventManager() {}
	};

	class UnitEventManager
	{
	public:
		static event_id_t RegisterCreateEvent(const std::function<void(event_id_t, IUnit *)> &func);
		static event_id_t RegisterDestroyEvent(const std::function<void(event_id_t, IUnit *)> &func);
		static event_id_t RegisterDeathEvent(const std::function<void(event_id_t, IUnit *)> &func);
		static event_id_t RegisterIssueOrderEvent(const std::function<bool(event_id_t, IUnit *, int, Vec3 *, IUnit *)> &func);
		static event_id_t RegisterPreCastEvent(const std::function<bool(event_id_t, int, IUnit *, Vec3 *, Vec3 *)> &func);
		static event_id_t RegisterUpdateChargedSpellEvent(const std::function<void(event_id_t, int, Vec3 *, bool *, bool *)> &func);
		static event_id_t RegisterProcessSpellCastEvent(const std::function<void(event_id_t, const CastedSpell &)> &func);
		static event_id_t RegisterDoCastEvent(const std::function<void(event_id_t, const CastedSpell &)> &func);
		static event_id_t RegisterProcessInterruptibleSpellEvent(const std::function<void(event_id_t, const InterruptibleSpell &)> &func);
		static event_id_t RegisterProcessGapCloserSpellEvent(const std::function<void(event_id_t, const GapCloserSpell &)> &func);
		static event_id_t RegisterBuffAddEvent(const std::function<void(event_id_t, IUnit *, void *)> &func);
		static event_id_t RegisterBuffRemoveEvent(const std::function<void(event_id_t, IUnit *, void *)> &func);
		static event_id_t RegisterLevelUpEvent(const std::function<void(event_id_t, IUnit *, int)> &func);
		static event_id_t RegisterDashEvent(const std::function<void(event_id_t, UnitDash *)> &func);
		static event_id_t RegisterEnterVisibilityEvent(const std::function<void(event_id_t, IUnit *)> &func);
		static event_id_t RegisterExitVisibilityEvent(const std::function<void(event_id_t, IUnit *)> &func);
		static event_id_t RegisterPlayAnimationEvent(const std::function<bool(event_id_t, IUnit *, const std::string)> &func);
		static event_id_t RegisterPauseAnimationEvent(const std::function<void(event_id_t, IUnit *)> &func);
		static event_id_t RegisterNewPathEvent(const std::function<void(event_id_t, IUnit *, const std::vector<Vec3> &)> &func);

		static void UnregisterCreateEvent(event_id_t id);
		static void UnregisterDestroyEvent(event_id_t id);
		static void UnregisterDeathEvent(event_id_t id);
		static void UnregisterIssueOrderEvent(event_id_t id);
		static void UnregisterPreCastEvent(event_id_t id);
		static void UnregisterUpdateChargedSpellEvent(event_id_t id);
		static void UnregisterProcessSpellCastEvent(event_id_t id);
		static void UnregisterDoCastEvent(event_id_t id);
		static void UnregisterProcessInterruptibleSpellEvent(event_id_t id);
		static void UnregisterProcessGapCloserSpellEvent(event_id_t id);
		static void UnregisterBuffAddEvent(event_id_t id);
		static void UnregisterBuffRemoveEvent(event_id_t id);
		static void UnregisterLevelUpEvent(event_id_t id);
		static void UnregisterDashEvent(event_id_t id);
		static void UnregisterEnterVisibilityEvent(event_id_t id);
		static void UnregisterExitVisibilityEvent(event_id_t id);
		static void UnregisterPlayAnimationEvent(event_id_t id);
		static void UnregisterPauseAnimationEvent(event_id_t id);
		static void UnregisterNewPathEvent(event_id_t id);
		static void UnregisterTeleportEvent(event_id_t id);

	private:
		UnitEventManager() {}
	};

	void RegisterEvents(IEventManager *eventManager);
	void UnregisterEvents(IEventManager *eventManager);
}
