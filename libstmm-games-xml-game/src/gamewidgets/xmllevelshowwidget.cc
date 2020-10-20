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
 * File:   xmllevelshowwidget.cc
 */

#include "gamewidgets/xmllevelshowwidget.h"

#include "layoutctx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/util/util.h>
#include <stmm-games/widgets/levelshowwidget.h>

#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sGameLevelShowWidgetNodeName = "LevelShowWidget";
static const std::string s_sGameLevelShowWidgetTopMarginAttr = "topMargin";
static const std::string s_sGameLevelShowWidgetBottomMarginAttr = "bottomMargin";
static const std::string s_sGameLevelShowWidgetLeftMarginAttr = "leftMargin";
static const std::string s_sGameLevelShowWidgetRightMarginAttr = "rightMargin";
static const std::string s_sGameLevelShowWidgetIsInteractiveAttr = "interactive";

XmlLevelShowWidgetParser::XmlLevelShowWidgetParser()
: XmlGameWidgetParser(s_sGameLevelShowWidgetNodeName)
{
}

shared_ptr<GameWidget> XmlLevelShowWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
	oCtx.addChecker(p0WidgetElement);
	LevelShowWidget::Init oInit;
	XmlGameWidgetParser::parseGameWidget(oInit, oCtx, p0WidgetElement);

	const bool bSubshows = oCtx.subshowIsSet();
	const int32_t nMate = oCtx.getContextMate();
//std::cout << "XmlLevelShowWidgetParser::parseGameWidget  bSubshows=" << bSubshows << "  nMate=" << nMate << '\n';
	if (bSubshows && (nMate < 0)) {
		throw XmlCommonErrors::error(oCtx, p0WidgetElement, XmlConditionalParser::s_sConditionalOwnerMateAttr
									, Util::stringCompose("In subshow mode the mate must be defined"));
	}
	//
	const auto oPairTopMargin = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameLevelShowWidgetTopMarginAttr);
	if (oPairTopMargin.first) {
		const std::string& sTopMargin = oPairTopMargin.second;
		oInit.m_fTopInTiles = XmlUtil::strToNumber<double>(oCtx, p0WidgetElement, s_sGameLevelShowWidgetTopMarginAttr, sTopMargin
																, false, true, 0, false, -1);
	}
	const auto oPairBottomMargin = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameLevelShowWidgetBottomMarginAttr);
	if (oPairBottomMargin.first) {
		const std::string& sBottomMargin = oPairBottomMargin.second;
		oInit.m_fBottomInTiles = XmlUtil::strToNumber<double>(oCtx, p0WidgetElement, s_sGameLevelShowWidgetBottomMarginAttr, sBottomMargin
																, false, true, 0, false, -1);
	}
	const auto oPairLeftMargin = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameLevelShowWidgetLeftMarginAttr);
	if (oPairLeftMargin.first) {
		const std::string& sLeftMargin = oPairLeftMargin.second;
		oInit.m_fLeftInTiles = XmlUtil::strToNumber<double>(oCtx, p0WidgetElement, s_sGameLevelShowWidgetLeftMarginAttr, sLeftMargin
																, false, true, 0, false, -1);
	}
	const auto oPairRightMargin = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameLevelShowWidgetRightMarginAttr);
	if (oPairRightMargin.first) {
		const std::string& sRightMargin = oPairRightMargin.second;
		oInit.m_fRightInTiles = XmlUtil::strToNumber<double>(oCtx, p0WidgetElement, s_sGameLevelShowWidgetRightMarginAttr, sRightMargin
																, false, true, 0, false, -1);
	}
	const auto oPairIsInteractive = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameLevelShowWidgetIsInteractiveAttr);
	if (oPairIsInteractive.first) {
		const std::string& sIsInteractive = oPairIsInteractive.second;
		oInit.m_bIsInteractive = XmlUtil::strToBool(oCtx, p0WidgetElement, s_sGameLevelShowWidgetIsInteractiveAttr, sIsInteractive);
	}
	//
	oCtx.removeChecker(p0WidgetElement, true);
	return std::make_shared<LevelShowWidget>(std::move(oInit));
}

} // namespace stmg
