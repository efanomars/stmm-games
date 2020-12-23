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
 * File:   stdtheme.cc
 */

#include "stdtheme.h"

#include "stdthemedrawingcontext.h"
#include "stdthemesound.h"
#include "modifiers/stopmodifier.h"
#include "gtkutil/image.h"
#include "modifiers/nextsubpaintermodifier.h"

#include <stmm-games/traitset.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/levelanimation.h>
#include <stmm-games/named.h>
#include <stmm-games/tile.h>
#include <stmm-games/util/recycler.h>
#include <stmm-games/util/util.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/intset.h>
#include <stmm-games/util/namedindex.h>

#include <stmm-input-au/playbackcapability.h>

#include <glibmm.h>
#include <cairomm/cairomm.h>
#include <pangomm/context.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <typeinfo>

namespace stmg { class ThemeContext; }
namespace stmg { class ThemeSound; }

namespace stmg { class ThemeAnimation; }
namespace stmg { class ThemeWidget; }
namespace stmg { class TileAni; }

namespace stmg
{

const std::string StdTheme::s_sSansFontDesc = "Sans";

StdTheme::StdTheme() noexcept
: m_sDefaultFont("")
, m_nDefaultPainterIdx(-1)
{
	m_oDefaultPalColor.setColorRGB(0,0,0);
	m_aPal256.resize(256);
//	m_aThemeStartBoardPP.push_back(0);
//	m_aThemeStartBlockPP.push_back(0);
}

NSize StdTheme::getBestTileSize(int32_t nHintTileW) const noexcept
{
	return m_oBoardTileSizing.getBest(nHintTileW);
}
double StdTheme::getTileWHRatio() const noexcept
{
	return m_oBoardTileSizing.m_fWHRatio;
}

void StdTheme::registerTileSize(int32_t nW, int32_t nH) noexcept
{
	registerTileSize(nW,nH, false);
}
void StdTheme::unregisterTileSize(int32_t nW, int32_t nH) noexcept
{
	registerTileSize(nW,nH, true);
}
void StdTheme::registerTileSize(int32_t nW, int32_t nH, bool bUn) noexcept
{
	for (TilePainter& oTP : m_aTilePainters) {
		for (auto& refMod : oTP.m_aModifiers) {
			if (bUn) {
				refMod->unregisterTileSize(nW,nH);
			} else {
				refMod->registerTileSize(nW,nH);
			}
		}
	}
	for (auto& oAssign : m_aAssign) {
		for (auto& refImg : oAssign.m_aImages) {
			if (bUn) {
				refImg->releaseCachedSize(nW, nH);
			} else {
				refImg->addCachedSize(nW, nH);
			}
		}
	}
}

bool StdTheme::addKnownImageFile(const std::string& sImgFileName, const File& oFile) noexcept
{
	assert(!sImgFileName.empty());
	NamedIndex& oImageFiles = m_oImageFileNames;
	if (oImageFiles.getIndex(sImgFileName) >= 0) {
		// already defined
		return false; //--------------------------------------------------------
	}
	const int32_t nIdx = oImageFiles.addName(sImgFileName);
	const size_t nTotImages = oImageFiles.size();
	if (nTotImages > m_aKnownImageFiles.size()) {
		m_aKnownImageFiles.resize(nTotImages);
		m_aImageByFileIdxs.resize(nTotImages);
	}
	m_aKnownImageFiles[nIdx] = oFile;
	//m_aImageByFileIdxs[nIdx] = not set yet, loaded lazily
	return true;
}
bool StdTheme::knowsImageFile(const std::string& sImgFileName) const noexcept
{
	assert(!sImgFileName.empty());
	const int32_t nIdx = m_oImageFileNames.getIndex(sImgFileName);
	return (nIdx >= 0);
}
const File& StdTheme::getKnownImageFile(const std::string& sImgFileName) const noexcept
{
	static const File s_oUndefinedFile{};
	assert(!sImgFileName.empty());
	const int32_t nIdx = m_oImageFileNames.getIndex(sImgFileName);
	if (nIdx < 0) {
		return s_oUndefinedFile;
	}
	return m_aKnownImageFiles[nIdx];
}
bool StdTheme::addKnownSoundFile(const std::string& sSndFileName, const File& oFile) noexcept
{
	assert(!sSndFileName.empty());
	NamedIndex& oSoundFiles = m_oSoundFileNames;
	if (oSoundFiles.getIndex(sSndFileName) >= 0) {
		// already defined
		return false; //--------------------------------------------------------
	}
	const int32_t nIdx = oSoundFiles.addName(sSndFileName);
	const size_t nTotSounds = oSoundFiles.size();
	if (nTotSounds > m_aKnownSoundFiles.size()) {
		m_aKnownSoundFiles.resize(nTotSounds);
	}
	m_aKnownSoundFiles[nIdx] = oFile;
	//m_aSoundByFileIdxs[nIdx] = not set yet, loaded lazily
	return true;
}
bool StdTheme::knowsSoundFile(const std::string& sSndFileName) const noexcept
{
	assert(!sSndFileName.empty());
	const int32_t nIdx = m_oSoundFileNames.getIndex(sSndFileName);
	return (nIdx >= 0);
}
const File& StdTheme::getKnownSoundFile(const std::string& sSndFileName) const noexcept
{
	static const File s_oUndefinedFile{};
	assert(!sSndFileName.empty());
	const int32_t nIdx = m_oSoundFileNames.getIndex(sSndFileName);
	if (nIdx < 0) {
		return s_oUndefinedFile;
	}
	return m_aKnownSoundFiles[nIdx];
}

const shared_ptr<Image>& StdTheme::getImageByFileName(const std::string& sImgFileName) noexcept
{
	assert(!sImgFileName.empty());
	NamedIndex& oImageFiles = m_oImageFileNames;
	const int32_t nImgFileIdx = oImageFiles.getIndex(sImgFileName);
	assert(nImgFileIdx >= 0);
	shared_ptr<Image>& refImage = m_aImageByFileIdxs[nImgFileIdx];
	if (refImage) {
		return refImage; //-----------------------------------------------------
	}
	const File& oImageFile = m_aKnownImageFiles[nImgFileIdx];
	refImage = std::make_shared<Image>(oImageFile);
	return refImage;
}
const shared_ptr<Image>& StdTheme::getImageById(const std::string& sImgId) noexcept
{
	const std::string& sImgFile = getImageIdFileName(sImgId);
	return getImageByFileName(sImgFile);
}
const shared_ptr<Image>& StdTheme::getImageById(int32_t nImgId) noexcept
{
	assert(nImgId >= 0);
	if (nImgId >= static_cast<int32_t>(m_aImageIdFileIdx.size())) {
		static shared_ptr<Image> s_refEmptyImg;
		return s_refEmptyImg;
	}
	const int32_t nImgFileIdx = m_aImageIdFileIdx[nImgId];
	shared_ptr<Image>& refImage = m_aImageByFileIdxs[nImgFileIdx];
	if (!refImage) {
		const File& oImageFile = m_aKnownImageFiles[nImgFileIdx];
		refImage = std::make_shared<Image>(oImageFile);
	}
	return refImage;
}
shared_ptr<Image> StdTheme::getSubImage(SubArrayData& oSubArrayData, int32_t nArrayIdx) noexcept
{
	assert(nArrayIdx < static_cast<int32_t>(oSubArrayData.m_aSubImages.size()));
	shared_ptr<Image>& refSubImage = oSubArrayData.m_aSubImages[nArrayIdx];
	if (!refSubImage) {
		assert(oSubArrayData.m_nPerRow > 0);
		const int32_t nImgPosY = (nArrayIdx / oSubArrayData.m_nPerRow) * (oSubArrayData.m_nImgH + oSubArrayData.m_nSpacingY);
		const int32_t nImgPosX = (nArrayIdx % oSubArrayData.m_nPerRow) * (oSubArrayData.m_nImgW + oSubArrayData.m_nSpacingX);
		refSubImage = std::make_shared<Image>(oSubArrayData.m_refImage, nImgPosX , nImgPosY
											, oSubArrayData.m_nImgW, oSubArrayData.m_nImgH);
	}
	return refSubImage;
}

void StdTheme::addColorName(const std::string& sColorName, uint8_t nR, uint8_t nG, uint8_t nB) noexcept
{
//std::cout << "StdTheme::addColorName  sColorName=" << sColorName << "  nR=" << 1u * nR << "  nG=" << 1u * nG << "  nB=" << 1u * nB << '\n';
	assert(!sColorName.empty());
	const int32_t nTotNamed = static_cast<int32_t>(m_aNamedColorIdx.size());
	NamedIndex& oNamedColors = m_oNamed.colors();
	int32_t nIdx = oNamedColors.getIndex(sColorName);
	if (nIdx >= 0) {
		if (nIdx < nTotNamed) {
			if (!m_aNamedColorIdx[nIdx].isEmpty()) {
				// redefinition is ignored
				return; //------------------------------------------------------
			}
		}
	} else {
		nIdx = oNamedColors.addName(sColorName);
	}
//std::cout << "StdTheme::addColorName  sColorName=" << sColorName << "  nIdx=" << nIdx << "  nR=" << 1u * nR << " nG=" << 1u * nG << " nB=" << 1u * nB << '\n';
	if (nIdx >= nTotNamed) {
		m_aNamedColorIdx.resize(nIdx + 1);
	}
	TileColor oColor;
	oColor.setColorRGB(nR, nG, nB);
	m_aNamedColorIdx[nIdx] = oColor;
}
bool StdTheme::getNamedColor(const std::string& sColorName, uint8_t& nR, uint8_t& nG, uint8_t& nB) const noexcept
{
//std::cout << "StdTheme::getNamedColor  sColorName=" << sColorName << '\n';
	assert(!sColorName.empty());
	const int32_t nIdx = m_oNamed.colors().getIndex(sColorName);
	if (nIdx < 0) {
		return false;
	}
	return getNamedColor(nIdx, nR, nG, nB);
}
bool StdTheme::getNamedColor(int32_t nIdx, uint8_t& nR, uint8_t& nG, uint8_t& nB) const noexcept
{
//std::cout << "StdTheme::getNamedColor  nIdx=" << nIdx << '\n';
	assert(nIdx >= 0);
	const int32_t nTotNamed = static_cast<int32_t>(m_aNamedColorIdx.size());
	if (nIdx >= nTotNamed) {
		return false; //--------------------------------------------------------
	}
	const TileColor& oColor = m_aNamedColorIdx[nIdx];
	if (oColor.isEmpty()) {
		return false; //--------------------------------------------------------
	}
	assert(oColor.getColorType() == TileColor::COLOR_TYPE_RGB);
	oColor.getColorRGB(nR, nG, nB);
//std::cout << "StdTheme::getNamedColor  nIdx=" << nIdx << "  nR=" << 1u * nR << " nG=" << 1u * nG << " nB=" << 1u * nB << '\n';
	return true;
}
void StdTheme::addColorPal(int32_t nPal, uint8_t nR, uint8_t nG, uint8_t nB) noexcept
{
	assert((nPal >= TileColor::COLOR_PAL_FIRST) && (nPal <= TileColor::COLOR_PAL_LAST));
	nPal -= TileColor::COLOR_PAL_FIRST; // pro forma (we know it's 0)
	if (nPal < 256) {
		if (!m_aPal256[nPal].isEmpty()) {
			return; //already set
		}
		m_aPal256[nPal].setColorRGB(nR, nG, nB);
	} else {
		std::map<int32_t, TileColor>::const_iterator it = m_oPalRest.find(nPal);
		if (it == m_oPalRest.end()) {
			TileColor oColor;
			oColor.setColorRGB(nR, nG, nB);
			m_oPalRest[nPal] = oColor;
		}
	}
}
void StdTheme::getPalColor(int32_t nPal, uint8_t& nR, uint8_t& nG, uint8_t& nB) const noexcept
{
	assert((nPal >= TileColor::COLOR_PAL_FIRST) && (nPal <= TileColor::COLOR_PAL_LAST));
	nPal -= TileColor::COLOR_PAL_FIRST; // pro forma (we know it's 0)
	if (nPal < 256) {
		const TileColor& oPal = m_aPal256[nPal];
		if (oPal.isEmpty()) {
			m_oDefaultPalColor.getColorRGB(nR, nG, nB);
		} else {
			oPal.getColorRGB(nR, nG, nB);
		}
//std::cout << "StdTheme::getPalColor  nPal=" << nPal << "  nR=" << 1u * nR << " nG=" << 1u * nG << " nB=" << 1u * nB << '\n';
	} else {
		std::map<int32_t, TileColor>::const_iterator it = m_oPalRest.find(nPal);
		if (it == m_oPalRest.end()) {
			m_oDefaultPalColor.getColorRGB(nR, nG, nB);
		} else {
			(it->second).getColorRGB(nR, nG, nB);
		}
	}
}
void StdTheme::getColorRgb(const TileColor& oColor
							, uint8_t& nR, uint8_t& nG, uint8_t& nB) const noexcept
{
//std::cout << "StdTheme::getColorRgb ";
	const TileColor::COLOR_TYPE eType = oColor.getColorType();
	if (eType == TileColor::COLOR_TYPE_EMPTY) {
//std::cout << "EMPTY";
		m_oDefaultPalColor.getColorRGB(nR, nG, nB);
	} else if (eType == TileColor::COLOR_TYPE_PAL) {
		const int32_t nPal = static_cast<int32_t>(oColor.getColorPal());
		getPalColor(nPal, nR, nG, nB);
//std::cout << "nPal=" << nPal;
	} else if (eType == TileColor::COLOR_TYPE_RGB) {
		oColor.getColorRGB(nR, nG, nB);
//std::cout << "RGB";
	} else { //if (eType == TileColor::COLOR_TYPE_INDEX)
		const int32_t nIdx = static_cast<int32_t>(oColor.getColorIndex());
//std::cout << "nIdx=" << nIdx;
		const bool bDef = getNamedColor(nIdx, nR, nG, nB);
		if (!bDef) {
			m_oDefaultPalColor.getColorRGB(nR, nG, nB);
		}
	}
//std::cout << '\n';
}

void StdTheme::addFontName(const std::string& sFontName, const std::string& sFontDesc) noexcept
{
	assert(!sFontName.empty());
	const int32_t nTotNamed = static_cast<int32_t>(m_aNamedFontDesc.size());
	NamedIndex& oNamedFonts = m_oNamed.fonts();
	int32_t nIdx = oNamedFonts.getIndex(sFontName);
	if (nIdx >= 0) {
		if (nIdx < nTotNamed) {
			if (!m_aNamedFontDesc[nIdx].empty()) {
				return; // redefinition is ignored
			}
		}
	} else {
		nIdx = oNamedFonts.addName(sFontName);
	}
	if (nIdx >= nTotNamed) {
		m_aNamedFontDesc.resize(nIdx + 1);
	}
	m_aNamedFontDesc[nIdx] = sFontDesc;
}
void StdTheme::setDefaultFont(const std::string& sFontDesc) noexcept
{
	if (!m_sDefaultFont.empty()) {
		return; // redefinition is ignored
	}
	m_sDefaultFont = sFontDesc;
}
bool StdTheme::getNamedFont(const std::string& sFontName, std::string& sFontDesc) noexcept
{
	assert(!sFontName.empty());
	const int32_t nIdx = m_oNamed.fonts().getIndex(sFontName);
	if (nIdx < 0) {
		return false;
	}
	return getNamedFont(nIdx, sFontDesc);
}
bool StdTheme::getNamedFont(int32_t nFontIdx, std::string& sFontDesc) noexcept
{
	assert(nFontIdx >= 0);
	const int32_t nTotNamed = static_cast<int32_t>(m_aNamedFontDesc.size());
	if (nFontIdx >= nTotNamed) {
		return false;
	}
	sFontDesc = m_aNamedFontDesc[nFontIdx];
	return !sFontDesc.empty();
}
bool StdTheme::isDefaultFontDefined() const noexcept
{
	return ! m_sDefaultFont.empty();
}
const std::string& StdTheme::getDefaultFont() const noexcept
{
	if (m_sDefaultFont.empty()) {
		return s_sSansFontDesc;
	} else {
		return m_sDefaultFont;
	}
}
const std::string& StdTheme::getFontDesc(int32_t nFontIdx) const noexcept
{
	assert(nFontIdx >= -1);
	if (nFontIdx >= 0) {
		const int32_t nTotNamed = static_cast<int32_t>(m_aNamedFontDesc.size());
		if (nFontIdx < nTotNamed) {
			const std::string& sNamedFont = m_aNamedFontDesc[nFontIdx];
			if (!sNamedFont.empty()) {
				return sNamedFont;
			}
		}
	}
	if (m_sDefaultFont.empty()) {
		return s_sSansFontDesc;
	} else {
		return m_sDefaultFont;
	}
}

bool StdTheme::hasImageId(const std::string& sImgId) const noexcept
{
	assert(!sImgId.empty());
	return (m_oNamed.images().getIndex(sImgId) >= 0);
}
const std::string& StdTheme::getImageIdFileName(const std::string& sImgId) const noexcept
{
	assert(!sImgId.empty());
	const int32_t nIdIdx = m_oNamed.images().getIndex(sImgId);
	if (nIdIdx < 0) {
		assert(false);
		return Util::s_sEmptyString;
	}
	assert(nIdIdx < static_cast<int32_t>(m_aImageIdFileIdx.size()));
	const int32_t nFileIdx = m_aImageIdFileIdx[nIdIdx];
	return m_oImageFileNames.getName(nFileIdx);
}
void StdTheme::addImageId(const std::string& sImgId, const std::string& sImgFileName) noexcept
{
	assert(!sImgId.empty());
	NamedIndex& oImageIds = m_oNamed.images();
	if (oImageIds.getIndex(sImgId) >= 0) {
		return;
	}
	const int32_t nIdIdx = oImageIds.addName(sImgId);
	const int32_t nFileIdx = m_oImageFileNames.getIndex(sImgFileName);
	assert(nFileIdx >= 0);
	const int32_t nTotIds = oImageIds.size();
	if (nTotIds > static_cast<int32_t>(m_aImageIdFileIdx.size())) {
		m_aImageIdFileIdx.resize(nTotIds, -1);
	}
	m_aImageIdFileIdx[nIdIdx] = nFileIdx;
}
bool StdTheme::hasSoundId(const std::string& sSndId) const noexcept
{
	assert(!sSndId.empty());
	return (m_oNamed.sounds().getIndex(sSndId) >= 0);
}
const std::string& StdTheme::getSoundIdFileName(const std::string& sSndId) const noexcept
{
	assert(!sSndId.empty());
	const int32_t nIdIdx = m_oNamed.sounds().getIndex(sSndId);
	if ((nIdIdx < 0) || (nIdIdx >= static_cast<int32_t>(m_aSoundIdFileIdx.size()))) {
		return Util::s_sEmptyString; //-----------------------------------------
	}
	const int32_t nFileIdx = m_aSoundIdFileIdx[nIdIdx];
	return m_oSoundFileNames.getName(nFileIdx);
}
void StdTheme::addSoundId(const std::string& sSndId, const std::string& sSndFileName) noexcept
{
	assert(!sSndId.empty());
	NamedIndex& oSoundIds = m_oNamed.sounds();
	if (oSoundIds.getIndex(sSndId) >= 0) {
		// already added
		return;
	}
	const int32_t nIdIdx = oSoundIds.addName(sSndId);
	const int32_t nFileIdx = m_oSoundFileNames.getIndex(sSndFileName);
	assert(nFileIdx >= 0);
	const int32_t nTotIds = oSoundIds.size();
	if (nTotIds > static_cast<int32_t>(m_aSoundIdFileIdx.size())) {
		m_aSoundIdFileIdx.resize(nTotIds, -1);
	}
	m_aSoundIdFileIdx[nIdIdx] = nFileIdx;
}

bool StdTheme::hasArray(const std::string& sArrayId) const noexcept
{
	assert(!sArrayId.empty());
	if (m_oSubArrayData.find(sArrayId) != m_oSubArrayData.end()) {
		return true;
	}
	return m_oFileArrayData.find(sArrayId) != m_oFileArrayData.end();
}
int32_t StdTheme::arraySize(const std::string& sArrayId) const noexcept
{
	assert(!sArrayId.empty());
	std::map<std::string, SubArrayData>::const_iterator itSA = m_oSubArrayData.find(sArrayId);
	if (itSA != m_oSubArrayData.end()) {
		const SubArrayData& oSubArray = itSA->second;
		return oSubArray.m_nArraySize;
	}
	std::map<std::string, FileArrayData>::const_iterator itFA = m_oFileArrayData.find(sArrayId);
	if (itFA != m_oFileArrayData.end()) {
		const FileArrayData& oFileArray = itFA->second;
		return static_cast<int32_t>(oFileArray.m_aImageArray.size());
	}
	return -1;
}

void StdTheme::addSubArray(const std::string& sArrayId, const std::string& sImgFileName
			, int32_t nImgW, int32_t nImgH, int32_t nSpacingX, int32_t nSpacingY
			, int32_t nPerRow, int32_t nArraySize) noexcept
{
	assert(knowsImageFile(sImgFileName));
	assert(! sArrayId.empty());
	assert((nImgW > 0) && (nImgH > 0));
	assert((nSpacingX >= 0) && (nSpacingY >= 0));
	assert(nPerRow > 0);
	assert(nArraySize > 0);
	if (hasArray(sArrayId)) {
		return; //--------------------------------------------------------------
	}
//std::cout << "StdTheme::addSubArray  sArrayId=" << sArrayId << "  nImgW=" << nImgW << " nPerRow=" << nPerRow << " nArraySize=" << nArraySize << '\n';
	m_oSubArrayData[sArrayId] = SubArrayData();
	SubArrayData& oArrayData = m_oSubArrayData[sArrayId];
	oArrayData.m_nImgW = nImgW;
	oArrayData.m_nImgH = nImgH;
	oArrayData.m_nSpacingX = nSpacingX;
	oArrayData.m_nSpacingY = nSpacingY;
	oArrayData.m_nPerRow = nPerRow;
	oArrayData.m_nArraySize = nArraySize;
	oArrayData.m_refImage = getImageByFileName(sImgFileName);
	oArrayData.m_aSubImages.resize(nArraySize);
}
void StdTheme::addFileArray(const std::string& sArrayId, const std::vector<std::string>& aFileArray) noexcept
{
	assert(! sArrayId.empty());
	assert(! aFileArray.empty());
	if (hasArray(sArrayId)) {
		return; //--------------------------------------------------------------
	}
	const auto nFileArraySize = aFileArray.size();
	m_oFileArrayData[sArrayId] = FileArrayData();
	FileArrayData& oArrayData = m_oFileArrayData[sArrayId];
	oArrayData.m_aImageArray.resize(nFileArraySize);
	int32_t nIdx = 0;
	for (auto& sImgFileName : aFileArray) {
		assert(knowsImageFile(sImgFileName));
		oArrayData.m_aImageArray[nIdx] = getImageByFileName(sImgFileName);
		++nIdx;
	}
}

bool StdTheme::hasTileAniId(const std::string& sId) const noexcept
{
	assert(!sId.empty());
	return (m_oTileAniIds.find(sId) != m_oTileAniIds.end());
}
void StdTheme::addTileAni(const std::string& sId, const shared_ptr<TileAni>& refTileAni) noexcept
{
	assert(refTileAni);
	assert(!sId.empty());
	if (m_oTileAniIds.find(sId) != m_oTileAniIds.end()) {
		return;
	}
	m_oTileAniIds[sId] = refTileAni;
}
const shared_ptr<TileAni>& StdTheme::getTileAni(const std::string& sId) noexcept
{
	assert(hasTileAniId(sId));
	return m_oTileAniIds[sId];
}

bool StdTheme::hasAnimationFactory(const std::string& sName) const noexcept
{
	assert(!sName.empty());
	return (m_oNamed.animations().getIndex(sName) >= 0);
}
void StdTheme::addAnimationFactory(const std::string& sName, unique_ptr<StdThemeAnimationFactory> refAnimationFactory
									, bool bAnonymousAnimations) noexcept
{
//std::cout << "StdTheme::addAnimationFactory()  sName=" << sName << '\n';
	assert(refAnimationFactory);
	assert(!sName.empty());
	if ((m_oNamed.animations().getIndex(sName) >= 0)) {
		return; //--------------------------------------------------------------
	}
	const int32_t nIdx = m_oNamed.animations().addName(sName);
	if (nIdx >= static_cast<int32_t>(m_aNamedAnimationFactories.size())) {
		m_aNamedAnimationFactories.resize(nIdx + 1);
	}
	if (bAnonymousAnimations) {
		m_aAnonymousModelAnimationFactories.push_back(refAnimationFactory.get());
	}
	m_aNamedAnimationFactories[nIdx] = std::move(refAnimationFactory);
}
StdThemeAnimationFactory* StdTheme::getAnimationFactory(const std::string& sName) noexcept
{
	const int32_t nIdx = m_oNamed.animations().getIndex(sName);
	if (nIdx < 0) {
		return nullptr;
	}
	assert(nIdx < static_cast<int32_t>(m_aNamedAnimationFactories.size()));
	return m_aNamedAnimationFactories[nIdx].get();
}
shared_ptr<ThemeAnimation> StdTheme::createAnimation(const shared_ptr<StdThemeContext>& refCtx, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
//std::cout << "StdTheme::createAnimation()" << '\n';
	assert(refLevelAnimation);

	const int32_t nTotFactoryNames = m_oNamed.animations().size();
	const int32_t nNameIdx = refLevelAnimation->getViewAnimationNameIdx();
//std::cout << "StdThemeContext::createAnimation()   nNameIdx=" << nNameIdx << "  nTotFactories=" << nTotFactoryNames << '\n';
	if ((nNameIdx >= 0) && (nNameIdx < nTotFactoryNames)) {
//std::cout << "        -> " << m_p1Owner->m_oNamed.animations().getName(nNameIdx) << '\n';
		auto& refAnimationFactory = m_aNamedAnimationFactories[nNameIdx];
		return refAnimationFactory->create(refCtx, refLevelAnimation); // ------
	}
//m_p1Owner->m_oNamed.animations().dump();
//std::cout << "      LevelAnimation  -> NO NAME" << '\n';
	LevelAnimation* p0LevelAnimation = refLevelAnimation.operator->();
	const auto nIdType = typeid(*p0LevelAnimation).hash_code();
//std::cout << "                 ::createAnimation() typeid(*p0LevelAnimation).name=" << typeid(*p0LevelAnimation).name() << '\n';
	std::unordered_map< size_t, std::vector<StdThemeAnimationFactory*> >& oTypeIdFactories = m_oAnimationTypeIdFactories;
	auto itTypeFactories = oTypeIdFactories.find(nIdType);
	const bool bTypeFactories = (itTypeFactories != oTypeIdFactories.end());
	if (bTypeFactories) {
		// Beware! It's just the hash_code that is the same, the id might actually differ!
//std::cout << "                 ::createAnimation() typeid factories  nIdType=" << nIdType << '\n';
		std::vector<StdThemeAnimationFactory*>& aFactories = itTypeFactories->second;
		for (auto& p0F : aFactories) {
			shared_ptr<ThemeAnimation> refAnimation = p0F->create(refCtx, refLevelAnimation);
			if (refAnimation) {
				return refAnimation; //-----------------------------------------
			}
		}
	}
	std::vector< StdThemeAnimationFactory* >& aAnonymFactories = m_aAnonymousModelAnimationFactories;
//std::cout << "                 ::createAnimation() all" << '\n';
	for (auto& p0Factory : aAnonymFactories) {
//std::cout << "                 ::createAnimation() factory" << '\n';
		shared_ptr<ThemeAnimation> refAnimation = p0Factory->create(refCtx, refLevelAnimation);
		const bool bCreated = refAnimation.operator bool();
		if (bCreated || p0Factory->supports(refLevelAnimation)) {
			if (bTypeFactories) {
				std::vector<StdThemeAnimationFactory*>& aFactories = itTypeFactories->second;
				aFactories.push_back(p0Factory);
			} else {
				std::vector<StdThemeAnimationFactory*> aFactories( {p0Factory} );
				oTypeIdFactories.insert(std::make_pair(nIdType, std::move(aFactories)));
			}
			if (bCreated) {
				return refAnimation; //-----------------------------------------
			}
		}
	}
	return shared_ptr<ThemeAnimation>();
}
int32_t StdTheme::getCachedFileIdFromCapaAndSoundIdx(int32_t nSoundIdx, int32_t nCapaId) noexcept
{
	int32_t nFileId = -1;
	if (static_cast<int32_t>(m_aCachedFileIds.size()) <= nSoundIdx) {
		m_aCachedFileIds.resize(nSoundIdx + 1);
	} else {
		auto& aCapaFileIds = m_aCachedFileIds[nSoundIdx];
		const auto itFind = std::find_if(aCapaFileIds.begin(), aCapaFileIds.end(), [&](const CapaToFileId& oCapaToFileId)
		{
			return (oCapaToFileId.m_nCapabilityId == nCapaId);
		});
		if (itFind != aCapaFileIds.end()) {
			nFileId = itFind->m_nFileId; // was cached
		}
	}
	return nFileId;
}
shared_ptr<ThemeSound> StdTheme::createSound(StdThemeContext* p0Ctx
											, int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks
											, FPoint oXYPosition, double fZPosition, bool bRelative
											, double fVolume, bool bLoop) noexcept
{
	assert(! aPlaybacks.empty());
	assert(static_cast<int32_t>(m_aSoundIdFileIdx.size()) == m_oNamed.sounds().size());
//std::cout << "StdTheme::createSound nSoundIdx = " << nSoundIdx << "  tot = " << static_cast<int32_t>(m_aSoundIdFileIdx.size()) << '\n';
	if (nSoundIdx >= static_cast<int32_t>(m_aSoundIdFileIdx.size())) {
		return shared_ptr<ThemeSound>{}; //-------------------------------------
	}
	assert(nSoundIdx >= 0);
	std::vector< int32_t > aSoundIds;
	int32_t nTotValidSounds = 0;
	const int32_t nTotCapas = aPlaybacks.size();
	const double fX = oXYPosition.m_fX * p0Ctx->m_fSoundScaleX;
	const double fY = oXYPosition.m_fY * p0Ctx->m_fSoundScaleY;
	const double fZ = fZPosition * p0Ctx->m_fSoundScaleZ;
//std::cout << "StdTheme::createSound fX = " << fX << "  fY = " << fY << "  bRelative=" << bRelative << '\n';
	aSoundIds.reserve(nTotCapas);
	for (int32_t nIdxCapa = 0; nIdxCapa < nTotCapas; ++nIdxCapa) {
		auto& refCapa = aPlaybacks[nIdxCapa];
		if (! refCapa) {
			continue;
		}
		const int32_t nCapaId = refCapa->getId();
		int32_t nFileId = getCachedFileIdFromCapaAndSoundIdx(nSoundIdx, nCapaId);
		int32_t nSoundId;
		if (nFileId >= 0) {
			nSoundId = refCapa->playSound(nFileId, fVolume, bLoop, bRelative, fX, fY, fZ);
//std::cout << "StdTheme::createSound nSoundId = " << nSoundId << "  with nFileId = " << nFileId << '\n';
		} else {
			const int32_t nFileIdx = m_aSoundIdFileIdx[nSoundIdx];
			const File& oFile = m_aKnownSoundFiles[nFileIdx];
			if (oFile.isBuffered()) {
				stmi::PlaybackCapability::SoundData oData = refCapa->playSound(oFile.getBuffer(), oFile.getBufferSize(), fVolume, bLoop, bRelative, fX, fY, fZ);
				nSoundId = oData.m_nSoundId;
				nFileId = oData.m_nFileId;
			} else {
				stmi::PlaybackCapability::SoundData oData = refCapa->playSound(oFile.getFullPath(), fVolume, bLoop, bRelative, fX, fY, fZ);
				nSoundId = oData.m_nSoundId;
				nFileId = oData.m_nFileId;
//std::cout << "StdTheme::createSound FIRST nSoundId = " << nSoundId << "  with nFileId = " << nFileId << '\n';
			}
			if (nFileId >= 0) {
				auto& aCapaFileIds = m_aCachedFileIds[nSoundIdx];
				aCapaFileIds.emplace_back(CapaToFileId{nCapaId, nFileId});
			}
		}
		aSoundIds.push_back(nSoundId); // nSoundId can be -1 !
		if (nSoundId >= 0) {
			++nTotValidSounds;
		}
	}
	shared_ptr<StdThemeSound> refStdThemeSound;
	if (nTotValidSounds > 0) {
		m_oThemeSounds.create(refStdThemeSound, this, p0Ctx, aPlaybacks, std::move(aSoundIds), bRelative);
	}
	return refStdThemeSound;
}
void StdTheme::preloadSound(int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks) noexcept
{
	if (nSoundIdx >= static_cast<int32_t>(m_aSoundIdFileIdx.size())) {
		return; //--------------------------------------------------------------
	}
	const int32_t nTotCapas = aPlaybacks.size();
	for (int32_t nIdxCapa = 0; nIdxCapa < nTotCapas; ++nIdxCapa) {
		auto& refCapa = aPlaybacks[nIdxCapa];
		if (! refCapa) {
			continue;
		}
		const int32_t nCapaId = refCapa->getId();
		int32_t nFileId = getCachedFileIdFromCapaAndSoundIdx(nSoundIdx, nCapaId);
		if (nFileId >= 0) {
			// already cached (and loaded)
			continue;
		}
		const int32_t nFileIdx = m_aSoundIdFileIdx[nSoundIdx];
		const File& oFile = m_aKnownSoundFiles[nFileIdx];
		if (oFile.isBuffered()) {
			nFileId = refCapa->preloadSound(oFile.getBuffer(), oFile.getBufferSize());
		} else {
			nFileId = refCapa->preloadSound(oFile.getFullPath());
		}
		auto& aCapaFileIds = m_aCachedFileIds[nSoundIdx];
		if (nFileId >= 0) {
			aCapaFileIds.emplace_back(CapaToFileId{nCapaId, nFileId});
		}
	}
}

bool StdTheme::hasWidgetFactory(const std::string& sName) const noexcept
{
	assert(!sName.empty());
	return (m_oNamed.widgets().getIndex(sName) >= 0);
}
void StdTheme::addWidgetFactory(const std::string& sName, unique_ptr<StdThemeWidgetFactory> refWidgetFactory
								, bool bAnonymousWidgets) noexcept
{
//std::cout << "StdTheme::addWidgetFactory()  sName=" << sName << '\n';
	assert(refWidgetFactory);
	assert(!sName.empty());
	if ((m_oNamed.widgets().getIndex(sName) >= 0)) {
		return; //--------------------------------------------------------------
	}
	const int32_t nIdx = m_oNamed.widgets().addName(sName);
	if (nIdx >= static_cast<int32_t>(m_aNamedWidgetFactories.size())) {
		m_aNamedWidgetFactories.resize(nIdx + 1);
	}
	if (bAnonymousWidgets) {
		m_aAnonymousModelWidgetFactories.push_back(refWidgetFactory.get());
	}
	m_aNamedWidgetFactories[nIdx] = std::move(refWidgetFactory);
}
StdThemeWidgetFactory* StdTheme::getWidgetFactory(const std::string& sName) noexcept
{
	const int32_t nIdx = m_oNamed.widgets().getIndex(sName);
	if (nIdx < 0) {
		return nullptr;
	}
	assert(nIdx < static_cast<int32_t>(m_aNamedWidgetFactories.size()));
	return m_aNamedWidgetFactories[nIdx].get();
}
shared_ptr<ThemeWidget> StdTheme::createWidget(const shared_ptr<GameWidget>& refGameWidget, double fTileWHRatio
												, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept
{
//std::cout << "StdTheme::createWidget()" << '\n';
	assert(refGameWidget);

	const int32_t nTotFactoryNames = m_oNamed.widgets().size();
	const int32_t nNameIdx = refGameWidget->getViewWidgetNameIdx();
	if ((nNameIdx >= 0) && (nNameIdx < nTotFactoryNames)) {
		auto& refWidgetFactory = m_aNamedWidgetFactories[nNameIdx];
		return refWidgetFactory->create(refGameWidget, fTileWHRatio, refFontContext); // --------- exit
	}
	std::vector< StdThemeWidgetFactory* >& aAnonymFactories = m_aAnonymousModelWidgetFactories;
	for (auto& p0Factory : aAnonymFactories) {
		shared_ptr<ThemeWidget> refWidget = p0Factory->create(refGameWidget, fTileWHRatio, refFontContext);
		if (refWidget) {
			return refWidget; //------------------------------------------------
		}
	}
	const auto& oGameWidget = *refGameWidget;
	std::cout << "StdTheme: No factory can create theme widget from game widget of type '"
				<< typeid(oGameWidget).name() << "'" << '\n';
	return shared_ptr<ThemeWidget>{};
}
void StdTheme::removeCapability(int32_t nCapabilityId) noexcept
{
	// clear cache
	for (auto& aSoundFileIds : m_aCachedFileIds) {
		aSoundFileIds.erase(std::remove_if(aSoundFileIds.begin(), aSoundFileIds.end(),
								[&](const CapaToFileId& oCapaToFileId)
								{
									return (oCapaToFileId.m_nCapabilityId == nCapabilityId);
								}), aSoundFileIds.end());
	}
}

int32_t StdTheme::getAssignId(const std::string& sAssId) const noexcept
{
	return m_oAssignIds.getIndex(sAssId);
}
bool StdTheme::hasAssignId(const std::string& sAssId) const noexcept
{
	return (getAssignId(sAssId) >= 0);
}
bool StdTheme::hasAssignId(int32_t nIdAss) const noexcept
{
	return (nIdAss >= 0) && (nIdAss < static_cast<int32_t>(m_oAssignIds.size()));
}
const std::string& StdTheme::getAssignId(int32_t nIdAss) const noexcept
{
	assert((nIdAss >= 0) && (nIdAss < static_cast<int32_t>(m_oAssignIds.size())));
	return m_oAssignIds.getName(nIdAss);
}
int32_t StdTheme::addAssign(const std::string& sAssId
							, std::unique_ptr<IntSet>&& refPlayerTraitSet, bool bPlayerFirst
							, std::vector< std::unique_ptr<TraitSet> >&& aTileTraitSets
							, const std::string& sArrayId, int32_t nArrayFromIdx) noexcept
{
//std::cout << "StdTheme::addAssign()  arrayId=" << sArrayId << '\n';
	assert(!sArrayId.empty());
	assert(nArrayFromIdx >= 0);
	assert(!sAssId.empty());
	const int32_t nFindId = getAssignId(sAssId);
	if (nFindId >= 0) {
		// name already used
		return nFindId; //------------------------------------------------------
	}

	const int32_t nAssId = m_oAssignIds.addName(sAssId);
	assert(static_cast<int32_t>(m_aAssign.size()) == nAssId);
	m_aAssign.emplace_back(AssignData{});
	AssignData& oAssign = m_aAssign.back();

	oAssign.m_aTileTraits = std::move(aTileTraitSets);
	oAssign.m_refPlayerTrait = std::move(refPlayerTraitSet);
	oAssign.m_bPlayerFirst = bPlayerFirst;
	oAssign.m_nTotValues = 1;

	if (oAssign.m_refPlayerTrait) {
		const int32_t nTotValues = oAssign.m_refPlayerTrait->size();
		assert(nTotValues > 0);
		oAssign.m_nTotValues *= nTotValues;
	}
	for (auto& refTileTraitSet : oAssign.m_aTileTraits) {
		const int32_t nTotValues = refTileTraitSet->getTotValues();
		oAssign.m_aTileTraitsCaches.push_back(std::make_pair(refTileTraitSet->hasEmptyValue(), nTotValues));
		assert(nTotValues > 0);
		oAssign.m_nTotValues *= nTotValues;
	}
	assert(oAssign.m_nTotValues > 0);
	std::vector< shared_ptr<Image> >& aImages = oAssign.m_aImages;
	aImages.resize(oAssign.m_nTotValues);
	if (m_oSubArrayData.find(sArrayId) != m_oSubArrayData.end()) {
		SubArrayData& oSubArrayData = m_oSubArrayData[sArrayId];
		assert(nArrayFromIdx + oAssign.m_nTotValues <= oSubArrayData.m_nArraySize);
		for (int32_t nIdx = 0; nIdx < oAssign.m_nTotValues; ++nIdx) {
			aImages[nIdx] = getSubImage(oSubArrayData, nArrayFromIdx + nIdx);
		}
	} else if (m_oFileArrayData.find(sArrayId) != m_oFileArrayData.end()) {
		const FileArrayData& oFileArrayData = m_oFileArrayData[sArrayId];
		assert(nArrayFromIdx + oAssign.m_nTotValues <= static_cast<int32_t>(oFileArrayData.m_aImageArray.size()));
		for (int32_t nIdx = 0; nIdx < oAssign.m_nTotValues; ++nIdx) {
			aImages[nIdx] = oFileArrayData.m_aImageArray[nArrayFromIdx + nIdx];
		}
	} else {
		assert(false);
	}
	return nAssId;
}
shared_ptr<Image> StdTheme::getAssignImage(int32_t nIdAss, const Tile& oTile, int32_t nPlayer) const noexcept
{
	assert((nIdAss >= 0) && (nIdAss < static_cast<int32_t>(m_aAssign.size())));
	const shared_ptr<Image> refEmpty;
	const AssignData& oAssign = m_aAssign[nIdAss];
	int32_t nImageIdx = 0;
	if (oAssign.m_bPlayerFirst) {
		if (oAssign.m_refPlayerTrait) {
			const int32_t nIdx = oAssign.m_refPlayerTrait->getIndexOfValue(nPlayer);
			if (nIdx < 0) {
				return refEmpty; //---------------------------------------------
			}
			nImageIdx = nIdx;
		} else {
			// player doesn't participate in the image idx
		}
	}
	const int32_t nTotTraitSets = static_cast<int32_t>(oAssign.m_aTileTraits.size());
	for (int32_t nTraitSet = 0; nTraitSet < nTotTraitSets; ++nTraitSet) {
		const auto& refTraitSet = oAssign.m_aTileTraits[nTraitSet];
		//
		const int32_t nIdx = refTraitSet->getIndexOfTileTraitValue(oTile);
		if (nIdx <= -2) {
			return refEmpty; //-------------------------------------------------
		}
		const auto& oPair = oAssign.m_aTileTraitsCaches[nTraitSet];
		const bool bHasEmptyValue = oPair.first;
		const int32_t nTotValues = oPair.second;
		nImageIdx *= nTotValues;
		nImageIdx += nIdx + (bHasEmptyValue ? 1 : 0); // empty value has idx = -1
	}
	if (!oAssign.m_bPlayerFirst) {
		if (oAssign.m_refPlayerTrait) {
			const int32_t nIdx = oAssign.m_refPlayerTrait->getIndexOfValue(nPlayer);
			if (nIdx < 0) {
				return refEmpty; //---------------------------------------------
			}
			const int32_t nTotValues = oAssign.m_refPlayerTrait->size();
			nImageIdx *= nTotValues;
			nImageIdx += nIdx;
		} else {
			// player doesn't participate in the image idx
		}
	}
	assert((nImageIdx >= 0) && (nImageIdx < oAssign.m_nTotValues));
	return oAssign.m_aImages[nImageIdx];
}


int32_t StdTheme::addPainter(const std::string& sPainterName, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
//std::cout << "StdTheme::addPainter " << sPainterName << '\n';
	assert(!sPainterName.empty());
	int32_t nPainterIdx = m_oNamed.painters().getIndex(sPainterName);
	if (nPainterIdx < 0) {
		nPainterIdx = m_oNamed.painters().addName(sPainterName);
		m_aTilePainters.resize(nPainterIdx + 1);
	}
	TilePainter& oTP = m_aTilePainters[nPainterIdx];
	if (oTP.m_bFinished) {
		// unreachable
		return nPainterIdx; //--------------------------------------------------
	}
	const size_t nStartPP = oTP.m_aModifiers.size();
	const size_t nNextPP = nStartPP + aModifiers.size() + 1;
	oTP.m_aModifiers.reserve(nNextPP);
	bool bHasNextSubPainter = false;
	for (auto& refMod : aModifiers) {
		auto p0NextSubPainter = dynamic_cast<NextSubPainterModifier*>(refMod.get());
		if (p0NextSubPainter != nullptr) {
			p0NextSubPainter->m_nPPOfNextSubPainter = nNextPP;
			bHasNextSubPainter = true;
		}
		oTP.m_aModifiers.push_back(std::move(refMod));
	}
	auto refStopModifier = std::make_unique<StopModifier>(this);
	oTP.m_aModifiers.push_back(std::move(refStopModifier));
	oTP.m_bFinished = ! bHasNextSubPainter;

	return nPainterIdx;
}
void StdTheme::setDefaultPainter(int32_t nPainterIdx) noexcept
{
	assert(nPainterIdx >= 0);
	assert(m_oNamed.painters().isIndex(nPainterIdx));
	if (m_nDefaultPainterIdx < 0) {
		m_nDefaultPainterIdx = nPainterIdx;
	}
}
int32_t StdTheme::getDefaultPainterIdx() noexcept
{
	if (m_nDefaultPainterIdx < 0) {
		if (m_aTilePainters.empty()) {
			// no painters available
			return -1;
		}
		return 0;
	}
	return m_nDefaultPainterIdx;
}

int32_t StdTheme::getVariableIndex(const std::string& sVariableName) noexcept
{
	return m_oVariableNames.addName(sVariableName);
}
int32_t StdTheme::getTotVariableIndexes() const noexcept
{
	return m_oVariableNames.size();
}
const std::string& StdTheme::getVariableNameFromIndex(int32_t nVariableIdx) const noexcept
{
	return m_oVariableNames.getName(nVariableIdx);
}
shared_ptr<ThemeContext> StdTheme::createContext(NSize oTileWH, bool bRegister, double fSoundScaleX, double fSoundScaleY, double fSoundScaleZ
												, const Glib::RefPtr<Pango::Context>& refFontContext, RuntimeVariablesEnv* p0RuntimeVariablesEnv) noexcept
{
//std::cout << "StdTheme::createContext()" << '\n';
	const int32_t& nTileW = oTileWH.m_nW;
	const int32_t& nTileH = oTileWH.m_nH;
	// Create not animated vector
	const int32_t nTotTileAnis = m_oNamed.tileAnis().size();
	m_aNoAniElapsed.resize(nTotTileAnis, -1.0);

	shared_ptr<PrivateStdThemeContext> refNew;
	m_oContexts.create(refNew, p0RuntimeVariablesEnv);
	refNew->m_p1Owner = this;
	refNew->m_nTileW = nTileW;
	refNew->m_nTileH = nTileH;
	refNew->m_fSoundScaleX = fSoundScaleX;
	refNew->m_fSoundScaleY = fSoundScaleY;
	refNew->m_fSoundScaleZ = fSoundScaleZ;
	refNew->m_refFontContext = refFontContext;

	if (bRegister) {
		registerTileSize(nTileW, nTileH);
	}
	return refNew;
}
void StdTheme::drawTile(int32_t nPainterIdx, const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeContext& oTc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
//std::cout << "StdTheme::drawTile()" << '\n';
//if ((!oTile.isEmpty()) && (!oTile.getTileChar().isEmpty()) && oTile.getTileFont().isDefined()) {
//std::cout << "StdTheme::drawTile()"; dumpTile(oTile);
////dumpNames(true, false, true);
//}
	assert(nPainterIdx >= 0);
	drawTileFromPP(0, refCc, oTc.m_oDrawingContext, oTile, nPlayer, aAniElapsed, m_aTilePainters[nPainterIdx].m_aModifiers);
}
void StdTheme::drawTileFromPP(size_t nPP, const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oTc
							, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed
							, std::vector< unique_ptr<StdThemeModifier> >& aModifiers) noexcept
{
	const auto nTotModifiers = aModifiers.size();
	while (nPP < nTotModifiers) {
		auto& refMod = aModifiers[nPP];
		const StdThemeModifier::FLOW_CONTROL eCtl = refMod->drawTile(refCc, oTc, oTile, nPlayer, aAniElapsed);
		if (eCtl == StdThemeModifier::FLOW_CONTROL_STOP) {
			break; // while -------------
		}
		++nPP;
	}
}
void StdTheme::drawTileFromPP(size_t nPP, const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oTc
							, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
	const int32_t nPainterIdx = oTc.m_p1Owner->m_nDrawingPainterIdx;
	auto& aModifiers = m_aTilePainters[nPainterIdx].m_aModifiers;
	drawTileFromPP(nPP, refCc, oTc, oTile, nPlayer, aAniElapsed, aModifiers);
}

#ifndef NDEBUG
void StdTheme::dumpTile(const Tile& oTile) const noexcept
{
	if (oTile.isEmpty()) {
		std::cout << "EMPTY" << '\n';
		return;
	}
	std::cout << "(";
	const TileChar& oChar = oTile.getTileChar();
	if (oChar.isEmpty()) {
		std::cout << "EMPTY";
	} else if (oChar.isCharIndex()) {
		std::cout << "i" << oChar.getCharIndex();
	} else {
		std::cout << "v" << oChar.getChar();
	}
	std::cout << ",";
	const TileColor& oColor = oTile.getTileColor();
	if (oColor.isEmpty()) {
		std::cout << "EMPTY";
	} else if (oColor.getColorType() == TileColor::COLOR_TYPE_INDEX) {
		std::cout << "i" << oColor.getColorIndex();
	} else if (oColor.getColorType() == TileColor::COLOR_TYPE_PAL) {
		std::cout << "p" << oColor.getColorPal();
	} else {
		if (oColor.getColorType() == TileColor::COLOR_TYPE_RGB) {
			uint8_t nR, nG, nB;
			oColor.getColorRGB(nR, nG, nB);
			std::cout << "rgb(" << static_cast<int32_t>(nR) << static_cast<int32_t>(nG) << static_cast<int32_t>(nB) << ")";
		} else {
			assert(false);
		}
	}
	std::cout << ",";
	const TileFont& oFont = oTile.getTileFont();
	if (oFont.isEmpty()) {
		std::cout << "UNDEF";
	} else {
		std::cout << "i" << oFont.getFontIndex();
	}
	std::cout << ")" << '\n';
}
void StdTheme::dumpNames(bool bDumpCharNames, bool bDumpColorNames, bool bDumpFontNames) const noexcept
{
	if (bDumpCharNames) {
		std::cout << "StdTheme::dumpNames()  Char[i]= NAME" << '\n';
		const int32_t nNamedChars = getNamed().chars().size();
		for (int32_t nChar = 0; nChar < nNamedChars; ++nChar) {
			std::cout << "    Char[" << nChar << "]= " << getNamed().chars().getName(nChar) << '\n';
		}
	}
	if (bDumpColorNames) {
		std::cout << "StdTheme::dumpNames()  Color[i]= NAME" << '\n';
		const int32_t nNamedColors = getNamed().colors().size();
		for (int32_t nColor = 0; nColor < nNamedColors; ++nColor) {
			std::cout << "    Color[" << nColor << "]= " << getNamed().colors().getName(nColor) << '\n';
		}
	}
	if (bDumpFontNames) {
		std::cout << "StdTheme::dumpNames()  Font[i]= NAME" << '\n';
		const int32_t nNamedFonts = getNamed().fonts().size();
		for (int32_t nFont = 0; nFont < nNamedFonts; ++nFont) {
			std::cout << "    Font[" << nFont << "]= " << getNamed().fonts().getName(nFont) << '\n';
		}
	}
}
#endif

} // namespace stmg
