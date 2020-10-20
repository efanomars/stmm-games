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
 * File:   xmlgamewidgetparser.cc
 */

#include "xmlgamewidgetparser.h"

#include "xmllayoutparser.h"

#include <stmm-games/containerwidget.h>
#include <stmm-games/relsizedgamewidget.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/ownertype.h>

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace stmg { class ConditionalCtx; }
namespace stmg { class LayoutCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlImageParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

XmlGameWidgetParser::XmlGameWidgetParser(const std::string& sGameWidgetName)
: m_sGameWidgetName(sGameWidgetName)
, m_p1Owner(nullptr)
{
	assert(!sGameWidgetName.empty());
}
XmlConditionalParser& XmlGameWidgetParser::getXmlConditionalParser()
{
	assert(m_p1Owner != nullptr);
	return m_p1Owner->m_oXmlConditionalParser;
}
XmlTraitsParser& XmlGameWidgetParser::getXmlTraitsParser()
{
	assert(m_p1Owner != nullptr);
	return m_p1Owner->m_oXmlTraitsParser;
}
XmlImageParser& XmlGameWidgetParser::getXmlImageParser()
{
	assert(m_p1Owner != nullptr);
	return m_p1Owner->m_oXmlImageParser;
}
const std::string& XmlGameWidgetParser::getGameWidgetName() const
{
	return m_sGameWidgetName;
}
std::vector<shared_ptr<GameWidget>> XmlGameWidgetParser::parseChildWidgets(LayoutCtx& oCtx, const xmlpp::Element* p0Parent)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return std::vector<shared_ptr<GameWidget>>{};
	}
	#endif //NDEBUG
	return m_p1Owner->parseChildWidgets(oCtx, p0Parent);
}
std::vector<shared_ptr<GameWidget>> XmlGameWidgetParser::parseChildWidgets(LayoutCtx& oCtx, const xmlpp::Element* p0Parent
																			, const std::vector<std::string const*>& aSkipChildNames)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return std::vector<shared_ptr<GameWidget>>{};
	}
	#endif //NDEBUG
	return m_p1Owner->parseChildWidgets(oCtx, p0Parent, aSkipChildNames);
}
std::pair<std::string, int32_t> XmlGameWidgetParser::parseEvIdMessage(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory
											, const std::string& sEvIdAttrName, const std::string& sMsgNameAttrName, const std::string& sMsgAttrName)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return std::make_pair("", -1);
	}
	#endif //NDEBUG
	return m_p1Owner->parseEvIdMessage(oCtx, p0Element, bMandatory, sEvIdAttrName, sMsgNameAttrName, sMsgAttrName);
}
std::pair<int32_t, OwnerType> XmlGameWidgetParser::parseVariable(LayoutCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sVarNameAttrName, bool bMandatory)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return std::make_pair(-1, OwnerType::GAME);
	}
	#endif //NDEBUG
	return m_p1Owner->parseVariable(oCtx, p0Element, sVarNameAttrName, bMandatory);
}
void XmlGameWidgetParser::parseGameWidget(GameWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return;
	}
	#endif //NDEBUG
	m_p1Owner->parseGameWidget(oInit, oCtx, p0WidgetElement);
}
void XmlGameWidgetParser::parseRelSizedGameWidget(RelSizedGameWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return;
	}
	#endif //NDEBUG
	m_p1Owner->parseRelSizedGameWidget(oInit, oCtx, p0WidgetElement);
}
void XmlGameWidgetParser::parseContainerWidget(ContainerWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return;
	}
	#endif //NDEBUG
	m_p1Owner->parseContainerWidget(oInit, oCtx, p0WidgetElement);
}
void XmlGameWidgetParser::parseContainerWidget(ContainerWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement
												, const std::vector<std::string const*>& aSkipChildNames)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return;
	}
	#endif //NDEBUG
	m_p1Owner->parseContainerWidget(oInit, oCtx, p0WidgetElement, aSkipChildNames);
}

} // namespace stmg
