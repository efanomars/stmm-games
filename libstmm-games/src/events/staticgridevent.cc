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
 * File:   staticgridevent.cc
 */

#include "events/staticgridevent.h"

#include "level.h"

#include <utility>
#include <vector>
//#include <cassert>
//#include <iostream>

namespace stmg
{

StaticGridEvent::StaticGridEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
{
}
void StaticGridEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	//
	m_refSGAnimation.reset(); // set state to stopped
}

void StaticGridEvent::trigger(int32_t nMsg, int32_t /*nValue*/, Event* p0TriggeringEvent) noexcept
{
//std::cout << "StaticGridEvent::trigger nGameTick=" << nGameTick << "  nMsg=" << nMsg << "   nValue = " << nValue << '\n';
	Level& oLevel = level();
	if (!m_refSGAnimation) {
		// stopped state
		if (! ((nMsg == MESSAGE_CONTROL_RESTART) || (p0TriggeringEvent == nullptr))) {
			// When not running either the event restarts by itself
			// or only the MESSAGE_CONTROL_RESTART can wake it up
			return; //----------------------------------------------------------
		}
		StaticGridAnimation::Init oSGAInit = m_oData.m_oAnimationData;
		m_oRecycler.create(m_refSGAnimation, std::move(oSGAInit));
		if (m_oData.m_eRefSys == LevelAnimation::REFSYS_BOARD) {
			oLevel.animationAddScrolled(m_refSGAnimation, 0); // -1
		} else {
			oLevel.animationAdd(m_refSGAnimation, m_oData.m_eRefSys, 0);
		}
		return; //--------------------------------------------------------------
	}
	// showing state
	if (nMsg != MESSAGE_CONTROL_STOP) {
		return; //--------------------------------------------------------------
	}
	m_refSGAnimation->stop(); // tell animation view to fade or just remove
	m_refSGAnimation.reset(); // set to stopped state
	oLevel.deactivateEvent(this);
}

} // namespace stmg
