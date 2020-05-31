/*
 * File:   othersevent.cc
 *
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

#include "events/othersevent.h"

#include "level.h"
#include "gameproxy.h"

#include <utility>
//#include <cassert>
//#include <iostream>

#include <stdint.h>

namespace stmg
{

OthersReceiverEvent::OthersReceiverEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_bInstalled(false)
{
}

void OthersReceiverEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_bInstalled = false;
}

void OthersReceiverEvent::trigger(int32_t nMsg, int32_t nValue, Event* /*p0TriggeringEvent*/) noexcept
{
	if (!m_bInstalled) {
//std::cout << "OthersReceiverEvent::trigger  install    (nMsg=" << nMsg << "  nValue=" << nValue << ")" << '\n';
		level().othersAddListener(this);
		m_bInstalled = true;
	} else {
		if (nMsg >= 0) {
//std::cout << "OthersReceiverEvent::trigger  nMsg=" << nMsg << "  nValue=" << nValue << '\n';
			informListeners(nMsg, nValue);
		}
	}
}

OthersSenderEvent::OthersSenderEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
{
}

void OthersSenderEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
}

void OthersSenderEvent::trigger(int32_t nMsg, int32_t nValue, Event* /*p0TriggeringEvent*/) noexcept
{
	level().game().othersSend(level().getLevel(), nMsg, nValue);
}

} // namespace stmg
