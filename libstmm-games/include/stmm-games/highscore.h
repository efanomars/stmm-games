/*
 * File:   highscore.h
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

#ifndef STMG_HIGHSCORE_H
#define STMG_HIGHSCORE_H

#include "apppreferences.h"
#include "variable.h"

#include <vector>
#include <string>
#include <cassert>
#include <memory>

#include <stdint.h>

namespace stmg { class HighscoresDefinition; }

namespace stmg
{

using std::shared_ptr;

class Game;

/** Class the holds highscores.
 * All position are 0 based. That is the best score has position 0, the second best
 * has position 1 and so on.
 */
class Highscore
{
public:
	struct Score
	{
		int32_t m_nTeam = -1; /**< The team number if the game was just played or -1. This value shouldn't be persisted. */
		std::string m_sTeam; /**< If the team doesn't have a name the name of its first human mate is used. */
		std::vector<Variable::Value> m_aValues; /**< Value: variable value, Size: m_refHighscoresDefinition->getScoreElements().size() */
	};

	/** Constructor.
	 * @param refHighscoresDefinition The highscore definition. Cannot be null.
	 * @param sCodeString The code string. Must have been generated by refHighscoresDefinition or be empty.
	 * @param sTitleString The title string. Should have been generated by refHighscoresDefinition or be empty.
	 */
	Highscore(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
			, const std::string& sCodeString, const std::string& sTitleString) noexcept;
	/** Constructor.
	 * @param refHighscoresDefinition The highscore definition. Cannot be null.
	 * @param sCodeString The code string. Must have been generated by refHighscoresDefinition.
	 * @param sTitleString The title string. Should have been generated by refHighscoresDefinition.
	 * @param aScores The initial scores. The team name of the scores must be defined.
	 */
	Highscore(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
			, const std::string& sCodeString, const std::string& sTitleString
			, const std::vector<Score>& aScores) noexcept;
protected:
	/** See constructor.
	 */
	void reInit(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
				, const std::string& sCodeString, const std::string& sTitleString) noexcept;
	/** See constructor.
	 */
	void reInit(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
				, const std::string& sCodeString, const std::string& sTitleString
				, const std::vector<Score>& aScores) noexcept;

public:
	/** The HighscoresDefinition this Highscore is based on.
	 * @return The highscores definition. Cannot be null.
	 */
	const shared_ptr<HighscoresDefinition>& getHighscoresDefinition() const noexcept { return m_refHighscoresDefinition; }

	/** The code string as passed to the constructor.
	 * @return The code string or empty.
	 */
	const std::string& getCodeString() const noexcept
	{
		return m_sCodeString;
	}
	/** The title string as passed to the constructor.
	 * @return The title string or empty.
	 */
	const std::string& getTitleString() const noexcept
	{
		return m_sTitleString;
	}
	/** The total number of scores.
	 * @return The number of scores if the table so far.
	 */
	int32_t getTotScores() const noexcept
	{
		return static_cast<int32_t>(m_aScores.size());
	}
	/** Get the score at a given position.
	 * @param nPosition The position. `0` is the top highscore.
	 * @return The score.
	 */
	const Score& getScore(int32_t nPosition) const noexcept
	{
		assert((nPosition >= 0) && (nPosition < getTotScores()));
		return m_aScores[nPosition];
	}
	/** Override the team name of a score.
	 * @param nPosition The score position. Must be valid.
	 * @param sTeam The new team name. Cannot be empty.
	 */
	void setScoreTeamName(int32_t nPosition, const std::string& sTeam) noexcept
	{
		assert((nPosition >= 0) && (nPosition < getTotScores()));
		assert(! sTeam.empty());
		m_aScores[nPosition].m_sTeam = sTeam;
	}
	/** Insert all the teams that ended the game in the highscores (if they are eligible).
	 * Before possibly inserting the teams it sets the m_nTeam value of the existing
	 * teams to -1 so that after this call that field is only set for the teams that
	 * just played.
	 * @param oGame The game.
	 * @return The positions of the inserted new scores. The top position is 0.
	 */
	std::vector<int32_t> includeGame(const Game& oGame) noexcept;
	/** Insert the teams in a game in the highscores.
	 * Before possibly inserting the teams it sets the m_nTeam value of the existing
	 * teams to -1 so that after this call that field is only set for the teams that
	 * just played.
	 *
	 * The eligibility check also includes whether a team has finished the game.
	 * This means that if you call this function when the game isn't finished yet
	 * with parameter bCheckElegibility set to true the empty vector is returned.
	 * @param oGame The game.
	 * @param bCheckElegibility Whether to check for eligibility.
	 * @return The positions of the inserted new scores. The top position is 0.
	 */
	std::vector<int32_t> includeGame(const Game& oGame, bool bCheckElegibility) noexcept;
	/** Get the position of a team if it were included in the highscores.
	 * Constraints and eligibilities are not evaluated.
	 * @param oGame The game. Might be running.
	 * @param nTeam The team. Must be valid.
	 * @return The position (starts from 0) or -1 if unknown or not good enough.
	 */
	int32_t positionIfIncluded(const Game& oGame, int32_t nTeam) const noexcept;
private:
	void reInitInternal(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
						, const std::string& sCodeString, const std::string& sTitleString
						, const std::vector<Score>& aScores) noexcept;
	bool isTeamIncluded(int32_t nLevel, int32_t nLevelTeam
						, const AppPreferences::PrefTeam& oPrefTeam, const Game& oGame) const noexcept;
	Score createScore(int32_t nLevel, int32_t nLevelTeam
					, const AppPreferences::PrefTeam& oPrefTeam, const Game& oGame) const noexcept;
	int32_t findNewScorePosition(const Score& oTheScore) const noexcept;
	int32_t insertInScores(int32_t nLevel, int32_t nLevelTeam
							, const AppPreferences::PrefTeam& oPrefTeam, const Game& oGame) noexcept;

private:
	shared_ptr<HighscoresDefinition> m_refHighscoresDefinition;
	std::string m_sCodeString;
	std::string m_sTitleString;
	// The top score is at position 0
	std::vector<Score> m_aScores; // Size: not bigger than m_refHighscoresDefinition.getMaxScores()
private:
	Highscore() = delete;
};

} // namespace stmg

#endif	/* STMG_HIGHSCORE_H */
