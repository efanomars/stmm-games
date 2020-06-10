/*
 * File:   gamediskfiles.h
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

#ifndef STMG_GAME_DISK_FILES_H
#define STMG_GAME_DISK_FILES_H

#include <stmm-games-file/file.h>

#include <stmm-games-xml-game/xmlgamefiles.h>

#include <vector>
#include <string>
#include <map>
//#include <iostream>
#include <utility>
#include <algorithm>
#include <iterator>

namespace stmg
{

class GameDiskFiles : public XmlGameFiles
{
public:
	/** Constructor.
	 * The app name is usually also the subdirectory name in the standard location paths.
	 *
	 * Example: app name `jointris`
	 *
	 *     /home/user/.local/share/stmm-games/themes/jointris       # for themes
	 *     /usr/local/share/stmm-games/themes/jointris              # for themes
	 *     /home/user/.local/share/stmm-games/games/jointris        # for games
	 *     /usr/local/share/stmm-games/games/jointris               # for games
	 *     /home/user/.local/share/stmm-games/preferences/jointris  # for preferences (main.prefs)
	 *     /home/user/.local/share/stmm-games/highscores/jointris   # for highscores
	 *     /usr/local/share/stmm-games/games/jointris.png           # for the app icon (128x128 pixel)
	 *
	 * Themes can also be non game specific and be imported by game specific themes.
	 *
	 * Example:
	 *
	 *     /home/user/.local/share/stmm-games/themes                # for generic themes
	 *     /usr/local/share/stmm-games/themes                       # for generic themes
	 *
	 * A theme is a folder named after the theme with suffix `.thm` containing
	 * files `theme.xml` and thumbnail.png and folders `images` and `sounds`
	 *
	 * Example: generic theme `foo`
	 *
	 *     /usr/share/stmm-games/themes/foo.thm/theme.xml       # theme definition file
	 *     /usr/share/stmm-games/themes/foo.thm/thumbnail.png   # 128x128 pixel
	 *     /usr/share/stmm-games/themes/foo.thm/images
	 *     /usr/share/stmm-games/themes/foo.thm/images/mybar.jpg
	 *     /usr/share/stmm-games/themes/foo.thm/sounds
	 *     /usr/share/stmm-games/themes/foo.thm/sounds/yourbang.mp3
	 *
	 * All themes also automatically have access to common images and sounds.
	 *
	 * Example: common images and sounds
	 *
	 *     /usr/share/stmm-games/themes/common/images
	 *     /usr/share/stmm-games/themes/common/sounds/background.jpg
	 *     /usr/share/stmm-games/themes/common/sounds
	 *     /usr/share/stmm-games/themes/common/sounds/test.wav
	 *
	 * A game is defined by an xml file and a corresponding picture.
	 *
	 * Example: app `jointris` can have many games
	 *
	 *     /usr/share/stmm-games/games/jointris/classic.xml       # `classic` game definition file
	 *     /usr/share/stmm-games/games/jointris/classic.png       # corresponding picture, 128x128 pixel
	 *     /usr/share/stmm-games/games/jointris/penta.xml         # `penta` game definition file
	 *     /usr/share/stmm-games/games/jointris/penta.png         # corresponding picture, 128x128 pixel
	 * @param sAppName The app's name. Cannot be empty. Cannot contain spaces or weird characters.
	 * @param bIncludeHomeLocal Whether games and themes should also be loaded from the user's home directory.
	 */
	GameDiskFiles(const std::string& sAppName, bool bIncludeHomeLocal);
	/** Non standard location constructor.
	 * The folder structure of a theme must be the same as for standard location themes.
	 *
	 * Beware! This constructor doesn't evaluate environment variable STMM_GAMES_EXTRA_BASE_PATH
	 * and add its content as a path from which to load games and themes.
	 *
	 * Example: /tmp/mytheme (note: the folder must not necessarily end with `.thm`)
	 *
	 *     /tmp/mytheme/theme.xml
	 *     /tmp/mytheme/thumbnail.png   # 128x128 pixel
	 *     /tmp/mytheme/images
	 *     /tmp/mytheme/sounds
	 *
	 * @param sAppName The app's name. Cannot be empty.
	 * @param bIncludeHomeLocal Whether the user's home directory is also considered as a standard location for games and themes.
	 * @param aGameFiles The game files. Can be empty. All files must be defined and not be buffers. Example: '/tmp/mygames/pong.xml'.
	 * @param bAddStandardLocationGameFiles Whether to add standard locations game files to those in aGameFiles.
	 * @param aThemeFiles The theme files. Can be empty. All files must be defined and not be buffers. Example: '/tmp/mytheme/theme.xml'.
	 * @param bAddStandardLocationThemeFiles Whether to add standard locations theme files to those in aThemeFiles.
	 * @param sIconFile The icon file. If not empty must exist. Example: '/tmp/myicons/pong128x128.png'.
	 * @param sHighscoresDir The highscores directory. If not empty, must exist.
	 * @param sPreferencesFile The preferences file. Can be empty.
	 */
	GameDiskFiles(const std::string& sAppName, bool bIncludeHomeLocal
				, std::vector< File >&& aGameFiles, bool bAddStandardLocationGameFiles
				, std::vector< File >&& aThemeFiles, bool bAddStandardLocationThemeFiles
				, const std::string& sIconFile, const std::string& sHighscoresDir, const std::string& sPreferencesFile);
	/** The game files for the app.
	 * @return The game files. Are all defined.
	 */
	const std::vector< File >& getGameFiles() noexcept override;
	/** The game's thumbnail.
	 * The name of the game's thumbnail image is usually the game's xml file with
	 * the image format suffix instead of the .xml suffix.
	 *
	 * Example: game file '/usr/local/share/stmm-games/games/jointris/classic.xml'
	 * could return thumbnail file
	 * '/usr/local/share/stmm-games/games/jointris/classic.jpg'
	 * @param oGameFile The game file. Must be defined.
	 * @return The thumbnail file or undefined if not found.
	 */
	const File& getGameThumbnailFile(const File& oGameFile) noexcept override;
	/** The icon file.
	 * Example: '/usr/local/share/stmm-games/games/jointris.png"
	 * @return The icon file or undefined.
	 */
	File getIconFile();
	/** Returns the full highscore file given the game name.
	 * Example: if sGameName is 'Classic' the returned file
	 * might have path '~/.local/share/stmm-games/highscores/jointris/Classic.scores'.
	 * @param sGameName The game name. Cannot be empty.
	 * @return The highscores file or an undefined File if not supported.
	 */
	File getHighscoreFile(const std::string& sGameName) noexcept override;
	/** Returns the preferences file.
	 * @return The preferences file or an undefined File if not supported.
	 */
	File getPreferencesFile() noexcept override;
	/** The theme files for the app.
	 * @return The theme files. Must all be defined.
	 */
	const std::vector< File >& getThemeFiles();
	/** The theme's image files.
	 * Example: for theme file '/usr/local/share/stmm-games/themes/jointris/simple_j.thm/theme.xml'
	 * all the images in the directory '/usr/local/share/stmm-games/themes/jointris/simple_j.thm/images'
	 * are returned.
	 * @param oThemeFile The theme file. Must be defined.
	 * @return The image files. Are all defined.
	 */
	const std::vector< std::pair<std::string, File> >& getThemeImageFiles(const File& oThemeFile);
	/** The theme's sound files.
	 * 
	 * @param oThemeFile The theme file. Must be defined.
	 * @return The sound files. Are all defined.
	 */
	const std::vector< std::pair<std::string, File> >& getThemeSoundFiles(const File& oThemeFile);
	/** The theme's thumbnail.
	 * @param oThemeFile The theme file. Must be defined.
	 * @return The thumbnail file or undefined if not found.
	 */
	const File& getThemeThumbnailFile(const File& oThemeFile);
	/** The image files shared by all themes.
	 * @return The shared image files. Must all be defined.
	 */
	const std::vector< std::pair<std::string, File> >& getDefaultImageFiles();
	/** The sound files shared by all themes.
	 * @return The shared sound files. Must all be defined.
	 */
	const std::vector< std::pair<std::string, File> >& getDefaultSoundFiles();

	/** The base path for preferences and highscores.
	 * Example: '/home/user/.local/share/stmm-games'.
	 * @return The path. Can be empty.
	 */
	const std::string& getPrefsAndHighscoresBasePath() const;
	/** The base paths for games and themes.
	 * @return The paths. Cannot be empty.
	 */
	const std::vector< std::string >& getGamesAndThemesBasePaths() const;
private:
	struct ResourceFileData
	{
		bool m_bLoaded = false;
		std::vector< std::pair<std::string, File> > m_aResourceFiles;
	};

	// Ex,: sShareMainDir = "games" or "games/jointris" or "themes"
	// aFiles is input output param (files should be appended to it)
	// if bDoSubDirs also do
	template<class Filter>
	void visitSysPathsAndSubdirs(const std::string& sShareMainDir, bool bDoSubDirs
										, std::vector<std::string>& aFiles, Filter oFilter)
	{
		std::vector<std::string> aSubDirNames;
		for (const auto& sDataDirPath : m_aReadOnlyPaths) {
			std::string sPath = sDataDirPath + "/" + sShareMainDir;
			aSubDirNames.clear();
			if (bDoSubDirs) {
				getDirSubTreeFiles(sPath, true, false, false, "", aSubDirNames);
			}
			oFilter(sPath, aSubDirNames, aFiles);
		}
	}
	/* Get files and/or directories.
	 * @param sDirPath Full path of the directory.
	 * @param bDirs Whether to retrieve sub directories.
	 * @param bRecurse Whether to recurse. Can be true even if bDirs is false.
	 * @param bRegularFiles Whether to retrieve files.
	 * @param aFiles The resulting files and/or directories with path relative to sDirPath. Ex. "jointris/mygame.xml"
	 */
	static void getDirSubTreeFiles(const std::string& sDirPath, bool bDirs, bool bRecurse, bool bRegularFiles
									, std::vector<std::string>& aFiles);
	/* Get files and/or directories.
	 * @param sDirPath Full path of the directory.
	 * @param bDirs Whether to retrieve sub directories.
	 * @param bRecurse Whether to recurse. Can be true even if bDirs is false.
	 * @param bRegularFiles Whether to retrieve files.
	 * @param sPrefix The prefix added to the files to get aFiles. Ex. "jointris/"
	 * @param aFiles The resulting files and/or directories with path relative to sDirPath. Ex. "jointris/animage.png"
	 */
	static void getDirSubTreeFiles(const std::string& sDirPath, bool bDirs, bool bRecurse, bool bRegularFiles
									, const std::string& sPrefix, std::vector<std::string>& aFiles);
	static void getAbsPathFromBaseAndRel(const std::string& sDirPath, const std::vector<std::string>& aRelFiles
										, std::vector<std::string>& aPaths);

	void getThemeFiles(const std::string& sThemesFolder, std::vector<std::string>& aFiles);
	static void removeNonImageFiles(std::vector<std::string>& aFiles);
	static void removeNonSoundFiles(std::vector<std::string>& aFiles);
	ResourceFileData& getResourceData(const std::string& sResource, std::map<std::string, ResourceFileData>& oThemeResourceData);
	void loadResourceFiles(ResourceFileData& oFileData, const std::vector<std::string>& aNames, const std::vector<std::string>& aPaths);

	// if sOnlyExt is empty returns all images
	// if bDoSubdirs is true
	//    if sOnlyExt is empty aNames are relative to sTheDir and recursion is infinite
	//    else aNames for the sub directories of sTheDir are relative to their sub directory and there is no recursion
	void getFilesAndPathsWithExt(const std::string& sTheDir, bool bDoSubdirs, const std::string& sOnlyExt
								, std::vector<std::string>& aNames, std::vector<std::string>& aPaths);
	void getFilesAndPathsOfImages(const std::string& sTheDir, bool bDoSubdirs
								, std::vector<std::string>& aNames, std::vector<std::string>& aPaths);
	void getFilesAndPathsOfSounds(const std::string& sTheDir, bool bDoSubdirs
								, std::vector<std::string>& aNames, std::vector<std::string>& aPaths);
	template<class Filter>
	void getFilesAndPaths(const std::string& sTheDir, bool bDoSubdirs
								, std::vector<std::string>& aNames, std::vector<std::string>& aPaths, Filter oFilter)
	{
		auto oAddFiles = [&](const std::string sThePath)
		{
			std::vector<std::string> aSubDirFiles;
			const bool bRecurse = false;
			const bool bRegularFiles = true;
			getDirSubTreeFiles(sThePath, bRecurse, bDoSubdirs, bRegularFiles, aSubDirFiles);
			aSubDirFiles.erase(std::remove_if(aSubDirFiles.begin(), aSubDirFiles.end(), [&](const std::string& sSubDirFile)
			{
				return ! oFilter(sSubDirFile);
			}), aSubDirFiles.end());
			getAbsPathFromBaseAndRel(sThePath, aSubDirFiles, aPaths);
			std::move(aSubDirFiles.begin(), aSubDirFiles.end(), std::back_inserter(aNames));
		};

		visitSysPathsAndSubdirs(sTheDir, bDoSubdirs, aNames
								, [&](const std::string& sDirPath, const std::vector<std::string>& aSubDirNames
									, std::vector<std::string>& /*aPathFiles*/)
		{
			oAddFiles(sDirPath);
			for (const auto& sSubDirName : aSubDirNames) {
				const std::string sSubDirPath = sDirPath + "/" + sSubDirName;
				oAddFiles(sSubDirPath);
			}
		});
	}

	static bool isImageFile(const std::string& sFile);
	static bool isSoundFile(const std::string& sFile);

private:
	std::string m_sAppName;

	const std::string m_sUserWritablePath;
	const std::string m_sPackagePath;
	const std::vector< std::string > m_aReadOnlyPaths;

	bool m_bGameFilesLoaded;
	std::vector< File > m_aGameFiles;
	bool m_bThemeFilesLoaded;
	std::vector< File > m_aThemeFiles;

	bool m_bHighscoresDirLoaded;
	std::string m_sHighscoresDir;
	bool m_bIconFileLoaded;
	File m_oAppIconFile;
	bool m_bPreferencesFileLoaded;
	File m_oPreferencesFile;

	// Key:   Game name xxx (ex. "Classic", that has a file ".../classic.xml")
	// Value: size 1 vector containing the highscores
	std::map<std::string, ResourceFileData> m_oGameHighscoreData;

	// Key: Game xxx.xml full path (utf8, not file system encoding)
	// Value: size 1 vector containing the thumbnail file
	std::map<std::string, ResourceFileData> m_oGameThumbnailData;
	// Key: Theme xxx.thm/theme.xml full path (utf8, not file system encoding)
	// Value: size 1 vector containing the thumbnail file
	std::map<std::string, ResourceFileData> m_oThemeThumbnailData;

	std::map<std::string, ResourceFileData> m_oThemeImagesData; // Key: theme.xml full path (utf8, not file system encoding)
	std::map<std::string, ResourceFileData> m_oThemeSoundsData; // Key: theme.xml full path (utf8, not file system encoding)
	ResourceFileData m_oDefaultImagesData;
	ResourceFileData m_oDefaultSoundsData;

	static const std::string::value_type* s_aImageFileExt[];
	static const std::string::value_type* s_aSoundFileExt[];

	static const File s_oEmptyFile;
private:
	GameDiskFiles() = delete;
	GameDiskFiles(const GameDiskFiles& oSource) = delete;
	GameDiskFiles& operator=(const GameDiskFiles& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_GAME_DISK_FILES_H */

