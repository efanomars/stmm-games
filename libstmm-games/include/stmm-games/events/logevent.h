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
 * File:   logevent.h
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
	/** Holds information about the messages received by all instances of LogEvent.
	 * Only the last MsgLog::s_nMaxLastBufferedEntries entries are buffered and
	 * can be accessed with MsgLog::last(int32_t nBack). */
	class MsgLog {
	public:
		/** Information about one received message. */
		class Entry
		{
		public:
			int32_t m_nTag = -1; /**< The tag number. The value passed with LocalInit::m_nTag. */
			int32_t m_nGameTick = -1; /**< The game tick the message was received. */
			int32_t m_nGameMillisec = -1; /**< The game's elapsed time in milliseconds. */
			int32_t m_nLevel = -1; /**< The level of the LogEvent that received the message. */
			int32_t m_nMsg = -1; /**< The message number. */
			int32_t m_nValue = -1; /**< The message value. */
			int64_t m_nTriggeringEventAddr = -1; /**< The address of the event sending the message.
													* Is a reinterpret_cast<int64_t>(Event *). */
			bool isEmpty() const noexcept
			{
				return (m_nLevel < 0);
			}
		};
		/** Clear the log.
		 */
		void reset() noexcept;
		/** The number of entries added to the log.
		 * Since creation or the last reset().
		 * @return The number of entries.
		 */
		int32_t totEntries() const noexcept { return m_nTotEntries; }
		// Total of the buffered entries that can be retrieved via last(nBack)
		/** The number of buffered entries.
		 * @return The number of buffered entries.
		 */
		int32_t totBuffered() const noexcept { return static_cast<int32_t>(m_aEntry.size()); }
		/** The last buffered entry.
		 * Corresponds to the last message received by any instance of LogEvent.
		 * Is the same as calling last(0).
		 * @return The last entry or an empty entry if totBuffered() is 0.
		 */
		const Entry& last() const noexcept;
		/** The n-th last buffered entry.
		 * @param nBack The back index. Must be &gt;= 0 and &lt; totBuffered().
		 * @return The last entry or an empty entry if nBack not smaller than totBuffered().
		 */
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
		/** The maximum number of entries that can be buffered.*/
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

