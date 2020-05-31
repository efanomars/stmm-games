/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   setup.cc
 */

#include "setup.h"

#include "setupstdconfig.h"
#include "setupxmlgameloader.h"
#include "setupxmlthemeloader.h"
#include "jsonstrings.h"

#include <stmm-games-xml-game/xmlgameloader.h>
#include <stmm-games-xml-gtk/xmlthemeloader.h>
#include <stmm-games-xml-game/xmlhighscoresloader.h>
#include <stmm-games-xml-game/xmlpreferencesloader.h>
#include <stmm-games-xml-gtk/gamediskfiles.h>

#include <stmm-games-gtk/mainwindow.h>

#include <stmm-input-gtk-dm/gtkdevicemanager.h>

#include <gtkmm.h>

//#include <iostream>
//#include <cassert>
#include <memory>

namespace stmg { class StdConfig; }


namespace stmg
{

std::string omniSetup(MainWindowData& oMainWindowData, const json& oConf, const std::string& sOmni, const std::string& sAppVersion
						, bool bTestMode, bool bFullScreen) noexcept
{
	const json& oDm = oConf[jas::s_sConfDm];
	stmi::GtkDeviceManager::Init oInit;
	oInit.m_sAppName = sOmni;
	oInit.m_bEnableEventClasses = oDm[jas::s_sConfDmEnableEventClasses].get<bool>();
	for (std::string sClassId : oDm[jas::s_sConfDmEnDisableEventClasses]) {
		stmi::Event::Class oEvClass = stmi::Event::getEventClassIdClass(sClassId);
		if (! oEvClass) {
			const std::string sError = "Error: Event class id not registered: " + sClassId;
			return sError; //-------------------------------------------------------
		}
		oInit.m_aEnDisableEventClasses.push_back(oEvClass);
	}
	const json& oPlugins = oDm[jas::s_sConfDmPlugins];
	oInit.m_bEnablePlugins = oPlugins[jas::s_sConfDmPluginsEnablePlugins].get<bool>();
	for (std::string sPlugin : oPlugins[jas::s_sConfDmPluginsEnDisablePlugins]) {
		if (sPlugin.empty()) {
			const std::string sError = "Error: Plugin cannot be empty";
			return sError; //-------------------------------------------------------
		}
		oInit.m_aEnDisablePlugins.push_back(sPlugin);
	}
	for (std::string sGroup : oPlugins[jas::s_sConfDmPluginsGroups]) {
		if (sGroup.empty()) {
			const std::string sError = "Error: Group cannot be empty";
			return sError; //-------------------------------------------------------
		}
		oInit.m_aGroups.push_back(sGroup);
	}
	oInit.m_bVerbose = oPlugins[jas::s_sConfDmPluginsVerbose].get<bool>();

	auto oPairDeviceManager = stmi::GtkDeviceManager::create(oInit);
	shared_ptr<stmi::DeviceManager> refDeviceManager = oPairDeviceManager.first;
	if (!refDeviceManager) {
		const std::string sError = "Error: Couldn't create device manager\n " + oPairDeviceManager.second;
		return sError; //-------------------------------------------------------
	}

	shared_ptr<StdConfig>& refStdConfig = oMainWindowData.m_refStdConfig;
	const std::string sError = omniSetupStdConfig(refStdConfig, oConf, refDeviceManager, sOmni, sAppVersion, bTestMode);
	if (! sError.empty()) {
		return sError; //-------------------------------------------------------
	}

	oMainWindowData.m_bFullscreen = bFullScreen;
	//
	std::vector<File> aGameFiles;
	aGameFiles.push_back(File("../data/games/empty.xml"));
	aGameFiles.push_back(File("../data/games/asound.xml"));
	std::vector<File> aThemeFiles;
	aThemeFiles.push_back(File("../data/themes/dummy.thm/theme.xml"));
	const std::string sPreferencesFile = "/tmp/omni.prefs";
	auto refGameDiskFiles = std::make_shared<GameDiskFiles>(sOmni, false
															, std::move(aGameFiles), false
															, std::move(aThemeFiles), true
															, "", "", sPreferencesFile);

	unique_ptr<XmlGameLoader> refXmlGameLoader;
	omniSetupXmlGameLoader(refXmlGameLoader, refStdConfig, refGameDiskFiles);
	unique_ptr<XmlThemeLoader> refXmlThemeLoader;
	omniSetupXmlThemeLoader(refXmlThemeLoader, refStdConfig, refGameDiskFiles);

	oMainWindowData.m_refGameLoader = std::move(refXmlGameLoader);
	oMainWindowData.m_refThemeLoader = std::move(refXmlThemeLoader);
	oMainWindowData.m_refHighscoresLoader = std::make_unique<XmlHighscoresLoader>(refStdConfig, refGameDiskFiles);
	oMainWindowData.m_refAllPreferencesLoader = std::make_unique<XmlPreferencesLoader>(refStdConfig, refGameDiskFiles);

	oMainWindowData.m_oIconFile = refGameDiskFiles->getIconFile();
	oMainWindowData.m_sCopyright = "© 2020 Stefano Marsili, Switzerland";
	oMainWindowData.m_sWebSite = "https://efanomars.com/games/" + sOmni;
	MainAuthorData oAuthor;
	oAuthor.m_sName = "Stefano Marsili";
	oAuthor.m_sEMail = "stemars@gmx.ch";
	oAuthor.m_sRole = "";
	oMainWindowData.m_aAuthors.push_back(std::move(oAuthor));

	return "";
}

} //namespace stmg

