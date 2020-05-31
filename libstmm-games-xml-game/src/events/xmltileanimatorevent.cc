/*
 * File:   xmltileanimatorevent.cc
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

#include "events/xmltileanimatorevent.h"

#include "gamectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmltraitsparser.h>
#include "xmlutil/xmlstrconv.h"
#include "xmlutil/xmlbasicparser.h"

#include <stmm-games/level.h>
#include <stmm-games/event.h>
#include <stmm-games/events/tileanimatorevent.h>
#include <stmm-games/named.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/utile/tileselector.h>

#include <cassert>
#include <iostream>
#include <cstdint>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventTileAnimatorNodeName = "TileAnimatorEvent";
static const std::string s_sEventTileAnimatorAniNameAttr = "aniName";
static const std::string s_sEventTileAnimatorDoBoardAttr = "board";
static const std::string s_sEventTileAnimatorAreaXAttr = "areaX";
static const std::string s_sEventTileAnimatorAreaYAttr = "areaY";
static const std::string s_sEventTileAnimatorAreaWAttr = "areaW";
static const std::string s_sEventTileAnimatorAreaHAttr = "areaH";
static const std::string s_sEventTileAnimatorDoBlocksAttr = "blocks";
static const std::string s_sEventTileAnimatorInitialWaitTicksAttr = "initialWaitTicks";
static const std::string s_sEventTileAnimatorInitialWaitTicksFromAttr = "initialWaitTicksFrom";
static const std::string s_sEventTileAnimatorInitialWaitTicksToAttr = "initialWaitTicksTo";
static const std::string s_sEventTileAnimatorInitialWaitMillisecAttr = "initialWaitMillisec";
static const std::string s_sEventTileAnimatorInitialWaitMillisecFromAttr = "initialWaitMillisecFrom";
static const std::string s_sEventTileAnimatorInitialWaitMillisecToAttr = "initialWaitMillisecTo";
static const std::string s_sEventTileAnimatorDurationTicksAttr = "durationTicks";
static const std::string s_sEventTileAnimatorDurationTicksFromAttr = "durationTicksFrom";
static const std::string s_sEventTileAnimatorDurationTicksToAttr = "durationTicksTo";
static const std::string s_sEventTileAnimatorDurationMillisecAttr = "durationMillisec";
static const std::string s_sEventTileAnimatorDurationMillisecFromAttr = "durationMillisecFrom";
static const std::string s_sEventTileAnimatorDurationMillisecToAttr = "durationMillisecTo";
static const std::string s_sEventTileAnimatorPauseTicksAttr = "pauseTicks";
static const std::string s_sEventTileAnimatorPauseTicksFromAttr = "pauseTicksFrom";
static const std::string s_sEventTileAnimatorPauseTicksToAttr = "pauseTicksTo";
static const std::string s_sEventTileAnimatorPauseMillisecAttr = "pauseMillisec";
static const std::string s_sEventTileAnimatorPauseMillisecFromAttr = "pauseMillisecFrom";
static const std::string s_sEventTileAnimatorPauseMillisecToAttr = "pauseMillisecTo";
static const std::string s_sEventTileAnimatorCountAttr = "count";
static const std::string s_sEventTileAnimatorCountFromAttr = "countFrom";
static const std::string s_sEventTileAnimatorCountToAttr = "countTo";
static const std::string s_sEventTileAnimatorMaxParallelAttr = "maxParallel";
static const std::string s_sEventTileAnimatorSelectNodeName = "Select";


XmlTileAnimatorEventParser::XmlTileAnimatorEventParser()
: XmlEventParser(s_sEventTileAnimatorNodeName)
{
}
Event* XmlTileAnimatorEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventTileAnimator(oCtx, p0Element), p0Element);
}

unique_ptr<Event> XmlTileAnimatorEventParser::parseEventTileAnimator(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlTileAnimatorEventParser::parseEventTileAnimator" << '\n';
	oCtx.addChecker(p0Element);
	TileAnimatorEvent::Init oInit;
	parseEventBase(oCtx, p0Element, oInit);

	Level& oLevel = oCtx.level();

	const auto oPairAniName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventTileAnimatorAniNameAttr);
	if (!oPairAniName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sEventTileAnimatorAniNameAttr);
	}
	const std::string& sAniName = oPairAniName.second;
	oInit.m_nAniNameIdx = oCtx.named().tileAnis().addName(sAniName);
	assert(oInit.m_nAniNameIdx >= 0);
//std::cout << "XmlGameParser::parseEventTileAnimator  nAniNameIdx=" << nAniNameIdx << '\n';

	oInit.m_bDoBoard = true;
	const auto oPairDoBoard = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventTileAnimatorDoBoardAttr);
	if (oPairDoBoard.first) {
		const std::string& sDoBoard = oPairDoBoard.second;
		oInit.m_bDoBoard = XmlUtil::strToBool(oCtx, p0Element, s_sEventTileAnimatorDoBoardAttr, sDoBoard);
	}
	XmlBasicParser oXmlBasicParser(getXmlConditionalParser());
	oInit.m_oArea = oXmlBasicParser.parseNRect(oCtx, p0Element
												, s_sEventTileAnimatorAreaXAttr, s_sEventTileAnimatorAreaYAttr, s_sEventTileAnimatorAreaWAttr, s_sEventTileAnimatorAreaHAttr
												, true, NRect{0, 0, oLevel.boardWidth(), oLevel.boardHeight()}
												, NSize{1, 1});
	//
	const auto oPairDoBlocks = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventTileAnimatorDoBlocksAttr);
	if (oPairDoBlocks.first) {
		const std::string& sDoBlocks = oPairDoBlocks.second;
		oInit.m_bDoBlocks = XmlUtil::strToBool(oCtx, p0Element, s_sEventTileAnimatorDoBlocksAttr, sDoBlocks);
	}

	getXmlConditionalParser().parseAttributeFromTo<int32_t>(oCtx, p0Element
							, s_sEventTileAnimatorInitialWaitTicksAttr, s_sEventTileAnimatorInitialWaitTicksFromAttr, s_sEventTileAnimatorInitialWaitTicksToAttr
							, false, true, 0, false, -1, oInit.m_oInitialWait.m_oTicks.m_nFrom, oInit.m_oInitialWait.m_oTicks.m_nTo);
	getXmlConditionalParser().parseAttributeFromTo<int32_t>(oCtx, p0Element
							, s_sEventTileAnimatorInitialWaitMillisecAttr, s_sEventTileAnimatorInitialWaitMillisecFromAttr, s_sEventTileAnimatorInitialWaitMillisecToAttr
							, false, true, 0, false, -1, oInit.m_oInitialWait.m_oMillisec.m_nFrom, oInit.m_oInitialWait.m_oMillisec.m_nTo);

	getXmlConditionalParser().parseAttributeFromTo<int32_t>(oCtx, p0Element
							, s_sEventTileAnimatorDurationTicksAttr, s_sEventTileAnimatorDurationTicksFromAttr, s_sEventTileAnimatorDurationTicksToAttr
							, false, true, 0, false, -1, oInit.m_oDuration.m_oTicks.m_nFrom, oInit.m_oDuration.m_oTicks.m_nTo);
	getXmlConditionalParser().parseAttributeFromTo<int32_t>(oCtx, p0Element
							, s_sEventTileAnimatorDurationMillisecAttr, s_sEventTileAnimatorDurationMillisecFromAttr, s_sEventTileAnimatorDurationMillisecToAttr
							, false, true, 0, false, -1, oInit.m_oDuration.m_oMillisec.m_nFrom, oInit.m_oDuration.m_oMillisec.m_nTo);

	getXmlConditionalParser().parseAttributeFromTo<int32_t>(oCtx, p0Element
							, s_sEventTileAnimatorPauseTicksAttr, s_sEventTileAnimatorPauseTicksFromAttr, s_sEventTileAnimatorPauseTicksToAttr
							, false, true, 0, false, -1, oInit.m_oPause.m_oTicks.m_nFrom, oInit.m_oPause.m_oTicks.m_nTo);
	getXmlConditionalParser().parseAttributeFromTo<int32_t>(oCtx, p0Element
							, s_sEventTileAnimatorPauseMillisecAttr, s_sEventTileAnimatorPauseMillisecFromAttr, s_sEventTileAnimatorPauseMillisecToAttr
							, false, true, 0, false, -1, oInit.m_oPause.m_oMillisec.m_nFrom, oInit.m_oPause.m_oMillisec.m_nTo);

	getXmlConditionalParser().parseAttributeFromTo<int32_t>(oCtx, p0Element
							, s_sEventTileAnimatorCountAttr, s_sEventTileAnimatorCountFromAttr, s_sEventTileAnimatorCountToAttr
							, false, true, 1, false, -1, oInit.m_oTotCount.m_nFrom, oInit.m_oTotCount.m_nTo);

	const auto oPairMaxParallel =  getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventTileAnimatorMaxParallelAttr);
	if (oPairMaxParallel.first) {
		const std::string& sMaxParallel = oPairMaxParallel.second;
		oInit.m_nMaxParallel = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventTileAnimatorMaxParallelAttr, sMaxParallel, false
															, true, -1, false, -1);
	}
	const xmlpp::Element* p0Selects = getXmlConditionalParser().parseUniqueElement(oCtx, p0Element, s_sEventTileAnimatorSelectNodeName, true);
	assert(p0Selects != nullptr);
	oInit.m_refSelect = parseEventTileAnimatorSelect(oCtx, p0Selects);

	oInit.m_nRepeat = parseEventAttrRepeat(oCtx, p0Element);

	oCtx.removeChecker(p0Element, false, true);

	return std::make_unique<TileAnimatorEvent>(std::move(oInit));
}
unique_ptr<TileSelector> XmlTileAnimatorEventParser::parseEventTileAnimatorSelect(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	unique_ptr<TileSelector> refSelector = getXmlTraitsParser().parseTileSelectorOr(oCtx, p0Element);
	// traits parser is responsible for checking child elements
	oCtx.removeChecker(p0Element, false, true);
	return refSelector;
}
int32_t XmlTileAnimatorEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
																, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "TILEANI_STARTED") {
		nListenerGroup = TileAnimatorEvent::LISTENER_GROUP_TILEANI_STARTED;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
