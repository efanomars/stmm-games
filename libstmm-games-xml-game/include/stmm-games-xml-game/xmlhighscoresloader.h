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
 * File:   xmlhighscoresloader.h
 */

#ifndef STMG_XML_HIGHSCORES_LOADER_H
#define STMG_XML_HIGHSCORES_LOADER_H

#include <stmm-games-file/highscoresloader.h>

#include <stmm-games/highscore.h>

#include <vector>
#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class AppConfig; }
namespace stmg { class AppPreferences; }
namespace stmg { class File; }
namespace stmg { class HighscoresDefinition; }
namespace xmlpp { class Element; }

namespace stmg
{

class XmlGameFiles;

class XmlHighscoresLoader : public HighscoresLoader
{
public:
	XmlHighscoresLoader(const shared_ptr<AppConfig>& refAppConfig, const shared_ptr<XmlGameFiles>& refXmlGameFiles);

	shared_ptr<Highscore> getHighscore(const std::string& sGameName, const AppPreferences& oPreferences
										, const shared_ptr<HighscoresDefinition>& refHighscoresDefinition) const noexcept override;
	bool updateHighscore(const std::string& sGameName, const AppPreferences& oPreferences, const Highscore& oHighscore) noexcept override;

	std::vector<shared_ptr<Highscore>> getHighscores(const std::string& sGameName
													, const shared_ptr<HighscoresDefinition>& refHighscoresDefinition) const noexcept override;
private:
	std::vector<shared_ptr<Highscore>> parseGameHighscores(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
															, const File& oHSFile, const std::string& sGameName
															, bool bAll, const std::string& sCode, const std::string& sTitle) const;
	std::vector<shared_ptr<Highscore>> parseXmlGameHighscores(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
															, bool bAll, const std::string& sCode
															, const xmlpp::Element* p0RootElement) const;
	shared_ptr<Highscore> parseXmlHighscores(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
											, const xmlpp::Element* p0HighscoresElement) const;
	Highscore::Score parseScore(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
								, const xmlpp::Element* p0ScoreElement) const;
	int32_t findHighscoreWithCode(const std::vector<shared_ptr<Highscore>>& aHighscores
								, const std::string& sCode) const;
	void writeHighscores(xmlpp::Element* p0RootElement, const Highscore& oHighscore);
private:
	const shared_ptr<AppConfig> m_refAppConfig;
	const shared_ptr<XmlGameFiles> m_refXmlGameFiles;

private:
	XmlHighscoresLoader() = delete;
	XmlHighscoresLoader(const XmlHighscoresLoader& oSource) = delete;
	XmlHighscoresLoader& operator=(const XmlHighscoresLoader& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_HIGHSCORES_LOADER_H */

