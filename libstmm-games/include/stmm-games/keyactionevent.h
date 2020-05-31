/*
 * File:   keyactionevent.h
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

#ifndef STMG_KEY_ACTION_EVENT_H
#define STMG_KEY_ACTION_EVENT_H

#include <stmm-input/event.h>

#include <memory>

#include <stdint.h>

namespace stmi { class Accessor; }
namespace stmi { class Capability; }

namespace stmg
{

using std::shared_ptr;
using std::weak_ptr;

class KeyActionEvent : public stmi::Event
{
public:
	KeyActionEvent(int64_t nTimeUsec, const shared_ptr<stmi::Accessor>& refAccessor
				, const shared_ptr<stmi::Capability>& refCapability, stmi::Event::AS_KEY_INPUT_TYPE eType, int32_t nKeyAction) noexcept;

	inline stmi::Event::AS_KEY_INPUT_TYPE getType() const noexcept { return m_eType; }
	inline int32_t getKeyAction() const noexcept { return m_nKeyAction; }
	//
	shared_ptr<stmi::Capability> getCapability() const noexcept override { return m_refCapability.lock(); }
	//
	static const char* const s_sClassId;
	static const stmi::Event::Class& getClass() noexcept;
protected:
	friend class Game;
	void setType(stmi::Event::AS_KEY_INPUT_TYPE eType) noexcept;
	void setKeyAction(int32_t nKeyAction) noexcept;
	/** Sets the capability of the key action event.
	 * @param refCapability Cannot be null.
	 */
	void setCapability(const shared_ptr<stmi::Capability>& refCapability) noexcept;
private:
	stmi::Event::AS_KEY_INPUT_TYPE m_eType;
	int32_t m_nKeyAction;
	weak_ptr<stmi::Capability> m_refCapability;
	//
	static RegisterClass<KeyActionEvent> s_oInstall;
private:
	KeyActionEvent() = delete;
};

} // namespace stmg

#endif	/* STMG_KEY_ACTION_EVENT_H */

