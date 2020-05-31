/*
 * File:   backgroundevent.cc
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

#include "events/backgroundevent.h"

#include "level.h"

#include <vector>
#include <cassert>
#include <utility>
#include <iostream>

namespace stmg { class TileRect; }


namespace stmg
{

BackgroundEvent::BackgroundEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
{
	commonInit(std::move(oInit));
}
void BackgroundEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	commonInit(std::move(oInit));
}
void BackgroundEvent::commonInit(LocalInit&& oInit) noexcept
{
	m_oBAInit.m_nAnimationNamedIdx = oInit.m_nAnimationNamedIdx;
	m_oBAInit.m_fDuration = LevelAnimation::s_fDurationInfinity;
	m_oBAInit.m_nZ = oInit.m_nZ;
	m_oBAInit.m_oPos.m_fX = oInit.m_oRect.m_fX;
	m_oBAInit.m_oPos.m_fY = oInit.m_oRect.m_fY;
	m_oBAInit.m_oSize.m_fW = oInit.m_oRect.m_fW;
	m_oBAInit.m_oSize.m_fH = oInit.m_oRect.m_fH;

	m_aPatternImages = std::move(oInit.m_aPatternImages);
	assert(m_aPatternImages.size() > 0);
	m_eRefSys = oInit.m_eRefSys;
	m_bScrolled = ((m_eRefSys == LevelAnimation::REFSYS_BOARD) ? oInit.m_bScrolled : false);
	m_oMove = oInit.m_oMove;

	m_nCurPatternImg = -1; // not running (stopped)
	m_bPaused = false;
	m_bMoving = ((m_oMove.m_fX != 0.0) || (m_oMove.m_fY != 0.0)); // shortcut
	m_fMoveFactor = 1.0;
}
void BackgroundEvent::trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept
{
	Level& oLevel = level();
	const int32_t nGameTick = oLevel.gameElapsed();
//std::cout << "BackgroundEvent::trigger nGameTick=" << nGameTick << "  nMsg=" << nMsg << "   nValue = " << nValue << '\n';
	const bool bStopped = (m_nCurPatternImg < 0);
	if ((bStopped && (p0TriggeringEvent == nullptr)) || (nMsg == MESSAGE_CONTROL_RESTART)) {
		if (! bStopped) {
			oLevel.animationRemove(m_refBGAnimation);
			m_refBGAnimation.reset();
		}
		assert(! m_refBGAnimation);
		m_oBGAnimations.create(m_refBGAnimation, m_oBAInit);
		m_nCurPatternImg = 0;
//std::cout << "BackgroundEvent::trigger START m_nCurPatternImg=" << m_nCurPatternImg << "" << '\n';
		PatternImage& oPatternImage = m_aPatternImages[m_nCurPatternImg];
		m_refBGAnimation->setImage(oPatternImage.m_nImgId, {oPatternImage.m_fImgW, oPatternImage.m_fImgH}
									, {oPatternImage.m_fImgRelPosX, oPatternImage.m_fImgRelPosY});
		if (m_bScrolled) {
			oLevel.animationAddScrolled(m_refBGAnimation, 0);
		} else {
			oLevel.animationAdd(m_refBGAnimation, m_eRefSys, 0);
		}
		oLevel.boardScrollAddListener(this);
		if (m_bMoving) {
			oLevel.activateEvent(this, nGameTick + 1);
		}
		return; //--------------------------------------------------------------
	}
//std::cout << "BackgroundEvent::trigger 2" << '\n';
	if (nMsg == MESSAGE_CONTROL_STOP) {
		m_nCurPatternImg = -1;
		assert(m_refBGAnimation);
		oLevel.boardScrollRemoveListener(this);
		oLevel.animationRemove(m_refBGAnimation);
		m_refBGAnimation.reset();
		oLevel.deactivateEvent(this);
		return; //--------------------------------------------------------------
	}
	if (m_bPaused) {
		if (nMsg == MESSAGE_MOVE_RESUME) {
			m_bPaused = false;
			if (m_bMoving) {
				oLevel.activateEvent(this, nGameTick);
			}
		}
		return; //--------------------------------------------------------------
	}
	if ((nMsg >= MESSAGE_ADD_POS_X) && (nMsg <= MESSAGE_ADD_POS_Y_PERC)) {
		FPoint oPos = m_refBGAnimation->getPos();
		if (nMsg == MESSAGE_ADD_POS_X) {
			oPos.m_fX += nValue;
		} else if (nMsg == MESSAGE_ADD_POS_Y) {
			oPos.m_fY += nValue;
		} else if (nMsg == MESSAGE_ADD_POS_X_PERC) {
			oPos.m_fX += 0.01 * nValue;
		} else if (nMsg == MESSAGE_ADD_POS_Y_PERC) {
			oPos.m_fY += 0.01 * nValue;
		}
		m_refBGAnimation->moveTo(oPos);
		keepMoving(nGameTick);
		return; //--------------------------------------------------------------
	}
	if ((nMsg == MESSAGE_IMG_NEXT) || (nMsg == MESSAGE_IMG_PREV)) {
		if (nMsg == MESSAGE_IMG_NEXT) {
			++m_nCurPatternImg;
			if (m_nCurPatternImg >= static_cast<int32_t>(m_aPatternImages.size())) {
				m_nCurPatternImg = 0;
			}
		} else {
			--m_nCurPatternImg;
			if (m_nCurPatternImg < 0) {
				m_nCurPatternImg = static_cast<int32_t>(m_aPatternImages.size()) - 1;
			}
		}
//std::cout << "BackgroundEvent::trigger m_nCurPatternImg=" << m_nCurPatternImg << "" << '\n';
		assert(m_refBGAnimation);
		PatternImage& oPatternImage = m_aPatternImages[m_nCurPatternImg];
		m_refBGAnimation->setImage(oPatternImage.m_nImgId, {oPatternImage.m_fImgW, oPatternImage.m_fImgH}
									, {oPatternImage.m_fImgRelPosX, oPatternImage.m_fImgRelPosY});
		keepMoving(nGameTick);
		return; //--------------------------------------------------------------
	}
	if (nMsg == MESSAGE_MOVE_PAUSE) {
		m_bPaused = true;
		oLevel.deactivateEvent(this);
		return; //--------------------------------------------------------------
	}
	if (nMsg == MESSAGE_MOVE_SET_FACTOR) {
		m_fMoveFactor = (1.0 * nValue) / 100.0;
		keepMoving(nGameTick);
		return; //--------------------------------------------------------------
	}
	if (p0TriggeringEvent == nullptr) {
		keepMoving(nGameTick);
	}
}
void BackgroundEvent::keepMoving(int32_t nGameTick) noexcept
{
	if (m_bMoving && (m_fMoveFactor != 0.0)) {
		// Move
		assert(m_refBGAnimation);
		const FPoint oPos = m_refBGAnimation->getImageRelPos(); //LevelAnimation::getPos();
		m_refBGAnimation->moveImageTo({oPos.m_fX + m_oMove.m_fX * m_fMoveFactor, oPos.m_fY + m_oMove.m_fY * m_fMoveFactor});
		level().activateEvent(this, nGameTick + 1);
	}
}
void BackgroundEvent::boardPreScroll(Direction::VALUE /*eDir*/, const shared_ptr<TileRect>& /*refTiles*/) noexcept
{
}
void BackgroundEvent::boardPostScroll(Direction::VALUE eDir) noexcept
{
	PatternImage& oPatternImage = m_aPatternImages[m_nCurPatternImg];
	if (! oPatternImage.m_bRelPosScrolled) {
		return;
	}
	assert(m_refBGAnimation);
	const Direction::VALUE eRelPosDir = (m_bScrolled ? Direction::opposite(eDir): eDir);
	m_refBGAnimation->boardAfterScroll(eRelPosDir);
}

} // namespace stmg
