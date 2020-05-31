/*
 * File:   xmlmodifierparser.cc
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

#include "xmlmodifierparser.h"
#include "xmlthememodifiersparser.h"

#include <stmm-games-gtk/stdthememodifier.h>

//#include <cassert>
//#include <iostream>

namespace stmg { class Image; }
namespace stmg { class ThemeCtx; }
namespace stmg { class TileAni; }
namespace xmlpp { class Element; }

namespace stmg
{

XmlModifierParser::XmlModifierParser(const std::string& sModifierName)
: m_sModifierName(sModifierName)
, m_p1Owner(nullptr)
{
}
const std::string& XmlModifierParser::getModifierName() const
{
	return m_sModifierName;
}
std::vector< unique_ptr<StdThemeModifier> > XmlModifierParser::parseSubModifiers(ThemeCtx& oCtx, const xmlpp::Element* p0ParentElement)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return std::vector< unique_ptr<StdThemeModifier> >{};
	}
	#endif //NDEBUG
	return m_p1Owner->parseModifiers(oCtx, p0ParentElement);
}
unique_ptr<StdThemeModifier> XmlModifierParser::parseSubModifier(ThemeCtx& oCtx, const xmlpp::Element* p0ModifierElement)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return unique_ptr<StdThemeModifier>{};
	}
	#endif //NDEBUG
	return m_p1Owner->parseModifier(oCtx, p0ModifierElement);
}
shared_ptr<TileAni> XmlModifierParser::parseModifierTileAniId(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return shared_ptr<TileAni>{};
	}
	#endif //NDEBUG
	return m_p1Owner->parseModifierTileAniId(oCtx, p0Element, bMandatory);
}
int32_t XmlModifierParser::parseModifierTileAniName(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return -1;
	}
	#endif //NDEBUG
	return m_p1Owner->parseModifierTileAniName(oCtx, p0Element, bMandatory);
}
double XmlModifierParser::parseModifierElapsedDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return -1.0;
	}
	#endif //NDEBUG
	return m_p1Owner->parseModifierElapsedDefault(oCtx, p0Element);
}
bool XmlModifierParser::parseModifierInvert(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return false;
	}
	#endif //NDEBUG
	return m_p1Owner->parseModifierInvert(oCtx, p0Element);
}
std::tuple<int32_t, double, bool> XmlModifierParser::parseTileAniNameDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return std::make_tuple(-1, -1.0, false);
	}
	#endif //NDEBUG
	return m_p1Owner->parseTileAniNameDefault(oCtx, p0Element, bMandatory);
}
std::tuple<int32_t, double, bool, shared_ptr<Image>, shared_ptr<TileAni>> XmlModifierParser::parseTileAniIdNameImgDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return std::make_tuple(-1, -1.0, false, shared_ptr<Image>{}, shared_ptr<TileAni>{});
	}
	#endif //NDEBUG
	return m_p1Owner->parseTileAniIdNameImgDefault(oCtx, p0Element);
}

XmlConditionalParser* XmlModifierParser::getXmlConditionalParser()
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return nullptr;
	}
	#endif //NDEBUG
	return &(m_p1Owner->m_oXmlConditionalParser);
}
XmlTraitsParser* XmlModifierParser::getXmlTraitsParser()
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return nullptr;
	}
	#endif //NDEBUG
	return &(m_p1Owner->m_oXmlTraitsParser);
}
XmlThemeImageParser* XmlModifierParser::getXmlThemeImageParser()
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return nullptr;
	}
	#endif //NDEBUG
	return &(m_p1Owner->m_oXmlThemeImageParser);
}

} // namespace stmg
