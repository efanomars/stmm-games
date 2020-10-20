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
 * File:   xmlthemewidgetparser.h
 */

#ifndef STMG_XML_THEME_WIDGET_PARSER_H
#define STMG_XML_THEME_WIDGET_PARSER_H

#include "xmlthwidgetfactoryparser.h"

#include <stmm-games-gtk/gtkutil/frame.h>
#include <stmm-games-gtk/gtkutil/tilesizing.h>

#include <vector>
#include <memory>
#include <string>

namespace stmg { class Image; }
namespace stmg { class StdTheme; }
namespace stmg { class ThemeCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlThemeImageParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlThemeWidgetParser
{
public:
	XmlThemeWidgetParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser, XmlThemeImageParser& oXmlThemeImageParser);

	void addXmlThWidgetFactoryParser(unique_ptr<XmlThWidgetFactoryParser> refXmlThWidgetFactoryParser);

	void parseTWidgetFactories(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

	//static const std::string s_sTWidgetsFactoryFrameNodeName;
	//static const std::string s_sTWidgetsFactoryTileSizingNodeName;

private:
	void parseTWidgetsFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	Frame parseLayoutFrame(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	TileSizing parseLayoutTileSizing(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

	void parseCommonImage(ThemeCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName
							, StdTheme& oTheme, shared_ptr<Image>& m_refImg);

private:
	friend class XmlThWidgetFactoryParser;
	XmlConditionalParser& m_oXmlConditionalParser;
	XmlTraitsParser& m_oXmlTraitsParser;
	XmlThemeImageParser& m_oXmlThemeImageParser;

	std::vector< unique_ptr<XmlThWidgetFactoryParser> > m_aXmlThWidgetFactoryParsers;

private:
	XmlThemeWidgetParser() = delete;
	XmlThemeWidgetParser(const XmlThemeWidgetParser& oSource) = delete;
	XmlThemeWidgetParser& operator=(const XmlThemeWidgetParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_THEME_WIDGET_PARSER_H */

