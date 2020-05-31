/*
 * File:   xmltraitsparser.h
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

#ifndef STMG_XML_TRAITS_PARSER_H
#define STMG_XML_TRAITS_PARSER_H

#include <stmm-games/utile/tileselector.h>

#include <memory>
#include <string>

namespace stmg { class AlphaTraitSet; }
namespace stmg { class CharTraitSet; }
namespace stmg { class ColorTraitSet; }
namespace stmg { class ConditionalCtx; }
namespace stmg { class FontTraitSet; }
namespace stmg { class IntSet; }
namespace stmg { class Tile; }
namespace stmg { class TileAlpha; }
namespace stmg { class TileChar; }
namespace stmg { class TileColor; }
namespace stmg { class TileFont; }
namespace stmg { class XmlConditionalParser; }
namespace xmlpp { class Element; }

namespace stmg
{

class XmlTraitsParser
{
public:
	/** Constructor.
	 * @param oXmlConditionalParser The conditional parser to use.
	 */
	XmlTraitsParser(XmlConditionalParser& oXmlConditionalParser)
	: m_oXmlConditionalParser(oXmlConditionalParser)
	{
	}
	XmlConditionalParser& getXmlConditionalParser() { return m_oXmlConditionalParser; }
	/** Parse all the traits of tile within the same element.
	 * If a trait is not defined its value in input/output parameter oTile is
	 * left unchanged, otherwise it is overwritten.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oTile The possibly modified tile.
	 */
	void parseTile(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, Tile& oTile);
	/** Parse the tile color.
	 * If the trait is not defined the input/output value oTileColor is left unchanged.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oTileColor The possibly modified tile color.
	 */
	void parseColor(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileColor& oTileColor);
	/** Parse the tile alpha.
	 * If the trait is not defined the input/output value oTileAlpha is left unchanged.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oTileAlpha The possibly modified tile alpha.
	 */
	void parseAlpha(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileAlpha& oTileAlpha);
	/** Parse the tile char.
	 * If the trait is not defined the input/output value oTileChar is left unchanged.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oTileChar The possibly modified tile char.
	 */
	void parseChar(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileChar& oTileChar);
	/** Parse the tile font.
	 * If the trait is not defined the input/output value oTileFont is left unchanged.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oTileFont The possibly modified tile font.
	 */
	void parseFont(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileFont& oTileFont);

	/** Parse a char trait set.
	 * If the trait is defined the trait set is overwritten otherwise it is left unchanged.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oTraitSet The char trait set.
	 */
	void parseChars(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, CharTraitSet& oTraitSet);
	/** Parse a color trait set.
	 * If the trait is defined the trait set is overwritten otherwise it is left unchanged.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param bAllowRgbFromTo Whether from to of rgb values is allowed.
	 * @param oTraitSet The color trait set.
	 */
	void parseColors(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bAllowRgbFromTo, ColorTraitSet& oTraitSet);
	/** Parse a font trait set.
	 * If the trait is defined the trait set is overwritten otherwise it is left unchanged.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oTraitSet The font trait set.
	 */
	void parseFonts(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, FontTraitSet& oTraitSet);
	/** Parse a alpha trait set.
	 * If the trait is defined the trait set is overwritten otherwise it is left unchanged.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oTraitSet The alpha trait set.
	 */
	void parseAlphas(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, AlphaTraitSet& oTraitSet);
	/** Parse a player trait set.
	 * If the trait is defined the trait set is overwritten otherwise it is left unchanged.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oIntSet The player trait set.
	 */
	void parsePlayers(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, IntSet& oIntSet);

	/** Parses a tile selector with children elements tied by an "or" condition.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @return The tile selector. Is not null.
	 */
	std::unique_ptr<TileSelector> parseTileSelectorOr(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
	/** Parses a tile selector with children elements tied by an "and" condition.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @return The tile selector. Is not null.
	 */
	std::unique_ptr<TileSelector> parseTileSelectorAnd(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
	/** Calls parseTileSelectorOr.
	 */
	inline std::unique_ptr<TileSelector> parseTileSelector(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
	{
		return parseTileSelectorOr(oCtx, p0Element);
	}
private:
	const std::string& parseCharAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
										, const std::string& sCharacterAttr
										, const std::string& sCharAttr
										, const std::string& sCharCodeAttr
										, const std::string& sCharNameAttr
										, TileChar& oTileChar);
	const std::string& parseColorAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
										, const std::string& sColorAttr, const std::string& sColorPalAttr, const std::string& sColorRgbAttr
										, const std::string& sColorNameAttr, TileColor& oTileColor);
	bool parseFontAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileFont& oTileFont);
	bool parseAlphaAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
						, const std::string& sAlphaAttr, TileAlpha& oTileAlpha);
	void parseCharsAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, CharTraitSet& oCharTraitSet
						, const std::string& sAlreadyAttr, const std::string& sAlreadyAttr2);
	void parseMultiChar(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, CharTraitSet& oCharTraitSet);
	void parseColorsAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, ColorTraitSet& oColorTraitSet
						, const std::string& sAlreadyAttr, const std::string& sAlreadyAttr2);
	void parseMultiColor(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
						, bool bAllowRgbFromTo, ColorTraitSet& oColorTraitSet);
	bool parseFontsAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, FontTraitSet& oFontTraitSet
						, const std::string& sAlreadyAttr);
	void parseMultiFont(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, FontTraitSet& oFontTraitSet);
	void parseMultiAlpha(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, AlphaTraitSet& oAlphaTraitSet);
	void parseMultiPlayer(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, IntSet& oIntSet);
	bool parseTileSelectorNotAttr(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool& bNot);
	std::unique_ptr<TileSelector::Trait> parseTileSelectorChar(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
	std::unique_ptr<TileSelector::Trait> parseTileSelectorColor(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
	std::unique_ptr<TileSelector::Trait> parseTileSelectorFont(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
	std::unique_ptr<TileSelector::Trait> parseTileSelectorAlpha(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
																, bool bMandatory);
	std::unique_ptr<TileSelector::Skin> parseTileSelectorPlayer(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
	std::unique_ptr<TileSelector::Operator> parseTileSelectorCond(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
																, TileSelector::Operator::OP_TYPE eOpType);
private:
	XmlConditionalParser& m_oXmlConditionalParser;
private:
	XmlTraitsParser() = delete;
	XmlTraitsParser(const XmlTraitsParser& oSource) = delete;
	XmlTraitsParser& operator=(const XmlTraitsParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_TRAITS_PARSER_H */

