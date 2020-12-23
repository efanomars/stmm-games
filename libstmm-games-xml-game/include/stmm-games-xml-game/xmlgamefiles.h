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
 * File:   xmlgamefiles.h
 */

#ifndef STMG_XML_GAME_FILES_H
#define STMG_XML_GAME_FILES_H

#include <stmm-games-file/file.h>

#include <vector>
#include <string>

namespace stmg
{

/** Interface locating xml files related to games.
 */
class XmlGameFiles
{
public:
	virtual ~XmlGameFiles() = default;
	/** The game files.
	 * @return The files. Are all defined. Can be empty.
	 */
	virtual const std::vector< File >& getGameFiles() noexcept = 0;
	/** The thumbnail file associated with a game file.
	 * @param oGameFile The game file. Must be one of the files returned by getGameFiles().
	 * @return The thumbnail file. Can be undefined (means no thumbnail available).
	 */
	virtual const File& getGameThumbnailFile(const File& oGameFile) noexcept = 0;
	/** The highscores file for a specific game.
	 * The game name is usually defined somewhere in the contents of the xml files
	 * returned by getGameFiles().
	 * @param sGameName The internal game name. Cannot be empty.
	 * @return The highscores file. Undefined if unknown game name or no highscores available.
	 */
	virtual File getHighscoreFile(const std::string& sGameName) noexcept = 0;
	/** The preferences file.
	 * @return The preferences file. Can be undefined.
	 */
	virtual File getPreferencesFile() = 0;
protected:
	XmlGameFiles() = default;
private:
	XmlGameFiles(const XmlGameFiles& oSource) = delete;
	XmlGameFiles& operator=(const XmlGameFiles& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_GAME_FILES_H */

