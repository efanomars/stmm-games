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
 * File:   xmlmodifierparser.h
 */

#ifndef STMG_XML_MODIFIER_PARSER_H
#define STMG_XML_MODIFIER_PARSER_H

#include <vector>
#include <memory>
#include <string>
#include <tuple>

#include <stdint.h>

namespace stmg { class Image; }
namespace stmg { class StdThemeModifier; }
namespace stmg { class ThemeCtx; }
namespace stmg { class TileAni; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

class XmlConditionalParser;
class XmlTraitsParser;
class XmlThemeImageParser;

class XmlThemeModifiersParser;

class XmlModifierParser
{
public:
	virtual ~XmlModifierParser() = default;
	/** Constructor.
	 * @param sModifierName The modifier name. Cannot be empty.
	 */
	XmlModifierParser(const std::string& sModifierName);

	/** The modifier name.
	 * @return The modifier name. Is not empty.
	 */
	const std::string& getModifierName() const;

	/** Parse a tile modifier element.
	 * @param oCtx The theme context.
	 * @param p0Element The element. Cannot be null.
	 * @return The created modifier. Is not null.
	 * @throws std::runtime_error.
	 */
	virtual unique_ptr<StdThemeModifier> parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element) = 0;

protected:
	friend class XmlThemeModifiersParser;
	/** Parse all children as modifiers.
	 * Use this if the child Elements are all Modifiers, if one isn't an error is thrown.
	 * Alternatively use parseSubModifier(), to parse them one by one.
	 * @param oCtx The theme context.
	 * @param p0ParentElement  The parent element. Cannot be null.
	 * @return The created submodifiers. All values are not null.
	 * @throws std::runtime_error.
	 */
	std::vector< unique_ptr<StdThemeModifier> > parseSubModifiers(ThemeCtx& oCtx, const xmlpp::Element* p0ParentElement);
	/** Parse all children as modifiers.
	 * Use this if the child Elements are all Modifiers, if one isn't an error is thrown.
	 * Alternatively use parseSubModifier(), to parse them one by one.
	 * @param oCtx The theme context.
	 * @param p0ParentElement  The parent element. Cannot be null.
	 * @param aSkipChildNames  The child names that should be ignored.
	 * @return The created submodifiers. All values are not null.
	 * @throws std::runtime_error.
	 */
	std::vector< unique_ptr<StdThemeModifier> > parseSubModifiers(ThemeCtx& oCtx, const xmlpp::Element* p0ParentElement
																, const std::vector<std::string const*>& aSkipChildNames);
	/** Parses a submodifier.
	 * If the element is not a modifier it throws.
	 * @param oCtx The theme context.
	 * @param p0ModifierElement The submodifier element. Cannot be null.
	 * @return The parsed submodifier. Is not null.
	 * @throws std::runtime_error.
	 */
	unique_ptr<StdThemeModifier> parseSubModifier(ThemeCtx& oCtx, const xmlpp::Element* p0ModifierElement);

	/** Parse tile ani id.
	 * @param oCtx The theme context.
	 * @param p0Element The element. Cannot be null.
	 * @param bMandatory Whether tile ani id must be defined.
	 * @return The tile ani or null if not defined.
	 */
	shared_ptr<TileAni> parseModifierTileAniId(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory);
	/** Parse tile ani name.
	 * @param oCtx The theme context.
	 * @param p0Element The element. Cannot be null.
	 * @param bMandatory Whether tile ani name must be defined.
	 * @return The index into Named::tileAnis() or -1 if not defined.
	 */
	int32_t parseModifierTileAniName(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory);
	/** Parse elapsed default.
	 * If not defined as attribute default is -1.0.
	 * @param oCtx The theme context.
	 * @param p0Element The element. Cannot be null.
	 * @return The elapsed default (0.0 to 1.0) or -1.0 if not defined.
	 */
	double parseModifierElapsedDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	/** Parse invert.
	 * If not defined as attribute default is false.
	 * @param oCtx The theme context.
	 * @param p0Element The element. Cannot be null.
	 * @return The invert value.
	 */
	bool parseModifierInvert(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	/** Parse tile ani name, default elapsed and invert.
	 * Used by RotateModifier, FadeModifier.
	 * @param oCtx The theme context.
	 * @param p0Element The element. Cannot be null.
	 * @param bMandatory Whether either tile ani name or default elapsed must be defined.
	 * @return Tuple (tileAniIdx, fDefaultElapsed, bInvert).
	 */
	std::tuple<int32_t, double, bool> parseTileAniNameDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory);
	/** Parse tile ani id, name, default image or elapsed and invert.
	 * Used by MaskModifier, TileAniModifier.
	 * @param oCtx The theme context.
	 * @param p0Element The element. Cannot be null.
	 * @return Tuple (tileAniIdx, fDefaultElapsed, bInvert,  refDefaultImage, refTileAni).
	 */
	std::tuple<int32_t, double, bool, shared_ptr<Image>, shared_ptr<TileAni>> parseTileAniIdNameImgDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

	XmlConditionalParser* getXmlConditionalParser();
	XmlTraitsParser* getXmlTraitsParser();
	XmlThemeImageParser* getXmlThemeImageParser();
private:
	const std::string m_sModifierName;
	XmlThemeModifiersParser* m_p1Owner;

private:
	XmlModifierParser(const XmlModifierParser& oSource) = delete;
	XmlModifierParser& operator=(const XmlModifierParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_MODIFIER_PARSER_H */

