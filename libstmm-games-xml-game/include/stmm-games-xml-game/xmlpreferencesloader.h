/*
 * File:   xmlpreferencesloader.h
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

#ifndef STMG_XML_PREFERENCES_LOADER_H
#define STMG_XML_PREFERENCES_LOADER_H

#include <stmm-games-file/allpreferencesloader.h>

#include <stmm-games/ownertype.h>
#include <stmm-games/stdpreferences.h>

#include <string>
#include <vector>
#include <memory>

#include <stdint.h>

namespace stmg { class AllPreferences; }
namespace stmg { class Option; }
namespace stmg { class StdConfig; }
namespace stmg { class Variant; }
namespace stmg { template <class T> class NamedObjIndex; }
namespace xmlpp { class Element; }

namespace stmg
{

class XmlGameFiles;

class XmlPreferencesLoader : public AllPreferencesLoader
{
public:
	XmlPreferencesLoader(const shared_ptr<StdConfig>& refStdConfig, const shared_ptr<XmlGameFiles>& refXmlGameFiles);

	shared_ptr<AllPreferences> getPreferences() const noexcept override;
	shared_ptr<AllPreferences> getPreferencesCopy(const shared_ptr<AllPreferences>& refAllPreferences) const noexcept override;
	bool updatePreferences(const shared_ptr<AllPreferences>& refAllPreferences) noexcept override;
private:
	bool parseXmlGameAllPreferences(const shared_ptr<AllPreferences>& refPrefs
									, const xmlpp::Element* p0RootElement) const;
	void parseTeams(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0RootElement) const;
	void parseMates(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0TeamElement
					, const shared_ptr<StdPreferences::Team>& refTeam) const;
	void parseMateCapabilities(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0MateElement
								, const shared_ptr<StdPreferences::Player>& refMate) const;
	void parseMateKeyActions(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0MateElement
							, const shared_ptr<StdPreferences::Player>& refMate) const;
	// -1 if name not key actions
	int32_t parseKeyActionNames(const xmlpp::Element* p0KeyActionElement) const;
	std::vector<Variant> parseOptions(const xmlpp::Element* p0RootElement, const std::string& sOwnerType
									, const NamedObjIndex<shared_ptr<Option>>& oOptions) const;

	void parsePlayedHistory(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0RootElement) const;
	void parsePlayedHistoryGames(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0Element) const;
	void checkIsValidName(const std::string& sName) const;

	void writeOptions(const shared_ptr<AllPreferences>& refAllPreferences, const OwnerType& eOwnerType
					, const shared_ptr<StdPreferences::Team>& refTeam, const shared_ptr<StdPreferences::Player>& refMate, xmlpp::Element* p0RootElement);
	void writeTeams(const shared_ptr<AllPreferences>& refAllPreferences, xmlpp::Element* p0RootElement);
	void writeMates(const shared_ptr<AllPreferences>& refAllPreferences
					, const shared_ptr<StdPreferences::Team>& refTeam, xmlpp::Element* p0TeamElement);
	void writeKeyActions(const shared_ptr<AllPreferences>& refAllPreferences
						, const shared_ptr<StdPreferences::Player>& refMate, xmlpp::Element* p0MateElement);
	void writeCapabilities(const shared_ptr<AllPreferences>& refAllPreferences
							, const shared_ptr<StdPreferences::Player>& refMate, xmlpp::Element* p0MateElement);
	void writePlayedHistory(const shared_ptr<AllPreferences>& refAllPreferences, xmlpp::Element* p0RootElement);
private:
	const shared_ptr<StdConfig> m_refStdConfig;
	const shared_ptr<XmlGameFiles> m_refXmlGameFiles;

private:
	XmlPreferencesLoader() = delete;
	XmlPreferencesLoader(const XmlPreferencesLoader& oSource) = delete;
	XmlPreferencesLoader& operator=(const XmlPreferencesLoader& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_PREFERENCES_LOADER_H */

