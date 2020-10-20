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
 * File:   fixtureGameOwner.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_GAME_OWNER_H
#define STMG_TESTING_FIXTURE_GAME_OWNER_H

#include <stmm-games/gameowner.h>
#include <stmm-games/gameproxy.h>

#include <memory>
#include <cassert>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

class GameOwnerFixture : public GameOwner
{
public:
	int32_t gameEndedCount() const noexcept
	{
		return m_nGameEndedCount;
	}
	int32_t gamePauseCount() const noexcept
	{
		return m_nGamePauseCount;
	}
	int32_t gameAbortCount() const noexcept
	{
		return m_nGameAbortCount;
	}
	int32_t gameAbortAskCount() const noexcept
	{
		return m_nGameAbortAskCount;
	}
	int32_t gameQuitCount() const noexcept
	{
		return m_nGameQuitCount;
	}
	int32_t gameQuitAskCount() const noexcept
	{
		return m_nGameQuitAskCount;
	}
	int32_t gameRestartCount() const noexcept
	{
		return m_nGameRestartCount;
	}
	int32_t gameRestartAskCount() const noexcept
	{
		return m_nGameRestartAskCount;
	}

	void gameEnded() noexcept override
	{
		++m_nGameEndedCount;
	}
	void gameInterrupt(GameProxy::INTERRUPT_TYPE eInterruptType) noexcept override
	{
		if (eInterruptType == GameProxy::INTERRUPT_ABORT) {
			++m_nGameAbortCount;
		} else if (eInterruptType == GameProxy::INTERRUPT_ABORT_ASK) {
			++m_nGameAbortAskCount;
		} else if (eInterruptType == GameProxy::INTERRUPT_QUIT_APP) {
			++m_nGameQuitCount;
		} else if (eInterruptType == GameProxy::INTERRUPT_QUIT_APP_ASK) {
			++m_nGameQuitAskCount;
		} else if (eInterruptType == GameProxy::INTERRUPT_RESTART) {
			++m_nGameRestartCount;
		} else if (eInterruptType == GameProxy::INTERRUPT_RESTART_ASK) {
			++m_nGameRestartAskCount;
		} else if (eInterruptType == GameProxy::INTERRUPT_PAUSE) {
			++m_nGamePauseCount;
		} else {
			assert(false);
		}
	}
protected:
	virtual void setup()
	{
		resetGameOwner();
	}
	virtual void teardown()
	{
		resetGameOwner();
	}
	void resetGameOwner() noexcept
	{
		m_nGameEndedCount = 0;
		m_nGamePauseCount = 0;
		m_nGameAbortCount = 0;
		m_nGameAbortAskCount = 0;
		m_nGameQuitCount = 0;
		m_nGameQuitAskCount = 0;
		m_nGameRestartCount = 0;
		m_nGameRestartAskCount = 0;
	}
public:
	int32_t m_nGameEndedCount;
	int32_t m_nGamePauseCount;
	int32_t m_nGameAbortCount;
	int32_t m_nGameAbortAskCount;
	int32_t m_nGameQuitCount;
	int32_t m_nGameQuitAskCount;
	int32_t m_nGameRestartCount;
	int32_t m_nGameRestartAskCount;
};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_GAME_OWNER_H */
