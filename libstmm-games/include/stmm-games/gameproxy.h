/*
 * File:   gameproxy.h
 *
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

#ifndef STMG_GAME_PROXY_H
#define STMG_GAME_PROXY_H

#include "ownertype.h"

#include <string>
#include <memory>
#include <utility>

#include <stdint.h>

namespace stmg { class Named; }
namespace stmg { class RandomSource; }
namespace stmg { class Variable; }
namespace stmg { class GameSound; }
namespace stmg { struct FPoint; }

namespace stmi { class Event; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

class Game;
class Level;
class Layout;
class Highscore;

/** Proxy class (of Game) exposed by Level available to events. */
class GameProxy
{
public:
	/** Return a random value within a range.
	 * This is just a shortcut to getRandomSource().random().
	 * @param nFrom The first of the possible values.
	 * @param nTo The last of the possible values.
	 * @return The random value &gt;= nFrom and &lt;= nTo.
	 */
	int32_t random(int32_t nFrom, int32_t nTo) noexcept;
	/** Return the game's random source.
	 * @return The random source.
	 */
	RandomSource& getRandomSource() noexcept;
	/** The unique game id.
	 * A new one is created each time a game is started.
	 * @return The game id.
	 */
	int64_t getGameId() const noexcept;

	Named& getNamed() noexcept;
	const Named& getNamed() const noexcept;

	/** Whether teams play all in the same level or each on its own level.
	 * @return Whether all teams play in one level.
	 */
	bool isAllTeamsInOneLevel() const noexcept;
	/** The current game interval in milliseconds.
	 * @return The current game interval.
	 */
	double gameInterval() const noexcept;
	/** Number of game ticks since start of game.
	 * @return Game ticks since game start.
	 */
	int32_t gameElapsed() const noexcept;
	/** Number of milliseconds since start of game.
	 * @return Milliseconds since game start.
	 */
	double gameElapsedMillisec() const noexcept;
	/** Tells whether within a game tick.
	 * This might be useful for events to determine whether within a game
	 * tick or a view tick callback.
	 * @return Whether in game tick.
	 */
	bool isInGameTick() const noexcept;
	/** Tell game the preferred next interval of the level has changed.
	 * The game interval is the time between game ticks.
	 * @param nLevel The level the preferred game interval has changed. Must be valid.
	 */
	void changedInterval(int32_t nLevel) noexcept;

	enum INTERRUPT_TYPE
	{
		INTERRUPT_ABORT = 0 /**< Abort the game. */
		, INTERRUPT_ABORT_ASK = 1 /**< Ask "Are you sure" and possibly abort the game. */
		, INTERRUPT_RESTART = 2 /**< Abort and restart the current game immediately. */
		, INTERRUPT_RESTART_ASK = 3 /**< Ask "Are you sure" and possibly abort and restart the current game immediately. */
		, INTERRUPT_QUIT_APP = 4 /**< Abort and quit the application.*/
		, INTERRUPT_QUIT_APP_ASK = 5 /**< Ask "Are you sure" and possibly abort and quit the application. */
		, INTERRUPT_PAUSE = 10 /**< Pause the game. If the game is already paused does nothing. */
	};
	/** Tells owner to interrupt the current game.
	 * @param eInterruptType The interrupt type.
	 */
	void interrupt(INTERRUPT_TYPE eInterruptType) noexcept;

	/** Whether a variable id is defined.
	 * @param nId The variable id.
	 * @param eOwnerType The owner type.
	 * @return Whether defined.
	 */
	bool hasVariableId(int32_t nId, OwnerType eOwnerType) const noexcept;
	/** Variable id and owner by name.
	 * @param sName The name of the variable. Cannot be empty.
	 * @return The id (>= 0) and owner or -1 (and undefined).
	 */
	std::pair<int32_t, OwnerType> variableIdAndOwner(const std::string& sName) const noexcept;
	/** Return the game variable instance for the id.
	 * @param nId A valid variable id for OwnerType::GAME.
	 * @return The const variable.
	 */
	const Variable& variable(int32_t nId) const noexcept;
	/** Return the game variable instance for the id.
	 * @param nId A valid variable id for OwnerType::GAME.
	 * @return The modifiable variable.
	 */
	Variable& variable(int32_t nId) noexcept;
	/** Return a game, team or player variable.
	 * @param nId The variable id. Must be valid for the owner type.
	 * @param nLevel The level. Must be valid or -1 for game variable.
	 * @param nLevelTeam The level team. Must be valid or -1 for game variable.
	 * @param nMate The mate. Must be valid or -1 for game or team variable.
	 * @return The const variable.
	 */
	const Variable& variable(int32_t nId, int32_t nLevel, int32_t nLevelTeam, int32_t nMate) const noexcept;
	/** Return a game, team or player variable.
	 * @param nId The variable id. Must be valid for the owner type.
	 * @param nLevel The level. Must be valid or -1 for game variable.
	 * @param nLevelTeam The level team. Must be valid or -1 for game variable.
	 * @param nMate The mate. Must be valid or -1 for game or team variable.
	 * @return The modifiable variable.
	 */
	Variable& variable(int32_t nId, int32_t nLevel, int32_t nLevelTeam, int32_t nMate) noexcept;

	/** Get the player number from the level player.
	 * @param nLevel The level. Must be valid.
	 * @param nLevelPlayer The level player. Must be valid.
	 * @return The player number.
	 */
	int32_t getPlayer(int32_t nLevel, int32_t nLevelPlayer) noexcept;
	/** The layout.
	 * Can be used to find named widgets.
	 * @return The layout. Not null.
	 */
	Layout const& getLayout() const noexcept;

	/** The values the special team variable with id getTeamVarIdStatus() can take.
	 */
	enum VAR_VALUE_TEAM_STATUS
	{
		VAR_VALUE_TEAM_STATUS_COMPLETED = 1
		, VAR_VALUE_TEAM_STATUS_PLAYING = 0
		, VAR_VALUE_TEAM_STATUS_FAILED = -1
	};
	/** The values the special player variable with id getPlayerVarIdStatus() can take.
	 */
	enum VAR_VALUE_PLAYER_STATUS
	{
		VAR_VALUE_PLAYER_STATUS_COMPLETED = 1
		, VAR_VALUE_PLAYER_STATUS_PLAYING = 0
		, VAR_VALUE_PLAYER_STATUS_FAILED = -1
		, VAR_VALUE_PLAYER_STATUS_OUT = -2
	};
	/** Game over time variable id.
	 * The variable's value is either the game over time is in millisec or -1 if still playing.
	 * @return Game over time variable id.
	 */
	int32_t getGameVarIdGameOverTime() const noexcept;
	/** The winner team variable id.
	 * The variable's value is either the winner team (numbered according to AppPreferences)
	 * or -1 if still playing.
	 * @return The winner team variable id.
	 */
	int32_t getGameVarIdWinnerTeam() const noexcept;
	/** The team's finished time variable id.
	 * The variable's value is either the time the team finished playing in millisec
	 * or -1 if still playing.
	 * @return The finished time variable id.
	 */
	int32_t getTeamVarIdFinishedTime() const noexcept;
	/** The team's rank variable id.
	 * The variable's value is either the rank of a team (starting from 0)
	 * or -1 if still playing. The normal rank is solely determined by the order
	 * of the calls to Level::gameStatusFailed() and Level::gameStatusCompleted().
	 * @return The rank variable id.
	 */
	int32_t getTeamVarIdRank() const noexcept;
	/** The team's highscore rank variable id.
	 * The highscore rank is only set when all the teams have finished and is based
	 * on the highscres definition for the game.
	 * @return The highscore rank variable id.
	 */
	int32_t getTeamVarIdHighscoreRank() const noexcept;
	/** The team's status variable id.
	 * The variable's value is in enum VAR_VALUE_TEAM_STATUS.
	 * @return The status variable id.
	 */
	int32_t getTeamVarIdStatus() const noexcept;
	/** The player's finished time variable id.
	 * The variable's value is either the time the player finished playing in millisec
	 * or -1 if still playing.
	 * @return The finished time variable id.
	 */
	int32_t getPlayerVarIdFinishedTime() const noexcept;
	/** The player's status variable id.
	 * The variable's value is in enum VAR_VALUE_PLAYER_STATUS.
	 * @return The status variable id.
	 */
	int32_t getPlayerVarIdStatus() const noexcept;
	/** Send a message to other levels.
	 * The message can be listened to by a OthersReceiverEvent instance.
	 * @param nSenderLevelInGame The sender level. Must be valid.
	 * @param nMsg The message to send.
	 * @param nValue The value to send with it.
	 */
	void othersSend(int32_t nSenderLevelInGame, int32_t nMsg, int32_t nValue) noexcept;
	/** Create a game sound.
	 * The team or mate select the player(s) that should hear the sound.
	 *
	 * When nTeam is not -1, the sound can only be heard in per player sound mode
	 * or if there is only one human team and it is nTeam.
	 *
	 * When nMate is not -1, the sound can only be heard in per player sound mode
	 * or if there is only one human player and it is (nTeam, nMate).
	 * @param nSoundIdx The sound index according to Named::sounds(). Must be valid.
	 * @param nTeam The (preferences) team or -1 if sound directed to all players.
	 * @param nMate The mate or -1 if sound directed to all players of team (provided it is not -1).
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
	/** Send input directly to a player.
	 * Can only be called within a game tick.
	 * @param nTeam The player's team. Must be &gt;= 0.
	 * @param nMate The player's mate number within the team. Must be &gt;= 0.
	 * @param refEvent The event. Cannot be null.
	 */
	void handleInput(int32_t nTeam, int32_t nMate, const shared_ptr<stmi::Event>& refEvent) noexcept;
	/** Create a key action input event for a player.
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
	/** The total number of levels participating in the game.
	 * @return The number of levels.
	 */
	int32_t getTotLevels() const noexcept;
	/** The level.
	 * @param nLevel The level number. Must be &gt;= 0 and &lt; getTotLevels().
	 * @return The level.
	 */
	Level& level(int32_t nLevel) noexcept;
	/** The const level.
	 * @param nLevel The level number. Must be &gt;= 0 and &lt; getTotLevels().
	 * @return The level.
	 */
	Level const& level(int32_t nLevel) const noexcept;
	/** The highscores before the game started.
	 * @return The pre game start highscore or null.
	 */
	const shared_ptr<const Highscore>& getPreGameHighscore() const noexcept;
	/** The game highscores.
	 * After game over contains only the teams that played the game.
	 * @return The game highscores. Is empty if game not ended.
	 */
	Highscore const& getInGameHighscore() const noexcept;
private:
	friend class Level;
	friend class Layout;
	Game* m_p0Game = nullptr;
};


} // namespace stmg

#endif	/* STMG_GAME_PROXY_H */

