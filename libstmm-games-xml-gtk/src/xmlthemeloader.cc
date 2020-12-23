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
 * File:   xmlthemeloader.cc
 */

#include "xmlthemeloader.h"

#include "xmlthemeparser.h"
#include "gamediskfiles.h"
#include "xmlthanimationfactoryparser.h"
#include "xmlmodifierparser.h"
#include "themectx.h"
#include "themeextradata.h"
#include "fontconfigloader.h"

#include <stmm-games-xml-base/parserctx.h>

#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/themeloader.h>
#include <stmm-games-file/file.h>

#include <stmm-games/appconfig.h>
#include <stmm-games/named.h>
#include <stmm-games/util/util.h>

#include <libxml++/libxml++.h>

#include <stdexcept>
#include <vector>
#include <cassert>
#include <iostream>
#include <exception>
#include <algorithm>
#include <utility>
#include <unordered_set>

#include <stdint.h>

namespace stmg { class Theme; }

namespace stmg
{

XmlThemeLoader::XmlThemeLoader(Init&& oInit)
: m_refAppConfig(std::move(oInit.m_refAppConfig))
, m_aAdditionalGameIds(oInit.m_aAdditionalGameIds)
, m_refGameDiskFiles(std::move(oInit.m_refGameDiskFiles))
, m_refXmlThemeParser(std::make_unique<XmlThemeParser>())
, m_bInfosLoaded(false)
, m_sDefaultThemeName(std::move(oInit.m_sDefaultThemeName))
{
	assert(m_refAppConfig);
	assert(m_refGameDiskFiles);
	for (auto& refModifierParser : oInit.m_aModifierParsers) {
		m_refXmlThemeParser->addXmlModifierParser(std::move(refModifierParser));
	}
	for (auto& refThAnimationParser : oInit.m_aThAnimationParsers) {
		m_refXmlThemeParser->addXmlThAnimationFactoryParser(std::move(refThAnimationParser));
	}
	for (auto& refThWidgetParser : oInit.m_aThWidgetParsers) {
		m_refXmlThemeParser->addXmlThWidgetFactoryParser(std::move(refThWidgetParser));
	}
	m_refFontConfigLoader = FontConfigLoader::create();
	if (! m_refFontConfigLoader) {
		std::cout << "XmlThemeLoader: could not create FontConfigLoader" << '\n';
		std::cout << "-> theme font files wont be used" << '\n';
	}
}
XmlThemeLoader::~XmlThemeLoader()
{
	// DO NOT REMOVE THIS!!! See header file!
	// DO NOT REMOVE THIS!!! See header file!
	// DO NOT REMOVE THIS!!! See header file!
}
const std::string& XmlThemeLoader::getDefaultThemeName() noexcept
{
	if (!m_sDefaultThemeName.empty()) {
		return m_sDefaultThemeName;
	}
	if (!m_bInfosLoaded) {
		loadThemeInfos();
	}
	if (m_aValidNames.empty()) {
		return Util::s_sEmptyString;
	}
	return m_aValidNames[0];
}
void XmlThemeLoader::loadThemeInfos()
{
	assert(m_oNamedThemeInfos.empty());

	const std::vector<File>& aFiles = m_refGameDiskFiles->getThemeFiles();

	Named oDummy;

	for (const auto& oThemeFile : aFiles) {
//std::cout << "XmlThemeLoader::loadThemeInfos() file " << oThemeFile.getFullPath() << '\n';
		assert(oThemeFile.isDefined());
		xmlpp::DomParser oParser;
		//We just want the text to be resolved/unescaped automatically.
		oParser.set_substitute_entities();
		try
		{
			//oParser.set_validate();
			assert(!oThemeFile.isBuffered());
			oParser.parse_file(oThemeFile.getFullPath());
			if (oParser)
			{
				const xmlpp::Node* p0Node = oParser.get_document()->get_root_node();
				const xmlpp::Element* p0RootElement = dynamic_cast<const xmlpp::Element*>(p0Node);
				if (p0RootElement == nullptr) {
					throw std::runtime_error("Fatal error: root node is not an element");
				}
				std::string sThemeName;
				XmlThemeLoader::ExtThemeInfo oThemeInfo;
				oThemeInfo.m_sThemeErrorString.clear();
				ParserCtx oCtx(m_refAppConfig, oDummy);
				const bool bSupportsAppId = m_refXmlThemeParser->parseXmlThemeInfo(oCtx, p0RootElement
															, sThemeName, oThemeInfo.m_oExtendsThemes, oThemeInfo);
//std::cout << "XmlThemeLoader::loadThemeInfos()    sThemeName='" << sThemeName << "'" << '\n';
				if (m_oNamedThemeInfos.find(sThemeName) == m_oNamedThemeInfos.end()) {
					oThemeInfo.m_bSupportsAppId = bSupportsAppId;
					oThemeInfo.m_bVisited = false;
					oThemeInfo.m_oThemeFile = oThemeFile;
					oThemeInfo.m_oThumbnailFile = m_refGameDiskFiles->getThemeThumbnailFile(oThemeFile);
					m_oNamedThemeInfos[sThemeName] = oThemeInfo;
				} else {
					std::cout << "Discarding theme";
					std::cout << " file '" << oThemeFile.getFullPath() << "'";
					std::cout << ": internal name '" << sThemeName << "' already used" << '\n';
				}
			} else {
				std::cout << "Could not parse";
				std::cout << " file '" << oThemeFile.getFullPath() << "'";
				std::cout << '\n';
			}
		}
		catch(const std::exception& ex)
		{
			std::cout << "Exception caught";
			std::cout << " loading '" << oThemeFile.getFullPath() << "'";
			std::cout << ": " << ex.what() << '\n';
		}
	}
	// select theme names that support the current gameId
	m_aGoodIdThemeNames.clear();
	for (const auto& oPairTI : m_oNamedThemeInfos) {
		const auto& sThemeName = oPairTI.first;
		const ExtThemeInfo& oTI = oPairTI.second;
		if (oTI.m_bSupportsAppId) {
			if (std::find(m_aGoodIdThemeNames.begin(), m_aGoodIdThemeNames.end(), sThemeName) == m_aGoodIdThemeNames.end()) {
				m_aGoodIdThemeNames.emplace_back(sThemeName);
			}
		}
	}
	// get the valid theme names (without cyclic or invalid dependencies)
	assert(m_aValidNames.empty());
	while (m_aGoodIdThemeNames.size() > 0) {
		const std::string sCurName = *(m_aGoodIdThemeNames.begin());
		bool bInCycle = hasCycleOrInvalid(sCurName);
		if (! bInCycle) {
			if (std::find(m_aValidNames.begin(), m_aValidNames.end(), sCurName) == m_aValidNames.end()) {
				m_aValidNames.emplace_back(sCurName);
			}
		}
		m_aGoodIdThemeNames.erase(m_aGoodIdThemeNames.begin());
	}
	// mark all themes that are either valid themes for this gameId or dependencies thereof
	for (const auto& sValidName : m_aValidNames) {
		traverseAndMark(sValidName);
//std::cout << "XmlThemeLoader::loadThemeInfos()  sValidName = " << sValidName << '\n';
	}
	// go through the ThemeInfos and remove those that are not marked
	auto itTI = m_oNamedThemeInfos.begin();
	while (itTI != m_oNamedThemeInfos.end()) {
		const std::string& sThemeName = itTI->first;
		assert(m_oNamedThemeInfos.find(sThemeName) != m_oNamedThemeInfos.end());
		ExtThemeInfo& oThemeInfo = m_oNamedThemeInfos[sThemeName];
		if (!oThemeInfo.m_bVisited) {
			itTI = m_oNamedThemeInfos.erase(itTI);
			const auto itName = std::find(m_aValidNames.begin(), m_aValidNames.end(), sThemeName);
			if (itName != m_aValidNames.end()) {
				m_aValidNames.erase(itName);
			}
		} else {
			++itTI;
		}
	}
	m_bInfosLoaded = true;
}
bool XmlThemeLoader::hasCycleOrInvalid(const std::string& sThemeName)
{
	ExtThemeInfo& oThemeInfo = m_oNamedThemeInfos[sThemeName];
	if (oThemeInfo.m_bVisited) {
		return true;
	}
	oThemeInfo.m_bVisited = true;
	for (const auto& sExtendsName : oThemeInfo.m_oExtendsThemes) {
		if (m_oNamedThemeInfos.find(sExtendsName) == m_oNamedThemeInfos.end()) {
			// the theme to be extended is not present or accessible! (invalid dependency)
			oThemeInfo.m_bVisited = false;
			return true;
		}
		bool bInCycle = hasCycleOrInvalid(sExtendsName);
		if (bInCycle) {
			//m_oGoodIdThemeNames.erase(sExtendsName);
			oThemeInfo.m_bVisited = false;
			return true;
		}
	}
	oThemeInfo.m_bVisited = false;
	return false;
}
void XmlThemeLoader::traverseAndMark(const std::string& sThemeName)
{
	ExtThemeInfo& oThemeInfo = m_oNamedThemeInfos[sThemeName];
	if (oThemeInfo.m_bVisited) {
		return;
	}
	oThemeInfo.m_bVisited = true;
	for (const auto& sExtendsName : oThemeInfo.m_oExtendsThemes) {
		traverseAndMark(sExtendsName);
	}
}
void XmlThemeLoader::traverseAndAdd(const std::string& sThemeName)
{
	m_aPreSortedThemeNames.push_back(sThemeName);
	ExtThemeInfo& oThemeInfo = m_oNamedThemeInfos[sThemeName];
	for (const auto& sExtendsName : oThemeInfo.m_oExtendsThemes) {
		traverseAndAdd(sExtendsName);
	}
}
const std::vector<std::string>& XmlThemeLoader::getThemeNames() noexcept
{
	if (!m_bInfosLoaded) {
		loadThemeInfos();
	}
	return m_aValidNames;
}
const XmlThemeLoader::ExtThemeInfo XmlThemeLoader::s_oNotFoundExtThemeInfo{};
const XmlThemeLoader::ThemeInfo& XmlThemeLoader::getThemeInfo(const std::string& sName) noexcept
{
	if (!m_bInfosLoaded) {
		loadThemeInfos();
	}
	if (std::find(m_aValidNames.begin(), m_aValidNames.end(), sName) == m_aValidNames.end()) {
		return s_oNotFoundExtThemeInfo;
	}
	return getExtThemeInfo(sName);
}
XmlThemeLoader::ExtThemeInfo& XmlThemeLoader::getExtThemeInfo(const std::string& sName)
{
//std::cout << "XmlThemeLoader::getExtThemeInfo " << sName << '\n';
	assert(std::find(m_aValidNames.begin(), m_aValidNames.end(), sName) != m_aValidNames.end());
	assert(m_oNamedThemeInfos.find(sName) != m_oNamedThemeInfos.end());
	return m_oNamedThemeInfos[sName];
}

shared_ptr<Theme> XmlThemeLoader::getTheme(const std::string& sName) noexcept
{
	if (!m_bInfosLoaded) {
		loadThemeInfos();
	}
	if (m_aValidNames.empty()) {
		return shared_ptr<Theme>(); //------------------------------------------
	}
	const std::string& sTheName = (sName.empty() ? m_aValidNames[0] : sName);
	const auto itFindValid = std::find(m_aValidNames.begin(), m_aValidNames.end(), sTheName);
	if (itFindValid == m_aValidNames.end()) {
		return shared_ptr<Theme>(); //------------------------------------------
	}
	ExtThemeInfo& oThemeInfo = getExtThemeInfo(sTheName);
//std::cout << "XmlThemeLoader::getTheme()    sName=" << sTheName << '\n';

	std::string& sErrorStr = oThemeInfo.m_sThemeErrorString;
	if (!sErrorStr.empty()) {
//std::cout << "XmlThemeLoader::getTheme()    sErrorStr=" << sErrorStr << '\n';
		return shared_ptr<Theme>(); //------------------------------------------
	}
	if (m_sCachedThemeName == sTheName) {
		assert(m_refCachedTheme);
		return m_refCachedTheme; //---------------------------------------------
	}

	auto refStdTheme = std::make_shared<StdTheme>();
	StdTheme& oStdTheme = *refStdTheme;
	m_aPreSortedThemeNames.clear();
	traverseAndAdd(sTheName);
	//load theme
	shared_ptr<Theme> refTheme;
	try
	{
		parseXmlTheme(oStdTheme);
		refTheme = refStdTheme;
	}
	catch(const std::exception& oEx)
	{
		sErrorStr = "Exception caught parsing theme '" + sTheName + "'"
					+ ": " + oEx.what();
		std::cout << sErrorStr << '\n';
		return shared_ptr<Theme>(); //------------------------------------------
	}
	m_sCachedThemeName = sTheName;
	m_refCachedTheme = refTheme;
	return refTheme;
}

void XmlThemeLoader::parseXmlTheme(StdTheme& oStdTheme)
{
//std::cout << "XmlThemeLoader::parseXmlTheme()    sThemeName=" << m_aPreSortedThemeNames[0] << '\n';

	const int32_t nTotThemes = static_cast<int32_t>(m_aPreSortedThemeNames.size());
//std::cout << "XmlThemeLoader::parseXmlTheme()    nTotThemes=" << nTotThemes << '\n';
	std::vector<xmlpp::DomParser> aDomParsers{static_cast<std::vector<xmlpp::DomParser>::size_type>(nTotThemes)};

	{ // Do not remove this! Segmentation fault!

	auto refThemeExtraData = std::make_shared<ThemeExtraData>();

	std::vector< unique_ptr<ThemeCtx> > aCtxs;
	aCtxs.reserve(nTotThemes);
	{
	for (int32_t nThemeNr = 0; nThemeNr < nTotThemes; ++nThemeNr) {
		const std::string& sName = m_aPreSortedThemeNames[nThemeNr];
		xmlpp::DomParser& oDomParser = aDomParsers[nThemeNr];
		//oDomParser.set_validate();
		//We just want the text to be resolved/unescaped automatically.
		oDomParser.set_substitute_entities();
//if (nThemeNr > 0) {
//std::cout << "XmlThemeLoader::parseXmlTheme()        extended theme sName=" << sName << '\n';
//}
		const File& oThemeFile = m_oNamedThemeInfos[sName].m_oThemeFile;
		assert(oThemeFile.isDefined());
		assert(!oThemeFile.isBuffered());
		const std::string& sFile = oThemeFile.getFullPath();
		assert(!sFile.empty());
		oDomParser.parse_file(sFile);
		const xmlpp::Node* p0RootNode = oDomParser.get_document()->get_root_node();
		const xmlpp::Element* p0RootElement = dynamic_cast<const xmlpp::Element*>(p0RootNode);
		if (p0RootElement == nullptr) {
			throw std::runtime_error("Fatal error: root node is not an element");
		}
//std::cout << "XmlThemeLoader::parseXmlTheme()   sName=" << sName << "   sFile=" << sFile << '\n';
		aCtxs.emplace_back(std::make_unique<ThemeCtx>(m_refAppConfig, oStdTheme, sName, oThemeFile, p0RootElement));
		auto& oThemeCtx = *aCtxs.back();
		oThemeCtx.m_nThemeNr = nThemeNr;
		oThemeCtx.m_refThemeExtraData = refThemeExtraData;

		const auto& aImageFiles = m_refGameDiskFiles->getThemeImageFiles(oThemeFile);
		for (auto& oPairThemeFile : aImageFiles) {
			const std::string& sThemeFileName = oPairThemeFile.first;
			const File& oThemeFile = oPairThemeFile.second;
			assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
			oStdTheme.addKnownImageFile(sThemeFileName, oThemeFile);
//std::cout << "XmlThemeLoader::parseXmlTheme()   image       sThemeFileName=" << sThemeFileName << '\n';
		}
		const auto& aSoundFiles = m_refGameDiskFiles->getThemeSoundFiles(oThemeFile);
		for (auto& oPairThemeFile : aSoundFiles) {
			const std::string& sThemeFileName = oPairThemeFile.first;
			const File& oThemeFile = oPairThemeFile.second;
			assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
			oStdTheme.addKnownSoundFile(sThemeFileName, oThemeFile);
//std::cout << "XmlThemeLoader::parseXmlTheme()    sound      sThemeFileName=" << sThemeFileName << '\n';
		}
		if (m_refFontConfigLoader) {
			const auto& aFontFiles = m_refGameDiskFiles->getThemeFontFiles(oThemeFile);
			for (auto& oPairThemeFile : aFontFiles) {
				const std::string& sThemeFileName = oPairThemeFile.first;
				const File& oThemeFile = oPairThemeFile.second;
				assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
				const std::string sFontDefine = m_refFontConfigLoader->addAppFontFile(oThemeFile.getFullPath());
				oThemeCtx.m_refThemeExtraData->m_oFontFileDefs.insert(std::make_pair(sThemeFileName, sFontDefine));
//std::cout << "XmlThemeLoader::parseXmlTheme()    font      sThemeFileName=" << sThemeFileName << '\n';
			}
		}
	}
	const auto& aDefaultImageFiles = m_refGameDiskFiles->getDefaultImageFiles();
	for (auto& oPairThemeFile : aDefaultImageFiles) {
		const std::string& sThemeFileName = oPairThemeFile.first;
		const File& oThemeFile = oPairThemeFile.second;
		assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
		oStdTheme.addKnownImageFile(sThemeFileName, oThemeFile);
//std::cout << "XmlThemeLoader::parseXmlTheme() common image         sThemeFileName=" << sThemeFileName << '\n';
	}
	const auto& aDefaultSoundFiles = m_refGameDiskFiles->getDefaultSoundFiles();
	for (auto& oPairThemeFile : aDefaultSoundFiles) {
		const std::string& sThemeFileName = oPairThemeFile.first;
		const File& oThemeFile = oPairThemeFile.second;
		assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
		oStdTheme.addKnownSoundFile(sThemeFileName, oThemeFile);
//std::cout << "XmlThemeLoader::parseXmlTheme() common sound         sThemeFileName=" << sThemeFileName << '\n';
	}
	if (m_refFontConfigLoader) {
		const auto& aDefaultFontFiles = m_refGameDiskFiles->getDefaultFontFiles();
		for (auto& oPairThemeFile : aDefaultFontFiles) {
			const std::string& sThemeFileName = oPairThemeFile.first;
			const File& oThemeFile = oPairThemeFile.second;
			assert(oThemeFile.isDefined() && !oThemeFile.isBuffered());
			const std::string sFontDefine = m_refFontConfigLoader->addAppFontFile(oThemeFile.getFullPath());
			refThemeExtraData->m_oFontFileDefs.insert(std::make_pair(sThemeFileName, sFontDefine));
//std::cout << "XmlThemeLoader::parseXmlTheme() common font         sThemeFileName=" << sThemeFileName << '\n';
		}
	}	
	//
	m_refXmlThemeParser->parseXmlTheme(aCtxs);

	// Resolve font aliases
	auto& oAliases = refThemeExtraData->m_oFontAliases;
//std::cout << "XmlThemeLoader::parseXmlTheme()  oAliases.size()=" << oAliases.size() << '\n';
	if (! oAliases.empty()) {
		std::unordered_set<std::string> oVisited;

		using traverserType=std::function<void(const std::string&, int32_t, const xmlpp::Element*)>;
		traverserType* p0Traverse = nullptr;
		traverserType oTraverse = [&](const std::string& sName, int32_t nThemeNr, const xmlpp::Element* p0Element)
		{
			if (oVisited.count(sName) == 1) {
				throw XmlCommonErrors::error(*(aCtxs[nThemeNr]), p0Element, "", Util::stringCompose("Font alias '%1' circular definition", sName));
			}
			oVisited.insert(sName);
			const auto itFind = oAliases.find(sName);
			if (itFind == oAliases.end()) {
				assert(p0Element != nullptr);
				throw XmlCommonErrors::error(*(aCtxs[nThemeNr]), p0Element, "", Util::stringCompose("Aliased font named '%1' not found", sName));
			}
			const auto& oTuple = itFind->second;
			const std::string& sUse = std::get<2>(oTuple);
			std::string sDefine;
			bool bDefined = oStdTheme.getNamedFont(sUse, sDefine);
			if (! bDefined) {
				(*p0Traverse)(sUse, std::get<0>(oTuple), std::get<1>(oTuple));
				bDefined = oStdTheme.getNamedFont(sUse, sDefine);
				assert(bDefined);
			}
			oStdTheme.addFontName(sName, sDefine);
		};
		p0Traverse = &oTraverse;
		const auto& oPair = *oAliases.begin();
		oTraverse(oPair.first, std::get<0>(oPair.second), std::get<1>(oPair.second));
	}

	const std::string& sDefaultFontName = refThemeExtraData->m_sDefaultFontName;
	if (! sDefaultFontName.empty()) {
		std::string sDefine;
		bool bDefined = oStdTheme.getNamedFont(sDefaultFontName, sDefine);
		if (! bDefined) {
			throw XmlCommonErrors::error(*(aCtxs[refThemeExtraData->m_nDefaultThemeNr]), refThemeExtraData->m_p0DefaultElement, ""
										, Util::stringCompose("Default font named '%1' not found", sDefaultFontName));
		}
		oStdTheme.setDefaultFont(sDefine);
	}

	}
	} // Do not remove this! Segmentation fault!
}


} // namespace stmg
