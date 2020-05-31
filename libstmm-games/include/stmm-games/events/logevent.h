/*
 * File:   logevent.h
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

#ifndef STMG_LOG_EVENT_H
#define STMG_LOG_EVENT_H

#include "event.h"

#include <vector>

#include <stdint.h>

namespace stmg
{

class LogEvent : public Event
{
public:
	struct LocalInit
	{
		bool m_bToStdOut = true; /**< Whether to log to std::cout. Default is true. */
		bool m_bElapsedMinuteFormat = true; /**< Whether the elapsed time should be formatted as MIN:SEC:MILLI
											 * rather than just milliseconds. Default is true. */
		bool m_bValueAsXY = false; /**< Whether nValue should shown as unpacked coords. Default is false. */
		int32_t m_nTag = 0; /**< The tag for the instance. Must be &gt;= 0. Default is 0. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit LogEvent(Init&& oInit) noexcept;
protected:
	/** Reinitialization.
	 * See LogEvent::LogEvent.
	 */
	void reInit(Init&& oInit) noexcept;
public:
	/** The triggered function.
	 * @param nMsg The logged message.
	 * @param nValue The logged value.
	 * @param p0TriggeringEvent The logged triggering event address.
	 */
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

public:
	class MsgLog {
	public:
		class Entry
		{
		public:
			int32_t m_nTag = -1;
			int32_t m_nGameTick = -1;
			int32_t m_nGameMillisec = -1;
			int32_t m_nLevel = -1;
			int32_t m_nMsg = -1;
			int32_t m_nValue = -1;
			int64_t m_nTriggeringEventAdr = -1;
			bool isEmpty() const noexcept
			{
				return (m_nLevel < 0);
			}
		};

		void reset() noexcept;
		// Total of the Entries that were added by LogEvent
		int32_t totEntries() const noexcept { return m_nTotEntries; }
		// Total of the buffered entries that can be retrieved via last(nBack)
		int32_t totBuffered() const noexcept { return static_cast<int32_t>(m_aEntry.size()); }
		// Same as last(0)
		const Entry& last() const noexcept;
		// The nBack before the last of the buffered Entries
		const Entry& last(int32_t nBack) const noexcept;

		/** Find for a specific entry.
		 * Visits all buffered entries from the last backwards and stops at the
		 * first entry that satisfies the predicate.
		 * @param oPred The predicate to check for.
		 * @return -1 if no entry satisfies oPred or the param to pass to last(nBack).
		 */
		template<class Pred>
		int32_t findEntry(Pred oPred) const noexcept
		{
			for (int32_t nBack = 0; nBack < totBuffered(); ++nBack) {
				if (oPred(last(nBack))) {
					return nBack;
				}
			}
			return -1;
		}
		#ifndef NDEBUG
		void dump() const noexcept;
		#endif //NDEBUG
		static const int32_t s_nMaxLastBufferedEntries;
	private:
		friend class LogEvent;
		MsgLog() noexcept
		: m_nTotEntries(0)
		, m_nLastPointer(0)
		{
		}
		void addEntry(Entry&& oEntry) noexcept;

		int32_t m_nTotEntries; // since creation or last reset
		int32_t m_nLastPointer;
		std::vector<Entry> m_aEntry;
	private:
		MsgLog(MsgLog const&) = delete;
		void operator=(MsgLog const&) = delete;
	};
	/** The log containing the entries from all instances.
	 * @return The singleton log.
	 */
	static MsgLog& msgLog() noexcept
	{
		static MsgLog s_oMsgLog;
		return s_oMsgLog;
	}

private:
	LocalInit m_oData;
	bool m_bManyLevels;

private:
	LogEvent() = delete;
	LogEvent(const LogEvent& oSource) = delete;
	LogEvent& operator=(const LogEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_LOG_EVENT_H */

