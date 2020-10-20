/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   sysevent.h
 */

#ifndef STMG_SYS_EVENT_H
#define STMG_SYS_EVENT_H

#include "event.h"

#include <stdint.h>

namespace stmg
{

/** Instances of this class end the game for a team or player.
 * If the type of action is SYS_EVENT_TYPE_PLAYER_OUT and it's the last player
 * of the team still playing, the team is terminated with SYS_EVENT_TYPE_LEVEL_FAILED.
 *
 * If the team terminating is the last the game is ended.
 */
class SysEvent : public Event
{
public:
	enum SYS_EVENT_TYPE
	{
		SYS_EVENT_TYPE_INVALID = -1
		, SYS_EVENT_TYPE_LEVEL_COMPLETED = 1 /**< Team completed level. */
		, SYS_EVENT_TYPE_LEVEL_FAILED = 2 /**< Team failed level. */
		, SYS_EVENT_TYPE_PLAYER_OUT = 3 /**< Player out with mate passed as nMsg to the trigger function */
	};
	struct LocalInit
	{
		int32_t m_nLevelTeam = 0; /**> The level team. Default is 0. */
		SYS_EVENT_TYPE m_eType = SYS_EVENT_TYPE_INVALID; /**< The type sys event. Default is SYS_EVENT_TYPE_INVALID. */
		bool m_bFinishIfPossible = false; /**< Whether the game should terminate other teams if possible. Default is false. */
		bool m_bCreateTexts = true; /**< Whether level should create default texts declaring winner and possibly losers. Default is true. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The parameters.
	 */
	explicit SysEvent(Init&& oInit) noexcept;
protected:
	/** Reinitialization.
	 * @param oInit The parameters.
	 */
	void reInit(Init&& oInit) noexcept;
public:
	/** The common triggering function.
	 * @param nMsg Either ignored or represents the mate number of a player. If invalid as if 0 is was passed.
	 * @param nValue Ignored.
	 * @param p0TriggeringEvent Not used. Can be null.
	 */
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// Outputs
	enum {
		LISTENER_GROUP_TEAM_FINISHED = 20 /**< The team is finished (succeeded or failed). Useful with SYS_EVENT_TYPE_PLAYER_OUT. */
		, LISTENER_GROUP_GAME_OVER = 50 /**< Listener group triggered when the input to the instance causes game over. */
	};

private:
	void commonInit() noexcept;
private:
	LocalInit m_oData;
	//
	int32_t m_nTotTeammates;
	int32_t m_nGameVarIdGameOverTime;
	int32_t m_nTeamVarIdFinishedTime;

private:
	SysEvent() = delete;
	SysEvent(const SysEvent& oSource) = delete;
	SysEvent& operator=(const SysEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_SYS_EVENT_H */

