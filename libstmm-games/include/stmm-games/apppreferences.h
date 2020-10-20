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
 * File:   apppreferences.h
 */

#ifndef STMG_APP_PREFERENCES_H
#define STMG_APP_PREFERENCES_H

#include "util/variant.h"

#include <stmm-input/hardwarekey.h>

#include <string>
#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class AppConfig; }
namespace stmi { class Capability; }

namespace stmg
{

using std::shared_ptr;

class AppPreferences
{
public:
	virtual ~AppPreferences() noexcept = default;

	/** The application config.
	 * @return The app config. Is not null.
	 */
	virtual const shared_ptr<AppConfig>& getAppConfig() const noexcept = 0;

	class PrefTeam;
	class PrefPlayer {
	public:
		virtual ~PrefPlayer() noexcept = default;
		/** The player's name.
		 * The name is unique among all players.
		 * @return The name. Is not empty.
		 */
		virtual const std::string& getName() const noexcept = 0;
		/** Whether the player is AI.
		 * This is a shortcut for the AI option.
		 * @see StdConfig::getAIOptionName
		 * @return Whether AI.
		 */
		virtual bool isAI() const noexcept = 0;
		/** Returns value of a player specific option.
		 * @param sName The option name.
		 * @return The value.  Null if option name not defined.
		 */
		virtual Variant getOptionValue(const std::string& sName) const noexcept = 0;
		/** The team the player belongs to as a mate.
		 * @return The team. Is not null.
		 */
		virtual const shared_ptr<PrefTeam> getTeam() const noexcept = 0;
		/** The mate number within the team.
		 * Note: *this == *(getTeam()->getMate(this->getMate()))
		 * @return The mate number. Is &gt;= 0 and &lt; getTeam()->getTotMates().
		 */
		virtual int32_t getMate() const noexcept = 0;
		/** The player number within the game.
		 * @return The player number. Is &gt;= 0 and &lt; AppPreferences::getTotPlayers().
		 */
		virtual int32_t get() const noexcept = 0;
		/** The capabilities assigned to the player.
		 * @return The capabilities. The pointers (if any) are not null.
		 */
		virtual std::vector< shared_ptr<stmi::Capability> > getCapabilities() const noexcept = 0;
	};
	class PrefTeam {
	public:
		virtual ~PrefTeam() noexcept = default;
		/** The team's name.
		 * The name is unique among all teams.
		 * @return The name. Is not empty.
		 */
		virtual const std::string& getName() const noexcept = 0;
		/** Whether the team is AI.
		 * The team is AI if all its mates are AI.
		 * @return Whether AI.
		 */
		virtual bool isAI() const noexcept = 0;
		/** The number of mates in the team.
		 * @return The number of players that belong to the team.
		 */
		virtual int32_t getTotMates() const noexcept = 0;
		/** Get a mate of the team.
		 * @param nMate The mate number. Must be &gt;=0 and &lt; getTotMates().
		 * @return The player. Is not null.
		 */
		virtual const shared_ptr<PrefPlayer> getMate(int32_t nMate) const  noexcept= 0;
		/** Returns value of a team specific option.
		 * @param sOptionName The option name.
		 * @return The value. Null if option name not defined.
		 */
		virtual Variant getOptionValue(const std::string& sOptionName) const noexcept = 0;
		/** The team number within the game.
		 * @return The team number. Is &gt;= 0 and &lt; AppPreferences::getTotTeams().
		 */
		virtual int32_t get() const noexcept = 0;
	};
	/** The total number of teams.
	 * @return The number of teams. Is always &gt;= 1.
	 */
	virtual int32_t getTotTeams() const noexcept = 0;
	/** Get team by number.
	 * @param nTeam The team number. Must be &gt;=0 and &lt; AppPreferences::getTotTeams().
	 * @return The team. Is not null.
	 */
	virtual const shared_ptr<PrefTeam> getTeam(int32_t nTeam) const noexcept = 0;
	/** Get team by name.
	 * Team names are unique.
	 * @param sTeamName The name of the team. Cannot be empty.
	 * @return The team or null if not found.
	 */
	virtual const shared_ptr<PrefTeam> getTeam(const std::string& sTeamName) const noexcept = 0;
	/** The number of teams.
	 * @return Total number of teams. Is always &gt;= 1.
	 */
	virtual std::vector<std::string> getTeamNames() const noexcept = 0;
	/** The number of players.
	 * @return Total number of players. Is always &gt;= 1.
	 */
	virtual int32_t getTotPlayers() const noexcept = 0;
	/** Get player by number.
	 * @param nPlayer The player number. Must be &gt;=0 and &lt; AppPreferences::getTotPlayers().
	 * @return The player. Is not null.
	 */
	virtual const shared_ptr<PrefPlayer> getPlayer(int32_t nPlayer) const noexcept = 0;
	// sPlayerName should exist and be unique among players
	/** Get player by name.
	 * Player names are unique.
	 * @param sPlayerName The name of the player. Cannot be empty.
	 * @return The player or null if not found.
	 */
	virtual const shared_ptr<PrefPlayer> getPlayer(const std::string& sPlayerName) const noexcept = 0;
	// nPlayer: 0 .. getTotPlayers() - 1
	/** Get team an mate number from player number.
	 * @param nPlayer The player number. Must be valid.
	 * @param nTeam (output) The team of the player.
	 * @param nMate (output) The mate number within the team of the player.
	 */
	virtual void getTeammate(int32_t nPlayer, int32_t& nTeam, int32_t& nMate) const noexcept = 0;

	/** The number of human players.
	 * @return Number of non AI players.
	 */
	virtual int32_t getTotHumanPlayers() const noexcept = 0;
	/** The number of AI players.
	 * @return Number of non human players.
	 */
	virtual int32_t getTotAIPlayers() const noexcept = 0;
	/** The number of human teams.
	 * @return Number of non AI teams.
	 */
	virtual int32_t getTotHumanTeams() const noexcept = 0;
	/** The number of AI teams.
	 * @return Number of non human teams.
	 */
	virtual int32_t getTotAITeams() const noexcept = 0;

	/** Returns value of a game option.
	 * @param sName The option name.
	 * @return The value. Null if option name not defined.
	 */
	virtual Variant getOptionValue(const std::string& sName) const noexcept = 0;

	/** Given a key generated by a capability, return the player's key action.
	 * @param nCapabilityId The id of the capability.
	 * @param eKey The key. Cannot be stmi::HK_NULL.
	 * @param refPlayer The player associated with the key. Unchanged if not found.
	 * @param nKeyActionId The key action associated with the key. Unchanged if not found.
	 * @return Whether a key action is associated with the capability key.
	 */
	virtual bool getPlayerKeyActionFromCapabilityKey(int32_t nCapabilityId, stmi::HARDWARE_KEY eKey
													, shared_ptr<PrefPlayer>& refPlayer, int32_t& nKeyActionId) const noexcept = 0;
	/** Given a key generated by a capability, return the player's key action.
	 * @param nCapabilityId The id of the capability.
	 * @param eKey The key. Cannot be stmi::HK_NULL.
	 * @param nTeam The player's team associated with the key, unchanged if not found.
	 * @param nMate The player's mate associated with the key, unchanged if not found.
	 * @param nKeyActionId The key action associated with the key. Unchanged if not found.
	 * @return Whether a key action is associated with the capability key.
	 */
	virtual bool getPlayerKeyActionFromCapabilityKey(int32_t nCapabilityId, stmi::HARDWARE_KEY eKey
													, int32_t& nTeam, int32_t& nMate, int32_t& nKeyActionId) const noexcept = 0;
	/** The player to which a capability is assigned.
	 * @param nCapabilityId The id of the capability.
	 * @param refPlayer The player the capability is assigned to or unchanged.
	 * @return Whether the capability is assigned to a player.
	 */
	virtual bool getCapabilityPlayer(int32_t nCapabilityId, shared_ptr<PrefPlayer>& refPlayer) const noexcept = 0;
	/** The player to which a capability is assigned.
	 * @param nCapabilityId The id of the capability.
	 * @param nTeam The team of the player the capability is assigned to or unchanged.
	 * @param nMate The mate of the player the capability is assigned to or unchanged.
	 * @return Whether the capability is assigned to a player.
	 */
	virtual bool getCapabilityPlayer(int32_t nCapabilityId, int32_t& nTeam, int32_t& nMate) const noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_APP_PREFERENCES_H */

