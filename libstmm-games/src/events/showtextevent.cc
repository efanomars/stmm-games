/*
 * File:   showtextevent.cc
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

#include "events/showtextevent.h"

#include "level.h"
#include "gameproxy.h"
#include "levelshow.h"
#include "ownertype.h"
#include "variable.h"
#include "util/util.h"

#include <vector>
#include <cassert>
#include <algorithm>
//#include <iostream>


namespace stmg
{

Recycler<ShowTextEvent::PrivateTextAnimation, TextAnimation> ShowTextEvent::s_oTextAnimationRecycler{};

ShowTextEvent::ShowTextEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
{
	commonInit();
}

void ShowTextEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);

	commonInit();
	m_refCurrentShowText.reset();
}
void ShowTextEvent::commonInit() noexcept
{
	assert(m_oData.m_aSobstLines.size() > 0);
	assert(m_oData.m_nAnimationNamedIdx >= -1);
	assert(m_oData.m_fTextSize > 0);
	assert(m_oData.m_nDuration >= -1);

	calcRectSortItems();
}
void ShowTextEvent::calcRectSortItems() noexcept
{
	auto& oRect = m_oData.m_oRect;
	if ((oRect.m_fW <= 0) || (oRect.m_fH <= 0)) {
		Level& oLevel = level();
		oRect.m_fX = 0;
		oRect.m_fY = 0;
		if (m_oData.m_eRefSys == LevelAnimation::REFSYS_BOARD) {
			oRect.m_fW = oLevel.boardWidth();
			oRect.m_fH = oLevel.boardHeight();
		} else if (m_oData.m_eRefSys == LevelAnimation::REFSYS_SHOW) {
			oRect.m_fW = oLevel.showGet().getW();
			oRect.m_fH = oLevel.showGet().getH();
		} else {
			assert(oLevel.subshowMode());
			assert(m_oData.m_eRefSys >= LevelAnimation::REFSYS_SUBSHOW);
			 // all subshows have the same size
			oRect.m_fW = oLevel.subshowGet(0,0).getW();
			oRect.m_fH = oLevel.subshowGet(0,0).getH();
		}
	}
	// sort the items in descending order by position, so that substitution doesn't
	// change the position of items preceding in the line.
	std::sort(m_oData.m_aSobsts.begin(), m_oData.m_aSobsts.end(), [](const SobstItem& oL, const SobstItem& oR)
	{
		return oL.m_nPos > oR.m_nPos;
	});
}

void ShowTextEvent::trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept
{
//std::cout << "ShowTextEvent(" << reinterpret_cast<int64_t>(this) << ")::trigger  m_aSobstLines.size = " << m_aSobstLines.size() << '\n';
	Level& oLevel = level();
	auto& oGame = oLevel.game();
	if (nMsg == MESSAGE_STOP_ANIMATION) {
		if (m_refCurrentShowText) {
			oLevel.animationRemove(m_refCurrentShowText);
			m_refCurrentShowText.reset();
		}
		return; //--------------------------------------------------------------
	}
	double fPosX = m_oData.m_oRect.m_fX;
	double fPosY = m_oData.m_oRect.m_fY;
	if (nMsg == MESSAGE_START_ANIMATION_AT) {
		const NPoint oPoint = Util::unpackPointFromInt32(nValue);
		fPosX = - m_oData.m_oRect.m_fW / 2 + oPoint.m_nX;
		fPosY = - m_oData.m_oRect.m_fH / 2 + oPoint.m_nY;
	}
	//TODO start subshow animations for each player!
	//TODO const bool bIsSubshow = oLevel.subshowMode();
	std::vector<std::string> aDestLine = m_oData.m_aSobstLines;
	const int32_t nTotSobst = static_cast<int32_t>(m_oData.m_aSobsts.size());
	if (nTotSobst == 0) {
	} else {
		const int32_t nGameTick = oGame.gameElapsed();
		for (const SobstItem& oSobstItem : m_oData.m_aSobsts) {
			std::string& sDestLine = aDestLine[oSobstItem.m_nLine];
			switch (oSobstItem.m_eSobstType) {
				case SOBST_TYPE_INPUT:
				{
					const auto eInputType = static_cast<SOBST_INPUT_TYPE>(oSobstItem.m_nParam1);
					if (eInputType == SOBST_INPUT_TYPE_MSG) {
						sDestLine.insert(oSobstItem.m_nPos, std::to_string(nMsg));
					} else if (eInputType == SOBST_INPUT_TYPE_VALUE) {
						sDestLine.insert(oSobstItem.m_nPos, std::to_string(nValue));
					} else if (eInputType == SOBST_INPUT_TYPE_VALUE_X) {
						const NPoint oXY = Util::unpackPointFromInt32(nValue);
						sDestLine.insert(oSobstItem.m_nPos, std::to_string(oXY.m_nX));
					} else if (eInputType == SOBST_INPUT_TYPE_VALUE_Y) {
						const NPoint oXY = Util::unpackPointFromInt32(nValue);
						sDestLine.insert(oSobstItem.m_nPos, std::to_string(oXY.m_nY));
					} else if (eInputType == SOBST_INPUT_TYPE_TICK) {
						sDestLine.insert(oSobstItem.m_nPos, std::to_string(nGameTick));
					} else if (eInputType == SOBST_INPUT_TYPE_EVENT) {
						if (p0TriggeringEvent == nullptr) {
							sDestLine.insert(oSobstItem.m_nPos, "N");
						} else {
							sDestLine.insert(oSobstItem.m_nPos, "Y");
						}
					} else {
						;
					}
				}
				break;
				case SOBST_TYPE_VAR_ID:
				{
					const auto eVarIdType = static_cast<OwnerType>(oSobstItem.m_nParam1);
					const int32_t nVarId = oSobstItem.m_nParam2;
					const std::string sVarStr = [&]()
					{
						switch (eVarIdType) {
						//case SOBST_VAR_ID_TYPE_GAME:
						//	return oLevel.variable(nVarId, -1, -1);
						case OwnerType::TEAM:
							if (oGame.hasVariableId(nVarId, OwnerType::TEAM)) {
								const auto& oVar = oLevel.variable(nVarId, oSobstItem.m_nParam3, -1);
								return oVar.toFormattedString();
							}
							return Util::s_sEmptyString;
						case OwnerType::PLAYER:
							if (oGame.hasVariableId(nVarId, OwnerType::PLAYER)) {
								const auto& oVar = oLevel.variable(nVarId, oSobstItem.m_nParam3, oSobstItem.m_nParam4);
								return oVar.toFormattedString();
							}
							return Util::s_sEmptyString;
						default:
							;
						}
						if (oGame.hasVariableId(nVarId, OwnerType::GAME)) {
							const auto& oVar = oLevel.variable(nVarId, -1, -1);
							return oVar.toFormattedString();
						}
						return Util::s_sEmptyString;
					}();
					sDestLine.insert(oSobstItem.m_nPos, sVarStr);
				}
				break;
				case SOBST_TYPE_INTERVAL:
				{
					const auto eIntervalType = static_cast<SOBST_INTERVAL_TYPE>(oSobstItem.m_nParam1);
					if (eIntervalType == SOBST_INTERVAL_TYPE_GAME) {
						sDestLine.insert(oSobstItem.m_nPos, std::to_string(oGame.gameInterval()));
					} else if (eIntervalType == SOBST_INTERVAL_TYPE_LEVEL) {
						sDestLine.insert(oSobstItem.m_nPos, std::to_string(oLevel.getInterval()));
					} else {
						;
					}
				}
				break;
			}
		}
	}
	animationCreateShowText(std::move(aDestLine), fPosX, fPosY);
//std::cout << "ShowTextEvent::trigger nTotSobst=" << nTotSobst << '\n';
//std::cout << "      oSobstItem.m_nLine=" << oSobstItem.m_nLine << '\n';
//std::cout << "      oSobstItem.m_nPos=" << oSobstItem.m_nPos << '\n';
//std::cout << "      oSobstItem.m_eSobstType=" << (int32_t)oSobstItem.m_eSobstType << '\n';
//std::cout << "      oSobstItem.m_nParam1=" << oSobstItem.m_nParam1 << '\n';
//std::cout << "      oSobstItem.m_sName=" << oSobstItem.m_sName << '\n';
}

void ShowTextEvent::animationCreateShowText(std::vector<std::string>&& aLines, double fPosX, double fPosY) noexcept
{
	if (m_refCurrentShowText) {
		level().animationRemove(m_refCurrentShowText);
		m_refCurrentShowText.reset();
	}
	TextAnimation::Init oInit;
	oInit.m_nAnimationNamedIdx = m_oData.m_nAnimationNamedIdx;
	oInit.m_fDuration = m_oData.m_nDuration;
	oInit.m_oPos.m_fX = fPosX;
	oInit.m_oPos.m_fY = fPosY;
	oInit.m_oSize.m_fW = m_oData.m_oRect.m_fW;
	oInit.m_oSize.m_fH = m_oData.m_oRect.m_fH;
	oInit.m_nZ = m_oData.m_nZ;
	oInit.m_aLines = std::move(aLines);
	oInit.m_fFontHeight = m_oData.m_fTextSize;
	s_oTextAnimationRecycler.create(m_refCurrentShowText, std::move(oInit));

	level().animationAdd(m_refCurrentShowText, m_oData.m_eRefSys, 0.0);
}

} // namespace stmg
