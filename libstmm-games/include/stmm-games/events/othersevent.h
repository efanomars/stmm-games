/*
 * File:   othersevent.h
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

#ifndef STMG_OTHERS_EVENT_H
#define STMG_OTHERS_EVENT_H

#include "event.h"

#include <stdint.h>

namespace stmg
{

////////////////////////////////////////////////////////////////////////////////
class OthersReceiverEvent : public Event
{
public:
	struct Init : public Event::Init
	{
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit OthersReceiverEvent(Init&& oInit) noexcept;
protected:
	void reInit(Init&& oInit) noexcept;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

private:
	bool m_bInstalled;

private:
	OthersReceiverEvent() = delete;
	OthersReceiverEvent(const OthersReceiverEvent& oSource) = delete;
	OthersReceiverEvent& operator=(const OthersReceiverEvent& oSource) = delete;
};

////////////////////////////////////////////////////////////////////////////////
class OthersSenderEvent : public Event
{
public:
	struct Init : public Event::Init
	{
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit OthersSenderEvent(Init&& oInit) noexcept;
protected:
	void reInit(Init&& oInit) noexcept;
public:
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

private:
	OthersSenderEvent() = delete;
	OthersSenderEvent(const OthersSenderEvent& oSource) = delete;
	OthersSenderEvent& operator=(const OthersSenderEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_OTHERS_EVENT_H */

