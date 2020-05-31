/*
 * File:   soundevent.cc
 *
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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

#include "events/soundevent.h"

#include "gamesound.h"

#include "gameproxy.h"
#include "level.h"
#include "named.h"

#include "util/namedindex.h"
#include "util/util.h"

#include <cassert>
#include <iostream>
#include <utility>


namespace stmg
{

SoundEvent::SoundEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
{
	initCommon();
}

void SoundEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	//
	m_oData = std::move(oInit);
	//
	initCommon();
}
void SoundEvent::initCommon() noexcept
{
//std::cout << "SoundEvent(" << reinterpret_cast<int64_t>(this) <<  ")::initCommon" << '\n';
	assert(! ((m_oData.m_nSoundTeam < 0) && (m_oData.m_nSoundMate >= 0)));
	assert(m_oData.m_nSoundIdx >= -1);
	m_oCurPosXY = m_oData.m_oPosXY;
	m_fCurPosZ = m_oData.m_fPosZ;
	m_fCurVolume = m_oData.m_fVolume;
}
void SoundEvent::trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept
{
//std::cout << "SoundEvent::trigger adr=" << reinterpret_cast<int64_t>(this) << "  nMsg=" << nMsg << "  nValue=" << nValue << '\n';

	Level& oLevel = level();
	auto& oGame = oLevel.game();
	//
	auto oPlayMaybeLoop = [&](int32_t nSoundIdx, bool bLoop)
	{
		return oGame.createSound(nSoundIdx, m_oData.m_nSoundTeam, m_oData.m_nSoundMate
								, m_oCurPosXY, m_fCurPosZ, m_oData.m_bListenerRelative
								, m_fCurVolume, bLoop);
	};
	auto oPlay = [&](int32_t nSoundIdx)
	{
		return oPlayMaybeLoop(nSoundIdx, m_oData.m_bLooping);
	};
	auto oActivate = [&]()
	{
		const int32_t nGameTick = oGame.gameElapsed();
		oLevel.activateEvent(this, nGameTick + 1);
	};
	if (p0TriggeringEvent == nullptr) {
//std::cout << "SoundEvent::trigger A" << '\n';
		if (! m_refSound) {
			if (m_oData.m_nSoundIdx >= 0) {
				m_refSound = oPlay(m_oData.m_nSoundIdx);
				if (m_refSound) {
					oActivate();
				} else {
					// some error
					std::cout << "Could not load sound" << '\n';
				}
			}
		} else {
//std::cout << "SoundEvent::trigger waiting" << '\n';
			if (m_refSound->isFinished()) {
//std::cout << "SoundEvent::trigger FINISHED" << '\n';
				oGame.removeSound(m_refSound);
				m_refSound.reset();
				informListeners(LISTENER_GROUP_SOUND_FINISHED, m_oData.m_nSoundIdx);
			} else {
				oActivate();
			}
		}
		return; //--------------------------------------------------------------
	}
	//
	switch (nMsg) {
		case MESSAGE_SET_SOUND_IDX:
		{
			const NamedIndex& oSounds = oLevel.game().getNamed().sounds();
			if (oSounds.isIndex(nValue)) {
				m_oData.m_nSoundIdx = nValue;
			}
		}
		break;
		case MESSAGE_PLAY:
		{
			if (! m_refSound) {
				if (m_oData.m_nSoundIdx >= 0) {
					m_refSound = oPlay(m_oData.m_nSoundIdx);
				}
			}
		}
		break;
		case MESSAGE_PLAY_FREE:
		{
			if (m_oData.m_nSoundIdx >= 0) {
				oPlay(m_oData.m_nSoundIdx);
			}
		}
		break;
		case MESSAGE_PLAY_FREE_ONCE:
		{
			if (m_oData.m_nSoundIdx >= 0) {
				oPlayMaybeLoop(m_oData.m_nSoundIdx, false);
			}
		}
		break;
		case MESSAGE_PLAY_FREE_IDX:
		{
			const NamedIndex& oSounds = oLevel.game().getNamed().sounds();
			if (oSounds.isIndex(nValue)) {
				oPlay(nValue);
			}
		}
		break;
		case MESSAGE_PLAY_FREE_IDX_ONCE:
		{
			const NamedIndex& oSounds = oLevel.game().getNamed().sounds();
			if (oSounds.isIndex(nValue)) {
				oPlayMaybeLoop(nValue, false);
			}
		}
		break;
		case MESSAGE_PRELOAD:
		{
			if (m_oData.m_nSoundIdx >= 0) {
				oGame.preloadSound(m_oData.m_nSoundIdx);
			}
		}
		break;
		case MESSAGE_PAUSE:
		{
			if (m_refSound) {
				m_refSound->pause();
			}
		}
		break;
		case MESSAGE_RESUME:
		{
			if (m_refSound) {
				m_refSound->resume();
			}
		}
		break;
		case MESSAGE_STOP:
		{
			if (m_refSound) {
				oGame.removeSound(m_refSound);
				m_refSound.reset();
			}
		}
		break;
		case MESSAGE_SET_VOL:
		{
			m_fCurVolume = [&]()
			{
				if (nValue <= 0) {
					return 0.0;
				} else if (nValue >= 100) {
					return 1.0;
				} else {
					return 0.01 * nValue;
				}
			}();
			if (m_refSound) {
				m_refSound->setVolume(m_fCurVolume);
			}
		}
		break;
		case MESSAGE_SET_POS_X:
		{
//std::cout << "MESSAGE_SET_POS_X  nValue=" << nValue << '\n';
			m_oCurPosXY.m_fX = 1.0 * nValue;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_SET_POS_Y:
		{
			m_oCurPosXY.m_fY = 1.0 * nValue;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_SET_POS_Z:
		{
			m_fCurPosZ = 1.0 * nValue;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_SET_POS_XY:
		{
			const NPoint oXY = Util::unpackPointFromInt32(nValue);
			m_oCurPosXY.m_fX = 1.0 * oXY.m_nX;
			m_oCurPosXY.m_fY = 1.0 * oXY.m_nY;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_ADD_TO_POS_X:
		{
			m_oCurPosXY.m_fX += 1.0 * nValue;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_ADD_TO_POS_Y:
		{
			m_oCurPosXY.m_fY += 1.0 * nValue;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_ADD_TO_POS_Z:
		{
			m_fCurPosZ += 1.0 * nValue;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_ADD_PERC_TO_POS_X:
		{
			m_oCurPosXY.m_fX += 0.01 * nValue;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_ADD_PERC_TO_POS_Y:
		{
			m_oCurPosXY.m_fY += 0.01 * nValue;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_ADD_PERC_TO_POS_Z:
		{
			m_fCurPosZ += 0.01 * nValue;
			if (m_refSound) {
				m_refSound->setPos(m_oCurPosXY, m_fCurPosZ);
			}
		}
		break;
		case MESSAGE_RESTART_RESET:
		{
			initCommon();
		} // fallthrough
		case MESSAGE_RESTART:
		{
			if (m_refSound) {
				oGame.removeSound(m_refSound);
				m_refSound.reset();
			}
			if (m_oData.m_nSoundIdx >= 0) {
				m_refSound = oPlay(m_oData.m_nSoundIdx);
			}
		}
		break;
		default:
		{
			// do nothing
		}
	}
	if (m_refSound) {
		oActivate();
	}
}

} // namespace stmg
