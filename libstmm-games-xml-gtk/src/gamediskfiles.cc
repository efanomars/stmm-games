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
 * File:   gamediskfiles.cc
 */

#include "gamediskfiles.h"
#include "stmm-games-xml-gtk-config.h"

#include "xmlutilfile.h"

#include <stmm-games-file/file.h>

#include <glibmm.h>

#include <cassert>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <iostream>

#include <stdint.h>

namespace stmg
{

static const std::string s_sGamesFolder = "games";
static const std::string s_sHighscoresFolder = "highscores";
static const std::string s_sHighscoresXmlExt = ".scores";
static const std::string s_sPreferencesFolder = "preferences";
static const std::string s_sPreferencesXmlName = "main";
static const std::string s_sPreferencesXmlExt = ".prefs";
static const std::string s_sThemesFolder = "themes";
static const std::string s_sImagesSubFolder = "images";
static const std::string s_sSoundsSubFolder = "sounds";
static const std::string s_sFontsSubFolder = "fonts";
static const std::string s_sDefaultsFolder = s_sThemesFolder + "/common";
static const std::string s_sDefaultImagesFolder = s_sDefaultsFolder + "/" + s_sImagesSubFolder;
static const std::string s_sDefaultSoundsFolder = s_sDefaultsFolder + "/" + s_sSoundsSubFolder;
static const std::string s_sDefaultFontsFolder = s_sDefaultsFolder + "/" + s_sFontsSubFolder;
static const std::string s_sThemeXmlFolderExt = ".thm";
static const std::string s_sThemeXmlFileName = "theme.xml";
static const std::string s_sGameXmlExt = ".xml";
static const std::string s_sThemeThumbnailBaseName = "thumbnail";

const std::string::value_type* GameDiskFiles::s_aImageFileExt[] = {
	".svg", ".png", ".bmp"
	, ".jpg", ".jpeg", ".jp2", ".jpx", ".tif", ".tiff", ".gif"
	, ".ppm", ".pgm", ".pbm", ".pnm"
	, ".webp"
};
const std::string::value_type* GameDiskFiles::s_aSoundFileExt[] = {
	".wav", ".mp3", ".ogg"
	, ".it", ".xm"
};
const std::string::value_type* GameDiskFiles::s_aFontFileExt[] = {
	".ttf", ".otf"
};

const File GameDiskFiles::s_oEmptyFile{};

static bool checkCanHomeLocal(bool bIncludeHomeLocal)
{
	#ifdef STMM_SNAP_PACKAGING
	if (! bIncludeHomeLocal) {
		return false; //--------------------------------------------------------
	}
	const std::string sHomeRelDir = libconfig::xmlgtk::getUserDataHomeRelDirPath();
	const std::string sSnapName = XmlUtil::getEnvString("SNAP_NAME");
	if (sSnapName.empty() || sHomeRelDir.empty()) {
		return false; //--------------------------------------------------------
	}
	std::string sResult;
	std::string sError;
	if (! XmlUtil::execCmd("snapctl is-connected dot-local-share-stmm-games", sResult, sError)) {
		sError = "Not allowed to load custom games and themes from '~/" + sHomeRelDir + "'."
				"\nGrant permission with 'sudo snap connect " + sSnapName + ":dot-local-share-stmm-games'";
		std::cout << sError << '\n';
		bIncludeHomeLocal = false;
	}
	#endif //STMM_SNAP_PACKAGING
	return bIncludeHomeLocal;
}

GameDiskFiles::GameDiskFiles(const std::string& sAppName, bool bIncludeHomeLocal)
: m_sAppName(sAppName)
, m_sUserWritablePath(libconfig::xmlgtk::getUserDataDirPath())
, m_sPackagePath(libconfig::xmlgtk::getPackageDataDirPath())
, m_aReadOnlyPaths(libconfig::xmlgtk::getDataDirPaths(checkCanHomeLocal(bIncludeHomeLocal), true))
, m_bGameFilesLoaded(false)
, m_bThemeFilesLoaded(false)
, m_bHighscoresDirLoaded(false)
, m_bIconFileLoaded(false)
, m_bPreferencesFileLoaded(false)
{
	assert(!m_sAppName.empty());
}
GameDiskFiles::GameDiskFiles(const std::string& sAppName, bool bIncludeHomeLocal
							, std::vector< File >&& aGameFiles, bool bAddStandardLocationGameFiles
							, std::vector< File >&& aThemeFiles, bool bAddStandardLocationThemeFiles
							, const std::string& sIconFile, const std::string& sHighscoresDir, const std::string& sPreferencesFile)
: m_sAppName(sAppName)
, m_sUserWritablePath(libconfig::xmlgtk::getUserDataDirPath())
, m_sPackagePath(libconfig::xmlgtk::getPackageDataDirPath())
, m_aReadOnlyPaths(libconfig::xmlgtk::getDataDirPaths(checkCanHomeLocal(bIncludeHomeLocal), false))
, m_bGameFilesLoaded(! bAddStandardLocationGameFiles)
, m_aGameFiles(std::move(aGameFiles))
, m_bThemeFilesLoaded(! bAddStandardLocationThemeFiles)
, m_aThemeFiles(std::move(aThemeFiles))
, m_bHighscoresDirLoaded(true)
, m_sHighscoresDir(sHighscoresDir)
, m_bIconFileLoaded(true)
, m_oAppIconFile((sIconFile.empty()) ? File{} : File{sIconFile})
, m_bPreferencesFileLoaded(true)
, m_oPreferencesFile((sPreferencesFile.empty()) ? File{} : File{sPreferencesFile})
{
	assert(!m_sAppName.empty());
	#ifndef NDEBUG
	for (const File& oFile : m_aGameFiles) {
		assert(oFile.isDefined() && ! oFile.isBuffered());
	}
	for (const File& oFile : m_aThemeFiles) {
		assert(oFile.isDefined() && ! oFile.isBuffered());
	}
	#endif //NDEBUG
}
const std::vector< File >& GameDiskFiles::getGameFiles() noexcept
{
//std::cout << "GameDiskFiles::getGameFiles() ---------------------------" << '\n';
	if (!m_bGameFilesLoaded) {
		// first the subfolder (and all its subfolders non recursively)
		std::vector<std::string> aFiles;
		std::vector<std::string> aPaths;
		getFilesAndPathsWithExt(s_sGamesFolder + "/" + m_sAppName, true, s_sGameXmlExt, aFiles, aPaths);
		// then the main folder
		getFilesAndPathsWithExt(s_sGamesFolder, false, s_sGameXmlExt, aFiles, aPaths);
		const int32_t nTotFiles = static_cast<int32_t>(aPaths.size());
		m_aGameFiles.reserve(nTotFiles);
		for (const auto& sPath : aPaths) {
			m_aGameFiles.emplace_back(sPath);
		}
		m_bGameFilesLoaded = true;
	}
	return m_aGameFiles;
}
const File& GameDiskFiles::getGameThumbnailFile(const File& oGameFile) noexcept
{
//std::cout << "GameDiskFiles::getGameThumbnailFile() " << oGameFile.getFullPath() << " ---------------------------" << '\n';
	assert(oGameFile.isDefined() && !oGameFile.isBuffered());
	const auto& sGameFullPath = oGameFile.getFullPath();
	assert(!sGameFullPath.empty());
	//ex.: sGameFullPath = /usr/share/stmm-games/games/jointris/classic.xml

	ResourceFileData& oFileData = getResourceData(sGameFullPath, m_oGameThumbnailData);
	if (!oFileData.m_bLoaded) {

		const std::string sGameName = Glib::path_get_basename(sGameFullPath);
		//ex.: sGameName = classic.xml
		// same as Glib::filename_to_utf8(Glib::path_get_basename(Glib::filename_from_utf8(sGameFullPath)));?
		assert(sGameName.size() > s_sGameXmlExt.size());
		assert(sGameName.substr(sGameName.size() - s_sGameXmlExt.size(), s_sGameXmlExt.size()) == s_sGameXmlExt);

		std::string sGamePath = Glib::path_get_dirname(sGameFullPath);
		//ex.: sGamePath = /usr/share/stmm-games/games/jointris
		std::vector<std::string> aNames;
		getDirSubTreeFiles(sGamePath, false, false, true, aNames);
		//ex.: aNames = {classic.xml, classic.png, hard.xml, hard.jpg, easy.xml}
		removeNonImageFiles(aNames);
		//ex.: aNames = {classic.png, hard.jpg}
		std::vector<std::string> aPaths;
		getAbsPathFromBaseAndRel(sGamePath, aNames, aPaths);
		//ex.: aPaths = {/usr/share/stmm-games/games/jointris/classic.png, /usr/share/stmm-games/games/jointris/hard.jpg}
		for (auto& sImgPath : aPaths) {
			const std::string sImgName = Glib::path_get_basename(sImgPath);
			auto nLastDotIdx = sImgName.find_last_of('.');
			assert(nLastDotIdx != std::string::npos);
			if (sImgName.substr(0, nLastDotIdx) + s_sGameXmlExt == sGameName) {
				oFileData.m_aResourceFiles.emplace_back("", File{sImgPath});
				break; // for
			}
		}
		if (oFileData.m_aResourceFiles.empty()) {
			oFileData.m_aResourceFiles.emplace_back("", File{});
		}
		oFileData.m_bLoaded = true;
	}
	return oFileData.m_aResourceFiles[0].second;
}
const File& GameDiskFiles::getThemeThumbnailFile(const File& oThemeFile)
{
//std::cout << "GameDiskFiles::getThemeThumbnailFile() " << oThemeFile.getFullPath() << " ---------------------------" << '\n';
	assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
	const auto& sThemeFullPath = oThemeFile.getFullPath();
	assert(!sThemeFullPath.empty());
	//ex.: sThemeFullPath = /usr/share/stmm-games/themes/simple.thm/theme.xml

	ResourceFileData& oFileData = getResourceData(sThemeFullPath, m_oThemeThumbnailData);
	if (!oFileData.m_bLoaded) {
		std::string sThemePath = Glib::path_get_dirname(sThemeFullPath);
		//ex.: sThemePath = /usr/share/stmm-games/themes/simple.thm

		std::vector<std::string> aNames;
		getDirSubTreeFiles(sThemePath, false, false, true, aNames);
		//ex.: aNames = {thumbnail.png, theme.xml}
		removeNonImageFiles(aNames);
		//ex.: aNames = {thumbnail.png}
		std::vector<std::string> aPaths;
		getAbsPathFromBaseAndRel(sThemePath, aNames, aPaths);
		//ex.: aPaths = {/usr/share/stmm-games/themes/simple.thm/thumbnail.png}
		for (auto& sImgPath : aPaths) {
			const std::string sImgName = Glib::path_get_basename(sImgPath);
			auto nLastDotIdx = sImgName.find_last_of('.');
			assert(nLastDotIdx != std::string::npos);
			if (sImgName.substr(0, nLastDotIdx) == s_sThemeThumbnailBaseName) {
				oFileData.m_aResourceFiles.emplace_back("", sImgPath);
				break; // for
			}
		}
		if (oFileData.m_aResourceFiles.empty()) {
			oFileData.m_aResourceFiles.emplace_back("", File{});
		}
		oFileData.m_bLoaded = true;
	}
	return oFileData.m_aResourceFiles[0].second;
}
File GameDiskFiles::getIconFile()
{
//std::cout << "GameDiskFiles::getIconFile() ---------------------------" << '\n';
	if (! m_bIconFileLoaded) {
		const std::string sFolder = m_sPackagePath + "/" + s_sGamesFolder;
		std::vector<std::string> aNames;
		getDirSubTreeFiles(sFolder, false, false, true, aNames);
		removeNonImageFiles(aNames);
		std::vector<std::string> aPaths;
		getAbsPathFromBaseAndRel(sFolder, aNames, aPaths);
		for (auto& sImgPath : aPaths) {
			const std::string sImgName = Glib::path_get_basename(sImgPath);
			auto nLastDotIdx = sImgName.find_last_of('.');
			if (nLastDotIdx == std::string::npos) {
				assert(false); // all images have a file type (ex. ".png")
				continue; // for sImgPath
			}
			if (sImgName.substr(0, nLastDotIdx) == m_sAppName) {
				m_oAppIconFile = File(sImgPath);
				m_bIconFileLoaded= true;
				return m_oAppIconFile; //-----------------------------------
			}
		}
		m_bIconFileLoaded = true;
	}
	return m_oAppIconFile;
}
File GameDiskFiles::getHighscoreFile(const std::string& sGameName) noexcept
{
//std::cout << "GameDiskFiles::getHighscoreFile() " << sGameName << " ---------------------------" << '\n';
	assert(! sGameName.empty());
	if (! m_bHighscoresDirLoaded) {
		m_sHighscoresDir = m_sUserWritablePath + "/" + s_sHighscoresFolder + "/" + m_sAppName;
		m_bHighscoresDirLoaded = true;
	}
	return File(m_sHighscoresDir + "/" + sGameName + s_sHighscoresXmlExt);
}
File GameDiskFiles::getPreferencesFile() noexcept
{
//std::cout << "GameDiskFiles::getPreferencesFile() ---------------------------" << '\n';
	if (! m_bPreferencesFileLoaded) {
		const std::string sFolder = m_sUserWritablePath + "/" + s_sPreferencesFolder + "/" + m_sAppName;
		m_oPreferencesFile = File(sFolder + "/" + s_sPreferencesXmlName + s_sPreferencesXmlExt);
		m_bPreferencesFileLoaded = true;
	}
	return m_oPreferencesFile;
}
const std::vector< File >& GameDiskFiles::getThemeFiles()
{
//std::cout << "GameDiskFiles::getThemeFiles() ---------------------------" << '\n';
	if (!m_bThemeFilesLoaded) {
		// first the subfolder
		std::vector<std::string> aThemeXmlPaths;
		getThemeFiles(s_sThemesFolder + "/" + m_sAppName, aThemeXmlPaths);
		// then the main folder
		getThemeFiles(s_sThemesFolder, aThemeXmlPaths);
		const int32_t nTotFiles = static_cast<int32_t>(aThemeXmlPaths.size());
		m_aThemeFiles.reserve(nTotFiles);
		for (const auto& sPath : aThemeXmlPaths) {
			m_aThemeFiles.emplace_back(sPath);
		}
		m_bThemeFilesLoaded = true;
	}
	return m_aThemeFiles;
}
void GameDiskFiles::getFilesAndPathsWithExt(const std::string& sTheDir, bool bDoSubdirs, const std::string& sOnlyExt
											, std::vector<std::string>& aNames, std::vector<std::string>& aPaths)
{
	assert(! sOnlyExt.empty());
	const std::string sUppOnlyExt = Glib::ustring{sOnlyExt}.uppercase();
	getFilesAndPaths(sTheDir, bDoSubdirs, aNames, aPaths, [&](const std::string sThePath)
	{
		return Glib::str_has_suffix(Glib::ustring{sThePath}.uppercase(), sUppOnlyExt);
	});
}
GameDiskFiles::ResourceFileData& GameDiskFiles::getResourceData(const std::string& sResource, std::map<std::string, ResourceFileData>& oResourceData)
{
//std::cout << "GameDiskFiles::getResourceData() sResource=" << sResource << '\n';
	auto itFD = oResourceData.find(sResource);
	if (itFD == oResourceData.end()) {
		oResourceData[sResource] = ResourceFileData(); //TODO own method
		itFD = oResourceData.find(sResource);
	}
	return itFD->second;
}
const std::vector< std::pair<std::string, File> >& GameDiskFiles::getThemeImageFiles(const File& oThemeFile)
{
	assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
	const std::string& sThemeFullPath = oThemeFile.getFullPath();
	assert(!sThemeFullPath.empty());

	ResourceFileData& oFileData = getResourceData(sThemeFullPath, m_oThemeImagesData);
	if (!oFileData.m_bLoaded) {
		// ex.: /usr/share/stmm-games/themes/bang.thm/theme.xml -> returns image files in
		//      /usr/share/stmm-games/themes/bang.thm/images
		// Subfolders within the images folder are part of the name
		// ex.: name "scene1/tl.jpg" identifies path file
		//      "/usr/share/stmm-games/themes/bang.thm/images/scene1/tl.jpg"
		const std::string sThemeName = Glib::path_get_basename(sThemeFullPath);
		assert(sThemeName.size() > 0);

		std::string sThemeImagesPath = sThemeFullPath.substr(0, sThemeFullPath.size() - sThemeName.size()) + s_sImagesSubFolder;
		std::vector<std::string> aNames;
		const bool bDirs = false;
		const bool bRecurse = true;
		const bool bRegularFiles = true;
		getDirSubTreeFiles(sThemeImagesPath, bDirs, bRecurse, bRegularFiles, aNames);
		//
		removeNonImageFiles(aNames);
		//
		std::vector<std::string> aPaths;
		getAbsPathFromBaseAndRel(sThemeImagesPath, aNames, aPaths);
		assert(aPaths.size() == aNames.size());
		//
		loadResourceFiles(oFileData, aNames, aPaths);
		oFileData.m_bLoaded = true;
	}
	return oFileData.m_aResourceFiles;
}
const std::vector< std::pair<std::string, File> >& GameDiskFiles::getThemeSoundFiles(const File& oThemeFile)
{
	assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
	const std::string& sThemeFullPath = oThemeFile.getFullPath();
	assert(!sThemeFullPath.empty());

	ResourceFileData& oFileData = getResourceData(sThemeFullPath, m_oThemeSoundsData);
	if (!oFileData.m_bLoaded) {
		// ex.: /usr/share/stmm-games/themes/bang.thm/theme.xml -> returns sound files in
		//      /usr/share/stmm-games/themes/bang.thm/sounds
		const std::string sThemeName = Glib::path_get_basename(sThemeFullPath);
		assert(sThemeName.size() > 0);

		std::string sThemeSoundsPath = sThemeFullPath.substr(0, sThemeFullPath.size() - sThemeName.size()) + s_sSoundsSubFolder;
		std::vector<std::string> aNames;
		const bool bDirs = false;
		const bool bRecurse = true;
		const bool bRegularFiles = true;
		getDirSubTreeFiles(sThemeSoundsPath, bDirs, bRecurse, bRegularFiles, aNames);
		//
		removeNonSoundFiles(aNames);
		//
		std::vector<std::string> aPaths;
		getAbsPathFromBaseAndRel(sThemeSoundsPath, aNames, aPaths);

		loadResourceFiles(oFileData, aNames, aPaths);
		oFileData.m_bLoaded = true;
	}
	return oFileData.m_aResourceFiles;
}
const std::vector< std::pair<std::string, File> >& GameDiskFiles::getThemeFontFiles(const File& oThemeFile)
{
	assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
	const std::string& sThemeFullPath = oThemeFile.getFullPath();
	assert(!sThemeFullPath.empty());

	ResourceFileData& oFileData = getResourceData(sThemeFullPath, m_oThemeFontsData);
	if (!oFileData.m_bLoaded) {
		// ex.: /usr/share/stmm-games/themes/bang.thm/theme.xml -> returns sound files in
		//      /usr/share/stmm-games/themes/bang.thm/sounds
		const std::string sThemeName = Glib::path_get_basename(sThemeFullPath);
		assert(sThemeName.size() > 0);

		std::string sThemeFontsPath = sThemeFullPath.substr(0, sThemeFullPath.size() - sThemeName.size()) + s_sFontsSubFolder;
		std::vector<std::string> aNames;
		const bool bDirs = false;
		const bool bRecurse = true;
		const bool bRegularFiles = true;
		getDirSubTreeFiles(sThemeFontsPath, bDirs, bRecurse, bRegularFiles, aNames);
		//
		removeNonFontFiles(aNames);
		//
		std::vector<std::string> aPaths;
		getAbsPathFromBaseAndRel(sThemeFontsPath, aNames, aPaths);

		loadResourceFiles(oFileData, aNames, aPaths);
		oFileData.m_bLoaded = true;
	}
	return oFileData.m_aResourceFiles;
}
void GameDiskFiles::getFilesAndPathsOfImages(const std::string& sTheDir, bool bDoSubdirs
											, std::vector<std::string>& aNames, std::vector<std::string>& aPaths)
{
	getFilesAndPaths(sTheDir, bDoSubdirs, aNames, aPaths, [&](const std::string& sFile)
	{
		return isImageFile(sFile);
	});
}
void GameDiskFiles::getFilesAndPathsOfSounds(const std::string& sTheDir, bool bDoSubdirs
											, std::vector<std::string>& aNames, std::vector<std::string>& aPaths)
{
	getFilesAndPaths(sTheDir, bDoSubdirs, aNames, aPaths, [&](const std::string& sFile)
	{
		return isSoundFile(sFile);
	});
}
void GameDiskFiles::getFilesAndPathsOfFonts(const std::string& sTheDir, bool bDoSubdirs
											, std::vector<std::string>& aNames, std::vector<std::string>& aPaths)
{
	getFilesAndPaths(sTheDir, bDoSubdirs, aNames, aPaths, [&](const std::string& sFile)
	{
		return isFontFile(sFile);
	});
}
const std::vector< std::pair<std::string, File> >& GameDiskFiles::getDefaultImageFiles()
{
	ResourceFileData& oFileData = m_oDefaultImagesData;
	if (!oFileData.m_bLoaded) {
		std::vector<std::string> aAllNames;
		std::vector<std::string> aAllPaths;
		// ex.: first "common/images/jointris" with recursive subfolders
		getFilesAndPathsOfImages(s_sDefaultImagesFolder + "/" + m_sAppName, true, aAllNames, aAllPaths);
		// then "common/images" without subfolders
		getFilesAndPathsOfImages(s_sDefaultImagesFolder, false, aAllNames, aAllPaths);
		// then "common/images/xxx" (xxx not being the app name) with recursive subfolders
		std::vector<std::string> aDirs;
		const bool bDirs = true;
		const bool bRecurse = false;
		const bool bRegularFiles = false;
		getDirSubTreeFiles(s_sDefaultImagesFolder, bDirs, bRecurse, bRegularFiles, aDirs);
		for (const auto& sDir : aDirs) {
			if (sDir != m_sAppName) {
				getFilesAndPathsOfImages(s_sDefaultImagesFolder + "/" + sDir, true, aAllNames, aAllPaths);
			}
		}
		assert(aAllNames.size() == aAllPaths.size());
		loadResourceFiles(oFileData, aAllNames, aAllPaths);
		oFileData.m_bLoaded = true;
	}
	return oFileData.m_aResourceFiles;
}
const std::vector< std::pair<std::string, File> >& GameDiskFiles::getDefaultSoundFiles()
{
//std::cout << "GameDiskFiles::getDefaultSoundFiles()" << '\n';
	ResourceFileData& oFileData = m_oDefaultSoundsData;
	if (!oFileData.m_bLoaded) {
		std::vector<std::string> aAllNames;
		std::vector<std::string> aAllPaths;
		// ex.: first "common/sounds/jointris" with recursive subfolders
		getFilesAndPathsOfSounds(s_sDefaultSoundsFolder + "/" + m_sAppName, true, aAllNames, aAllPaths);
		// then "common/sounds" without subfolders
		getFilesAndPathsOfSounds(s_sDefaultSoundsFolder, false, aAllNames, aAllPaths);
		// then "common/sounds/xxx" with recursive subfolders
		std::vector<std::string> aDirs;
		getDirSubTreeFiles(s_sDefaultSoundsFolder, true, false, false, aDirs);
		for (const auto& sDir : aDirs) {
			if (sDir != m_sAppName) {
				getFilesAndPathsOfSounds(s_sDefaultSoundsFolder + "/" + sDir, true, aAllNames, aAllPaths);
			}
		}
		assert(aAllNames.size() == aAllPaths.size());
		loadResourceFiles(oFileData, aAllNames, aAllPaths);
		oFileData.m_bLoaded = true;
	}
	return oFileData.m_aResourceFiles;
}
const std::vector< std::pair<std::string, File> >& GameDiskFiles::getDefaultFontFiles()
{
//std::cout << "GameDiskFiles::getDefaultFontFiles()" << '\n';
	ResourceFileData& oFileData = m_oDefaultFontsData;
	if (!oFileData.m_bLoaded) {
		std::vector<std::string> aAllNames;
		std::vector<std::string> aAllPaths;
		// ex.: first "common/sounds/jointris" with recursive subfolders
		getFilesAndPathsOfFonts(s_sDefaultFontsFolder + "/" + m_sAppName, true, aAllNames, aAllPaths);
		// then "common/sounds" without subfolders
		getFilesAndPathsOfFonts(s_sDefaultFontsFolder, false, aAllNames, aAllPaths);
		// then "common/sounds/xxx" with recursive subfolders
		std::vector<std::string> aDirs;
		getDirSubTreeFiles(s_sDefaultFontsFolder, true, false, false, aDirs);
		for (const auto& sDir : aDirs) {
			if (sDir != m_sAppName) {
				getFilesAndPathsOfFonts(s_sDefaultFontsFolder + "/" + sDir, true, aAllNames, aAllPaths);
			}
		}
		assert(aAllNames.size() == aAllPaths.size());
		loadResourceFiles(oFileData, aAllNames, aAllPaths);
		oFileData.m_bLoaded = true;
	}
	return oFileData.m_aResourceFiles;
}

void GameDiskFiles::getThemeFiles(const std::string& sThemesFolder, std::vector<std::string>& aFiles)
{
//std::cout << "GameDiskFiles::getThemeFiles()  sThemesFolder=" << sThemesFolder << '\n';
	visitSysPathsAndSubdirs(sThemesFolder, true, aFiles, [](const std::string& sDirPath, const std::vector<std::string>& aSubDirNames
													, std::vector<std::string>& aFiles)
	{
//std::cout << "GameDiskFiles::getThemeFiles()    sDirPath=" << sDirPath << '\n';
		for (const std::string& sSubDirName : aSubDirNames) {
//std::cout << "GameDiskFiles::getThemeFiles()      sSubDirName=" << sSubDirName << '\n';
			const std::string sSubDirPath = sDirPath + "/" + sSubDirName;
			if (Glib::str_has_suffix(Glib::ustring{sSubDirPath}.uppercase(), Glib::ustring{s_sThemeXmlFolderExt}.uppercase())) {
				const std::string sThemeXmlPath = sSubDirPath + "/" + s_sThemeXmlFileName;
				try {
					const auto sFSPath = Glib::filename_from_utf8(sThemeXmlPath);
					if (Glib::file_test(sFSPath, Glib::FILE_TEST_EXISTS)) {
						if (Glib::file_test(sFSPath, Glib::FILE_TEST_IS_REGULAR)) {
							aFiles.push_back(sThemeXmlPath);
						}
					}
				} catch (const Glib::ConvertError&) {
				}
			}
		}
	});
}
void GameDiskFiles::loadResourceFiles(ResourceFileData& oFileData, const std::vector<std::string>& aNames, const std::vector<std::string>& aPaths)
{
	const int32_t nTotResources = static_cast<int32_t>(aPaths.size());
	assert(aNames.size() == aPaths.size());
	oFileData.m_aResourceFiles.reserve(nTotResources);
	for (int32_t nIdx = 0; nIdx < nTotResources; ++nIdx) {
		const std::string& sName = aNames[nIdx];
		const std::string& sPath = aPaths[nIdx];
		oFileData.m_aResourceFiles.emplace_back(sName, File{sPath});
	}
}
void GameDiskFiles::getDirSubTreeFiles(const std::string& sDirPath, bool bDirs, bool bRecurse, bool bRegularFiles
										, std::vector<std::string>& aFiles)
{
	getDirSubTreeFiles(sDirPath, bDirs, bRecurse, bRegularFiles, "", aFiles);
}
void GameDiskFiles::getDirSubTreeFiles(const std::string& sDirPath, bool bDirs, bool bRecurse, bool bRegularFiles
										, const std::string& sPrefix, std::vector<std::string>& aFiles)
{
	try {
		Glib::Dir oDir(Glib::filename_from_utf8(sDirPath));
		for (auto itFile = oDir.begin(); itFile != oDir.end(); ++itFile) {
			const std::string& sFile = *itFile;
			Glib::ustring sUtf8;
			try {
				sUtf8 = Glib::filename_to_utf8(sFile);
			} catch (const Glib::ConvertError&) {
				continue; // for----
			}
			const std::string sFilePath = Glib::filename_from_utf8(sDirPath + "/" + sUtf8);
			if (bRegularFiles) {
				const bool bIsRegular = Glib::file_test(sFilePath, Glib::FILE_TEST_IS_REGULAR);
				if (bIsRegular) {
					aFiles.push_back(sPrefix + sUtf8);
				}
			}
			if (bDirs || bRecurse) {
				const bool bIsDir = Glib::file_test(sFilePath, Glib::FILE_TEST_IS_DIR);
				if (bIsDir) {
					if (bDirs) {
						aFiles.push_back(sPrefix + sUtf8);
					}
					if (bRecurse) {
						getDirSubTreeFiles(sDirPath + "/" + sUtf8, bDirs, true, bRegularFiles, sPrefix + sUtf8 + "/", aFiles);
					}
				}
			}
		}
	} catch (const Glib::FileError&) {
	}
}
void GameDiskFiles::getAbsPathFromBaseAndRel(const std::string& sBaseDirPath, const std::vector<std::string>& aRelFiles
											, std::vector<std::string>& aAbsPathFiles)
{
	for (const std::string& sRelFile : aRelFiles) {
		aAbsPathFiles.push_back(sBaseDirPath + "/" + sRelFile);
	}
}
void GameDiskFiles::removeNonImageFiles(std::vector<std::string>& aFiles)
{
	aFiles.erase(std::remove_if(aFiles.begin(), aFiles.end(), [&](const std::string& sFile)
	{
		return ! isImageFile(sFile);
	}), aFiles.end());
}
void GameDiskFiles::removeNonSoundFiles(std::vector<std::string>& aFiles)
{
	aFiles.erase(std::remove_if(aFiles.begin(), aFiles.end(), [&](const std::string& sFile)
	{
		return ! isSoundFile(sFile);
	}), aFiles.end());
}
void GameDiskFiles::removeNonFontFiles(std::vector<std::string>& aFiles)
{
	aFiles.erase(std::remove_if(aFiles.begin(), aFiles.end(), [&](const std::string& sFile)
	{
		return ! isFontFile(sFile);
	}), aFiles.end());
}
bool GameDiskFiles::isImageFile(const std::string& sFile)
{
	const int32_t nTotExts = sizeof(s_aImageFileExt) / sizeof(s_aImageFileExt[0]);
	for (int32_t nIdx = 0; nIdx < nTotExts; ++nIdx) {
		const std::string::value_type* p0Ext = s_aImageFileExt[nIdx];
		if (Glib::str_has_suffix(Glib::ustring{sFile}.uppercase(), Glib::ustring{p0Ext}.uppercase())) {
			return true;
		}
	}
	return false;
}
bool GameDiskFiles::isSoundFile(const std::string& sFile)
{
	const int32_t nTotExts = sizeof(s_aSoundFileExt) / sizeof(s_aSoundFileExt[0]);
	for (int32_t nIdx = 0; nIdx < nTotExts; ++nIdx) {
		const std::string::value_type* p0Ext = s_aSoundFileExt[nIdx];
		if (Glib::str_has_suffix(Glib::ustring{sFile}.uppercase(), Glib::ustring{p0Ext}.uppercase())) {
			return true;
		}
	}
	return false;
}
bool GameDiskFiles::isFontFile(const std::string& sFile)
{
	const int32_t nTotExts = sizeof(s_aFontFileExt) / sizeof(s_aFontFileExt[0]);
	for (int32_t nIdx = 0; nIdx < nTotExts; ++nIdx) {
		const std::string::value_type* p0Ext = s_aFontFileExt[nIdx];
		if (Glib::str_has_suffix(Glib::ustring{sFile}.uppercase(), Glib::ustring{p0Ext}.uppercase())) {
			return true;
		}
	}
	return false;
}

const std::string& GameDiskFiles::getPrefsAndHighscoresBasePath() const
{
	return m_sUserWritablePath;
}
const std::vector< std::string >& GameDiskFiles::getGamesAndThemesBasePaths() const
{
	return m_aReadOnlyPaths;
}


} // namespace stmg
