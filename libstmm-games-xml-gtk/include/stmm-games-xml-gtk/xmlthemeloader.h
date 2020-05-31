/*
 * File:   xmlthemeloader.h
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

#ifndef STMG_XML_THEME_LOADER_H
#define STMG_XML_THEME_LOADER_H

#include "xmlmodifierparser.h"
#include "xmlthanimationfactoryparser.h"
#include "xmlthwidgetfactoryparser.h"

#include <stmm-games-gtk/themeloader.h>

#include <vector>
#include <map>
#include <memory>
#include <string>

namespace stmg { class AppConfig; }
namespace stmg { class StdTheme; }
namespace stmg { class Theme; }


namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class GameDiskFiles;
class XmlThemeParser;

class XmlThemeLoader : public ThemeLoader
{
public:
	struct Init
	{
		shared_ptr<AppConfig> m_refAppConfig; /**< The app configuration data. Cannot be null. */
		std::vector<std::string> m_aAdditionalGameIds; /**< Additional game ids the theme should accept. */
		shared_ptr<GameDiskFiles> m_refGameDiskFiles; /**< The disk files. Cannot be null. */
		std::string m_sDefaultThemeName; /**< The default theme name. Can be empty. */
		std::vector<unique_ptr<XmlModifierParser>> m_aModifierParsers; /**< The modifier parsers. Cannot contain nulls. */
		std::vector<unique_ptr<XmlThAnimationFactoryParser>> m_aThAnimationParsers; /**< The theme animation parsers. Cannot contain nulls. */
		std::vector<unique_ptr<XmlThWidgetFactoryParser>> m_aThWidgetParsers; /**< The theme widget parsers. Cannot contain nulls. */
	};
	/** Constructor.
	 * @param oInit Inizialization data..
	 */
	explicit XmlThemeLoader(Init&& oInit);
	// The following is needed because m_refXmlThemeParser destructor is inlined here
	// by unique_ptr where it is an incomplete type
	// https://stackoverflow.com/questions/6012157/is-stdunique-ptrt-required-to-know-the-full-definition-of-t
	virtual ~XmlThemeLoader();

	const std::vector<std::string>& getThemeNames() noexcept override;
	const ThemeInfo& getThemeInfo(const std::string& sThemeName) noexcept override;
	shared_ptr<Theme> getTheme(const std::string& sThemeName) noexcept override;

	const std::string& getDefaultThemeName() noexcept override;

private:
	void loadThemeInfos();
	bool hasCycleOrInvalid(const std::string& sThemeName);
	void traverseAndMark(const std::string& sThemeName);
	void traverseAndAdd(const std::string& sThemeName);

	struct ExtThemeInfo : public ThemeInfo
	{
		ExtThemeInfo()
		: m_bSupportsAppId(false)
		, m_bVisited(false)
		{
			m_bLoaded = false;
			m_sThemeErrorString = "Theme not found";
		}
		std::vector<std::string> m_oExtendsThemes;
		bool m_bSupportsAppId;
		bool m_bVisited;
	};

	ExtThemeInfo& getExtThemeInfo(const std::string& sName);

	void parseXmlTheme(StdTheme& oStdTheme);

private:
	const shared_ptr<AppConfig> m_refAppConfig;
	const std::vector<std::string> m_aAdditionalGameIds;
	const shared_ptr<GameDiskFiles> m_refGameDiskFiles;

	unique_ptr<XmlThemeParser> m_refXmlThemeParser;

	bool m_bInfosLoaded;
	std::vector<std::string> m_aValidNames;
	std::map<std::string, ExtThemeInfo> m_oNamedThemeInfos; // Key: theme name (after loading only contains themes in m_aValidNames)
	static const ExtThemeInfo s_oNotFoundExtThemeInfo;

	// Helper used to find extends cycles
	std::vector<std::string> m_aGoodIdThemeNames;
	// The theme name and its extended theme names in a top down depth first order
	std::vector<std::string> m_aPreSortedThemeNames; //helper used when loading Theme

	std::string m_sCachedThemeName;
	shared_ptr<Theme> m_refCachedTheme;

	const std::string m_sDefaultThemeName;

private:
	XmlThemeLoader() = delete;
	XmlThemeLoader(const XmlThemeLoader& oSource) = delete;
	XmlThemeLoader& operator=(const XmlThemeLoader& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_THEME_LOADER_H */

