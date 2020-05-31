/*
 * File:   staticgridevent.h
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

#ifndef STMG_STATIC_GRID_EVENT_H
#define STMG_STATIC_GRID_EVENT_H

#include "event.h"

#include "levelanimation.h"
#include "animations/staticgridanimation.h"
#include "util/recycler.h"

#include <memory>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

class StaticGridEvent : public Event
{
public:
	struct LocalInit
	{
		LevelAnimation::REFSYS m_eRefSys = LevelAnimation::REFSYS_BOARD; /**< The reference system. Default: LevelAnimation::REFSYS_BOARD. */
		StaticGridAnimation::Init m_oAnimationData; /**< The animation. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The parameters.
	 */
	explicit StaticGridEvent(Init&& oInit) noexcept;
protected:
	/** See StaticGridEvent::StaticGridEvent().
	 */
	void reInit(Init&& oInit) noexcept;
public:
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// Inputs
	enum {
		MESSAGE_CONTROL_STOP = 100 /**< Stops the Animation (which might remove itself shortly after).
									 * If no animation is running does nothing. */
		, MESSAGE_CONTROL_RESTART = 105 /**< (Re)Adds a new animation. If already running does nothing. */
	};

private:
	LocalInit m_oData;

	struct PrivateStaticGridAnimation : public StaticGridAnimation
	{
		using StaticGridAnimation::StaticGridAnimation;
		using StaticGridAnimation::reInit;
	};
	// The current animation. If null means stopped state, otherwise shown state
	shared_ptr<PrivateStaticGridAnimation> m_refSGAnimation;
	// Where to get the animation. The view might hold a reference to the former
	// current animation a bit longer after the current animation is stopped
	// (for example to fade the images)
	Recycler<PrivateStaticGridAnimation> m_oRecycler;
	
private:
	StaticGridEvent() = delete;
	StaticGridEvent(const StaticGridEvent& oSource) = delete;
	StaticGridEvent& operator=(const StaticGridEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STATIC_GRID_EVENT_H */

