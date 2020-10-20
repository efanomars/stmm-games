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
 * File:   stdthemecontext.h
 */

#ifndef STMG_STD_THEME_CONTEXT_H
#define STMG_STD_THEME_CONTEXT_H

#include "themecontext.h"
#include "theme.h"
#include "stdthemedrawingcontext.h"

#include <stmm-games/util/basictypes.h>

#include <cairomm/context.h>
#include <glibmm/refptr.h>
#include <pangomm/context.h>

#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class ThemeSound; }
namespace stmi { class PlaybackCapability; }
namespace stmg { class LevelAnimation; }
namespace stmg { class ThemeAnimation; }
namespace stmg { class Tile; }

namespace stmg
{

using std::shared_ptr;

class StdTheme;

/** The theme context for a StdTheme.
 */
class StdThemeContext : public ThemeContext, public std::enable_shared_from_this<StdThemeContext>
{
public:
	/** Constructor.
	 */
	StdThemeContext(Theme::RuntimeVariablesEnv* p0RuntimeVariablesEnv) noexcept;
	void drawBoardTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile, int32_t nPlayer
						, const std::vector<double>& aAniElapsed) noexcept override;
	void drawBoardTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile, int32_t nPlayer) noexcept override;
	void drawBlockTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile, int32_t nPlayer
						, const std::vector<double>& aAniElapsed) noexcept override;
	void drawBlockTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile, int32_t nPlayer) noexcept override;

	shared_ptr<ThemeAnimation> createAnimation(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;

	void preloadSound(int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks) noexcept override;
	shared_ptr<ThemeSound> createSound(int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks
										, FPoint oXYPosition, double fZPosition, bool bRelative
										, double fVolume, bool bLoop) noexcept override;

	NSize getTileSize() const noexcept override { return NSize{m_nTileW, m_nTileH}; }
	void setTileSize(NSize oSize) noexcept override;
	const Glib::RefPtr<Pango::Context>& getFontContext() noexcept override;

protected:
	/** Reinitialization.
	 */
	void reInit(Theme::RuntimeVariablesEnv* p0RuntimeVariablesEnv) noexcept;
private:
	friend class StdTheme;
	friend class StdThemeSound;
	friend class StdThemeDrawingContext;
private:

	StdTheme* m_p1Owner;
	int32_t m_nTileW;
	int32_t m_nTileH;
	bool m_bRegistered;
	bool m_bDrawingBoardTile;
	double m_fSoundScaleX;
	double m_fSoundScaleY;
	double m_fSoundScaleZ;
	StdThemeDrawingContext m_oDrawingContext;
	Glib::RefPtr<Pango::Context> m_refFontContext;
};

} // namespace stmg

#endif	/* STMG_STD_THEME_CONTEXT_H */

