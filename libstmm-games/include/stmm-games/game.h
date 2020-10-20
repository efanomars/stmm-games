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
 * File:   game.h
 */

#ifndef STMG_GAME_H
#define STMG_GAME_H

#include "level.h"
#include "randomsource.h"
#include "highscore.h"

#include "gameproxy.h"
#include "named.h"
#include "ownertype.h"
#include "util/namedobjindex.h"
#include "variable.h"

#include <stmm-input/event.h>

#include <cassert>
#include <vector>
#include <atomic>
#include <memory>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class AppPreferences; }
namespace stmg { class HighscoresDefinition; }
namespace stmg { class KeyActionEvent; }
namespace stmg { class Layout; }
namespace stmg { class LevelView; }
namespace stmg { class GameSound; }
namespace stmg { struct FPoint; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class GameOwner;
class GameView;

class Game : public RandomSource
{
public:
	/** Level factory callback.
	 * This interface allows to implement recycling
	 */
	class CreateLevelCallback
	{
	public:
		virtual ~CreateLevelCallback() noexcept = default;
		virtual shared_ptr<Level> createLevel(Game* p0Game, int32_t nLevel
											, const shared_ptr<AppPreferences>& refPreferences
											, const Level::Init& oInit) noexcept = 0;
	};
	struct Init
	{
		std::string m_sName; /**< The name of the game. Cannot be empty. */
		GameOwner* m_p0GameOwner; /**< The game owner. Cannot be null. */
		AssignableNamed m_oNamed; /**< The named indexes from the theme. */
		AssignableNamedObjIndex<Variable::VariableType> m_oGameVariableTypes; /**< The global named variable types. */
		AssignableNamedObjIndex<Variable::VariableType> m_oTeamVariableTypes; /**< The named variable types for each team. */
		AssignableNamedObjIndex<Variable::VariableType> m_oPlayerVariableTypes; /**< The named variable types for each player. */
		shared_ptr<Layout> m_refLayout; /**< The layout. Cannot be null. */
		unique_ptr<RandomSource> m_refRandomSource; /**< The random number source. Can be null. */
		shared_ptr<HighscoresDefinition> m_refHighscoresDefinition; /**< The highscores definition. Can be null. */
		shared_ptr<Highscore> m_refHighscore; /**< The current highscores. Can be null. */
		double m_fMinGameInterval = 1.0; /**< The minimal tick interval in milliseconds. Default: 1. */
		double m_fInitialGameInterval = 100.0; /**< The initial tick interval in milliseconds. Default: 100. */
		int32_t m_nMaxViewTicks = 8; /**< Maximum number of view ticks in a game interval. Must be &gt; 0. Default: 8. */
		int32_t m_nAdditionalHighscoresWait = 0; /**< Additional milliseconds before highscores dialog appears at end of game. Must be &gt; 0. Default: 0. */
		double m_fSoundScaleX = 1.0; /**< The x axis scale from tiles to sound coordinates. Default: 1. */
		double m_fSoundScaleY = 1.0; /**< The y axis scale from tiles to sound coordinates. Default: 1. */
		double m_fSoundScaleZ = 1.0; /**< The z axis scale from tiles to sound coordinates. Default: 1. */
	};
	/** Constructor.
	 * See Game::reInit().
	 */
	Game(Init&& oInit, CreateLevelCallback& oCreateLevelCallback, const Level::Init& oLevelInit) noexcept;

	/** Reinitialize game instance.
	 * If the random number source is null, the game creates its own instance.
	 *
	 * Allowed characters for the game name are alphanumeric, '-' and '_'.
	 *
	 * Variable names must be unique among all owner types (for example there cannot
	 * be a variable named Points for both team and player). The names must also
	 * not start with "__" (reserved).
	 *
	 * The callback oCreateLevelCallback is only used during the call.
	 *
	 * If oInit.m_refHighscoresDefinition is defined it is used to determine the ranking
	 * among the teams. Its constraints and eligibilities are not evaluated.
	 *
	 * If oInit.m_refHighscore is defined it represents the saved highscores prior
	 * the game start. This can be used to inform the player(s) where it stands
	 * during a game.
	 *
	 * If both oInit.m_refHighscoresDefinition and oInit.m_refHighscore are defined
	 * oInit.m_refHighscore->getHighscoresDefinition() must be equivalent to oInit.m_refHighscoresDefinition.
	 *
	 * The oInit.m_refHighscoresDefinition.getMaxScores() cannot be lower than the number
	 * of teams playing.
	 * @param oInit The initialization data.
	 * @param oCreateLevelCallback The callback function that creates level instances.
	 * @param oLevelInit The level initialization object.
	 */
	void reInit(Init&& oInit, CreateLevelCallback& oCreateLevelCallback, const Level::Init& oLevelInit) noexcept;

	/** The name of the game.
	 * @return The name.
	 */
	const std::string& getName() const noexcept { return m_sName; }
	const AppPreferences& prefs() const noexcept { return *m_refPrefs; }
	inline GameOwner& owner() noexcept { return *m_p0GameOwner; }
	inline const GameOwner& owner() const noexcept { return *m_p0GameOwner; }

	int32_t getMaxViewTicks() const noexcept { return m_nMaxViewTicks; }
	int32_t getAdditionalHighscoresWait() const noexcept { return m_nAdditionalHighscoresWait; }
	/** Connect to the game view.
	 * @param p0GameView Can be null.
	 */
	void setGameView(GameView* p0GameView) noexcept { m_p0GameView = p0GameView; }
	/** The game identifier.
	 * The game id is a unique number set when the game is started.
	 * @return The game id.
	 */
	int64_t getGameId() const noexcept { return m_nGameId; }

	Named& getNamed() noexcept { return m_oNamed; }
	const Named& getNamed() const noexcept { return m_oNamed; }

	bool isAllTeamsInOneLevel() const noexcept { return m_bAllTeamsInOneLevel; }
	/** Returns the preference player number from the level player number.
	 * @param nLevel The level. Must be &gt;= 0 and &lt; getTotLevels().
	 * @param nLevelPlayer The level player.
	 * @return The preference player.
	 */
	int32_t getPlayer(int32_t nLevel, int32_t nLevelPlayer) noexcept;

	inline int32_t getTotLevels() const noexcept { return m_aLevel.size(); }
	inline shared_ptr<Level>& level(int32_t nLevel) noexcept
	{
		assert((nLevel >= 0) && (nLevel < getTotLevels()));
		return m_aLevel[nLevel];
	}
	inline const shared_ptr<Level>& level(int32_t nLevel) const noexcept
	{
		assert((nLevel >= 0) && (nLevel < getTotLevels()));
		return m_aLevel[nLevel];
	}

	/** Get a game variable by id.
	 * @param nId The id. Must be valid.
	 * @return The variable. Its address is stable throughout a game.
	 */
	const Variable& variable(int32_t nId) const noexcept;
	/** Get a game variable by id.
	 * @param nId The id. Must be valid.
	 * @return The variable. Its address is stable throughout a game.
	 */
	Variable& variable(int32_t nId) noexcept;

	/** Get variable by context.
	 * @param nId The id. Must be valid for the context defined by nLevel, nLevelTeam and nMate.
	 * @param nLevel The level. Must exist or -1 for game variables.
	 * @param nLevelTeam The level team. Must exist or -1 for game variables.
	 * @param nMate The mate. Must exist or -1 for game or team variables.
	 * @return The variable. Its address is stable throughout a game.
	 */
	const Variable& variable(int32_t nId, int32_t nLevel, int32_t nLevelTeam, int32_t nMate) const noexcept;
	/** Get variable by context.
	 * @param nId The id. Must be valid for the context defined by nLevel, nLevelTeam and nMate.
	 * @param nLevel The level. Must exist or -1 for game variables.
	 * @param nLevelTeam The level team. Must exist or -1 for game variables.
	 * @param nMate The mate. Must exist or -1 for game or team variables.
	 * @return The variable. Its address is stable throughout a game.
	 */
	Variable& variable(int32_t nId, int32_t nLevel, int32_t nLevelTeam, int32_t nMate) noexcept;
	/** Get variable id and owner type by name.
	 * @param sName The name. Cannot be empty.
	 * @return The variable id and owner type or -1 and undefined if not found.
	 */
	std::pair<int32_t, OwnerType> variableIdAndOwner(const std::string& sName) const noexcept;
	/** Whether a variable id exists.
	 * @param nId The id.
	 * @param eOwnerType The owner type.
	 * @return Whether it is defined.
	 */
	bool hasVariableId(int32_t nId, OwnerType eOwnerType) const noexcept;

	int32_t getGameVarIdGameOverTime() const noexcept { return m_nGameVarIdGameOverTime; }
	int32_t getGameVarIdWinnerTeam() const noexcept { return m_nGameVarIdWinnerTeam; }
	//
	int32_t getTeamVarIdFinishedTime() const noexcept { return m_nTeamVarIdFinishedTime; }
	int32_t getTeamVarIdRank() const noexcept { return m_nTeamVarIdRank; }
	int32_t getTeamVarIdHighscoreRank() const noexcept { return m_nTeamVarIdHighscoreRank; }
	int32_t getTeamVarIdStatus() const noexcept { return m_nTeamVarIdStatus; }
	//
	int32_t getPlayerVarIdFinishedTime() const noexcept { return m_nPlayerVarIdFinishedTime; }
	int32_t getPlayerVarIdStatus() const noexcept { return m_nPlayerVarIdStatus; }
	/** @see GameProxy::getLayout().
	 */
	const shared_ptr<Layout>& getLayout() const noexcept;
	const shared_ptr<const Highscore>& getPreGameHighscore() const noexcept { return m_refPreGameHighscore; }
	Highscore const& getInGameHighscore() const noexcept { return *m_refInGameHighscore; }

	/** @see GameProxy::interrupt(GameProxy::INTERRUPT_TYPE eInterruptType).
	 */
	void interrupt(GameProxy::INTERRUPT_TYPE eInterruptType) noexcept;

	/** Set the view for a level.
	 * @param nLevel The level. Must &gt;= 0 and &lt; getTotLevels().
	 * @param p0View The view. Can be null.
	 */
	void setLevelView(int32_t nLevel, LevelView* p0View) noexcept
	{
		assert((nLevel >= 0) && (nLevel < getTotLevels()));
		m_aLevel[nLevel]->setLevelView(p0View);
	}

	/** Start the game.
	 * Calling this function when the game is running is an error.
	 */
	void start() noexcept;
	/** End the game.
	 * Calling this function when the game is not running is an error.
	 */
	void end() noexcept;
	/** Whether the game is running.
	 */
	bool isRunning() const noexcept;

	/** The game's input event handler.
	 * Called by the device manager listener.
	 * All input events go through here.
	 * @param refEvent The event. Cannot be null.
	 */
	void handleInput(const shared_ptr<stmi::Event>& refEvent) noexcept;

	/** The game progress function.
	 * This is the game tick.
	 */
	void handleTimer() noexcept;

	bool isInGameTick() const noexcept { return m_bInGameTick; }

	/** The game interval.
	 * In a game tick the current interval cannot be changed. Levels can
	 * only determine the next game interval.
	 *
	 * The game's interval is a function of each level's interval. Currently
	 * it is always set to the minimum of all levels.
	 * @return The interval in millisec.
	 */
	double gameInterval() const noexcept { return m_fLastInterval; }
	/** The next game interval.
	 * The game's interval is a function of each level's interval. Currently
	 * it is always set to the minimum of all levels.
	 * @return The next interval in millisec.
	 */
	double gameNextInterval() const noexcept { return m_fNextInterval; }
	/** The elapsed time since the game start in milliseconds.
	 * @return The elapsed time (milliseconds).
	 */
	double gameElapsedMillisec() const noexcept { return m_fElapsedTime; }
	/** The elapsed time since the game start in intervals.
	 * @return The elapsed time (ticks).
	 */
	int32_t gameElapsed() const noexcept { return m_nTick; }
	/** Signal a level's interval has changed.
	 * @param nLevel The level.
	 */
	void changedInterval(int32_t nLevel) noexcept;

	/** The number of teams still playing.
	 * @return The number of teams that haven't completed or failed the game.
	 */
	int32_t gameStatusPlayingTeams() noexcept;
	/** Declare team completed the game.
	 * The first team that completes the game has rank 1, the second rank 2, etc.
	 * @param nCompletedLevel The level of the team that completed the game.
	 * @param nLevelTeam The level team that completed the game.
	 * @param bForceOthersFailed Whether other still playing teams status is set to failed.
	 * @param bCreateTexts Whether to create default texts.
	 * @return The rank (1 is first to complete).
	 */
	int32_t gameStatusCompleted(int32_t nCompletedLevel, int32_t nLevelTeam, bool bForceOthersFailed, bool bCreateTexts) noexcept;
	/** Declare team failed the game.
	 * The first team that fails the game has rank "prefs().getTotTeams()",
	 * the second has rank "prefs().getTotTeams() - 1", etc.
	 * @param nFailedLevel The level of the team that failed the game.
	 * @param nLevelTeam The level team that failed the game.
	 * @param bForceOtherCompleted Whether other still playing team status is set to completed.
	 * @param bCreateTexts Whether to create default texts.
	 * @return The rank (1 is last to fail).
	 */
	int32_t gameStatusFailed(int32_t nFailedLevel, int32_t nLevelTeam, bool bForceOtherCompleted, bool bCreateTexts) noexcept;
	/** Technical game over.
	 * @param nBadLevel The level in which a technical issue occurred.
	 * @param aIssue The strings describing the issue.
	 */
	void gameStatusTechnical(int32_t nBadLevel, const std::vector<std::string>& aIssue) noexcept;

	/** Send a message to other level's receivers.
	 * @param nSenderLevel The sender level. Usually the level of the event calling this function.
	 * @param nMsg The message.
	 * @param nValue The value.
	 */
	void othersSend(int32_t nSenderLevel, int32_t nMsg, int32_t nValue) noexcept;
	/** Create a game sound.
	 * The team or mate select the player(s) that should hear the sound.
	 * When nTeam is not -1, the sound can only be heard in per player sound mode,
	 * or there is only one human player and nTeamand nMate select it (or its team).
	 * @param nSoundIdx The sound index according to Named::sounds(). Must be valid.
	 * @param nTeam The team or -1 if a sound directed to all.
	 * @param nMate The team or -1 if a sound directed to all ot team.
	 * @param oXYPos The position of the sound on the board. Absolute or relative to the listener.
	 * @param fZPos The z position of the sound.
	 * @param bListenerRelative Whether oXYPos is relative to the listener.
	 * @param fVolume01 The volume. Must be from 0.0 (inaudible) to 1.0 (max).
	 * @param bLooping Whether the sound should continuously repeat.
	 * @return The created sound or null if not supported.
	 */
	shared_ptr<GameSound> createSound(int32_t nSoundIdx, int32_t nTeam, int32_t nMate
									, FPoint oXYPos, double fZPos, bool bListenerRelative
									, double fVolume01, bool bLooping) noexcept;
	/** Pre-load a sound.
	 * Makes sure that subsequent calls to createSound with th e same nSoundIdx
	 * are as fast as possible.
	 * @param nSoundIdx The sound index according to Named::sounds(). Must be valid.
	 */
	void preloadSound(int32_t nSoundIdx) noexcept;
	/** Remove a sound.
	 * In order for the sound to be recycled the caller should also remove reference to it.
	 * @param refSound The sound. Cannot be  null.
	 * @return Whether the sound was still active.
	 */
	bool removeSound(const shared_ptr<GameSound>& refSound) noexcept;

	/** The x scale from tiles to sound coordinates.
	 * @return The x scale.
	 */
	double getSoundScaleX() const noexcept { return m_fSoundScaleX; }
	/** The y scale from tiles to sound coordinates.
	 * @return The y scale.
	 */
	double getSoundScaleY() const noexcept { return m_fSoundScaleY; }
	/** The z scale from tiles to sound coordinates.
	 * @return The z scale.
	 */
	double getSoundScaleZ() const noexcept { return m_fSoundScaleZ; }

	int32_t random(int32_t nFrom, int32_t nTo) noexcept override;

	/** Create a key action input event for a player from an xy event.
	 * Note that the creation might fail if a key action with the same id and
	 * player is already happening, for example from a (keyboard) key.
	 * @param nLevel The player's level. Must be &gt;= 0.
	 * @param nLevelTeam The player's level team. Must be &gt;= 0.
	 * @param nMate The player's mate number within the team. Must be &gt;= 0.
	 * @param nKeyActionId The key action id. Must be valid.
	 * @param refXYEvent The input event. Must be of type stmi::XYEvent. Cannot be null.
	 */
	void createKeyActionFromXYEvent(int32_t nLevel, int32_t nLevelTeam, int32_t nMate
									, int32_t nKeyActionId, const shared_ptr<stmi::Event>& refXYEvent) noexcept;

	/** Send an input event directly to a player.
	 * This function is called by LevelShowWidget.
	 * @param nTeam The player's team. Must be &gt;= 0.
	 * @param nMate The player's mate number within the team. Must be &gt;= 0.
	 * @param refEvent The input event. Cannot be null.
	 */
	//TODO is refEvent actually always an XYEvent? If yes change at least the name of the param
	inline void handleInput(int32_t nTeam, int32_t nMate, const shared_ptr<stmi::Event>& refEvent) noexcept
	{
		assert(isInGameTick());
		assert((nTeam >= 0) && (nMate >= 0));
		assert(refEvent);
		const int32_t nLevel = (isAllTeamsInOneLevel() ? 0 : nTeam);
		const int32_t nLevelTeam = (isAllTeamsInOneLevel() ? nTeam : 0);
		level(nLevel)->handleInput(nLevelTeam, nMate, refEvent);
	}
	/** Get the unique active human player within a context.
	 *
	 * @param nLevel (Input) The level context (if &gt;=0) or -1. (Output) The level of the found player.
	 * @param nLevelTeam (Input) The level team context (if &gt;=0) or -1. (Output) The level team of the found player.
	 * @param nMate (Input) The mate number context (if &gt;=0) or -1. (Output) The mate number of the found player.
	 * @return Whether there is an active player and it is only one.
	 */
	bool getUniqueActiveHumanPlayer(int32_t& nLevel, int32_t& nLevelTeam, int32_t& nMate) noexcept;
	/** Get the unique active human player within a context.
	 *
	 * @param nTeam (Input) The team context (if &gt;=0) or -1. (Output) The team of the found player.
	 * @param nMate (Input) The mate number context (if &gt;=0) or -1. (Output) The mate number of the found player.
	 * @return Whether there is an active player and it is only one.
	 */
	bool getUniqueActiveHumanPlayer(int32_t& nTeam, int32_t& nMate) noexcept;
private:
	int32_t gameStatusPlayingTeams(int32_t& nPlayingLevel) noexcept;
	void gameHasEnded() noexcept;

	void calcGameInterval() noexcept;

	void dispatchInputs() noexcept;
	void dispatchInput(const shared_ptr<stmi::Event>& refEvent) noexcept;
	void createKeyAction(int32_t nLevel, int32_t nLevelTeam, int32_t nMate
						, int32_t nKeyActionId, stmi::Event::AS_KEY_INPUT_TYPE eType
						, int64_t nXYGrabId, const shared_ptr<stmi::Event>& refEvent) noexcept;

	void convertPrefToLevelTeam(int32_t nPrefTeam, int32_t& nLevel, int32_t& nLevelTeam) noexcept;

	void setDefaultHighscoreDefinition(int32_t nTotScores) noexcept;

private:
	static int32_t getNewGameId() noexcept;
private:
	static std::atomic<int32_t> s_nNewIdCounter;
private:
	int64_t m_nGameId;

	std::string m_sName;
	GameOwner* m_p0GameOwner;
	GameView* m_p0GameView;
	shared_ptr<AppPreferences> m_refPrefs;

	unique_ptr<RandomSource> m_refRandomSource;

	double m_fMinGameInterval; // in millisec

	shared_ptr<const Highscore> m_refPreGameHighscore; // The highscores before the game started
	shared_ptr<HighscoresDefinition> m_refHighscoresDefinition;
	class RecycledHighscore : public Highscore
	{
	public:
		using Highscore::Highscore;
		void reInit(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
					, const std::string& sCodeString, const std::string& sTitleString) noexcept
		{
			Highscore::reInit(refHighscoresDefinition, sCodeString, sTitleString);
		}
	};
	unique_ptr<RecycledHighscore> m_refInGameHighscore; // Filled when game over with just the teams playing

	int32_t m_nMaxViewTicks;
	int32_t m_nAdditionalHighscoresWait;
	bool m_bAllTeamsInOneLevel;
	bool m_bSoundEnabled;
	double m_fSoundScaleX;
	double m_fSoundScaleY;
	double m_fSoundScaleZ;

	AssignableNamed m_oNamed;

	AssignableNamedObjIndex<Variable::VariableType> m_oGameVariableTypes;
	AssignableNamedObjIndex<Variable::VariableType> m_oTeamVariableTypes;
	AssignableNamedObjIndex<Variable::VariableType> m_oPlayerVariableTypes;

	//friend class Level;
	int32_t m_nGameVarIdGameOverTime = -1;
	int32_t m_nGameVarIdWinnerTeam = -1;
	//
	int32_t m_nTeamVarIdFinishedTime = -1;
	int32_t m_nTeamVarIdRank = -1;
	int32_t m_nTeamVarIdHighscoreRank = -1;
	int32_t m_nTeamVarIdStatus = -1;
	//
	int32_t m_nPlayerVarIdFinishedTime = -1;
	int32_t m_nPlayerVarIdStatus = -1;

	Variable::Variables m_oGameVariables;

	std::vector< shared_ptr<Level> > m_aLevel;

	// GameStatus
	bool m_bGameEnded;
	bool m_bGameEndedEmitted;
	int32_t m_nRankCompleted;
	int32_t m_nRankFailed;

	bool m_bInGameTick;
	int32_t m_nTick;

	double m_fNextInterval; // The next game interval to use just after the current game tick, in millisec
	double m_fLastInterval; // The current game interval to be used by the view just after the current game tick, in millisec

	double m_fElapsedTime; // From start of game, in millisec (Sum of all m_nInterval so far)

	std::vector< shared_ptr<stmi::Event> > m_aInputQueue;
	std::vector< shared_ptr<KeyActionEvent> > m_aInputRecycle;

	shared_ptr<Layout> m_refLayout;

	bool m_bHasKeyActions;
	struct OpenKeyAction
	{
		int32_t m_nLevel;
		int32_t m_nLevelTeam;
		int32_t m_nMate;
		int32_t m_nCapabilityId;
		int64_t m_nXYGrabId;
	};
	std::vector< std::vector< OpenKeyAction > > m_aOpenKeyActions; // Size: prefs().getAppConfig()->TODO ???

	bool m_bIsEventAssignedToActivePlayer;
private: // no implementation
	Game() = delete;
	Game(const Game& oSource) = delete;
	Game& operator=(const Game& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_GAME_H */

