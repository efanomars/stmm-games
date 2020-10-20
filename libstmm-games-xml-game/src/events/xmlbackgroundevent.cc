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
 * File:   xmlbackgroundevent.cc
 */

#include "events/xmlbackgroundevent.h"

#include "gamectx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlcommonparser.h>

#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/level.h>
#include <stmm-games/game.h>
#include <stmm-games/apppreferences.h>
#include <stmm-games/event.h>
#include <stmm-games/events/backgroundevent.h>
#include <stmm-games/levelanimation.h>
#include <stmm-games/levelshow.h>
#include <stmm-games/named.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/util/util.h>

#include <vector>
//#include <cassert>
//#include <iostream>
#include <utility>
#include <cstdint>

namespace stmg { class ConditionalCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventBackgroundNodeName = "BackgroundEvent";
static const std::string s_sEventBackgroundAnimationNameAttr = "animation";
static const std::string s_sEventBackgroundAniRefSysAttr = "refSys";
	static const std::string s_sEventBackgroundAniRefSysAttrBoard = "BOARD";
	static const std::string s_sEventBackgroundAniRefSysAttrShow = "SHOW";
	static const std::string s_sEventBackgroundAniRefSysAttrSubshow = "SUBSHOW";
static const std::string s_sEventBackgroundAniScrolledAttr = "scrolled";
static const std::string s_sEventBackgroundAniPosXAttr = "posX";
static const std::string s_sEventBackgroundAniPosYAttr = "posY";
static const std::string s_sEventBackgroundAniWAttr = "width";
static const std::string s_sEventBackgroundAniHAttr = "height";
static const std::string s_sEventBackgroundZPosAttr = "posZ";
static const std::string s_sEventBackgroundMoveXAttr = "moveX";
static const std::string s_sEventBackgroundMoveYAttr = "moveY";
static const std::string s_sEventBackgroundImageNodeName = "Image";
static const std::string s_sEventBackgroundImageIdAttr = "imgId";
static const std::string s_sEventBackgroundImageWAttr = "width";
static const std::string s_sEventBackgroundImageHAttr = "height";
static const std::string s_sEventBackgroundImageRelXAttr = "relPosX";
static const std::string s_sEventBackgroundImageRelYAttr = "relPosY";
static const std::string s_sEventBackgroundImageRelPosScrolledAttr = "relPosScrolled";

XmlBackgroundEventParser::XmlBackgroundEventParser()
: XmlEventParser(s_sEventBackgroundNodeName)
{
}

Event* XmlBackgroundEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventBackground(oCtx, p0Element), p0Element);
}

unique_ptr<Event> XmlBackgroundEventParser::parseEventBackground(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlBackgroundEventParser::parseEventBackground" << '\n';
	oCtx.addChecker(p0Element);
	BackgroundEvent::Init oBEInit;
	parseEventBase(oCtx, p0Element, oBEInit);

	const auto oPairAniName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundAnimationNameAttr);
	if (oPairAniName.first) {
		const std::string& sName = oPairAniName.second;
		oBEInit.m_nAnimationNamedIdx = oCtx.named().animations().addName(sName);
	}

	static std::vector<char const *> s_aRefSysEnumString{s_sEventBackgroundAniRefSysAttrBoard.c_str(), s_sEventBackgroundAniRefSysAttrShow.c_str(), s_sEventBackgroundAniRefSysAttrSubshow.c_str()};
	static const std::vector<LevelAnimation::REFSYS> s_aRefSysEnumValue{LevelAnimation::REFSYS_BOARD, LevelAnimation::REFSYS_SHOW, LevelAnimation::REFSYS_SUBSHOW};
	//
	const int32_t nIdxRefSys = getXmlConditionalParser().getEnumAttributeValue(oCtx, p0Element, s_sEventBackgroundAniRefSysAttr, s_aRefSysEnumString);
	if (nIdxRefSys >= 0) {
		oBEInit.m_eRefSys = s_aRefSysEnumValue[nIdxRefSys];
	}
	if (oBEInit.m_eRefSys == LevelAnimation::REFSYS_SUBSHOW) {
		if (!oCtx.level().subshowMode()) {
			throw XmlCommonErrors::error(oCtx, p0Element, s_sEventBackgroundAniRefSysAttr, "Subshow background needs Subshow mode.");
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
		oBEInit.m_eRefSys = static_cast<LevelAnimation::REFSYS>(static_cast<int32_t>(oBEInit.m_eRefSys) + nLevelPlayer);
	}

	const auto oPairAniScrolled = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundAniScrolledAttr);
	if (oPairAniScrolled.first) {
		const std::string& sAniScrolled = oPairAniScrolled.second;
		oBEInit.m_bScrolled = XmlUtil::strToBool(oCtx, p0Element, s_sEventBackgroundAniScrolledAttr, sAniScrolled);
		if (oBEInit.m_bScrolled && (oBEInit.m_eRefSys != LevelAnimation::REFSYS_BOARD)) {
			throw XmlCommonErrors::error(oCtx, p0Element, s_sEventBackgroundAniScrolledAttr
										, Util::stringCompose("Attribute '%1' can be true only when attribute '%2' is '%3'"
															, s_sEventBackgroundAniScrolledAttr, s_sEventBackgroundAniRefSysAttr
															, s_sEventBackgroundAniRefSysAttrBoard));
		}
	}

	const auto oPairZPos = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundZPosAttr);
	if (oPairZPos.first) {
		const std::string& sZPos = oPairZPos.second;
		oBEInit.m_nZ = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventBackgroundZPosAttr, sZPos, false
													, false, -1, false, -1);
	}
	Level& oLevel = oCtx.level();
	int32_t nDestW;
	int32_t nDestH;
	if (oBEInit.m_eRefSys >= LevelAnimation::REFSYS_SUBSHOW) {
		nDestW = oLevel.subshowGet(0).getW();
		nDestH = oLevel.subshowGet(0).getH();
	} else if (oBEInit.m_eRefSys == LevelAnimation::REFSYS_SHOW) {
		nDestW = oLevel.showGet().getW();
		nDestH = oLevel.showGet().getH();
	} else {//if (eRefSys == LevelAnimation::REFSYS_BOARD) {
		nDestW = oLevel.boardWidth();
		nDestH = oLevel.boardHeight();
	}

	const auto oPairAniPosX = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundAniPosXAttr);
	if (oPairAniPosX.first) {
		const std::string& sAniPosX = oPairAniPosX.second;
		oBEInit.m_oRect.m_fX = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundAniPosXAttr, sAniPosX, false
															, true, 0.0, true, nDestW);
	}
	const auto oPairAniPosY = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundAniPosYAttr);
	if (oPairAniPosY.first) {
		const std::string& sAniPosY = oPairAniPosY.second;
		oBEInit.m_oRect.m_fY = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundAniPosYAttr, sAniPosY, false
															, true, 0.0, true, nDestH);
	}
	oBEInit.m_oRect.m_fW = nDestW - oBEInit.m_oRect.m_fX;
	const auto oPairAniSizeW = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundAniWAttr);
	if (oPairAniSizeW.first) {
		const std::string& sAniSizeW = oPairAniSizeW.second;
		oBEInit.m_oRect.m_fW = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundAniWAttr, sAniSizeW, false
															, true, 0.0, true, oBEInit.m_oRect.m_fW);
	}
	oBEInit.m_oRect.m_fH = nDestH - oBEInit.m_oRect.m_fY;
	const auto oPairAniSizeH = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundAniHAttr);
	if (oPairAniSizeH.first) {
		const std::string& sAniSizeH = oPairAniSizeH.second;
		oBEInit.m_oRect.m_fH = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundAniHAttr, sAniSizeH, false
															, true, 0.0, true, oBEInit.m_oRect.m_fH);
	}
	if (oBEInit.m_oRect.m_fX >= nDestW) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sEventBackgroundAniPosXAttr, Util::stringCompose("Attribute '%1' too big", s_sEventBackgroundAniPosXAttr));
	}
	if (oBEInit.m_oRect.m_fY >= nDestH) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sEventBackgroundAniPosYAttr, Util::stringCompose("Attribute '%1' too big", s_sEventBackgroundAniPosYAttr));
	}
	if (oBEInit.m_oRect.m_fW <= 0.0) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sEventBackgroundAniWAttr, Util::stringCompose("Attribute '%1' cannot be 0.0", s_sEventBackgroundAniWAttr));
	}
	if (oBEInit.m_oRect.m_fH <= 0.0) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sEventBackgroundAniHAttr, Util::stringCompose("Attribute '%1' cannot be 0.0", s_sEventBackgroundAniHAttr));
	}

	const auto oPairMoveX = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundMoveXAttr);
	if (oPairMoveX.first) {
		const std::string& sMoveX = oPairMoveX.second;
		oBEInit.m_oMove.m_fX = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundMoveXAttr, sMoveX, false
															, false, -1, false, -1);
	}
	const auto oPairMoveY = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundMoveYAttr);
	if (oPairMoveY.first) {
		const std::string& sMoveY = oPairMoveY.second;
		oBEInit.m_oMove.m_fY = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundMoveYAttr, sMoveY, false
															, false, -1, false, -1);
	}

	getXmlConditionalParser().visitNamedElementChildren(oCtx, p0Element, s_sEventBackgroundImageNodeName, [&](const xmlpp::Element* p0ImageElement)
	{
		const int32_t nSize = static_cast<int32_t>(oBEInit.m_aPatternImages.size());
		oBEInit.m_aPatternImages.resize(nSize + 1);
		BackgroundEvent::PatternImage& oPatternImage = oBEInit.m_aPatternImages[nSize];
		parseEventBackgroundImage(oCtx, p0ImageElement, oPatternImage);
	});

	if (oBEInit.m_aPatternImages.empty()) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sEventBackgroundImageNodeName);
	}
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<BackgroundEvent>(std::move(oBEInit));
}
void XmlBackgroundEventParser::parseEventBackgroundImage(GameCtx& oCtx, const xmlpp::Element* p0Element, BackgroundEvent::PatternImage& oPatternImage)
{
	oCtx.addChecker(p0Element);
	const auto oPairImageName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundImageIdAttr);
	if (oPairImageName.first) {
		const std::string& sImageName = oPairImageName.second;
		oPatternImage.m_nImgId = oCtx.named().images().addName(sImageName);
	}

	const auto oPairImgW = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundImageWAttr);
	if (oPairImgW.first) {
		const std::string& sImgW = oPairImgW.second;
		oPatternImage.m_fImgW = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundImageWAttr, sImgW, true
															, true, 0.0, false, -1);
	}
	const auto oPairImgH = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundImageHAttr);
	if (oPairImgH.first) {
		const std::string& sImgH = oPairImgH.second;
		oPatternImage.m_fImgH = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundImageHAttr, sImgH, true
															, true, 0.0, false, -1);
	}

	const auto oPairImgRelX = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundImageRelXAttr);
	if (oPairImgRelX.first) {
		const std::string& sImgRelX = oPairImgRelX.second;
		oPatternImage.m_fImgRelPosX = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundImageRelXAttr, sImgRelX, true
																	, false, -1, false, -1);
	}
	const auto oPairImgRelY = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundImageRelYAttr);
	if (oPairImgRelY.first) {
		const std::string& sImgRelY = oPairImgRelY.second;
		oPatternImage.m_fImgRelPosY = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventBackgroundImageRelYAttr, sImgRelY, true
																	, false, -1, false, -1);
	}

	const auto oPairRelPosScrolled = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventBackgroundImageRelPosScrolledAttr);
	if (oPairRelPosScrolled.first) {
		const std::string& sRelPosScrolled = oPairRelPosScrolled.second;
		oPatternImage.m_bRelPosScrolled = XmlUtil::strToBool(oCtx, p0Element, s_sEventBackgroundImageRelPosScrolledAttr, sRelPosScrolled);
	}
	//
	oCtx.removeChecker(p0Element, true);
}
int32_t XmlBackgroundEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
																	, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "CONTROL_STOP") {
		nMsg = BackgroundEvent::MESSAGE_CONTROL_STOP;
	} else if (sMsgName == "CONTROL_RESTART") {
		nMsg = BackgroundEvent::MESSAGE_CONTROL_RESTART;
	} else if (sMsgName == "MOVE_PAUSE") {
		nMsg = BackgroundEvent::MESSAGE_MOVE_PAUSE;
	} else if (sMsgName == "MOVE_RESUME") {
		nMsg = BackgroundEvent::MESSAGE_MOVE_RESUME;
	} else if (sMsgName == "MOVE_SET_FACTOR") {
		nMsg = BackgroundEvent::MESSAGE_MOVE_SET_FACTOR;
	} else if (sMsgName == "ADD_POS_X") {
		nMsg = BackgroundEvent::MESSAGE_ADD_POS_X;
	} else if (sMsgName == "ADD_POS_Y") {
		nMsg = BackgroundEvent::MESSAGE_ADD_POS_Y;
	} else if (sMsgName == "ADD_POS_X_PERC") {
		nMsg = BackgroundEvent::MESSAGE_ADD_POS_X_PERC;
	} else if (sMsgName == "ADD_POS_Y_PERC") {
		nMsg = BackgroundEvent::MESSAGE_ADD_POS_Y_PERC;
	} else if (sMsgName == "IMG_NEXT") {
		nMsg = BackgroundEvent::MESSAGE_IMG_NEXT;
	} else if (sMsgName == "IMG_PREV") {
		nMsg = BackgroundEvent::MESSAGE_IMG_PREV;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}
int32_t XmlBackgroundEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
																				, const std::string& sListenerGroupName)
{
	return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
}

} // namespace stmg
