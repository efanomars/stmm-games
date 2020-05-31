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
 * File:   stmm-games-xml-gtk-config.h
 */

#ifndef STMG_STMM_GAMES_XML_GTK_LIB_CONFIG_H
#define STMG_STMM_GAMES_XML_GTK_LIB_CONFIG_H

#include <string>
#include <vector>

namespace stmg
{

namespace libconfig
{

namespace xmlgtk
{

/** The stmm-games-xml-gtk library version.
 * @return The version string. Cannot be empty.
 */
const std::string& getVersion();

/** The data directory path relative to the user's home directory.
 * Example: '.local/share/stmm-games'.
 * @return The relative base path. Can be empty.
 */
std::string getUserDataHomeRelDirPath();

/** The user data directory path.
 * This is used to store preferences and highscores.
 *
 * Example: if this function returns `/home/user/.local/share/stmm-games`,
 * an app named jointris would have its preferences in directory
 * `/home/user/.local/share/stmm-games/preferences/jointris`.
 *
 * @return The absolute path of preferences and highscores base directory. Can be empty.
 */
std::string getUserDataDirPath();

/** The data directory path for the installation prefix.
 *
 * This is also one of the paths of getDataDirPaths().
 *
 * During development this function returns '/usr/local/share/stmm-games',
 * on a release install '/usr/share/stmm-games'.
 *
 * @return The installation prefix data directory. Cannot be empty.
 */
const std::string getPackageDataDirPath();

/** All the games and themes base directory paths.
 * These directories are used by game-apps to store games and themes and are usually
 * readonly, except for the home local path, which can optionally be excluded.
 * The home local path can be used to create new games and themes.
 *
 * Example: if this function returns the path `/usr/local/share/stmm-games`,
 * an app named jointris would have its games in directory
 * `/usr/local/share/stmm-games/games/jointris` and its themes in directory
 * `/usr/local/share/stmm-games/themes/jointris`.
 *
 * @param bIncludeHomeLocal Whether to include the '/home/user/.local/share/stmm-games' base path.
 * @param bIncludeAdditionalEnvPath Whether to include the additional base path in environment variable.
 * @return The absolute paths of games and themes base directories. Cannot be empty.
 */
std::vector<std::string> getDataDirPaths(bool bIncludeHomeLocal, bool bIncludeAdditionalEnvPath);

} // namespace xmlgtk

} // namespace libconfig

} // namespace stmg

#endif	/* STMG_STMM_GAMES_XML_GTK_LIB_CONFIG_H */

