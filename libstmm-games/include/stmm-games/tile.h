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
 * File:   tile.h
 */

#ifndef STMG_TILE_H
#define STMG_TILE_H

#include <cassert>
#include <iostream>

#include <stdint.h>

namespace stmg
{

////////////////////////////////////////////////////////////////////////////////
class TileColor
{
public:
	// 32 bit
	// 0         1         2         3
	// 01234567890123456789012345678901
	// --------------------------------
	// 00000000000000000000000000000000 empty
	// cccccccccccccccccccccccc000000tt c:color t:type
	// bbbbbbbbggggggggrrrrrrrr00000001 r:red g:green b:blue (a:alpha)
	// iiiiiiiiiiiiiiiiiiiiiiii00000010 i:index
	// ssssssssssssssssssssssss00000011 s:COLOR_PAL
	//
	enum COLOR_TYPE {
		COLOR_TYPE_EMPTY = 0,
		COLOR_TYPE_INDEX = 1,
		COLOR_TYPE_RGB = 2,
		COLOR_TYPE_PAL = 3
	};
	TileColor() noexcept : m_nColor(0) {}
	inline bool isEmpty() const noexcept
	{
		return m_nColor == 0;
	};
	inline bool operator==(const TileColor& oTileColor) const noexcept
	{
		return m_nColor == oTileColor.m_nColor;
	};
	inline void clear() noexcept
	{
		m_nColor = 0;
	};
	inline COLOR_TYPE getColorType() const noexcept
	{
		return static_cast<COLOR_TYPE>((m_nColor >> 30) & 3);
	};
	enum {
		COLOR_INDEX_MIN = 0,
		COLOR_INDEX_MAX = ((1 << 24) - 1)
	};
	uint32_t getColorIndex() const noexcept
	{
		assert(getColorType() == COLOR_TYPE_INDEX);
		return m_nColor & ((1 << 24) -1);
	};
	void setColorIndex(uint32_t nIdx) noexcept
	{
		assert(nIdx <= COLOR_INDEX_MAX);
		m_nColor = (nIdx & ((1 << 24) -1)) | (COLOR_TYPE_INDEX << 30);
	}
	enum COLOR_PAL {
		COLOR_PAL_FIRST = 0,

		COLOR_PAL_STD_FIRST = 0,
		COLOR_PAL_STD_BLACK = 0,
		COLOR_PAL_STD_WHITE = 1,
		COLOR_PAL_STD_RED = 2,
		COLOR_PAL_STD_GREEN = 3,
		COLOR_PAL_STD_BLUE = 4,
		COLOR_PAL_STD_YELLOW = 5,
		COLOR_PAL_STD_MAGENTA = 6,
		COLOR_PAL_STD_CYAN = 7,
		COLOR_PAL_STD_ORANGE = 8,
		COLOR_PAL_STD_BROWN = 9,
		COLOR_PAL_STD_PINK = 10,
		COLOR_PAL_STD_GRAY = 11,
		COLOR_PAL_STD_LAST = 11,

		COLOR_PAL_LAST = ((1 << 24) - 1)
	};
	uint32_t getColorPal() const noexcept
	{
		assert(getColorType() == COLOR_TYPE_PAL);
		return (m_nColor & ((1 << 24) -1));
	};
	void setColorPal(uint32_t nColor) noexcept
	{
		assert(nColor <= COLOR_PAL_LAST);
		m_nColor = (nColor & ((1 << 24) -1)) | (COLOR_TYPE_PAL << 30);
	}
	void getColorRGB(uint8_t& nR, uint8_t& nG, uint8_t& nB) const noexcept
	{
		assert(getColorType() == COLOR_TYPE_RGB);
		const uint32_t nRGB = m_nColor & ((1 << 24) -1);
		nR = static_cast<uint8_t>((nRGB >> 16) & 255);
		nG = static_cast<uint8_t>((nRGB >> 8) & 255);
		nB = static_cast<uint8_t>(nRGB & 255);
	};
	inline void setColorRGB(uint8_t nR, uint8_t nG, uint8_t nB) noexcept
	{
		m_nColor = (static_cast<uint32_t>(nR) << 16) | (static_cast<uint32_t>(nG) << 8) | static_cast<uint32_t>(nB)
					| (COLOR_TYPE_RGB << 30);
	}
	double getColorRed1() const noexcept
	{
		assert(getColorType() == COLOR_TYPE_RGB);
		const uint32_t nRGB = m_nColor & ((1 << 24) -1);
		const uint8_t nR = static_cast<uint8_t>((nRGB >> 16) & 255);
		return 1.0 * nR / 255;
	};
	double getColorGreen1() const noexcept
	{
		assert(getColorType() == COLOR_TYPE_RGB);
		const uint32_t nRGB = m_nColor & ((1 << 24) -1);
		const uint8_t nG = static_cast<uint8_t>((nRGB >> 8) & 255);
		return 1.0 * nG / 255;
	};
	double getColorBlue1() const noexcept
	{
		assert(getColorType() == COLOR_TYPE_RGB);
		const uint32_t nRGB = m_nColor & ((1 << 24) -1);
		const uint8_t nB = static_cast<uint8_t>(nRGB & 255);
		return 1.0 * nB / 255;
	};
	void setColorRGB1(double fR, double fG, double fB) noexcept
	{
		assert((fR >= 0) && (fR <= 1.0));
		assert((fG >= 0) && (fG <= 1.0));
		assert((fB >= 0) && (fB <= 1.0));
		const uint32_t nR = 0.5 + fR * 255;
		const uint32_t nG = 0.5 + fG * 255;
		const uint32_t nB = 0.5 + fB * 255;
		setColorRGB(nR, nG, nB);
	}
	inline static double colorUint8ToDouble1(uint8_t nCol) noexcept
	{
		return 1.0 * nCol / 255;
	}
	inline  static int32_t rgbToInt32(uint8_t nR, uint8_t nG, uint8_t nB)  noexcept
	{
		const uint32_t nRGB = (nR << 16) | (nG << 8) | nB;
		return nRGB;
	}
	inline static void int32ToRgb(int32_t nRgb, uint8_t& nR, uint8_t& nG, uint8_t& nB) noexcept
	{
		const uint32_t nRGB = nRgb & ((1 << 24) -1);
		nR = static_cast<uint8_t>((nRGB >> 16) & 255);
		nG = static_cast<uint8_t>((nRGB >> 8) & 255);
		nB = static_cast<uint8_t>(nRGB & 255);
	}
	#ifndef NDEBUG
	void dump() const noexcept
	{
		const COLOR_TYPE eType = getColorType();
		if (eType == TileColor::COLOR_TYPE_EMPTY) {
			std::cout << "-";
		} else if (eType == TileColor::COLOR_TYPE_INDEX) {
			std::cout << "i" << getColorIndex();
		} else if (eType == TileColor::COLOR_TYPE_PAL) {
			std::cout << "p" << getColorPal();
		} else {
			if (eType == TileColor::COLOR_TYPE_RGB) {
				uint8_t nR, nG, nB;
				getColorRGB(nR, nG, nB);
				std::cout << "rgb(" << 1u * nR << "," << 1u * nG << "," << 1u * nB << ")";
			} else {
				assert(false);
			}
		}
	}
	#endif //NDEBUG
private:
	uint32_t m_nColor;
};

////////////////////////////////////////////////////////////////////////////////
class TileAlpha
{
public:
	enum {
		ALPHA_MIN = 0, //TODO RENAME to just MAX and MIN!!!
		ALPHA_MAX = 255 /**< 255 corresponds to opaque. */
	};
	TileAlpha() noexcept : m_nAlpha(0), m_nNonEmpty(0) {}
	inline bool isEmpty() const noexcept
	{
		return m_nNonEmpty == 0;
	}
	/** Sets alpha to empty.
	 */
	inline void clear() noexcept
	{
		m_nNonEmpty = 0;
		m_nAlpha = 0;
	}
	/** Whether alpha is ALPHA_MAX.
	 * @return Whether alpha is not empty and opaque.
	 */
	inline bool isOpaque() const noexcept
	{
		return (m_nAlpha == ALPHA_MAX);
	}
	inline bool operator==(const TileAlpha& oTileAlpha) const noexcept
	{
		return (m_nNonEmpty == oTileAlpha.m_nNonEmpty) && (m_nAlpha == oTileAlpha.m_nAlpha);
	}
	/** The current alpha or ALPHA_MAX if empty.
	 * @return The alpha value.
	 */
	inline uint8_t getAlpha() const noexcept
	{
		if (m_nNonEmpty == 0) {
			return ALPHA_MAX; // sort of default
		}
		return m_nAlpha;
	}
	inline void setAlpha(uint8_t nA) noexcept
	{
		m_nNonEmpty = true;
		m_nAlpha = nA;
	}
	double getAlpha1() const noexcept
	{
		return 1.0 * getAlpha() / ALPHA_MAX;
	};
	double setAlpha1(double fA01) noexcept
	{
		assert((fA01 >= 0.0) && (fA01 <= 1.0));
		const uint8_t nAlpha = 0.5 + fA01 * ALPHA_MAX;
		setAlpha(nAlpha);
		return getAlpha1();
	}
	inline static double alphaUint8ToDouble1(uint8_t nAlpha) noexcept
	{
		return 1.0 * nAlpha / 255;
	}
	#ifndef NDEBUG
	void dump() const noexcept
	{
		if (isEmpty()) {
			std::cout << "-";
		} else {
			std::cout << "a" << 1u * getAlpha();
		}
	}
	#endif //NDEBUG
private:
	uint8_t	m_nAlpha; // if m_nNonEmpty == 0 this is 0.
	uint8_t	m_nNonEmpty; // m_nNonEmpty == 0: empty, m_nNonEmpty== 1: non-empty
};

////////////////////////////////////////////////////////////////////////////////
class TileChar
{
public:
	// 32 bit
	// 0         1         2         3
	// 01234567890123456789012345678901
	// --------------------------------
	// 00000000000000000000000000000000 empty (char 0 not allowed!)
	// cccccccccccccccccccccccccccccccC c:char, C:type
	// uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu0 u:UCS-4 (UTF-32)
	// iiiiiiiiiiiiiiiiiiiiiiiiiiiiiii1 i:char index
	//
	TileChar() noexcept : m_nChar(0) {}
	inline bool isEmpty() const noexcept
	{
		return m_nChar == 0;
	};
	inline bool operator==(const TileChar& oTileChar) const noexcept
	{
		return m_nChar == oTileChar.m_nChar;
	};
	inline void clear() noexcept
	{
		m_nChar = 0;
	};
private:
	enum CHAR_TYPE {
		CHAR_TYPE_UCS4 = 0,
		CHAR_TYPE_INDEX = 1 << 31
	};
public:
	enum {
		CHAR_UCS4_MIN = 1,
		CHAR_UCS4_MAX = (1 << 30) + ((1 << 30) - 1)
	};
	enum {
		CHAR_INDEX_MIN = 0,
		CHAR_INDEX_MAX = (1 << 30) + ((1 << 30) - 1)
	};

	inline bool isCharIndex() const noexcept
	{
		return (m_nChar & (1 << 31)) != 0;
	}
	uint32_t getChar() const noexcept
	{
		assert(!isCharIndex());
		return m_nChar & (~(1 << 31));
	}
	void setChar(uint32_t nChar) noexcept
	{
		assert((nChar >= CHAR_UCS4_MIN) && (nChar <= CHAR_UCS4_MAX));
		m_nChar = nChar | CHAR_TYPE_UCS4;
	}
	uint32_t getCharIndex() const noexcept
	{
		assert(isCharIndex());
		return m_nChar & (~(1 << 31));
	}
	void setCharIndex(uint32_t nCharIndex) noexcept
	{
		assert(nCharIndex <= CHAR_INDEX_MAX);
		assert((nCharIndex > CHAR_INDEX_MIN) || (nCharIndex == CHAR_INDEX_MIN));
		m_nChar = nCharIndex | CHAR_TYPE_INDEX;
	}
	#ifndef NDEBUG
	void dump() const noexcept
	{
		if (isEmpty()) {
			std::cout << "-";
		} else if (isCharIndex()) {
			std::cout << "i" << getCharIndex();
		} else {
			std::cout << "u" << getChar();
		}
	}
	#endif //NDEBUG

private:
	uint32_t m_nChar;
};

////////////////////////////////////////////////////////////////////////////////
class TileFont
{
public:
	enum {
		CHAR_FONT_INDEX_MIN = 0,
		CHAR_FONT_INDEX_MAX = ((1 << 8) - 2)
	};
	TileFont() noexcept : m_nFontIdx(0) {}
	inline bool isEmpty() const noexcept
	{
		return m_nFontIdx == 0;
	}
	inline void clear()
	{
		m_nFontIdx = 0;
	}
	inline bool operator==(const TileFont& oTileFont) const noexcept
	{
		return m_nFontIdx == oTileFont.m_nFontIdx;
	}
	inline int32_t getFontIndex() const noexcept
	{
		return m_nFontIdx - 1;
	}
	void setFontIndex(int32_t nFontIndex) noexcept
	{
		assert((nFontIndex >= CHAR_FONT_INDEX_MIN) && (nFontIndex <= CHAR_FONT_INDEX_MAX));
		m_nFontIdx = nFontIndex + 1;
	}
	#ifndef NDEBUG
	void dump() const noexcept
	{
		if (isEmpty()) {
			std::cout << "-";
		} else {
			std::cout << "i" << getFontIndex();
		}
	}
	#endif //NDEBUG
private:
	uint8_t	m_nFontIdx;
};

////////////////////////////////////////////////////////////////////////////////
class Tile : private TileChar, private TileColor, private TileAlpha, private TileFont
{
public:
	// 96 bit = 32 bit Color + 32 bit Char + 16 bit Alpha + 8 bit Font + 8 unused

	Tile() noexcept {};
	inline bool operator==(const Tile& oTile) const noexcept
	{
		return TileChar::operator==(oTile) && TileColor::operator==(oTile)
				&& TileFont::operator==(oTile) && TileAlpha::operator==(oTile);
	}
	inline bool isEmpty() const noexcept
	{
		return TileChar::isEmpty() && TileColor::isEmpty()
				&& TileFont::isEmpty() && TileAlpha::isEmpty();
	}
	inline void clear() noexcept
	{
		static_cast<TileChar&>(*this).clear();
		static_cast<TileColor&>(*this).clear();
		static_cast<TileFont&>(*this).clear();
		static_cast<TileAlpha&>(*this).clear();
	}
	inline const TileColor& getTileColor() const noexcept
	{
		return *this;
	}
	inline TileColor& getTileColor() noexcept
	{
		return *this;
	}
	inline void setTileColor(const TileColor& oTileColor) noexcept
	{
		static_cast<TileColor&>(*this) = oTileColor;
	}
	inline const TileChar& getTileChar() const noexcept
	{
		return *this;
	}
	inline TileChar& getTileChar() noexcept
	{
		return *this;
	}
	inline void setTileChar(const TileChar& oTileChar) noexcept
	{
		static_cast<TileChar&>(*this) = oTileChar;
	}
	inline const TileFont& getTileFont() const noexcept
	{
		return *this;
	}
	inline TileFont& getTileFont() noexcept
	{
		return *this;
	}
	inline void setTileFont(const TileFont& oTileFont) noexcept
	{
		static_cast<TileFont&>(*this) = oTileFont;
	}
	inline const TileAlpha& getTileAlpha() const noexcept
	{
		return *this;
	}
	inline TileAlpha& getTileAlpha() noexcept
	{
		return *this;
	}
	inline void setTileAlpha(const TileAlpha& oTileAlpha) noexcept
	{
		static_cast<TileAlpha&>(*this) = oTileAlpha;
	}
	#ifndef NDEBUG
	void dump() const noexcept
	{
		std::cout << "(";
		TileChar::dump();
		std::cout << ",";
		TileColor::dump();
		std::cout << ",";
		TileAlpha::dump();
		std::cout << ",";
		TileFont::dump();
		std::cout << ")";
	}
	#endif //NDEBUG
public:
	static const Tile s_oEmptyTile;
};


} // namespace stmg

#endif	/* STMG_TILE_H */

