/*
 * File:   textmodifier.cc
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

#include "modifiers/textmodifier.h"
#include "stdtheme.h"

#include "stdthemedrawingcontext.h"

#include <stmm-games/tile.h>
#include <stmm-games/util/basictypes.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <glib.h>

#include <cassert>
#include <utility>

#include <stdint.h>

namespace Pango { class Context; }

namespace stmg
{

TextModifier::TextModifier(StdTheme* p1Owner, Init&& oInit) noexcept
: StdThemeModifier(p1Owner)
, m_p0CacheContext(nullptr)
, m_sText(std::move(oInit.m_sText))
, m_nAddToChar(oInit.m_nAddToChar)
, m_bUseTileColor(oInit.m_bUseTileColor)
, m_bCalcColor(false)
, m_fR1(0.)
, m_fG1(0.)
, m_fB1(0.)
, m_fA1(1.)
, m_fFontSize1(oInit.m_fFontSize1)
, m_bUseTileFont(oInit.m_bUseTileFont)
, m_oFont(oInit.m_oFont)
{
	if ((m_fFontSize1 <= 0.0) || (m_fFontSize1 > 1.0)) {
		m_fFontSize1 = 0.78;
	}
	if (!m_bUseTileColor) {
		TileColor::COLOR_TYPE eType = oInit.m_oColor.getColorType();
		if (eType == TileColor::COLOR_TYPE_EMPTY) {
			m_bCalcColor = true;
		} else {
			uint8_t nR, nG, nB;
			owner()->getColorRgb(oInit.m_oColor, nR, nG, nB);
			m_fR1 = TileColor::colorUint8ToDouble1(nR);
			m_fG1 = TileColor::colorUint8ToDouble1(nG);
			m_fB1 = TileColor::colorUint8ToDouble1(nB);
		}
	}
}
StdThemeModifier::FLOW_CONTROL TextModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
													, const Tile& oTile, int32_t /*nPlayer*/, const std::vector<double>& /*aAniElapsed*/) noexcept
{
	if (owner() == nullptr) {
		return FLOW_CONTROL_STOP; //--------------------------------------------
	}
	std::string sText;
	if (m_sText.empty()) {
		const TileChar& oChar = oTile.getTileChar();
		if (oChar.isEmpty() || oChar.isCharIndex()) {
			return FLOW_CONTROL_CONTINUE; //------------------------------------
		}
		uint32_t nChar = oChar.getChar();
		if (m_nAddToChar != 0) {
			int64_t n64Char = static_cast<int64_t>(nChar);
			n64Char += m_nAddToChar;
			if (n64Char < TileChar::CHAR_UCS4_MIN) {
				nChar = TileChar::CHAR_UCS4_MIN;
			} else if (n64Char > TileChar::CHAR_UCS4_MAX) {
				nChar = TileChar::CHAR_UCS4_MAX;
			} else {
				nChar = static_cast<uint32_t>(n64Char);
			}
		}
		sText = std::string(1, static_cast<gunichar>(nChar));
	} else {
		sText = m_sText;
	}
	//
	const std::string& sFontDesc = [&]()
	{
		const TileFont& oFont = (m_bUseTileFont ? oTile.getTileFont() : m_oFont);
		if (!oFont.isEmpty()) {
			const int32_t nFontIdx = oFont.getFontIndex();
			assert(nFontIdx >= 0);
			return owner()->getFontDesc(nFontIdx);
		} else {
			return owner()->getDefaultFont();
		}
	}();

	const NSize oSize = oDc.getTileSize();
	const int32_t& nW = oSize.m_nW;
	const int32_t& nH = oSize.m_nH;
	assert(nW > 0);
	assert(nH > 0);
	const Glib::RefPtr<Pango::Context>& refFontContext = oDc.getFontContext();
	assert(refFontContext);
	if (m_p0CacheContext != refFontContext.operator->()) {
		m_p0CacheContext = refFontContext.operator->();
		//
		m_refFontLayout = Pango::Layout::create(refFontContext);
		m_refFont = std::make_unique<Pango::FontDescription>(sFontDesc);
		m_refFontLayout->set_font_description(*(m_refFont));
	}

	refCc->save();

	m_refFontLayout->set_text(sText);
	int32_t nTextW;
	int32_t nTextH;
	m_refFontLayout->get_pixel_size(nTextW, nTextH);

	refCc->translate(1.0 * nW / 2, 1.0 * nH / 2);
	const double fFractionW = 0.74 * m_fFontSize1;
	const double fFractionH = 0.8 * m_fFontSize1;
	refCc->scale(fFractionW * nW / nTextW, fFractionH * nH / nTextH);
	refCc->translate(-1.0 * nTextW / 2, -1.0 * nTextH / 2);

	if (m_bUseTileColor || m_bCalcColor) {
		uint8_t nR, nG, nB;
		owner()->getColorRgb(oTile.getTileColor(), nR, nG, nB);
		const double fR1 = TileColor::colorUint8ToDouble1(nR);
		const double fG1 = TileColor::colorUint8ToDouble1(nG);
		const double fB1 = TileColor::colorUint8ToDouble1(nB);
		const double fA1 = 1.0; // Alpha can be done with Alpher
		if (m_bUseTileColor) {
			assert(!m_bCalcColor);
			refCc->set_source_rgba(fR1, fG1, fB1, fA1);
		} else { // m_bCalcColor
			assert(m_bCalcColor);
			const double dLum = ((fR1 * 299) + (fG1 * 587) + (fB1 * 114)) / 1000;
			if (dLum > 0.45) {
				refCc->set_source_rgba(0.0, 0.0, 0.0, fA1); // black
			} else {
				refCc->set_source_rgba(1.0, 1.0, 1.0, fA1); // white
			}
		}
	} else { // use specific color
		refCc->set_source_rgba(m_fR1, m_fG1, m_fB1, m_fA1);
	}

	m_refFontLayout->show_in_cairo_context(refCc);

	refCc->restore();
	return FLOW_CONTROL_CONTINUE;
}

} // namespace stmg
