/*
 * File:   textmodifier.h
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

#ifndef STMG_TEXT_MODIFIER_H
#define STMG_TEXT_MODIFIER_H

#include "stdthememodifier.h"

#include <stmm-games/tile.h>

#include <glibmm/refptr.h>
#include <pangomm/fontdescription.h>
#include <pangomm/layout.h>

#include <string>
#include <memory>
#include <vector>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Pango { class Context; }
namespace stmg { class StdThemeDrawingContext; }

namespace stmg
{

using std::unique_ptr;

class StdTheme;

/** Draws tile's char or a very short text.
 */
class TextModifier : public StdThemeModifier
{
public:
	// sText if empty() then draws the tile char (but only if !isEmpty() and !isCharIndex())
	// bUseTileColor if true the tile color (if defined) is used, otherwise see oColor
	// oColor  if isEmpty(), black or white will be chosen depending on tile color
	struct Init
	{
		std::string m_sText; /**< The (very short) text. If m_sText.empty() draws the tile char (but only if is defined and !isCharIndex()). */
		bool m_bUseTileColor = false; /**< If true the text color is defined by the tile color. If false use m_oColor. Default is false. */
		TileColor m_oColor; /**< The color to use if m_bUseTileColor is false. If empty the color will be either black or white,
							 * depending on the tile color (white on a dark color, black on a light color). */
		double m_fFontSize1 = 0.78; /**< The font size in tiles. Must be &gt; `0.0` and &lt;= `1.0`. Default is `0.78`. */
		bool m_bUseTileFont = false; /**< If true the text font is defined by the tile font. If false use m_oFont. Default is false. */
		TileFont m_oFont; /**< The font to use if m_bUseTileFont is false. If empty the font will be the theme default font. */
		int32_t m_nAddToChar = 0; /**< Added to char if TileChar of the drawn tile is !isEmpty() and !isCharIndex(). Default is 0.*/
	};
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 * @param oInit The initialization data.
	 */
	TextModifier(StdTheme* p1Owner, Init&& oInit) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	Pango::Context* m_p0CacheContext;
	Glib::RefPtr<Pango::Layout> m_refFontLayout;
	unique_ptr<Pango::FontDescription> m_refFont;
	const std::string m_sText;
	int32_t m_nAddToChar;
	bool m_bUseTileColor;
	bool m_bCalcColor;
	double m_fR1, m_fG1, m_fB1, m_fA1;
	double m_fFontSize1;
	bool m_bUseTileFont;
	TileFont m_oFont;
private:
	TextModifier() = delete;
	TextModifier(const TextModifier& oSource) = delete;
	TextModifier& operator=(const TextModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_TEXT_MODIFIER_H */

