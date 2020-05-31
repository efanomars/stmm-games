/*
 * File:   positionerevent.h
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

#ifndef STMG_POSITIONER_EVENT_H
#define STMG_POSITIONER_EVENT_H

#include "event.h"

#include "util/basictypes.h"

#include "levelshow.h"
#include "levellisteners.h"

#include <vector>

#include <stdint.h>

namespace stmg { class LevelBlock; }

namespace stmg
{

/** The positioner event.
 * This event positions the level's show or player's subshow (depending on the mode)
 * so that the controlled level block(s) are within a tracking rectangle.
 *
 * In subshow mode it handles the position of each level player!
 */
class PositionerEvent : public Event, public BlocksPlayerChangeListener
{
public:

	struct LocalInit
	{
		NRect m_oTrackingRect; /**< The tracking rectangle. */
		int32_t m_nCheckEachTicks = 1; /**< Check position in game ticks. Default is 1. */
		int32_t m_nTransitionTicks = 1; /**< The number of game ticks it takes to get to a target position. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * The tracking rectangle is the area in which the level block(s) should be shown.
	 * Its position is relative to the (sub)show top left corner. If width and height are
	 * defined it must be within the subshow rectangle. If width or height are
	 * &lt;= 0 the level block(s) (sort of) "center of mass" is always at the center
	 * of the (sub)show.
	 * @param oInit The parameters.
	 */
	explicit PositionerEvent(Init&& oInit) noexcept;
protected:
	/** See constructor.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// input
	//TODO make paused more fine grained, => player subshow specific
	//TODO player might be punished by making him temporarily "blind*
	enum {
		MESSAGE_PAUSE_TRACKING = 100 /**< Pause tracking if not already paused. */
		, MESSAGE_RESUME_TRACKING = 101 /**< Resume tracking if paused. */
	};

	//enum {
	//	LISTENER_GROUP_SHOW_POSITION_CHANGED = 10
	//	, LISTENER_GROUP_SUBSHOW_POSITION_CHANGED = 11
	//};

	void blockChangedPlayer(LevelBlock& oLevelBlock, int32_t nOldPlayer) noexcept override;

private:
	void initCommon(LocalInit&& oInit) noexcept;
	void initTracking() noexcept;
	void handleTransitions() noexcept;
	void checkNewPositions() noexcept;
private:
	LocalInit m_oData;

	NSize m_oParentSize;
	bool m_bSubshowMode;
	int32_t m_nTotLevelPlayers;

	struct ShowPosition : public LevelShow::Positioner
	{
		FPoint m_oPos; // The current position
		FPoint m_oTargetPos; // The target position or undefined if m_nGameTicksToTarget == -1
		int32_t m_nGameTicksToTarget = -1; // Game ticks to get to the target position.
		std::vector<int32_t> m_aLevelBlockIds; // the controlled level block ids. Size: 1 (subshow mode), all controlled LBs (show mode)
		NRect m_oLastRect; /**< The bounding rect of the last block losing control by a player (subshow mode only). */
		LevelShow* m_p0LevelShow = nullptr; // The level (sub)show to be positioned
	public:
		FPoint getPos(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
	};
	std::vector<ShowPosition> m_aShowPositions; // Size: 1 (show mode), Level::getTotLevelPlayers() (subshow mode)

	enum POSITIONER_STATE
	{
		POSITIONER_STATE_ACTIVATE = 0
		, POSITIONER_STATE_INIT = 1
		, POSITIONER_STATE_TRACKING = 2
	};
	POSITIONER_STATE m_eState;
	int32_t m_nLastCheck; // in elapsed game ticks
	bool m_bPaused;
	int32_t m_nTotTransitioning;
private:
	PositionerEvent() = delete;
	PositionerEvent(const PositionerEvent& oSource) = delete;
	PositionerEvent& operator=(const PositionerEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_POSITIONER_EVENT_H */

