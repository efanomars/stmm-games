/*
 * File:   highscoresdefinition.h
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

#ifndef STMG_HIGHSCORES_DEFINITION_H
#define STMG_HIGHSCORES_DEFINITION_H

#include "ownertype.h"

#include <vector>
#include <string>
#include <memory>
#include <utility>

#include <stdint.h>

namespace stmg { class AppConfig; }
namespace stmg { class AppPreferences; }
namespace stmg { class Option; }

namespace stmg
{

using std::shared_ptr;

class Game;

class HighscoresDefinition
{
public:
	enum DISCRIMINATOR_TYPE
	{
		DISCRIMINATOR_TYPE_OPTION = 0 /**< The value of an option. */
		, DISCRIMINATOR_TYPE_TOT_TEAMS = 1 /**< Number of teams playing. */
		, DISCRIMINATOR_TYPE_TOT_HUMAN_TEAMS = 2 /**< Number of human teams playing. */
		, DISCRIMINATOR_TYPE_TOT_AI_TEAMS = 3 /**< Number of ai teams playing. */
		, DISCRIMINATOR_TYPE_TOT_MATES_OF_HUMAN_TEAM = 4 /**< Number of mates in human teams. */
	};
	/** Separates highscores according to preference values.
	 * In the DISCRIMINATOR_TYPE_OPTION case, with owner type OwnerType::GAME,
	 * for a team to get in the highscores selected by this discriminator's HighscoresDefinition,
	 * its game has to have a master or enabled slave option with the given name.
	 * If the owner type is OwnerType::TEAM there has to be one and only
	 * one team with master or enabled slave option with the given name.
	 * If the owner type is OwnerType::PLAYER there has to be one and only
	 * one player in the game with master or enabled slave option with the given name.
	 * 
	 * Example of discriminator: a game enum option called "Difficulty" which has
	 * values "Easy", "Medium", "Hard". If it's the only discriminator passed
	 * to a HighscoresDefinition instance a Highscore will be created for
	 * all three values.
	 */
	struct Discriminator
	{
		DISCRIMINATOR_TYPE m_eType = DISCRIMINATOR_TYPE_OPTION; /**< The type of the discriminator. Default: DISCRIMINATOR_TYPE_OPTION */
		std::string m_sOptionName = ""; /**< Name of option. Default: empty string.
										* Only set if m_eType is DISCRIMINATOR_TYPE_OPTION. */
		OwnerType m_eOptionOwnerType = OwnerType::GAME; /**< Whether the option is of game, team or player. Default: OwnerType::GAME. 
														 * Only set if m_eType is DISCRIMINATOR_TYPE_OPTION. */
		bool operator==(const Discriminator& oHS) const noexcept;
	};
	enum HIGHSCORE_CONSTRAINT_TYPE
	{
		HIGHSCORE_CONSTRAINT_TYPE_TOT_TEAMS = 0 /**< Number of teams playing. */
		, HIGHSCORE_CONSTRAINT_TYPE_TOT_HUMAN_TEAMS = 1 /**< Number of human teams playing. */
		, HIGHSCORE_CONSTRAINT_TYPE_TOT_AI_TEAMS = 2 /**< Number of ai teams playing. */
		, HIGHSCORE_CONSTRAINT_TYPE_TOT_MATES_OF_HUMAN_TEAM = 3 /**< Number of mates in human team. */
	};
	/** All constraints must be satisfied for a highscore to be created.
	 */
	struct HighscoreConstraint
	{
		HIGHSCORE_CONSTRAINT_TYPE m_eType; /**< The type of the highscore existance constraint. */
		int32_t m_nMin; /**< The minimum of the valid range of tot teams or tot AI teams, etc. */
		int32_t m_nMax; /**< The maximum of the valid range of tot teams or tot AI teams, etc. */
		bool operator==(const HighscoreConstraint& oHS) const noexcept;
	};
	/** All constraints must be satisfied for a team to be added to the highscore.
	 *
	 * Note: HighscoresDefinition doesn't check for the validity of the variable name.
	 * The Highscore class should do it.
	 */
	struct Eligibility
	{
		std::string m_sVarName; /**< Name of variable. Cannot be empty. */
		int32_t m_nMin = 0; /**< The minimum value of the eligible range. */
		int32_t m_nMax = 0; /**< The maximum value of the eligible range. */
		bool operator==(const Eligibility& oHS) const noexcept;
	};
	/** Element of a score for a team.
	 * Many elements can be used for a score.
	 * Example: {"Points", OwnerType::TEAM, nPointsVarId, false},
	 * {"Time", OwnerType::GAME, nTimeVarId, true}
	 *
	 * Note: HighscoresDefinition doesn't check for the validity of the variable name.
	 * The Highscore class should do it.
	 */
	struct ScoreElement
	{
		std::string m_sVarDescription; /**< If this field is empty the value shouldn't be shown in a highscore table. */
		std::string m_sVarName = ""; /**< The name of the variable, the value of which is used to build a score. Cannot be empty. */
		bool m_bLowestBest = false; /**< If false a higher value means a better score. Default is false. */
		bool operator==(const ScoreElement& oHS) const noexcept;
	};
	/** Constructor.
	 * 
	 * If more than one ScoreElement is passed the one at index 0 (in the vector)
	 * is the most significant. Note: the variables are expected to be correct,
	 * this class doesn't check whether the Game really has them.
	 * 
	 * Since a highscore (file) might be created for each combination of Discriminators
	 * the total number should be kept low. So options that can take a huge amount of values
	 * shouldn't be used as discriminator. Example: an IntOption "Initial lives"
	 * that can take any positive number used as discriminator would create a highscore file
	 * for each value set in the preferences.
	 *
	 * @param refAppConfig The config. Cannot be null.
	 * @param aHighscoreConstraints The game type conditions for this definition to apply. All must be true. Can be empty.
	 * @param aDiscriminators The discriminators. Can be empty.
	 * @param aEligibilitys Constraints for on a team to be eligible to a highscore. All must apply. If empty teams always eligible.
	 * @param aScoreElements The elements of a score. From the most significant to the least significant. Cannot be empty.
	 * @param nMaxScores The number of highscore entries.
	 */
	HighscoresDefinition(const shared_ptr<AppConfig>& refAppConfig
						, const std::vector<HighscoreConstraint>& aHighscoreConstraints
						, const std::vector<Discriminator>& aDiscriminators
						, const std::vector<Eligibility>& aEligibilitys
						, const std::vector<ScoreElement>& aScoreElements, int32_t nMaxScores) noexcept;
protected:
	/** See constructor.
	 */
	void reInit(const shared_ptr<AppConfig>& refAppConfig
				, const std::vector<HighscoreConstraint>& aHighscoreConstraints
				, const std::vector<Discriminator>& aDiscriminators
				, const std::vector<Eligibility>& aEligibilitys
				, const std::vector<ScoreElement>& aScoreElements, int32_t nMaxScores) noexcept;
public:
	//const Constraints& getConstraints() const { return m_oConstraints; }
	const std::vector<Discriminator>& getDiscriminators() const noexcept { return m_aDiscriminators; }
	const std::vector<Eligibility>& getEligibilitys() const noexcept { return m_aEligibilitys; }
	const std::vector<ScoreElement>& getScoreElements() const noexcept { return m_aScoreElements; }
	/** The maximum number of scores in a highscore.
	 * @return The maximum number.
	 */
	int32_t getMaxScores() const noexcept { return m_nMaxScores; }

	/** Whether this instance satisfies the given preferences.
	 * The preferences must satisfy the constraints and all the discriminators by option
	 * must be either master or enabled slave option.
	 * @param oAppPreferences The preferences.
	 * @return Whether this instance can be used to generate highscores.
	 */
	bool satisfiesPreferences(const AppPreferences& oAppPreferences) const noexcept
	{
		if (! preferencesSatisfyConstraints(oAppPreferences)) {
			return false;
		}
//std::cout << "satisfiesPreferences" << '\n';
		return getStringFromPreferences(oAppPreferences, true).first;
	}
	/** Return a code string that can identify a highscore based on the preferences.
	 * This string can be used along with the game name (and the app name) to
	 * uniquely identify a highscore. It can be used to build the name of a highscore
	 * file or build the path of a key in a settings hierarchy like gconf.
	 * 
	 * Example: if HighscoresDefinition was initialized with one discriminator
	 * of type EnumOption (localized to french) with the name "Difficulty" and values
	 * (1,"Easy","Facile"), (2,"Medium","Moyen") and (3,"Hard","Difficile")
	 * this function would return "Medium" if the current value in the preferences
	 * for this option is 2.
	 * This string can then be used to define the highscore file name: for
	 * example "Jointris-Medium.sco".
	 *
	 * If more discriminators are defined the values are separated with an "_"
	 * (underscore).
	 * 
	 * If one of the discriminator options is a not enabled slave option
	 * `{false, ""}` is returned.

	 * @param oAppPreferences The preferences.
	 * @return Whether a string could be generated and the string. If no discriminators `{true, ""}` is returned.
	 */
	std::pair<bool, std::string> getCodeFromPreferences(const AppPreferences& oAppPreferences) const noexcept
	{
		if (! preferencesSatisfyConstraints(oAppPreferences)) {
			return {false, ""};
		}
		return getStringFromPreferences(oAppPreferences, true);
	}
	/** Return a title string that can identify a highscore based on the preferences.
	 * This string can be used along with the game name (and the app name) as
	 * title of a highscore.
	 * 
	 * Example: if HighscoresDefinition was initialized with one discriminator
	 * of type EnumOption (localized to french) with the name "Difficulty", description "Difficulté"
	 * and values (1,"Easy","Facile"), (2,"Medium","Moyen") and (3,"Hard","Difficile")
	 * this function would return "Difficulté: Moyen" if the current value in the preferences
	 * for this option is 2.
	 * This string can then be used to define the title of a highscore dialog: for
	 * example "Jointris scores - Difficulté: Moyen".
	 *
	 * If more discriminators are defined the values are separated with an ", ".
	 * 
	 * If one of the discriminator options is a not enabled slave option
	 * `{false, ""}` is returned.
	 *
	 * @param oAppPreferences The preferences.
	 * @return Whether a string could be generated and the string. If no discriminators `{true, ""}` is returned.
	 */
	std::pair<bool, std::string> getTitleFromPreferences(const AppPreferences& oAppPreferences) const noexcept
	{
		if (! preferencesSatisfyConstraints(oAppPreferences)) {
			return {false, ""};
		}
		return getStringFromPreferences(oAppPreferences, false);
	}
	/** Whether a code string is valid.
	 * @param sCode The code string. Can be empty.
	 * @return Whether code could have been generated by getCodeFromPreferences().
	 */
	bool isValidCode(const std::string& sCode) const noexcept;
	/** Check compatibility with a game instance.
	 * Checks whether all the variable names in Eligibility and ScoreElement are defined.
	 * @param oGame The game.
	 * @return The error string or empty if it's compatible.
	 */
	std::string isCompatibleWithGame(const Game& oGame) const noexcept;

	bool operator==(const HighscoresDefinition& oHS) const noexcept;

	#ifndef NDEBUG
	void dump() noexcept;
	#endif //NDEBUG
private:
	void checkParams() const noexcept;
	bool preferencesSatisfyConstraints(const AppPreferences& oAppPreferences) const noexcept;
	std::pair<bool, std::string> getStringFromPreferences(const AppPreferences& oAppPreferences, bool bCode) const noexcept;
	shared_ptr<Option> isMasterOrEnabledSlaveOption(const AppPreferences& oAppPreferences
													, const std::string& sOptionName, OwnerType eOwner
													, int32_t nPrefTeam, int32_t nMate) const noexcept;
private:
	shared_ptr<AppConfig> m_refAppConfig;
	std::vector<HighscoreConstraint> m_aHighscoreConstraints;
	std::vector<Discriminator> m_aDiscriminators;
	std::vector<Eligibility> m_aEligibilitys;
	std::vector<ScoreElement> m_aScoreElements;
	int32_t m_nMaxScores;
private:
	HighscoresDefinition() = delete;
};

} // namespace stmg

#endif	/* STMG_HIGHSCORES_DEFINITION_H */

