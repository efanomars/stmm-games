/*
 * File:   xmlthemeparser.h
 *
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

#ifndef STMG_XML_THEME_PARSER_H
#define STMG_XML_THEME_PARSER_H

#include "xmlthemeimagesparser.h"
#include "xmlthemesoundsparser.h"
#include "xmlthemeaniparser.h"
#include "xmlthemeassignparser.h"
#include "xmlthememodifiersparser.h"
#include "xmlthemewidgetparser.h"
#include "xmlthemeimageparser.h"
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games-gtk/themeloader.h>
#include <stmm-games-xml-base/xmlutil/xmlvariantsetparser.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <vector>
#include <memory>
#include <string>

namespace stmg { class ParserCtx; }
namespace stmg { class ThemeCtx; }
namespace stmg { class XmlThWidgetFactoryParser; }
namespace stmg { class StdThemeModifier; }

namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlModifierParser;
class XmlThAnimationFactoryParser;

class XmlThemeParser
{
public:
	XmlThemeParser();

	void addXmlModifierParser(unique_ptr<XmlModifierParser> refXmlModifierParser);
	void addXmlThAnimationFactoryParser(unique_ptr<XmlThAnimationFactoryParser> refXmlThAnimationFactoryParser);
	void addXmlThWidgetFactoryParser(unique_ptr<XmlThWidgetFactoryParser> refXmlThWidgetFactoryParser);

	bool parseXmlThemeInfo(ParserCtx& oCtx, const xmlpp::Element* p0RootElement
							, std::string& sThemeName, std::vector<std::string>& aExtendsThemes, ThemeLoader::ThemeInfo& oThemeInfo);

	void parseXmlTheme(const std::vector< unique_ptr<ThemeCtx>>& aCtxs);

	XmlConditionalParser& getXmlConditionalParser()	{ return m_oXmlConditionalParser; }
private:
	friend class XmlModifierParser;

	void parseRoot(ParserCtx& oCtx, const xmlpp::Element* p0RootElement, std::string& sThemeName);

	void parseColors(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	void parseColorsColor(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	void parseColorsStdColors(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

	void parseFonts(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	void parseFontsFont(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bDefault);

	bool parseDrawTile(ThemeCtx& oCtx, const xmlpp::Element* p0Element, std::vector< unique_ptr<StdThemeModifier> >& aModifiers);

private:
	XmlVariantSetParser m_oXmlVariantSetParser;
	XmlConditionalParser m_oXmlConditionalParser;
	XmlTraitsParser m_oXmlTraitsParser;
	XmlThemeImageParser m_oXmlThemeImageParser;
	XmlThemeImagesParser m_oXmlThemeImagesParser;
	XmlThemeSoundsParser m_oXmlThemeSoundsParser;
	XmlThemeAniParser m_oXmlThemeAniParser;
	XmlThemeAssignParser m_oXmlThemeAssignParser;
	XmlThemeModifiersParser m_oXmlThemeModifiersParser;
	XmlThemeWidgetParser m_oXmlThemeWidgetParser;

private:
	XmlThemeParser(const XmlThemeParser& oSource) = delete;
	XmlThemeParser& operator=(const XmlThemeParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_THEME_PARSER_H */

