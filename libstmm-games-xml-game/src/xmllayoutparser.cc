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
 * File:   xmllayoutparser.cc
 */

#include "xmllayoutparser.h"
#include "xmlgameparser.h"
#include "gameinitctx.h"
#include "layoutctx.h"

#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlcommonparser.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games-file/gameconstraints.h>

#include <stmm-games/layout.h>
#include <stmm-games/named.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/util/util.h>
#include <stmm-games/containerwidget.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/relsizedgamewidget.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <cassert>
#include <algorithm>
#include <array>
#include <cstdint>

namespace stmg { class ConditionalCtx; }
namespace stmg { class LevelShowWidget; }
namespace stmg { class XmlImageParser; }

namespace stmg
{

const std::string XmlLayoutParser::s_sGameWidgetsNodeName = "Widgets";
static const std::string s_sGameWidgetsGroupNodeName = "Pack";
static const std::string s_sGameWidgetsGroupNameAttr = "name";

const std::string XmlLayoutParser::s_sGameLayoutNodeName = "Layout";

static const std::string s_sGameWidgetNameAttr = "name";
static const std::string s_sGameWidgetViewNameAttr = "viewName";
//static const std::string s_sGameWidgetIsSwitchableAttr = "switchable";
static const std::string s_sGameWidgetAlignHorizAttr = "alignHoriz";
static const std::string s_sGameWidgetAlignVertAttr = "alignVert";
	static const std::string s_sGameWidgetAlignAttrFill = "FILL";
	static const std::string s_sGameWidgetAlignAttrStart = "START";
	static const std::string s_sGameWidgetAlignAttrCenter = "CENTER";
	static const std::string s_sGameWidgetAlignAttrEnd = "END";
static const std::string s_sGameWidgetExpandHorizAttr = "expandHoriz";
static const std::string s_sGameWidgetExpandVertAttr = "expandVert";

static const std::string s_sGameWidgetReferenceWFactorAttr = "refWFactor";
static const std::string s_sGameWidgetReferenceHFactorAttr = "refHFactor";
static const std::string s_sGameWidgetWHAttr = "whFactor";

static const std::string s_sContainerWidgetExpandHorizIfChildAttr = "expandHorizIfChild";
static const std::string s_sContainerWidgetExpandVertIfChildAttr = "expandVertIfChild";
static const std::string s_sContainerWidgetFillHorizIfChildAttr = "fillHorizIfChild";
static const std::string s_sContainerWidgetFillVertIfChildAttr = "fillVertIfChild";

XmlLayoutParser::XmlLayoutParser(XmlTraitsParser& oXmlTraitsParser, XmlImageParser& oXmlImageParser)
: m_oXmlConditionalParser(oXmlTraitsParser.getXmlConditionalParser())
, m_oXmlTraitsParser(oXmlTraitsParser)
, m_oXmlImageParser(oXmlImageParser)
, m_p0XmlGameParser(nullptr)
{
}

void XmlLayoutParser::addXmlGameWidgetParser(unique_ptr<XmlGameWidgetParser> refXmlGameWidgetParser)
{
	assert(refXmlGameWidgetParser);
	assert(refXmlGameWidgetParser->m_p1Owner == nullptr);
	refXmlGameWidgetParser->m_p1Owner = this;
	m_aXmlGameWidgetParsers.push_back(std::move(refXmlGameWidgetParser));
}

shared_ptr<Layout> XmlLayoutParser::parseLayout(GameInitCtx& oCtx, const xmlpp::Element* p0WidgetsElement, const xmlpp::Element* p0LayoutElement)
{
//std::cout << "XmlLayoutParser::parseLayout() " << '\n';
	if (p0WidgetsElement != nullptr) {
		throw XmlCommonErrors::error(oCtx, p0WidgetsElement, Util::s_sEmptyString, "Layout: <Widgets/> not implemented yet");
	}
	if (p0LayoutElement == nullptr) {
		throw XmlCommonErrors::error(oCtx, p0LayoutElement, Util::s_sEmptyString, "Layout: auto generation not implemented yet");
	}
	oCtx.addChecker(p0LayoutElement);
	auto aRoot = parseChildWidgets(oCtx, p0LayoutElement);
	// just check it has no attributes
	oCtx.removeChecker(p0LayoutElement, false, true);
	const auto nSize = aRoot.size();
	if (nSize == 0) {
		// generate automatically? Or is <Auto/> needed?
		throw XmlCommonErrors::error(oCtx, p0LayoutElement, Util::s_sEmptyString, "Layout: auto generation not implemented yet");
	} else if (nSize > 1) {
		throw XmlCommonErrors::error(oCtx, p0LayoutElement, Util::s_sEmptyString, "Layout: can only have one root widget");
	}
	auto refLayout = std::make_shared<Layout>(aRoot[0], shared_ptr<LevelShowWidget>{}
											, oCtx.getGameConstraints().isAllTeamsInOneLevel(), oCtx.subshowIsSet()
											, oCtx.appPreferences());
	if (!refLayout->isValid()) {
		throw XmlCommonErrors::error(oCtx, p0LayoutElement, Util::s_sEmptyString, refLayout->getErrorString());
	}
//std::cout << "XmlLayoutParser::parseLayout" << '\n';
//#ifndef NDEBUG
//std::cout << ""; refLayout->dump();
//#endif //NDEBUG
	return refLayout;
}
std::pair<std::string, int32_t> XmlLayoutParser::parseEvIdMessage(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory
																, const std::string& sEvIdAttrName, const std::string& sMsgNameAttrName, const std::string& sMsgAttrName)
{
	assert(m_p0XmlGameParser != nullptr);
	return m_p0XmlGameParser->parseEvIdMessage(oCtx, p0Element, bMandatory, sEvIdAttrName, sMsgNameAttrName, sMsgAttrName);
}
std::pair<int32_t, OwnerType> XmlLayoutParser::parseVariable(LayoutCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sVarNameAttrName, bool bMandatory)
{
	const auto oPairVariableName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sVarNameAttrName);
	if (! oPairVariableName.first) {
		if (bMandatory) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sVarNameAttrName);
		}
		return std::make_pair(-1, OwnerType::GAME);
	}
	const std::string& sVariableName = oPairVariableName.second;
	const auto oPairIdOwnerType = oCtx.getVariableIdAndOwnerType(sVariableName);
	int32_t nVarId = oPairIdOwnerType.first;
	if (nVarId < 0) {
		if (sVariableName.substr(0,2) == "__") {
			throw XmlCommonErrors::error(oCtx, p0Element, sVarNameAttrName
										, Util::stringCompose("System variables (%1) are not allowed", sVariableName));
		}
		throw XmlCommonErrors::errorAttrVariableNotDefined(oCtx, p0Element, sVarNameAttrName, sVariableName);
	}
	const int32_t nTeam = oCtx.getContextTeam();
	const int32_t nMate = oCtx.getContextMate();
	// the variable name was found, let's see if the context suffices
	OwnerType eVarOwnerType = oPairIdOwnerType.second;
	if (nTeam < 0) {
		// no team is defined, must be a game variable
		assert(nMate < 0);
		if (eVarOwnerType != OwnerType::GAME) {
			throw XmlCommonErrors::error(oCtx, p0Element, sVarNameAttrName
										, Util::stringCompose("Context for variable %1 insufficient: try defining team", sVariableName));
		}
	} else if (nMate < 0) {
		// team defined, but not mate
		if (eVarOwnerType == OwnerType::PLAYER) {
			throw XmlCommonErrors::error(oCtx, p0Element, sVarNameAttrName
										, Util::stringCompose("Context for variable %1 insufficient: mate must be defined", sVariableName));
		}
	} else {
		// mate defined, any variable has sufficient context
	}
	return std::make_pair(nVarId, eVarOwnerType);
}
std::vector<shared_ptr<GameWidget>> XmlLayoutParser::parseChildWidgets(LayoutCtx& oCtx, const xmlpp::Element* p0Parent)
{
	return parseChildWidgets(oCtx, p0Parent, std::vector<std::string const*>{});
}
std::vector<shared_ptr<GameWidget>> XmlLayoutParser::parseChildWidgets(LayoutCtx& oCtx, const xmlpp::Element* p0Parent
																		, const std::vector<std::string const*>& aSkipChildNames)
{
//std::cout << "XmlLayoutParser::parseChildWidgets()" << '\n';
	oCtx.addChecker(p0Parent);
	std::vector<shared_ptr<GameWidget>> aPack;
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Parent, [&](const xmlpp::Element* p0WidgetElement)
	{
		const std::string sChildName = p0WidgetElement->get_name();
		auto itFind = std::find_if(aSkipChildNames.begin(), aSkipChildNames.end(), [&](std::string const* p0ChildName)
		{
			return (sChildName == *p0ChildName);
		});
		if (itFind != aSkipChildNames.end()) {
//std::cout << "XmlLayoutParser::parseChildWidgets() skip sChildName=" << sChildName << '\n';
			oCtx.addValidChildElementName(p0Parent, sChildName);
		} else {
			shared_ptr<GameWidget> refWidget = parseWidget(oCtx, p0WidgetElement);
			aPack.push_back(refWidget);
		}
	});
	oCtx.removeChecker(p0Parent, false);
	return aPack;
}
shared_ptr<GameWidget> XmlLayoutParser::parseWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlLayoutParser::parseWidget() ======================= " << p0WidgetElement->get_name() << '\n';
	oCtx.addChecker(p0WidgetElement);
	const int32_t nParentTeam = oCtx.getContextTeam();
	const int32_t nParentMate = oCtx.getContextMate();
	const auto oPairTeamMate = m_oXmlConditionalParser.parseOwner(oCtx, p0WidgetElement);
	oCtx.setTeam(oPairTeamMate.first);
	oCtx.setMate(oPairTeamMate.second);
	//
	shared_ptr<GameWidget> refWidget;
	const std::string sName = p0WidgetElement->get_name();
//std::cout << "XmlLayoutParser::parseWidget() sWidgetName=" << sName << "  nTeam=" << oCtx.getContextTeam() << "  nMate=" << oCtx.getContextMate() << '\n';
	if (sName == s_sGameWidgetsGroupNodeName) {
		//
		throw XmlCommonErrors::error(oCtx, p0WidgetElement, Util::s_sEmptyString, "Layout: <Widgets/> not implemented yet");
	}
	for (auto& refXmlGameWidgetParser : m_aXmlGameWidgetParsers) {
		if (refXmlGameWidgetParser->getGameWidgetName() == sName) {
			refWidget = refXmlGameWidgetParser->parseGameWidget(oCtx, p0WidgetElement);
			if (refWidget) {
				break;
			}
		}
	}
	if (!refWidget) {
		throw XmlCommonErrors::errorElementInvalid(oCtx, p0WidgetElement, sName);
	}

	oCtx.setTeam(nParentTeam);
	oCtx.setMate(nParentMate);
	oCtx.removeChecker(p0WidgetElement, false);
	return refWidget;
}

void XmlLayoutParser::parseGameWidget(GameWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlLayoutParser::parseGameWidget" << '\n';
	oCtx.addChecker(p0WidgetElement);
	oInit.m_nTeam = oCtx.getContextTeam();
	oInit.m_nMate = oCtx.getContextMate();
	//
	const auto oPairName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0WidgetElement, s_sGameWidgetNameAttr);
	if (oPairName.first) {
		oInit.m_sName = oPairName.second;
		XmlCommonParser::validateName(oCtx, p0WidgetElement, s_sGameWidgetNameAttr, oInit.m_sName, false);
	} else {
		assert(oInit.m_sName.empty());
	}

	const auto oPairType = m_oXmlConditionalParser.getAttributeValue(oCtx, p0WidgetElement, s_sGameWidgetViewNameAttr);
	if (oPairType.first) {
		const std::string& sType = oPairType.second;
		oInit.m_nViewWidgetNameIdx = oCtx.named().widgets().addName(sType);
	} else {
		assert(oInit.m_nViewWidgetNameIdx == -1);
	}

	static const std::vector<char const *> s_aAlignEnumString{s_sGameWidgetAlignAttrFill.c_str(), s_sGameWidgetAlignAttrStart.c_str(), s_sGameWidgetAlignAttrCenter.c_str(), s_sGameWidgetAlignAttrEnd.c_str()};
	static const std::vector<WidgetAxisFit::ALIGN> s_aAlignEnumValue{WidgetAxisFit::ALIGN_FILL, WidgetAxisFit::ALIGN_START, WidgetAxisFit::ALIGN_CENTER, WidgetAxisFit::ALIGN_END};
	//
	m_oXmlConditionalParser.visitLayoutConfigNamedEnumAttributes(oCtx, p0WidgetElement, s_sGameWidgetAlignHorizAttr, g_nTotLayoutConfigs, s_aAlignEnumString
																, [&](const std::string& /*sLCAttrName*/, int32_t nEnum, int32_t nLayoutConfig)
	{
		const WidgetAxisFit::ALIGN eAlign = s_aAlignEnumValue[nEnum];
		if (nLayoutConfig < 0) {
			oInit.setHorizAlign(eAlign);
		} else {
			oInit.m_aHorizAxisFit[nLayoutConfig].m_eAlign = eAlign;
		}
	});
	m_oXmlConditionalParser.visitLayoutConfigNamedEnumAttributes(oCtx, p0WidgetElement, s_sGameWidgetAlignVertAttr, g_nTotLayoutConfigs, s_aAlignEnumString
																, [&](const std::string& /*sLCAttrName*/, int32_t nEnum, int32_t nLayoutConfig)
	{
		const WidgetAxisFit::ALIGN eAlign = s_aAlignEnumValue[nEnum];
		if (nLayoutConfig < 0) {
			oInit.setVertAlign(eAlign);
		} else {
			oInit.m_aVertAxisFit[nLayoutConfig].m_eAlign = eAlign;
		}
	});
//std::cout << "  XmlLayoutParser::parseGameWidget() nIdxH=" << nIdxH << "  nIdxV=" << nIdxV << '\n';
	//
	m_oXmlConditionalParser.visitLayoutConfigNamedAttributes(oCtx, p0WidgetElement, s_sGameWidgetExpandHorizAttr, g_nTotLayoutConfigs
																, [&](const std::string& sLCAttrName, const std::string& sValue, int32_t nLayoutConfig)
	{
		const bool bExpand = XmlUtil::strToBool(oCtx, p0WidgetElement, sLCAttrName, sValue);
		if (nLayoutConfig < 0) {
			oInit.setHorizExpand(bExpand);
		} else {
			oInit.m_aHorizAxisFit[nLayoutConfig].m_bExpand = bExpand;
		}
	});
	m_oXmlConditionalParser.visitLayoutConfigNamedAttributes(oCtx, p0WidgetElement, s_sGameWidgetExpandVertAttr, g_nTotLayoutConfigs
																, [&](const std::string& sLCAttrName, const std::string& sValue, int32_t nLayoutConfig)
	{
		const bool bExpand = XmlUtil::strToBool(oCtx, p0WidgetElement, sLCAttrName, sValue);
		if (nLayoutConfig < 0) {
			oInit.setVertExpand(bExpand);
		} else {
			oInit.m_aVertAxisFit[nLayoutConfig].m_bExpand = bExpand;
		}
	});
	oCtx.removeChecker(p0WidgetElement, false);
}
void XmlLayoutParser::parseContainerWidget(ContainerWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
	parseContainerWidget(oInit, oCtx, p0WidgetElement, std::vector<std::string const*>{});
}
void XmlLayoutParser::parseContainerWidget(ContainerWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement
											, const std::vector<std::string const*>& aSkipChildNames)
{
	oCtx.addChecker(p0WidgetElement);
	parseGameWidget(oInit, oCtx, p0WidgetElement);

	oInit.m_aChildWidgets = parseChildWidgets(oCtx, p0WidgetElement, aSkipChildNames);

	m_oXmlConditionalParser.visitLayoutConfigNamedAttributes(oCtx, p0WidgetElement, s_sContainerWidgetExpandHorizIfChildAttr, g_nTotLayoutConfigs
																, [&](const std::string& sLCAttrName, const std::string& sValue, int32_t nLayoutConfig)
	{
		const int32_t nFromLayoutConfig = ((nLayoutConfig < 0) ? 0 : nLayoutConfig);
		const int32_t nToLayoutConfig = ((nLayoutConfig < 0) ? g_nTotLayoutConfigs - 1 : nLayoutConfig);
		for (int32_t nCurLayoutConfig = nFromLayoutConfig; nCurLayoutConfig <= nToLayoutConfig; ++nCurLayoutConfig) {
			if (! oInit.m_aHorizAxisFit[nCurLayoutConfig].m_bExpand) {
				const bool bExpandIfChild = XmlUtil::strToBool(oCtx, p0WidgetElement, sLCAttrName, sValue);
				if (bExpandIfChild) {
					oInit.m_aHorizAxisFit[nCurLayoutConfig].m_bExpand = ContainerWidget::getChildExpand(oInit.m_aChildWidgets, nCurLayoutConfig, true);
				}
			}
		}
	});
	m_oXmlConditionalParser.visitLayoutConfigNamedAttributes(oCtx, p0WidgetElement, s_sContainerWidgetExpandVertIfChildAttr, g_nTotLayoutConfigs
																, [&](const std::string& sLCAttrName, const std::string& sValue, int32_t nLayoutConfig)
	{
		const int32_t nFromLayoutConfig = ((nLayoutConfig < 0) ? 0 : nLayoutConfig);
		const int32_t nToLayoutConfig = ((nLayoutConfig < 0) ? g_nTotLayoutConfigs - 1 : nLayoutConfig);
		for (int32_t nCurLayoutConfig = nFromLayoutConfig; nCurLayoutConfig <= nToLayoutConfig; ++nCurLayoutConfig) {
			if (! oInit.m_aVertAxisFit[nCurLayoutConfig].m_bExpand) {
				const bool bExpandIfChild = XmlUtil::strToBool(oCtx, p0WidgetElement, sLCAttrName, sValue);
				if (bExpandIfChild) {
					oInit.m_aVertAxisFit[nCurLayoutConfig].m_bExpand = ContainerWidget::getChildExpand(oInit.m_aChildWidgets, nCurLayoutConfig, false);
				}
			}
		}
	});
	m_oXmlConditionalParser.visitLayoutConfigNamedAttributes(oCtx, p0WidgetElement, s_sContainerWidgetFillHorizIfChildAttr, g_nTotLayoutConfigs
																, [&](const std::string& sLCAttrName, const std::string& sValue, int32_t nLayoutConfig)
	{
		const int32_t nFromLayoutConfig = ((nLayoutConfig < 0) ? 0 : nLayoutConfig);
		const int32_t nToLayoutConfig = ((nLayoutConfig < 0) ? g_nTotLayoutConfigs - 1 : nLayoutConfig);
		for (int32_t nCurLayoutConfig = nFromLayoutConfig; nCurLayoutConfig <= nToLayoutConfig; ++nCurLayoutConfig) {
			if (! (oInit.m_aHorizAxisFit[nCurLayoutConfig].m_eAlign == WidgetAxisFit::ALIGN_FILL)) {
				const bool bFillIfChild = XmlUtil::strToBool(oCtx, p0WidgetElement, sLCAttrName, sValue);
				if (bFillIfChild) {
					const bool bFill = ContainerWidget::getChildFill(oInit.m_aChildWidgets, nCurLayoutConfig, true);
					if (bFill) {
						oInit.m_aHorizAxisFit[nCurLayoutConfig].m_eAlign = WidgetAxisFit::ALIGN_FILL;
					}
				}
			}
		}
	});
	m_oXmlConditionalParser.visitLayoutConfigNamedAttributes(oCtx, p0WidgetElement, s_sContainerWidgetFillVertIfChildAttr, g_nTotLayoutConfigs
																, [&](const std::string& sLCAttrName, const std::string& sValue, int32_t nLayoutConfig)
	{
		const int32_t nFromLayoutConfig = ((nLayoutConfig < 0) ? 0 : nLayoutConfig);
		const int32_t nToLayoutConfig = ((nLayoutConfig < 0) ? g_nTotLayoutConfigs - 1 : nLayoutConfig);
		for (int32_t nCurLayoutConfig = nFromLayoutConfig; nCurLayoutConfig <= nToLayoutConfig; ++nCurLayoutConfig) {
			if (! (oInit.m_aVertAxisFit[nCurLayoutConfig].m_eAlign == WidgetAxisFit::ALIGN_FILL)) {
				const bool bFillIfChild = XmlUtil::strToBool(oCtx, p0WidgetElement, sLCAttrName, sValue);
				if (bFillIfChild) {
					const bool bFill = ContainerWidget::getChildFill(oInit.m_aChildWidgets, nCurLayoutConfig, false);
					if (bFill) {
						oInit.m_aVertAxisFit[nCurLayoutConfig].m_eAlign = WidgetAxisFit::ALIGN_FILL;
					}
				}
			}
		}
	});
	oCtx.removeChecker(p0WidgetElement, false);
}
void XmlLayoutParser::parseRelSizedGameWidget(RelSizedGameWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlLayoutParser::parseRelSizedGameWidget" << '\n';
	oCtx.addChecker(p0WidgetElement);
	parseGameWidget(oInit, oCtx, p0WidgetElement);
	//
	int32_t nTotDefined = 0;
	const auto oPairReferenceFactorW = m_oXmlConditionalParser.getAttributeValue(oCtx, p0WidgetElement, s_sGameWidgetReferenceWFactorAttr);
	if (oPairReferenceFactorW.first) {
		const std::string& sReferenceFactorW = oPairReferenceFactorW.second;
		oInit.m_fReferenceWFactor = XmlUtil::strToNumber<double>(oCtx, p0WidgetElement, s_sGameWidgetReferenceWFactorAttr, sReferenceFactorW
																		, true, true, 0, false, -1);
		++nTotDefined;
	} else {
		assert(oInit.m_fReferenceWFactor == -1.0);
	}
	const auto oPairReferenceFactorH = m_oXmlConditionalParser.getAttributeValue(oCtx, p0WidgetElement, s_sGameWidgetReferenceHFactorAttr);
	if (oPairReferenceFactorH.first) {
		const std::string& sReferenceFactorH = oPairReferenceFactorH.second;
		oInit.m_fReferenceHFactor = XmlUtil::strToNumber<double>(oCtx, p0WidgetElement, s_sGameWidgetReferenceHFactorAttr, sReferenceFactorH
																		, true, true, 0, false, -1);
		++nTotDefined;
	} else {
		assert(oInit.m_fReferenceHFactor == -1.0);
	}
	const auto oPairWHRatio = m_oXmlConditionalParser.getAttributeValue(oCtx, p0WidgetElement, s_sGameWidgetWHAttr);
	if (oPairWHRatio.first) {
		const std::string& sWHRatio = oPairWHRatio.second;
		oInit.m_fWHRatio = XmlUtil::strToNumber<double>(oCtx, p0WidgetElement, s_sGameWidgetWHAttr, sWHRatio
												, true, true, 0, false, -1);
		++nTotDefined;
	} else {
		assert(oInit.m_fWHRatio == -1.0);
	}
	if (nTotDefined != 2) {
		throw XmlCommonErrors::error(oCtx, p0WidgetElement, Util::s_sEmptyString, Util::stringCompose(
										"Exactly two of attributes '%1', '%2', '%3' must be defined."
										, s_sGameWidgetReferenceWFactorAttr, s_sGameWidgetReferenceHFactorAttr, s_sGameWidgetWHAttr));
	}
	oCtx.removeChecker(p0WidgetElement, false);
}

} // namespace stmg
