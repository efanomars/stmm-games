/*
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   testTile.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "tile.h"

#include <cassert>
#include <iostream>


namespace stmg
{

namespace testing
{

TEST_CASE("testTile, Constructor")
{
	Tile oTile;
	REQUIRE( oTile.isEmpty() );
}

TEST_CASE("testTile, ColorEmptyAndType")
{
	TileColor oColor;
	REQUIRE( oColor.isEmpty() );
	oColor.setColorIndex(22);
	REQUIRE( !oColor.isEmpty() );
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_INDEX );
	oColor.clear();
	REQUIRE( oColor.isEmpty() );
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_EMPTY );
	oColor.setColorPal(TileColor::COLOR_PAL_STD_CYAN);
	REQUIRE( !oColor.isEmpty() );
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_PAL );
	oColor.setColorRGB(2,3,4);
	REQUIRE( !oColor.isEmpty() );
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_RGB );
	oColor.clear();
	REQUIRE( oColor.isEmpty() );
}
TEST_CASE("testTile, ColorIndex")
{
	TileColor oColor;
	oColor.setColorIndex(22);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_INDEX );
	REQUIRE( oColor.getColorIndex() == 22 );

	oColor.setColorIndex(TileColor::COLOR_INDEX_MIN);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_INDEX );
	REQUIRE( oColor.getColorIndex() == TileColor::COLOR_INDEX_MIN );

	oColor.setColorIndex(TileColor::COLOR_INDEX_MAX);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_INDEX );
	REQUIRE( oColor.getColorIndex() == TileColor::COLOR_INDEX_MAX );
}
TEST_CASE("testTile, ColorPal")
{
	TileColor oColor;
	oColor.setColorPal(1278);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_PAL );
	REQUIRE( oColor.getColorPal() == 1278 );

	oColor.setColorPal(TileColor::COLOR_PAL_FIRST);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_PAL );
	REQUIRE( oColor.getColorPal() == TileColor::COLOR_PAL_FIRST );

	oColor.setColorPal(TileColor::COLOR_PAL_LAST);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_PAL );
	REQUIRE( oColor.getColorPal() == TileColor::COLOR_PAL_LAST );
}
TEST_CASE("testTile, ColorRgb")
{
	TileColor oColor;
	oColor.setColorRGB(5, 76, 237);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_RGB );
	uint8_t nR, nG, nB;
	oColor.getColorRGB(nR, nG, nB);
	REQUIRE(( (nR == 5) && (nG == 76) && (nB == 237) ));

	oColor.setColorRGB(255,255,255);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_RGB );
	REQUIRE( oColor.getColorRed1() == 1.0 );
	REQUIRE( oColor.getColorGreen1() == 1.0 );
	REQUIRE( oColor.getColorBlue1() == 1.0 );

	oColor.setColorRGB(0,0,0);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_RGB );
	REQUIRE( oColor.getColorRed1() == 0.0 );
	REQUIRE( oColor.getColorGreen1() == 0.0 );
	REQUIRE( oColor.getColorBlue1() == 0.0 );

	oColor.setColorRGB1(1.0, 1.0, 1.0);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_RGB );
	REQUIRE( oColor.getColorRed1() == 1.0 );
	REQUIRE( oColor.getColorGreen1() == 1.0 );
	REQUIRE( oColor.getColorBlue1() == 1.0 );
	oColor.getColorRGB(nR, nG, nB);
	REQUIRE(( (nR == 255) && (nG == 255) && (nB == 255) ));

	oColor.setColorRGB1(0.0, 0.0, 0.0);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_RGB );
	REQUIRE( oColor.getColorRed1() == 0.0 );
	REQUIRE( oColor.getColorGreen1() == 0.0 );
	REQUIRE( oColor.getColorBlue1() == 0.0 );
	oColor.getColorRGB(nR, nG, nB);
	REQUIRE(( (nR == 0) && (nG == 0) && (nB == 0) ));

	oColor.setColorRGB1(0.5, 0.5, 0.5);
	REQUIRE( oColor.getColorType() == TileColor::COLOR_TYPE_RGB );
	const double fR = oColor.getColorRed1();
	const double fG = oColor.getColorGreen1();
	const double fB = oColor.getColorBlue1();
	REQUIRE(( (fR >= 0.49) && (fR <= 0.51) ));
	REQUIRE(( (fG >= 0.49) && (fG <= 0.51) ));
	REQUIRE(( (fB >= 0.49) && (fB <= 0.51) ));
	oColor.getColorRGB(nR, nG, nB);
	REQUIRE(( (nR == 127) || (nR == 128) ));
	REQUIRE(( (nG == 127) || (nG == 128) ));
	REQUIRE(( (nB == 127) || (nB == 128) ));
}

TEST_CASE("testTile, CharEmptyAndType")
{
	TileChar oChar;
	REQUIRE( oChar.isEmpty() );
	oChar.setChar(22);
	REQUIRE( !oChar.isEmpty() );
	REQUIRE( !oChar.isCharIndex() );
	oChar.clear();
	REQUIRE( oChar.isEmpty() );
	oChar.setCharIndex(4324);
	REQUIRE( !oChar.isEmpty() );
	REQUIRE( oChar.isCharIndex() );
	oChar.clear();
	REQUIRE( oChar.isEmpty() );
}
TEST_CASE("testTile, CharCode")
{
	TileChar oChar;
	oChar.setChar(7767);
	REQUIRE( !oChar.isCharIndex() );
	REQUIRE( oChar.getChar() == 7767 );
	oChar.setChar(TileChar::CHAR_UCS4_MIN);
	REQUIRE( !oChar.isCharIndex() );
	REQUIRE( oChar.getChar() == TileChar::CHAR_UCS4_MIN );
	oChar.setChar(TileChar::CHAR_UCS4_MAX);
	REQUIRE( !oChar.isCharIndex() );
	REQUIRE( oChar.getChar() == TileChar::CHAR_UCS4_MAX );
}
TEST_CASE("testTile, CharIndex")
{
	TileChar oChar;
	oChar.setCharIndex(76567);
	REQUIRE( oChar.isCharIndex() );
	REQUIRE( oChar.getCharIndex() == 76567 );
	oChar.setCharIndex(TileChar::CHAR_INDEX_MIN);
	REQUIRE( oChar.isCharIndex() );
	REQUIRE( oChar.getCharIndex() == TileChar::CHAR_INDEX_MIN );
	oChar.setCharIndex(TileChar::CHAR_INDEX_MAX);
	REQUIRE( oChar.isCharIndex() );
	REQUIRE( oChar.getCharIndex() == TileChar::CHAR_INDEX_MAX );
}

TEST_CASE("testTile, Alpha")
{
	TileAlpha oAlpha;
	REQUIRE( oAlpha.isEmpty() );
	oAlpha.clear();
	REQUIRE( oAlpha.isEmpty() );
	oAlpha.setAlpha(77);
	REQUIRE( !oAlpha.isEmpty() );
	REQUIRE( !oAlpha.isOpaque() );
	REQUIRE( oAlpha.getAlpha() == 77 );
	oAlpha.clear();
	REQUIRE( oAlpha.isEmpty() );
}
TEST_CASE("testTile, Font")
{
	TileFont oFont;
	REQUIRE( oFont.isEmpty() );
	oFont.clear();
	REQUIRE( oFont.isEmpty() );
	oFont.setFontIndex(44);
	REQUIRE_FALSE( oFont.isEmpty() );
	REQUIRE( oFont.getFontIndex() == 44 );
	oFont.clear();
	REQUIRE( oFont.isEmpty() );
	oFont.setFontIndex(TileFont::CHAR_FONT_INDEX_MIN);
	REQUIRE_FALSE( oFont.isEmpty() );
	REQUIRE( oFont.getFontIndex() == TileFont::CHAR_FONT_INDEX_MIN );
	oFont.setFontIndex(TileFont::CHAR_FONT_INDEX_MAX);
	REQUIRE_FALSE( oFont.isEmpty() );
	REQUIRE( oFont.getFontIndex() == TileFont::CHAR_FONT_INDEX_MAX );
}
TEST_CASE("testTile, Tile")
{
	Tile oTile;
	REQUIRE( oTile.isEmpty() );
	oTile.clear();
	REQUIRE( oTile.isEmpty() );
	Tile oTile2;
	REQUIRE( oTile == oTile2 );

	oTile.getTileColor().setColorIndex(0);
	REQUIRE( !oTile.isEmpty() );
	oTile.clear();
	REQUIRE( oTile.isEmpty() );

	oTile.getTileChar().setChar(TileChar::CHAR_UCS4_MIN);
	REQUIRE( !oTile.isEmpty() );
	oTile.clear();
	REQUIRE( oTile.isEmpty() );

	oTile.getTileFont().setFontIndex(0);
	REQUIRE( !oTile.isEmpty() );
	oTile.clear();
	REQUIRE( oTile.isEmpty() );

	oTile.getTileAlpha().setAlpha(0);
	REQUIRE( !oTile.isEmpty() );
	oTile.clear();
	REQUIRE( oTile.isEmpty() );

	oTile.getTileColor().setColorIndex(0);
	oTile2.getTileColor().setColorIndex(1);
	REQUIRE( ! (oTile == oTile2) );
	oTile2.getTileColor().setColorIndex(0);
	REQUIRE( oTile == oTile2 );

	oTile.clear();
	oTile2.clear();
	oTile.getTileColor().setColorIndex(0);
	oTile2.getTileColor().setColorIndex(1);
	REQUIRE( ! (oTile == oTile2) );
	oTile2.getTileColor().setColorIndex(0);
	REQUIRE( oTile == oTile2 );

	oTile.clear();
	oTile2.clear();
	oTile.getTileChar().setChar(65);
	oTile2.getTileChar().setChar(121);
	REQUIRE( ! (oTile == oTile2) );
	oTile.getTileChar().setChar(121);
	REQUIRE( oTile == oTile2 );

	oTile.clear();
	oTile2.clear();
	oTile.getTileFont().setFontIndex(1);
	oTile2.getTileFont().setFontIndex(2);
	REQUIRE( ! (oTile == oTile2) );
	oTile.getTileFont().setFontIndex(2);
	REQUIRE( oTile == oTile2 );

	oTile.clear();
	oTile2.clear();
	oTile.getTileAlpha().setAlpha(1);
	oTile2.getTileAlpha().setAlpha(2);
	REQUIRE( ! (oTile == oTile2) );
	oTile.getTileAlpha().setAlpha(2);
	REQUIRE( oTile == oTile2 );
}

} // namespace testing

} // namespace stmg
