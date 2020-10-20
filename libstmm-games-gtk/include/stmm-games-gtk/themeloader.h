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
 * File:   themeloader.h
 */

#ifndef STMG_THEME_LOADER_H
#define STMG_THEME_LOADER_H

#include <stmm-games-file/file.h>

#include <vector>
#include <string>
#include <memory>

namespace stmg
{

using std::shared_ptr;

class Theme;

class ThemeLoader
{
public:
	virtual ~ThemeLoader() noexcept = default;

	struct ThemeInfo
	{
		File m_oThemeFile; /**< The file where the theme is defined, if available. Can be undefined. */
		struct Author
		{
			std::string m_sName; /**< Author name. */
			std::string m_sEmail; /**< Author email. */
			std::string m_sRole; /**< Author role. */
		};
		std::vector<Author> m_aAuthors; /**< All the authors. */
		std::string m_sThemeDescription; /**< Possibly multiline description of the theme. */
		File m_oThumbnailFile; /**< The location of the thumbnail image of the theme. Can be undefined. */
		bool m_bTesting = false; /**< Whether the theme should be hidden when not testing (see StdConfig). */
		bool m_bLoaded; /**< Whether tried to load the game at least once. */
		std::string m_sThemeErrorString; /**< The error if loading failed or empty. Empty if m_bLoaded is `false`. */
	};
	/** The theme names supported by this loader.
	 * @return The (unique) non-empty names of the themes.
	 */
	virtual const std::vector<std::string>& getThemeNames() noexcept = 0;

	/** The theme information by name.
	 * @param sThemeName The name. The theme must exist. Cannot be empty.
	 * @return The theme information.
	 */
	virtual const ThemeInfo& getThemeInfo(const std::string& sThemeName) noexcept = 0;

	/** Get the theme with the given name or a default one.
	 * @param sThemeName The name of the theme or empty if loader should choose.
	 * @return The theme or null if could not load (error string is set in ThemeInfo).
	 */
	virtual shared_ptr<Theme> getTheme(const std::string& sThemeName) noexcept = 0;
	/** The default theme name.
	 * @return The theme name. Can be empty.
	 */
	virtual const std::string& getDefaultThemeName() noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_THEME_LOADER_H */

