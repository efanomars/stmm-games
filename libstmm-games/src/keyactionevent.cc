/*
 * File:   keyactionevent.cc
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

#include "keyactionevent.h"

#include <stmm-input/event.h>
#include <stmm-input/capability.h>

#include <cassert>
//#include <iostream>

namespace stmi { class Accessor; }

namespace stmg
{

const char* const KeyActionEvent::s_sClassId = "stmg::KeyActionEvent";
stmi::Event::RegisterClass<KeyActionEvent> KeyActionEvent::s_oInstall(s_sClassId);

KeyActionEvent::KeyActionEvent(int64_t nTimeUsec, const shared_ptr<stmi::Accessor>& refAccessor
			, const shared_ptr<stmi::Capability>& refCapability, stmi::Event::AS_KEY_INPUT_TYPE eType, int32_t nKeyAction) noexcept
: Event(s_oInstall.getEventClass(), nTimeUsec, refCapability ? refCapability->getId() : -1, refAccessor)
, m_eType(eType)
, m_nKeyAction(nKeyAction)
, m_refCapability(refCapability)
{
	assert(nKeyAction >= 0);
}

const stmi::Event::Class& KeyActionEvent::getClass() noexcept
{
	static const stmi::Event::Class s_oKeyClass = s_oInstall.getEventClass();
	return s_oKeyClass;
}
void KeyActionEvent::setType(stmi::Event::AS_KEY_INPUT_TYPE eType) noexcept
{
	assert((eType == stmi::Event::AS_KEY_PRESS) || (eType == stmi::Event::AS_KEY_RELEASE)
			|| (eType == stmi::Event::AS_KEY_RELEASE_CANCEL));
	m_eType = eType;
}
void KeyActionEvent::setKeyAction(int32_t nKeyAction) noexcept
{
	assert(nKeyAction >= 0);
	m_nKeyAction = nKeyAction;
}
void KeyActionEvent::setCapability(const shared_ptr<stmi::Capability>& refCapability) noexcept
{
	assert(refCapability);
	m_refCapability = refCapability;
	setCapabilityId(refCapability->getId());
}

} // namespace stmg
