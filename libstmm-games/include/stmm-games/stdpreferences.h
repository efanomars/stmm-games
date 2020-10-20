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
 * File:   stdpreferences.h
 */

#ifndef STMG_STD_PREFERENCES_H
#define STMG_STD_PREFERENCES_H

#include "apppreferences.h"
#include "util/variant.h"

#include <stmm-input/capability.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/hardwarekey.h>

#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <string>
#include <tuple>
#include <utility>
#include <type_traits>

#include <stdint.h>

namespace stmg { class AppConfig; }
namespace stmg { class StdConfig; }
namespace stmi { class Device; }

namespace stmg
{

using std::shared_ptr;

class StdPreferences : public AppPreferences
{
public:
	/** Constructor.
	 * The initial number of teams is defined by AppConstraints::m_nTeamsMin
	 * which is obtained with refStdConfig->getAppConstraints(). Initially each
	 * team has one human player.
	 *
	 * The instance is created in edit mode.
	 * @param refStdConfig The configuration. Cannot be null.
	 */
	explicit StdPreferences(const shared_ptr<StdConfig>& refStdConfig) noexcept;
	/** Copy constructor.
	 * @param oSource The source.
	 */
	StdPreferences(const StdPreferences& oSource) noexcept;
	/** Copy assignment.
	 * @param oSource The source.
	 * @return The copy.
	 */
	StdPreferences& operator=(const StdPreferences& oSource) noexcept;

	/** Get edit mode.
	 * @return Whether in edit mode.
	 */
	bool isInEditMode() const noexcept { return m_bEditMode; }
	/** Set edit mode.
	 * The instance can be either in edit or in running mode. In running mode
	 * players and teams cannot be added or removed, options cannot be changed.
	 * In running mode removed and then re-added devices are automatically
	 * assigned to a player.
	 * @param bInEditMode The chosen mode.
	 */
	void setEditMode(bool bInEditMode) noexcept;

	/** The player name maximum length in unicode characters.
	 * @return The max length.
	 */
	int32_t getMaxPlayerNameLength() const noexcept;
	/** The team name maximum length in unicode characters.
	 * @return The max length.
	 */
	int32_t getMaxTeamNameLength() const noexcept;

	/** Move a player up in the hierarchy.
	 * If the player is not the first mate of the team it is swapped with the preceding mate.
	 * If the player is the first mate of the team it is swapped with the last mate of
	 * the preceding team. The swap is only allowed if it doesn't violate some
	 * AI player constraint.
	 *
	 * The first player of the first team cannot be moved up.
	 * @param sPlayerName The player's name. Must exist.
	 * @return Whether the player could be moved up.
	 */
	bool playerMoveUp(const std::string& sPlayerName) noexcept;
	/** Move a player down in the hierarchy.
	 * If the player is not the last mate of the team it is swapped with the following mate.
	 * If the player is the last mate of the team it is swapped with the first mate of
	 * the following team. The swap is only allowed if it doesn't violate some
	 * AI player constraint.
	 *
	 * The last player of the last team cannot be moved down.
	 * @param sPlayerName The player's name. Must exist.
	 * @return Whether the player could be moved down.
	 */
	bool playerMoveDown(const std::string& sPlayerName) noexcept;
	/** Move a player to the preceding team in the hierarchy.
	 * If a player violates some AI constraints the function might fail.
	 *
	 * A player of the first team cannot "team up".
	 * @param sPlayerName The player's name. Must exist.
	 * @return Whether the player could be moved to the preceding team.
	 */
	bool playerTeamUp(const std::string& sPlayerName) noexcept;
	/** Move a player to the following team in the hierarchy.
	 * If a player violates some AI constraints the function might fail.
	 *
	 * A player of the last team cannot "team down".
	 * @param sPlayerName The player's name. Must exist.
	 * @return Whether the player could be moved to the following team.
	 */
	bool playerTeamDown(const std::string& sPlayerName) noexcept;

	class Team;
	class Player;

	/** Get the player.
	 * @param nPlayer The player number. Must be valid.
	 * @return The player. Is not null.
	 */
	const shared_ptr<Player>& getPlayerFull(int32_t nPlayer) const noexcept;
	/** Get the player.
	 * @param sPlayerName The player name. Must exist.
	 * @return The player. Is not null.
	 */
	const shared_ptr<Player>& getPlayerFull(const std::string& sPlayerName) const noexcept;
	/** Get the team.
	 * @param nTeam The team number. Must be valid.
	 * @return The team. Is not null.
	 */
	const shared_ptr<Team>& getTeamFull(int32_t nTeam) const noexcept;
	/** Get the team.
	 * @param sTeamName The team name. Must exist.
	 * @return The team. Is not null.
	 */
	const shared_ptr<Team>& getTeamFull(const std::string& sTeamName) const noexcept;

	/** Set the total number of teams.
	 * If the constraints limited the addition or removal of teams
	 * `false` is returned. The limiting factor might be the minimum or
	 * maximum number of teams.
	 * @param nTeams The number of teams. Must be &gt;= 0.
	 * @return Whether the desired number could be set.
	 */
	bool setTotTeams(int32_t nTeams) noexcept;
	/** Set the the total number of players.
	 * If the constraints limited the addition or removal of players
	 * `false` is returned. The limiting factors might be the minimum number
	 * of teams, the maximum number of AI mates, etc.
	 *
	 * Unless it's full, new players are added to the last team, otherwise
	 * to the preceding team and so on.
	 * @param nPlayers The number of players. Must be &gt;= 0.
	 * @return Whether the desired number could be set.
	 */
	bool setTotPlayers(int32_t nPlayers) noexcept;

	int32_t getTotTeams() const noexcept override;
	const shared_ptr<PrefTeam> getTeam(int32_t nTeam) const noexcept override;
	const shared_ptr<PrefTeam> getTeam(const std::string& sTeamName) const noexcept override;
	std::vector<std::string> getTeamNames() const noexcept override;
	int32_t getTotPlayers() const noexcept override;
	const shared_ptr<PrefPlayer> getPlayer(int32_t nPlayer) const noexcept override;
	const shared_ptr<PrefPlayer> getPlayer(const std::string& sPlayerName) const noexcept override;
	void getTeammate(int32_t nPlayer, int32_t& nTeam, int32_t& nMate) const noexcept override;

	int32_t getTotHumanPlayers() const noexcept override;
	int32_t getTotAIPlayers() const noexcept override;
	int32_t getTotHumanTeams() const noexcept override;
	int32_t getTotAITeams() const noexcept override;

	Variant getOptionValue(const std::string& sOptionName) const noexcept override;
	/** Set the value of a game's option.
	 * @param sOptionName The name of the option (OwnerType::GAME). Must exist and not be read-only.
	 * @param oValue The new value.
	 * @return Whether the operation succeeded.
	*/
	bool setOptionValue(const std::string& sOptionName, const Variant& oValue) noexcept;

	bool getPlayerKeyActionFromCapabilityKey(int32_t nCapabilityId, stmi::HARDWARE_KEY eKey
											, shared_ptr<PrefPlayer>& refPlayer, int32_t& nKeyActionId) const noexcept override;
	bool getPlayerKeyActionFromCapabilityKey(int32_t nCapabilityId, stmi::HARDWARE_KEY eKey
											, int32_t& nTeam, int32_t& nMate, int32_t& nKeyActionId) const noexcept override;
	bool getCapabilityPlayer(int32_t nCapabilityId, shared_ptr<PrefPlayer>& refPlayer) const noexcept override;
	bool getCapabilityPlayer(int32_t nCapabilityId, int32_t& nTeam, int32_t& nMate) const noexcept override;
	/** The player to which a capability is assigned.
	 * @param nCapabilityId The id of the capability.
	 * @param refPlayer (output) The player the capability is assigned to or unchanged.
	 * @return Whether the capability is assigned to a player.
	 */
	bool getCapabilityPlayerFull(int32_t nCapabilityId, shared_ptr<Player>& refPlayer) const noexcept;

	/** Get all the capability classes handled by the preferences.
	 * These are the classes of devices that can be assigned to key actions and players.
	 * @return The capability classes.
	 */
	std::vector<stmi::Capability::Class> getCapabilityClasses() const noexcept;
	/** Get the capability class device index of a capability.
	 * The device index is not the device id. The device index is an internal representation
	 * of the preferences. All the capabilities of the same capability class are
	 * assigned indexes 0, 1, and so on.
	 *
	 * This can be used to serialize the preferences.
	 * @param p0Capability The capability. Cannot be null.
	 * @return The device index. Is &gt;= 0 for a supported capability or -1 if not found (the corresponding device was removed).
	 */
	int32_t getCapabilityClassDeviceIdx(stmi::Capability const* p0Capability) const noexcept;
	/** Get the capability from the capability class device index.
	 * It is the inverse of getCapabilityClassDeviceIdx().
	 *
	 * This can be used to serialize the preferences.
	 * @param oCapaClass The capability class. Must be in getCapabilityClasses().
	 * @param nDeviceIdx The device index. Must be &gt;= 0.
	 * @return The capability or null if not found (device index too high).
	 */
	shared_ptr<stmi::Capability> getCapabilityFromClassDeviceIdx(const stmi::Capability::Class& oCapaClass, int32_t nDeviceIdx) const noexcept;

	const shared_ptr<AppConfig>& getAppConfig() const noexcept override { return m_refAppConfig; }
	const shared_ptr<StdConfig>& getStdConfig() noexcept { return m_refStdConfig; }

	class Player : public PrefPlayer, public std::enable_shared_from_this<Player>
	{
	public:
		Player() noexcept
		: m_nMate(0)
		, m_nPlayer(0)
		, m_nAIOptionIdx(0)
		, m_p0Team(nullptr)
		{
		}
		const std::string& getName() const noexcept override { return m_sName; }
		bool isAI() const noexcept override { return m_aOption[m_nAIOptionIdx].getBool(); }
		const shared_ptr<PrefTeam> getTeam() const noexcept override { return m_p0Team->shared_from_this(); }
		Variant getOptionValue(const std::string& sOptionName) const noexcept override;
		int32_t getMate() const noexcept override { return m_nMate; }
		int32_t get() const noexcept override { return m_nPlayer; }

		/** Get the player's team.
		 * @return The team. Can't be null.
		 */
		inline shared_ptr<Team>& getTeamFull() noexcept { return m_p0Team->m_p0Prefs->m_aTeam[m_p0Team->get()]; }

		/** Set the player's name.
		 * If another existing player already has the name the operation fails
		 * and `false` is returned.
		 * @param sNewName The new name. Cannot be empty.
		 * @return Whether the name could be changed.
		 */
		bool setName(const std::string& sNewName) noexcept;
		/** Get the capability and key assigned a key action.
		 * The returned pair is {null, stmi::HK_NULL} if the key action wasn't defined
		 * or {null, eKey} if the capability was removed at runtime.
		 * @param nKeyActionId The key action id.
		 * @return The capability and key.
		 */
		std::pair<stmi::Capability*, stmi::HARDWARE_KEY> getKeyValue(int32_t nKeyActionId) const noexcept;
		/** Assign a capability key to a player's key action.
		 * If the new capability key is already assigned to another key action it is
		 * swapped.
		 * @param nKeyActionId The key action id.
		 * @param p0Capability The capability. Cannot be null.
		 * @param eKey The key. Cannot be stmi::HK_NULL.
		 * @return Whether a swap was necessary.
		 */
		bool setKeyValue(int32_t nKeyActionId, stmi::Capability* p0Capability, stmi::HARDWARE_KEY eKey) noexcept;
		/** Sets player to AI or human.
		 * @param bIsAI Whether the player is AI.
		 * @return Whether the operation succeeded.
		 */
		bool setAI(bool bIsAI) noexcept;
		/** Set the value of a player's option.
		 * @param sOptionName The name of the option. Must exist and not be read only.
		 * @param oValue The new value.
		 * @return Whether the operation succeeded.
		 */
		bool setOptionValue(const std::string& sOptionName, const Variant& oValue) noexcept;
		/** Assign capability to player.
		 * If a capability is already assigned to another player it is
		 * first unassigned from the other player.
		 * @param refCapability The capability. Cannot be null.
		 * @return Whether could assign it.
		 */
		bool assignCapability(const shared_ptr<stmi::Capability>& refCapability) noexcept;
		/** Unassign capability from player.
		 * @param refCapability The capability. Cannot be null.
		 * @return Whether could unassign it.
		 */
		bool unassignCapability(const shared_ptr<stmi::Capability>& refCapability) noexcept;
		/** Get the capabilities assigned to the player.
		 * @return The capabilities. Elements can't be null.
		 */
		std::vector< shared_ptr<stmi::Capability> > getCapabilities() const noexcept override;
		/** Request a capability of a registered class assigned to this player.
		 * If more than one capability of class TCapa is assigned to this player,
		 * one is chosen.
		 * @param refCapa [out] The registered Capability subclass or null if not assigned.
		 * @return Whether refCapa was set.
		 */
		template < typename TCapa >
		bool getCapability(shared_ptr<TCapa>& refCapa) const noexcept
		{
			static_assert(std::is_base_of<stmi::Capability, TCapa>::value && !std::is_base_of<TCapa, stmi::Capability>::value
							, "TCapa must be subclass of stmi::Capability");
			const shared_ptr<stmi::Capability>& refSubCapa = getCapabilityInternal(stmi::Capability::Class{typeid(TCapa)});
			if (!refSubCapa) {
				refCapa.reset();
				return false; //----------------------------------------------------
			}
			refCapa = std::static_pointer_cast<TCapa>(refSubCapa);
			return true;
		}
		/** Request a capability of a registered class assigned to this player.
		 * If more than one capability of class oClass is assigned to this player
		 * one is chosen.
		 * @param oClass The registered capability class.
		 * @return The assigned capability or null.
		 */
		shared_ptr<stmi::Capability> getCapability(const stmi::Capability::Class& oClass) const noexcept;
	private:
		const shared_ptr<stmi::Capability>& getCapabilityInternal(const stmi::Capability::Class& oClass) const noexcept;
	private:
		friend class StdPreferences;
		int32_t m_nMate;
		int32_t m_nPlayer;
		// Capabilities assigned to this player
		// Value: (nCapabilityClassIdx, nCapabilityIdx) where
		// nCapabilityClassIdx: index in m_aCapabilityClass
		// nCapabilityIdx: index in m_aCapabilityClass[nCapabilityClassIdx].m_aCapability
		std::vector< std::pair<int32_t, int32_t> > m_aCapabilities;
		std::string m_sName; // has to be unique among players and not empty
		// Index: KeyActionId, Size: m_refStdConfig->getTotKeyActionIds()
		// Value: (nCapabilityClassIdx, nCapabilityIdx, key)
		// nCapabilityClassIdx: index in m_aCapabilityClass
		// nCapabilityIdx: index in m_aCapabilityClass[nCapabilityClassIdx].m_aCapability
		// The keyaction is undefined if either
		//   nCapabilityIdx is -1
		//   key is HK_NULL
		std::vector< std::tuple<int32_t, int32_t, stmi::HARDWARE_KEY> > m_aKeyActionCapaHK;
		// Size: m_refStdConfig->getOptions(OwnerType::PLAYER).size()
		std::vector<Variant> m_aOption;
		// Index in m_aOption of AI Option
		int32_t m_nAIOptionIdx;
		Team* m_p0Team;
	};
	class Team : public PrefTeam, public std::enable_shared_from_this<Team>
	{
	public:
		Team() noexcept
		: m_nTeam(0)
		, m_nAIOptionIdx(0)
		, m_p0Prefs(nullptr)
		{
		}
		const std::string& getName() const noexcept override { return m_sName; }
		bool isAI() const noexcept override { return !isHumanPriv(); }
		int32_t getTotMates() const noexcept override { return static_cast<int32_t>(m_aTeammate.size()); }
		const shared_ptr<PrefPlayer> getMate(int32_t nMate) const noexcept override;
		Variant getOptionValue(const std::string& sOptionName) const noexcept override;
		int32_t get() const noexcept override { return m_nTeam; }

		/** Get a teammate.
		 * @param nMate The index. Must be `>= 0` and smaller than getTotMates().
		 * @return The team's player. Cannot be null.
		 */
		const shared_ptr<Player>& getMateFull(int32_t nMate) const noexcept;

		/** Set the team's name.
		 * If another existing team already has the name the operation fails
		 * and `false` is returned.
		 * @param sNewName The new name. Cannot be empty.
		 * @return Whether the name could be changed.
		 */
		bool setName(const std::string& sNewName) noexcept;
		/** Set the value of a team's option.
		 * @param sOptionName The name of the option. Must exist and not be read only.
		 * @param oValue The new value.
		 * @return Whether the operation succeeded.
		 */
		bool setOptionValue(const std::string& sOptionName, const Variant& oValue) noexcept;
		/** Get the number of AI teammates.
		 * @return The number of team's non human players.
		 */
		int32_t getTotAIMates() const noexcept;
		// /* Sets the AI for all the team's mates.
		//  * Depending on the configuration's constraints this operation might
		//  * (partially) fail in which case `false` is returned.
		//  * @param bAI  Whether the team's mates are AI.
		//  * @return Whether the operation was successful.
		//  */
		//bool setAllMatesAI(bool bAI) noexcept;
		/** Sets the number of the mates of the team.
		 * Depending on the current number of mates, players are deleted or added
		 * accordingly.
		 * @param nTotMates The new number of mates. Must be &gt;= 1.
		 * @return Whether the target number of mates was reached.
		 */
		bool setTotMates(int32_t nTotMates) noexcept;
	private:
		bool isHumanPriv() const noexcept;
	private:
		friend class StdPreferences;
		friend class Player;
		std::string m_sName; // has to be unique among teams and not empty
		int32_t m_nTeam;
		std::vector< shared_ptr<Player> > m_aTeammate;
		// Size: m_refAppConfig->getOptions(OwnerType::TEAM).size()
		std::vector<Variant> m_aOption;
		// Index in m_aOption of AI Option
		int32_t m_nAIOptionIdx;
		StdPreferences* m_p0Prefs;
	};
private:

	friend class StdPreferences::Player;
	friend class StdPreferences::Team;

	void populatePlayersKeyActions() noexcept;
	// nClassIdx, nCapaIdx, eKey are unchanged if false is returned
	// which means couldn't populate
	bool populatePlayerKeyAction(const shared_ptr<Player>& refPlayer, int32_t nKeyActionId
								, int32_t& nClassIdx, int32_t& nCapaIdx, stmi::HARDWARE_KEY& eKey
								, int32_t& nPriorityClassIdx, int32_t& nPriorityCapaIdx, int32_t& nPriorityDefaultIdx) noexcept;
	bool capabilityKeyInUse(int32_t nClassIdx, int32_t nCapaIdx, stmi::HARDWARE_KEY eKey) const noexcept;
	stmi::HARDWARE_KEY findFreeKey(int32_t nClassIdx, int32_t nCapaIdx) noexcept;
	//TODO Eliminate methods that are not used
	void initCreatePlayers() noexcept;
	void initCreateCapabilities() noexcept;
	void initListenToDeviceMgmt() noexcept;
	void deviceAdded(const shared_ptr<stmi::Device>& refDevice) noexcept;
	void deviceRemoved(const shared_ptr<stmi::Device>& refDevice) noexcept;
	struct CapabilityData;
	void removeReferencesToCapability(int32_t nClassIdx, int32_t nCapaIdx, CapabilityData& oCapabilityData) noexcept;
	void moveReferencesToCapability(int32_t nClassIdx, int32_t nRemovedCapaIdx, int32_t nLastCapaIdx
									, CapabilityData& oCapabilityData, CapabilityData& oLastCapabilityData) noexcept;
	// returns -1 if not found
	int32_t getCapabilityClassIdx(const stmi::Capability::Class& oClass) const noexcept;
	// returns null if not found
	stmi::Capability* getCapabilityFromIdxs(int32_t nClassIdx, int32_t nCapaIdx) const noexcept;
	// returns false if not found
	bool getCapabilityFromIdxs(int32_t nClassIdx, int32_t nCapaIdx, shared_ptr<stmi::Capability>& refCapability) const noexcept;
	// returns false if not found
	bool getIdxsFromCapability(stmi::Capability const* p0Capability, int32_t& nClassIdx, int32_t& nCapaIdx) const noexcept;
	// returns false if not found
	bool getIdxsFromCapabilityId(int32_t nCapabilityId, int32_t& nClassIdx, int32_t& nCapaIdx) const noexcept;
	// returns null if not assigned to a player
	const shared_ptr<Player>& getCapabilityPlayer(int32_t nCapabilityId) const noexcept;
	bool getPlayerKeyActionFromCapabilityKey(int32_t nCapabilityId, stmi::HARDWARE_KEY eKey
											, std::unordered_map<stmi::HARDWARE_KEY, std::pair< shared_ptr<Player>, int32_t > >::const_iterator& itFindKey) const noexcept;
	// BEWARE! When allocated a player is already counted in getTotPlayers()
	// even though it isn't yet added to a team!
	shared_ptr<Player> playerAlloc() noexcept;
	void playerFree(const shared_ptr<Player>& refPlayer) noexcept;
	void playerAddToTeam(const shared_ptr<Player>& refPlayer, const shared_ptr<Team>& refTeam) noexcept;
	void playerRemoveFromTeam(const shared_ptr<Player>& refPlayer, const shared_ptr<Team>& refTeam) noexcept;
	// This actually adds an empty team leaving it in an invalid state,
	// it has to be followed by playerAddToTeam()
	shared_ptr<Team> teamAlloc() noexcept;
	void teamFree(const shared_ptr<Team>& refTeam) noexcept;

	bool setTotPlayers(int32_t nPlayers, int32_t nTeam) noexcept;

	void recalcStuff() noexcept;

private:
	shared_ptr<AppConfig> m_refAppConfig;
	shared_ptr<StdConfig> m_refStdConfig;
	shared_ptr<stmi::EventListener> m_refDeviceMgmtListener;
	shared_ptr<stmi::CallIf> m_refCallIfDeviceMgmt;

	bool m_bEditMode; // true: edit mode, false: runtime mode

	// If some Player's keyaction's key is not set this is true
	// This value tells to populateKeyActions whether to go through
	// all Players and try to set their undefined keys
	// It is set to true when a new player is added or a capability is removed.
	bool m_bUndefinedKeyActions;

	std::vector< shared_ptr<Team> > m_aTeam; // teams in use
	std::vector< shared_ptr<Team> > m_aFreeTeams; // teams created that can be reused
	std::vector< shared_ptr<Player> > m_aFreePlayers; // players created that can be reused
	std::unordered_map<std::string, shared_ptr<Team> > m_oNamedTeams; // Key: team name
	std::unordered_map<std::string, shared_ptr<Player> > m_oNamedPlayers; // Key: player name

	// The playerNr is unique and is the index into the m_aPlayerNrToPlayer
	//   m_aPlayerNrToPlayer[nPlayer].get() == nPlayer
	// ex. 2 teams: the first has two mates, the second three
	//   nPlayer 0 is nTeam 0, nMate 0
	//           1 is       0, nMate 1
	//           2 is nTeam 1, nMate 0
	//           3 is       1, nMate 1
	//           4 is       1, nMate 2
	std::vector< shared_ptr<Player> > m_aPlayerNrToPlayer; // Size: getTotPlayers(), Index: playerNr, Value: Player

	std::vector<Variant> m_aOption; // Size: m_refStdConfig->getOptions(OwnerType::GAME).size())

	struct CapabilityData
	{
		// This reference is only here for Player::getCapabilities()
		shared_ptr<stmi::Capability> m_refCapability;
		// hardware-key to (player, key action id) mapping for the capability.
		std::unordered_map<stmi::HARDWARE_KEY, std::pair< shared_ptr<Player>, int32_t > > m_oHKPlayerKeyAction;
		// null if not assigned to a player
		shared_ptr<Player> m_refPlayer;
		// During runtime mode removed capability are only marked as such
		// so that if a device is re-added with the same capability class
		// it will substitute the marked one with all its key actions
		// so that the game doesn't have to be interrupted.
		// If the capability is m_bRuntimeAdded==true and then removed it is
		// just removed from this structure.
		// In edit mode this is always false.
		bool m_bRuntimeRemoved;
		// During runtime mode added capabilities that do not substitute a
		// removed one are marked as added so that they can be used when
		// a capability (not marked as added) is removed.
		// In edit mode this is always false.
		bool m_bRuntimeAdded;
		//TODO void swap( CapabilityData& oOther );
	};
	struct CapabilityClassData
	{
		// The device capabilities of the class currently in the device manager.
		// Used for fast find.
		// A pointer is used instead of a shared_ptr because we rely on
		// DeviceMgmtEvent to inform about removed devices (and their capabilities)
		// which should avoid the pointers dangling.
		// In running mode the capability value can be set to null, which means
		// the slot is free if a capability is added again (can't be null in edit mode).
		std::vector<stmi::Capability*> m_aCapability;
		// The data for m_aCapability capabilities.
		// Size: m_aCapability.size()
		std::vector<CapabilityData> m_aCapabilityData;
		// The number of capabilities assigned to a player.
		// If m_bMoreCapabilitiesPerClassAssignedToPlayer is false and
		// there actually is only one human player (m_refDefaultPlayer is not null),
		// this variable is used to determine whether `m_aCapability[0]` is the
		// default capability for the class.
		// If the value is 0 then no capability of this class is assigned to
		// the default player so m_aCapability[0], if available, is the default capability
		// (is automatically assigned to default player).
		// If the value is not 0 then `getCapabilityPlayer(refCapability)` only
		// returns the default player for the explicitly assigned capabilities.
		int32_t m_nTotAssignedCapabilities;
	};
	// The capability classes covered by the device manager that are not for
	// device management itself. Used for fast find.
	// The classes can't change after initialization, that is, the device manager
	// cannot add or remove classes its devices are supposed to provide.
	std::vector<stmi::Capability::Class> m_aCapabilityClass;
	// The data for m_aCapabilityClass classes.
	// Size: m_aCapabilityClass.size()
	std::vector<CapabilityClassData> m_aCapabilityClassData;

	// This pointer is not null if
	// - m_refStdConfig->isEventAssignedToActivePlayer() is true  and
	// - there is exactly one human player
	shared_ptr<Player> m_refDefaultPlayer;
private:
	StdPreferences() = delete;
};

} // namespace stmg

#endif	/* STMG_STD_PREFERENCES_H */

