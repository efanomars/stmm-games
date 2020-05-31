/*
 * File:   xmlthwidgetfactoryparser.h
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

#ifndef STMG_XML_TH_WIDGET_FACTORY_PARSER_H
#define STMG_XML_TH_WIDGET_FACTORY_PARSER_H

#include <stmm-games-gtk/gtkutil/tilesizing.h>
#include <stmm-games-gtk/gtkutil/frame.h>

#include <memory>
#include <string>

namespace stmg { class StdThemeWidgetFactory; }
namespace stmg { class ThemeCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class XmlThemeWidgetParser;
class XmlThemeImageParser;

/** Base class for ThemeWidget factory parsers.
 */
class XmlThWidgetFactoryParser
{
public:
	virtual ~XmlThWidgetFactoryParser() = default;
	/** Constructor.
	 * @param sFactoryName The factory name. Cannot be empty.
	 */
	XmlThWidgetFactoryParser(const std::string& sFactoryName);

	/** The theme widget factory name.
	 * @return The factory name.
	 */
	const std::string& getFactoryName() const;

	/** Parse the theme widget factory element.
	 * @param oCtx The theme context.
	 * @param p0Element The element. Cannot be null.
	 * @return The factory. Is not null.
	 * @throws std::runtime_error.
	 */
	virtual unique_ptr<StdThemeWidgetFactory> parseWidgetFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element) = 0;
protected:
	XmlConditionalParser* getXmlConditionalParser();
	XmlTraitsParser* getXmlTraitsParser();
	XmlThemeImageParser* getXmlThemeImageParser();

	Frame parseLayoutFrame(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	TileSizing parseLayoutTileSizing(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

private:
	const std::string m_sFactoryName;

	friend class XmlThemeWidgetParser;
	XmlThemeWidgetParser* m_p1Owner;

private:
	XmlThWidgetFactoryParser(const XmlThWidgetFactoryParser& oSource) = delete;
	XmlThWidgetFactoryParser& operator=(const XmlThWidgetFactoryParser& oSource) = delete;
};

} // namespace stmg

#endif /* STMG_XML_TH_WIDGET_FACTORY_PARSER_H */

