/*
 * File:   named.h
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

#ifndef STMG_NAMED_TILES_H
#define STMG_NAMED_TILES_H

#include "util/namedindex.h"

namespace stmg
{

/** String indexing for assets provided by the view (Theme) to the model(s).
 * Since strings processing is slow, integers are used to identify them.
 */
class Named
{
public:
	/** Empty constructor.
	 */
	Named() noexcept = default;
	/** Copy constructor.
	 */
	Named(const Named& oSource) noexcept = default;
	/** Move constructor.
	 */
	Named(Named&& oSource) noexcept = default;

	/** Color names.
	 * Used by TileColor class.
	 * @return The colors named index.
	 */
	inline NamedIndex& colors() noexcept { return m_oColors; };
	/** Color names.
	 * Used by TileColor class.
	 * @return The colors named index.
	 */
	inline const NamedIndex& colors() const noexcept { return m_oColors; };
	/** Character names.
	 * Used by TileChar class.
	 * @return The characters named index.
	 */
	inline NamedIndex& chars() noexcept { return m_oChars; };
	/** Character names.
	 * Used by TileChar class.
	 * @return The characters named index.
	 */
	inline const NamedIndex& chars() const noexcept { return m_oChars; };
	/** Font names.
	 * Used by TileFont class.
	 * @return The fonts named index.
	 */
	inline NamedIndex& fonts() noexcept { return m_oFonts; };
	/** Font names.
	 * Used by TileFont class.
	 * @return The fonts named index.
	 */
	inline const NamedIndex& fonts() const noexcept { return m_oFonts; };

	/** Tile animation names.
	 * Used by TileAnimator class.
	 * @return The tile animations named index.
	 */
	inline NamedIndex& tileAnis() noexcept { return m_oTileAnis; };
	/** Tile animation names.
	 * Used by TileAnimator class.
	 * @return The tile animations named index.
	 */
	inline const NamedIndex& tileAnis() const noexcept { return m_oTileAnis; };
	/** Image ids.
	 * @return The image ids named index.
	 */
	inline NamedIndex& images() noexcept { return m_oImageIds; };
	/** Image ids.
	 * @return The image ids named index.
	 */
	inline const NamedIndex& images() const noexcept { return m_oImageIds; };
	/** Sound ids.
	 * @return The sound ids named index.
	 */
	inline NamedIndex& sounds() noexcept { return m_oSoundIds; };
	/** Sound ids.
	 * @return The sound ids named index.
	 */
	inline const NamedIndex& sounds() const noexcept { return m_oSoundIds; };

	/** Theme animations.
	 * @return Theme animations named index.
	 */
	inline NamedIndex& animations() noexcept { return m_oAnimations; };
	/** Theme animations.
	 * @return Theme animations named index.
	 */
	inline const NamedIndex& animations() const noexcept { return m_oAnimations; };

	/** Theme widgets.
	 * @return Theme widgets named index.
	 */
	inline NamedIndex& widgets() noexcept { return m_oWidgets; };
	/** Theme widgets.
	 * @return Theme widgets named index.
	 */
	inline const NamedIndex& widgets() const noexcept { return m_oWidgets; };
protected:
	/** Move assignment.
	 * If you want to allow assignment you have to subclass Named and make it public
	 * or use AssignableNamed
	 * @param oSource The source.
	 * @return This.
	 */
	Named& operator=(Named&& oSource) noexcept = default;
	/** Copy assignment.
	 * If you want to allow assignment you have to subclass Named and make it public
	 * or use AssignableNamed
	 * @param oSource The source.
	 * @return This.
	 */
	Named& operator=(const Named& oSource) noexcept = default;
	/** Clears the instance.
	 * If you want to allow clear you have to subclass Named and make it public
	 * or use AssignableNamed.
	 */
	void clear() noexcept
	{
		m_oColors.clear();
		m_oChars.clear();
		m_oFonts.clear();
		m_oTileAnis.clear();
		m_oImageIds.clear();
		m_oSoundIds.clear();
		m_oAnimations.clear();
		m_oWidgets.clear();
	};
private:
	AssignableNamedIndex m_oColors;
	AssignableNamedIndex m_oChars;
	AssignableNamedIndex m_oFonts;

	AssignableNamedIndex m_oTileAnis;

	AssignableNamedIndex m_oImageIds;
	AssignableNamedIndex m_oSoundIds;

	AssignableNamedIndex m_oAnimations;

	AssignableNamedIndex m_oWidgets;
};

/** Assignable and clearable named class.
 */
class AssignableNamed : public Named
{
public:
	using Named::operator=;
	using Named::clear;
};

} // namespace stmg

#endif	/* STMG_NAMED_TILES_H */

