/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   themecontext.h
 */

#ifndef STMG_THEME_CONTEXT_H
#define STMG_THEME_CONTEXT_H

#include <stmm-games/util/basictypes.h>

#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class LevelAnimation; }
namespace stmg { class ThemeAnimation; }
namespace stmg { class ThemeSound; }
namespace stmg { class Tile; }

namespace stmi { class PlaybackCapability; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

using std::shared_ptr;

class ThemeContext
{
public:
	virtual ~ThemeContext() noexcept = default;
	/** Create theme animation.
	 * @param refLevelAnimation The model. Cannot be null.
	 * @return The theme animation or null if model not supported.
	 */
	virtual shared_ptr<ThemeAnimation> createAnimation(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept = 0;
	/** Pre-load a sound.
	 * @param nSoundIdx The sound index according to Named::sounds(). Must be non-negative.
	 * @param aPlaybacks All the devices that should pre-load the sound. Cannot be empty. Devices can be null.
	 */
	virtual void preloadSound(int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks) noexcept = 0;
	/** Create a sound.
	 * @param nSoundIdx The sound index according to Named::sounds(). Must be non-negative.
	 * @param aPlaybacks All the devices the sound should be played to. Cannot be empty. Devices can be null.
	 * @param oXYPosition The x and y position at which the sound should be played.
	 * @param fZPosition The z position.
	 * @param bRelative Whether the positions are relative to the listener of each capability.
	 * @param fVolume The volume.
	 * @param bLoop Whether the sound(s) should loop.
	 * @return The created compound sound or null if sound is not enabled or sound index wrong.
	 */
	virtual shared_ptr<ThemeSound> createSound(int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks
												, FPoint oXYPosition, double fZPosition, bool bRelative
												, double fVolume, bool bLoop) noexcept = 0;

	/** Draw a tile with tile animations.
	 * The elapsed values in aAniElapsed are numbers from 0.0 to 1.0.
	 * The index into aAniElapsed is the index into Theme::getNamed().tileAnis().
	 * @param refCc The drawing context. Cannot be null.
	 * @param oTile The tile to draw.
	 * @param nPlayer The level player or -1 if tile not part of a block.
	 * @param aAniElapsed The elapsed tile animations.
	 */
	virtual void drawBoardTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept = 0;
	/** Draw a tile without tile animations.
	 * @param refCc The drawing context. Cannot be null.
	 * @param oTile The tile to draw.
	 * @param nPlayer The level player or -1 if tile not part of a block.
	 */
	virtual void drawBoardTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile, int32_t nPlayer) noexcept = 0;
	/** Draw a tile with tile animations.
	 * The elapsed values in aAniElapsed are numbers from 0.0 to 1.0.
	 * The index into aAniElapsed is the index into Theme::getNamed().tileAnis().
	 * @param refCc The drawing context. Cannot be null.
	 * @param oTile The tile to draw.
	 * @param nPlayer The level player or -1 if tile not part of a block.
	 * @param aAniElapsed The elapsed tile animations.
	 */
	virtual void drawBlockTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept = 0;
	/** Draw a tile without tile animations.
	 * @param refCc The drawing context. Cannot be null.
	 * @param oTile The tile to draw.
	 * @param nPlayer The level player or -1 if tile not part of a block.
	 */
	virtual void drawBlockTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile, int32_t nPlayer) noexcept = 0;

	/** Get the current tile size.
	 * @return The tile size in pixels.
	 */
	virtual NSize getTileSize() const noexcept = 0;
	/** Set the current tile size.
	 * @param oSize The new tile size in pixels. Must be positive for both coordinates.
	 */
	virtual void setTileSize(NSize oSize) noexcept = 0;
	/** Get the font context.
	 * @return The pango context used for writing text.
	 */
	virtual const Glib::RefPtr<Pango::Context>& getFontContext() noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_THEME_CONTEXT_H */

