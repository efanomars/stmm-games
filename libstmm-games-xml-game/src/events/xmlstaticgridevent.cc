/*
 * File:   xmlstaticgridevent.cc
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

#include "events/xmlstaticgridevent.h"

#include "gamectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/animations/staticgridanimation.h>
#include <stmm-games/apppreferences.h>
#include <stmm-games/event.h>
#include <stmm-games/events/staticgridevent.h>
#include <stmm-games/levelanimation.h>
#include <stmm-games/levelshow.h>
#include <stmm-games/named.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/util/util.h>
#include <stmm-games/level.h>
#include <stmm-games/game.h>

#include <vector>
//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

namespace stmg { class ConditionalCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventStaticGridNodeName = "StaticGridEvent";
static const std::string s_sEventStaticGridAnimationNameAttr = "animation";
static const std::string s_sEventStaticGridBoardRefSysAttr = "refSys";
	static const std::string s_sEventStaticGridBoardRefSysAttrBoard = "BOARD";
	static const std::string s_sEventStaticGridBoardRefSysAttrShow = "SHOW";
	static const std::string s_sEventStaticGridBoardRefSysAttrSubshow = "SUBSHOW";
static const std::string s_sEventStaticGridAniGridXAttr = "gridX";
static const std::string s_sEventStaticGridAniGridYAttr = "gridY";
static const std::string s_sEventStaticGridAniGridWAttr = "gridW";
static const std::string s_sEventStaticGridAniGridHAttr = "gridH";
static const std::string s_sEventStaticGridAniGridCellsWAttr = "gridCellsW";
static const std::string s_sEventStaticGridAniGridCellsHAttr = "gridCellsH";
static const std::string s_sEventStaticGridZPosAttr = "posZ";
static const std::string s_sEventStaticGridImageNodeName = "Image";
static const std::string s_sEventStaticGridImageIdAttr = "imgId";
static const std::string s_sEventStaticGridImageRelXAttr = "relPosX";
static const std::string s_sEventStaticGridImageRelYAttr = "relPosY";
static const std::string s_sEventStaticGridImageWAttr = "width";
static const std::string s_sEventStaticGridImageHAttr = "height";
static const std::string s_sEventStaticGridImageRepeatXAttr = "repeatX";
static const std::string s_sEventStaticGridImageRepeatYAttr = "repeatY";

XmlStaticGridEventParser::XmlStaticGridEventParser()
: XmlEventParser(s_sEventStaticGridNodeName)
{
}

Event* XmlStaticGridEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventStaticGrid(oCtx, p0Element), p0Element);
}

unique_ptr<Event> XmlStaticGridEventParser::parseEventStaticGrid(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlGameParser::parseEventStaticGrid" << '\n';
	Level& oLevel = oCtx.level();
	oCtx.addChecker(p0Element);
	StaticGridEvent::Init oSGEInit;
	parseEventBase(oCtx, p0Element, oSGEInit);

	const auto oPairAniName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridAnimationNameAttr);
	if (oPairAniName.first) {
		const std::string& sName = oPairAniName.second;
		oSGEInit.m_oAnimationData.m_nAnimationNamedIdx = oCtx.named().animations().addName(sName);
	}
	oSGEInit.m_oAnimationData.m_fDuration = LevelAnimation::s_fDurationInfinity;

	static std::vector<char const *> s_aRefSysEnumString{s_sEventStaticGridBoardRefSysAttrBoard.c_str(), s_sEventStaticGridBoardRefSysAttrShow.c_str(), s_sEventStaticGridBoardRefSysAttrSubshow.c_str()};
	static const std::vector<LevelAnimation::REFSYS> s_aRefSysEnumValue{LevelAnimation::REFSYS_BOARD, LevelAnimation::REFSYS_SHOW, LevelAnimation::REFSYS_SUBSHOW};
	//
	const int32_t nIdxRefSys = getXmlConditionalParser().getEnumAttributeValue(oCtx, p0Element, s_sEventStaticGridBoardRefSysAttr, s_aRefSysEnumString);
	if (nIdxRefSys >= 0) {
		oSGEInit.m_eRefSys = s_aRefSysEnumValue[nIdxRefSys];
	}
	if (oSGEInit.m_eRefSys == LevelAnimation::REFSYS_SUBSHOW) {
		if (!oLevel.subshowMode()) {
			throw XmlCommonErrors::error(oCtx, p0Element, s_sEventStaticGridBoardRefSysAttr, "Subshow staticgrid needs Subshow mode.");
		}
		// add the level player (TODO level team if subshow shows team!?)
		const auto oPairOwner = getXmlConditionalParser().parseOwner(oCtx, p0Element, OwnerType::PLAYER);
		const int32_t nTeam = oPairOwner.first;
		const int32_t nMate = oPairOwner.second;
		if (nMate < 0) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, XmlConditionalParser::s_sConditionalOwnerMateAttr);
		}
		// calculate level player
		const bool bATIOL = oCtx.game().isAllTeamsInOneLevel();
		int32_t nLevelPlayer;
		if (bATIOL) {
			auto refPlayer = oCtx.appPreferences()->getTeam(nTeam)->getMate(nMate);
			nLevelPlayer = refPlayer->get();
		} else {
			nLevelPlayer = nMate;
		}
		oSGEInit.m_eRefSys = static_cast<LevelAnimation::REFSYS>(static_cast<int32_t>(oSGEInit.m_eRefSys) + nLevelPlayer);
	}

	const auto oPairZPos = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridZPosAttr);
	if (oPairZPos.first) {
		const std::string& sZPos = oPairZPos.second;
		oSGEInit.m_oAnimationData.m_nZ = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStaticGridZPosAttr, sZPos, false
																		, false, -1, false, -1);
	}
	int32_t nDestW;
	int32_t nDestH;
	if (oSGEInit.m_eRefSys >= LevelAnimation::REFSYS_SUBSHOW) {
		nDestW = oLevel.subshowGet(0).getW();
		nDestH = oLevel.subshowGet(0).getH();
	} else if (oSGEInit.m_eRefSys == LevelAnimation::REFSYS_SHOW) {
		nDestW = oLevel.showGet().getW();
		nDestH = oLevel.showGet().getH();
	} else {//if (eRefSys == LevelAnimation::REFSYS_BOARD) {
		nDestW = oLevel.boardWidth();
		nDestH = oLevel.boardHeight();
	}

	const auto oPairAniPosX = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridAniGridXAttr);
	if (oPairAniPosX.first) {
		const std::string& sAniPosX = oPairAniPosX.second;
		oSGEInit.m_oAnimationData.m_oPos.m_fX = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventStaticGridAniGridXAttr, sAniPosX, false
																			, true, 0.0, true, nDestW);
	}
	const auto oPairAniPosY = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridAniGridYAttr);
	if (oPairAniPosY.first) {
		const std::string& sAniPosY = oPairAniPosY.second;
		oSGEInit.m_oAnimationData.m_oPos.m_fY = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventStaticGridAniGridYAttr, sAniPosY, false
																			, true, 0.0, true, nDestH);
	}
	oSGEInit.m_oAnimationData.m_oSize.m_fW = nDestW - oSGEInit.m_oAnimationData.m_oPos.m_fX;
	const auto oPairAniSizeW = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridAniGridWAttr);
	if (oPairAniSizeW.first) {
		const std::string& sAniSizeW = oPairAniSizeW.second;
		oSGEInit.m_oAnimationData.m_oSize.m_fW = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventStaticGridAniGridWAttr, sAniSizeW, false
																			, true, 0.0, true, oSGEInit.m_oAnimationData.m_oSize.m_fW);
	}
	oSGEInit.m_oAnimationData.m_oSize.m_fH = nDestH - oSGEInit.m_oAnimationData.m_oPos.m_fY;
	const auto oPairAniSizeH = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridAniGridHAttr);
	if (oPairAniSizeH.first) {
		const std::string& sAniSizeH = oPairAniSizeH.second;
		oSGEInit.m_oAnimationData.m_oSize.m_fH = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventStaticGridAniGridHAttr, sAniSizeH, false
																			, true, 0.0, true, oSGEInit.m_oAnimationData.m_oSize.m_fH);
	}
	if (oSGEInit.m_oAnimationData.m_oPos.m_fX >= nDestW) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sEventStaticGridAniGridXAttr, Util::stringCompose("Attribute '%1' too big", s_sEventStaticGridAniGridXAttr));
	}
	if (oSGEInit.m_oAnimationData.m_oPos.m_fY >= nDestH) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sEventStaticGridAniGridYAttr, Util::stringCompose("Attribute '%1' too big", s_sEventStaticGridAniGridYAttr));
	}
	if (oSGEInit.m_oAnimationData.m_oSize.m_fW <= 0.0) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sEventStaticGridAniGridWAttr, Util::stringCompose("Attribute '%1' cannot be 0.0", s_sEventStaticGridAniGridWAttr));
	}
	if (oSGEInit.m_oAnimationData.m_oSize.m_fH <= 0.0) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sEventStaticGridAniGridHAttr, Util::stringCompose("Attribute '%1' cannot be 0.0", s_sEventStaticGridAniGridHAttr));
	}

	const auto oPairCellsW = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridAniGridCellsWAttr);
	if (oPairCellsW.first) {
		const std::string& sCellsW = oPairCellsW.second;
		oSGEInit.m_oAnimationData.m_oCellsSize.m_nW = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStaticGridAniGridCellsWAttr, sCellsW, false
																					, true, 1, false, -1);
	}
	const auto oPairCellsH = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridAniGridCellsHAttr);
	if (oPairCellsH.first) {
		const std::string& sCellsH = oPairCellsH.second;
		oSGEInit.m_oAnimationData.m_oCellsSize.m_nH = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventStaticGridAniGridCellsHAttr, sCellsH, false
																					, true, 1, false, -1);
	}

	getXmlConditionalParser().visitNamedElementChildren(oCtx, p0Element, s_sEventStaticGridImageNodeName, [&](const xmlpp::Element* p0ImageElement)
	{
		const int32_t nSize = static_cast<int32_t>(oSGEInit.m_oAnimationData.m_aSpans.size());
		oSGEInit.m_oAnimationData.m_aSpans.resize(nSize + 1);
		StaticGridAnimation::ImageSpan& oImageSpan = oSGEInit.m_oAnimationData.m_aSpans[nSize];
		parseEventStaticGridImage(oCtx, p0ImageElement, oSGEInit.m_oAnimationData, oImageSpan);
	});

	if (oSGEInit.m_oAnimationData.m_aSpans.empty()) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sEventStaticGridImageNodeName);
	}
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<StaticGridEvent>(std::move(oSGEInit));
}
void XmlStaticGridEventParser::parseEventStaticGridImage(GameCtx& oCtx, const xmlpp::Element* p0Element, StaticGridAnimation::LocalInit& oAniInit
														, StaticGridAnimation::ImageSpan& oImageSpan)
{
	oCtx.addChecker(p0Element);
	int32_t nImgIdx = -1;
	const auto oPairImageName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridImageIdAttr);
	if (oPairImageName.first) {
		const std::string& sImageName = oPairImageName.second;
		//const std::string sUpName = Glib::ustring{sName}.uppercase();
		nImgIdx = oCtx.named().images().addName(sImageName);
	}

	int32_t nPosX = 0;
	const auto oPairPosX = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridImageRelXAttr);
	if (oPairPosX.first) {
		const std::string& sPosX = oPairPosX.second;
		nPosX = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStaticGridImageRelXAttr, sPosX, false
											, true, 0, true, oAniInit.m_oCellsSize.m_nW - 1);
	}
	int32_t nPosY = 0;
	const auto oPairPosY = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridImageRelYAttr);
	if (oPairPosY.first) {
		const std::string& sPosY = oPairPosY.second;
		nPosY = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStaticGridImageRelYAttr, sPosY, false
											, true, 0, true, oAniInit.m_oCellsSize.m_nH - 1);
	}

	int32_t nCellsW = 1;
	const auto oPairCellsW = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridImageWAttr);
	if (oPairCellsW.first) {
		const std::string& sCellsW = oPairCellsW.second;
		nCellsW = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStaticGridImageWAttr, sCellsW, false
												, true, 1, true, oAniInit.m_oCellsSize.m_nW - nPosX);
	}
	int32_t nCellsH = 1;
	const auto oPairCellsH = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridImageHAttr);
	if (oPairCellsH.first) {
		const std::string& sCellsH = oPairCellsH.second;
		nCellsH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStaticGridImageHAttr, sCellsH, false
												, true, 1, true, oAniInit.m_oCellsSize.m_nH - nPosY);
	}

	int32_t nRepeatW = 1;
	const auto oPairRepeatW = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridImageRepeatXAttr);
	if (oPairRepeatW.first) {
		const std::string& sRepeatW = oPairRepeatW.second;
		nRepeatW = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStaticGridImageRepeatXAttr, sRepeatW, false
												, true, 1, true, (oAniInit.m_oCellsSize.m_nW - nPosX) / nCellsW);
	}
	int32_t nRepeatH = 1;
	const auto oPairRepeatH = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventStaticGridImageRepeatYAttr);
	if (oPairRepeatH.first) {
		const std::string& sRepeatH = oPairRepeatH.second;
		nRepeatH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStaticGridImageRepeatYAttr, sRepeatH, false
												, true, 1, true, (oAniInit.m_oCellsSize.m_nH - nPosY) / nCellsH);
	}

	oImageSpan.m_nImgId = nImgIdx;
	oImageSpan.m_oPos.m_nX = nPosX;
	oImageSpan.m_oPos.m_nY = nPosY;
	oImageSpan.m_oSpanSize.m_nW = nCellsW;
	oImageSpan.m_oSpanSize.m_nH = nCellsH;
	oImageSpan.m_oRepeatSize.m_nW = nRepeatW;
	oImageSpan.m_oRepeatSize.m_nH = nRepeatH;
	//
	oCtx.removeChecker(p0Element, true);
}
int32_t XmlStaticGridEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
													, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "CONTROL_STOP") {
		nMsg = StaticGridEvent::MESSAGE_CONTROL_STOP;
	} else if (sMsgName == "CONTROL_RESTART") {
		nMsg = StaticGridEvent::MESSAGE_CONTROL_RESTART;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}
int32_t XmlStaticGridEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
																				, const std::string& sListenerGroupName)
{
	return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
}

} // namespace stmg
