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
 * File:   fontconfigloader.cc
 */

#include "fontconfigloader.h"

#include <string>
#include <memory>
#include <cassert>
#include <iostream>

#include <fontconfig/fontconfig.h>

#include <stdint.h>

namespace stmg
{

unique_ptr<FontConfigLoader> FontConfigLoader::create() noexcept
{
	static_assert(NULL == nullptr, "");
	//
	::FcBool bOk = ::FcInit();
	if (bOk == FcFalse) {
		std::cout << "Error: Could not initialize fontconfig" << '\n';
		return unique_ptr<FontConfigLoader>{};
	}
	::FcConfig* p0TheFonts = ::FcInitLoadConfigAndFonts();
	if (p0TheFonts == nullptr) {
		std::cout << "Error: Could not load fontconfig fonts" << '\n';
		return unique_ptr<FontConfigLoader>{};
	}
	bOk = ::FcConfigSetRescanInterval(nullptr, 0);
	if (bOk == FcFalse) {
		std::cout << "Error: Could not set fontconfig rescan interval" << '\n';
		return unique_ptr<FontConfigLoader>{};
	}
	auto refInstance = unique_ptr<FontConfigLoader>(new FontConfigLoader());
	return refInstance;
}
FontConfigLoader::~FontConfigLoader()
{
	//::FcFini();
}

static std::string slantToString(int32_t nSlant) noexcept
{
	if (nSlant <= 0) {
		return ""; //"roman";
	} else if (nSlant <= 100){
		return "italic";
	} else {
		return "oblique";
	}
}
static std::string weightToString(int32_t nWeight) noexcept
{
	if (nWeight <= 0) {
		return "thin";
	} else if (nWeight <= 40){
		return "ultralight";
	} else if (nWeight <= 50){
		return "light";
	} else if (nWeight <= 75){
		return "book";
	} else if (nWeight <= 80){
		return "regular";
	} else if (nWeight <= 100){
		return "medium";
	} else if (nWeight <= 180){
		return "semibold";
	} else if (nWeight <= 200){
		return "bold";
	} else if (nWeight <= 205){
		return "extrabold";
	} else {
		return "heavy";
	}
}

static std::string findAppFontFile(const std::string& sFontFilePath) noexcept
{
	static_assert(sizeof(int) <= sizeof(int32_t), "");

	::FcFontSet* p0AppFontSet = ::FcConfigGetFonts(nullptr, ::FcSetApplication);
	if (p0AppFontSet == nullptr) {
		return "";
	}
	const int32_t nTotAppFonts = p0AppFontSet->nfont;
//std::cout << "App specific fonts: " << nTotAppFonts << "  (" << p0AppFontSet->sfont << ")" << '\n';
	for (int32_t nIdx = 0; nIdx < nTotAppFonts; ++nIdx) {
		::FcPattern* p0Pattern = p0AppFontSet->fonts[nIdx];
//std::cout << "  Pattern " << nIdx << "  " << '\n';
//::FcPatternPrint(p0Pattern);
		::FcObjectSet* pObjectSet = ::FcObjectSetBuild(FC_FAMILY/*, FC_STYLE, FC_FULLNAME*/, FC_FILE
														, FC_SLANT, FC_WEIGHT, NULL);
		if (pObjectSet == nullptr) {
			continue;
		}
		::FcFontSet* pFontSet = ::FcFontList(NULL, p0Pattern, pObjectSet);
		if (pFontSet == nullptr) {
			::FcObjectSetDestroy(pObjectSet);
			continue;
		}
		std::string sFamily;
		//std::string sStyle;
		//std::string sFullName;
		std::string sFilePath;
		FcChar8* p0Family;
		//FcChar8* p0Style;
		//FcChar8* p0FullName;
		FcChar8* p0FilePath;
		int nSlant = 0;
		int nWeight = 80;
		//
		const int32_t nPropIdx = 0;
		if (::FcPatternGetString(pFontSet->fonts[nPropIdx], FC_FAMILY, 0, &p0Family) == FcResultMatch) {
			sFamily = std::string{reinterpret_cast<const char*>(p0Family)};
		}
		//if (::FcPatternGetString(pFontSet->fonts[nPropIdx], FC_STYLE, 0, &p0Style) == FcResultMatch) {
		//	sStyle = std::string{reinterpret_cast<const char*>(p0Style)};
		//}
		//if (::FcPatternGetString(pFontSet->fonts[nPropIdx], FC_FULLNAME, 0, &p0FullName) == FcResultMatch) {
		//	sFullName = std::string{reinterpret_cast<const char*>(p0FullName)};
		//}
		if (::FcPatternGetInteger(pFontSet->fonts[nPropIdx], FC_SLANT, 0, &nSlant) == FcResultMatch) {
			nSlant = nSlant;
		}
		if (::FcPatternGetInteger(pFontSet->fonts[nPropIdx], FC_WEIGHT, 0, &nWeight) == FcResultMatch) {
			nWeight = nWeight;
		}
		if (::FcPatternGetString(pFontSet->fonts[nPropIdx], FC_FILE, 0, &p0FilePath) == FcResultMatch) {
			sFilePath = std::string{reinterpret_cast<const char*>(p0FilePath)};
		}
		//
		::FcObjectSetDestroy(pObjectSet);
		::FcFontSetDestroy(pFontSet);
		if (sFilePath == sFontFilePath) {
			// found the font
			const std::string sFontDesc = sFamily
							+ " " + slantToString(nSlant)
							+ " " + weightToString(nWeight);
//std::cout << "      sFontDesc: " << sFontDesc << '\n';
			return sFontDesc;
		}
	}
	return "";
}
std::string FontConfigLoader::addAppFontFile(const std::string& sFontFilePath) noexcept
{
	const std::string sDefine = findAppFontFile(sFontFilePath);
	if (! sDefine.empty()) {
		// was already added
		return sDefine; //------------------------------------------------------
	}

//std::cout << "      Adding: " << sFontFilePath << '\n';

	::FcBool bOk = ::FcConfigAppFontAddFile(nullptr, reinterpret_cast<const FcChar8*>(sFontFilePath.c_str()));
	if (bOk == FcFalse) {
		return ""; //-----------------------------------------------------------
	}
	// find the just added font and return the description useful to Pango
	return findAppFontFile(sFontFilePath);
}


} // namespace stmg
