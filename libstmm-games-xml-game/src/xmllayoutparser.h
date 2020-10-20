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
 * File:   xmllayoutparser.h
 */

#ifndef STMG_XML_LAYOUT_PARSER_H
#define STMG_XML_LAYOUT_PARSER_H

#include "xmlgamewidgetparser.h"

#include <stmm-games/containerwidget.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/relsizedgamewidget.h>

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class ConditionalCtx; }
namespace stmg { class GameInitCtx; }
namespace stmg { class Layout; }
namespace stmg { class LayoutCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlImageParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class XmlGameParser;

class XmlLayoutParser
{
public:
	XmlLayoutParser(XmlTraitsParser& oXmlTraitsParser, XmlImageParser& oXmlImageParser);

	void addXmlGameWidgetParser(unique_ptr<XmlGameWidgetParser> refXmlGameWidgetParser);

	XmlTraitsParser& getXmlTraitsParser() { return m_oXmlTraitsParser; }
	//
	static const std::string s_sGameWidgetsNodeName;
	static const std::string s_sGameLayoutNodeName;

	shared_ptr<Layout> parseLayout(GameInitCtx& oCtx, const xmlpp::Element* p0WidgetsElement
									, const xmlpp::Element* p0LayoutElement);
	//
	std::vector<shared_ptr<GameWidget>> parseChildWidgets(LayoutCtx& oCtx, const xmlpp::Element* p0Parent);
	std::vector<shared_ptr<GameWidget>> parseChildWidgets(LayoutCtx& oCtx, const xmlpp::Element* p0Parent
														, const std::vector<std::string const*>& aSkipChildNames);
	std::pair<std::string, int32_t> parseEvIdMessage(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory
													, const std::string& sEvIdAttrName, const std::string& sMsgNameAttrName, const std::string& sMsgAttrName);
	std::pair<int32_t, OwnerType> parseVariable(LayoutCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sVarNameAttrName, bool bMandatorys);
	shared_ptr<GameWidget> parseWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement);

	void parseGameWidget(GameWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement);
	void parseContainerWidget(ContainerWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement);
	void parseContainerWidget(ContainerWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement
							, const std::vector<std::string const*>& aSkipChildNames);
	void parseRelSizedGameWidget(RelSizedGameWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement);
private:
	friend class XmlGameWidgetParser;
	friend class XmlGameParser;
private:
	XmlConditionalParser& m_oXmlConditionalParser;
	XmlTraitsParser& m_oXmlTraitsParser;
	XmlImageParser& m_oXmlImageParser;
	XmlGameParser* m_p0XmlGameParser;

	std::vector< unique_ptr<XmlGameWidgetParser> > m_aXmlGameWidgetParsers;
private:
	XmlLayoutParser() = delete;
	XmlLayoutParser(const XmlLayoutParser& oSource) = delete;
	XmlLayoutParser& operator=(const XmlLayoutParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_LAYOUT_PARSER_H */

