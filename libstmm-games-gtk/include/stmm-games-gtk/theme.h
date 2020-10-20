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
 * File:   theme.h
 */

#ifndef STMG_THEME_H
#define STMG_THEME_H

#include <stmm-games/util/basictypes.h>

#include <memory>
#include <utility>
#include <string>

#include <stdint.h>

namespace stmg { class ThemeContext; }
namespace stmg { class Named; }
namespace stmg { class GameWidget; }
namespace stmg { class ThemeWidget; }

namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

using std::shared_ptr;

class Theme
{
public:
	virtual ~Theme() noexcept = default;
	/** Get best tile size given a requested width.
	 * @param nHintTileW The requested width. If 0 the minimum size is returned.
	 * @return The best size.
	 */
	virtual NSize getBestTileSize(int32_t nHintTileW) const noexcept = 0;
	// nTileW = fTileWHRatio * nTileH
	/** The tile width to height ratio.
	 * According to `nTileW ~= fTileWHRatio * nTileH`.
	 * @return The ratio.
	 */
	virtual double getTileWHRatio() const noexcept = 0; //

	/** The named values.
	 * Contains the strings relevant to the game.
	 * @return The named values.
	 */
	virtual const Named& getNamed() const noexcept = 0;

	/** Create a theme widget from a game widget (the model).
	 * @param refGameWidget The game widget. Cannot be null.
	 * @param fTileWHRatio The tile wh ratio. Must be positive.
	 * @param refFontContext The font context. Cannot be null.
	 * @return The created theme widget or null if model not supported.
	 */
	virtual shared_ptr<ThemeWidget> createWidget(const shared_ptr<GameWidget>& refGameWidget, double fTileWHRatio
												, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept = 0;

	class RuntimeVariablesEnv
	{
	public:
		virtual ~RuntimeVariablesEnv() noexcept = default;
		/** Get variable id from name callback.
		 * @param sVarName The variable name. Cannot be empty.
		 * @return The variable id or -1 if not defined.
		 */
		virtual int32_t getVariableIdFromName(const std::string& sVarName) noexcept = 0;
		/** Get variable value callback.
		 * @param nVarId The variable id as returned from getVariableIdFromName().
		 * @return The current value.
		 */
		virtual int32_t getVariableValue(int32_t nVarId) noexcept = 0;
	};
	/** Create a theme context.
	 * The bRegister value tells to the theme whether the tile size of the context
	 * is stable and will be used often. If true images and possibly other data structures
	 * might be cached.
	 * @param oTileWH The initial tile width and height in pixels.
	 * @param bRegister Whether the tile size of the context should be registered.
	 * @param fSoundScaleX The tile to sound reference system scale for the x axis.
	 * @param fSoundScaleY The tile to sound reference system scale for the y axis.
	 * @param fSoundScaleZ The tile to sound reference system scale for the z axis.
	 * @param refFontContext The font context. Cannot be null.
	 * @param p0RuntimeVariablesEnv Cannot be null.
	 * @return The theme context.
	 */
	virtual shared_ptr<ThemeContext> createContext(NSize oTileWH, bool bRegister, double fSoundScaleX, double fSoundScaleY, double fSoundScaleZ
													, const Glib::RefPtr<Pango::Context>& refFontContext
													, RuntimeVariablesEnv* p0RuntimeVariablesEnv) noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_THEME_H */

