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
 * File:   stdconfig.h
 */

#ifndef STMG_STD_CONFIG_H
#define STMG_STD_CONFIG_H

#include "appconstraints.h"
#include "appconfig.h"

#include "ownertype.h"
#include "util/namedobjindex.h"
#include "util/namedobjindex.h"

#include <stmm-input/hardwarekey.h>
#include <stmm-input/capability.h>

#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class BoolOption; }
namespace stmg { class IntOption; }
namespace stmg { class Option; }
namespace stmi { class DeviceManager; }
namespace stmi { class PlaybackCapability; }
namespace stmi { class SndMgmtCapability; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class StdConfig : public AppConfig
{
public:
	/** Tells how device capabilities can be assigned (or not) to players.
	 * The default instance disables assignment of capabilities (explicitly
	 * and automatically) to players. That is the events that they generate
	 * are not sent to any player unless they define a key action.
	 */
	class CapabilityAssignment final
	{
	public:
		/** Whether all non device manager capability class instances can be assigned
		 *  to a player. Default is `true`. */
		bool m_bAllCapabilityClasses = true;
		/** The capability classes whose instances can be assigned to a player.
		 * If m_bAllCapabilityClasses is `true` this field is ignored.
		 *
		 * Note that the stmi::PlaybackCapability can always be assigned to a player
		 * if Init::m_bSoundPerPlayerAllowed is true so it cannot be an element of
		 * this vector.
		 * The default is empty.
		 */
		std::vector<stmi::Capability::Class> m_aCapabilityClasses;
		/** The maximum number of capabilities that can be explicitly assigned
		 * to a player. The default is `0`, which means you cannot explicitly
		 * assign capabilities to a player, only automatically to a single
		 * active human player.
		 *
		 * Beware! This number doesn't include the playback capability that
		 * can be assigned to a player if Init::m_bSoundPerPlayerAllowed is
		 * true.
		 * @see m_bCapabilitiesAutoAssignedToActivePlayer.
		 */
		int32_t m_nMaxCapabilitiesExplicitlyAssignedToPlayer = 0;
		/** Whether more than one instance per capability class can be assigned
		 * to a player. The default is `false`.
		 * This only makes sense in very unlikely games where for example
		 * two keyboards or joysticks per player are needed.
		 *
		 * Note: at most one sound playback capability can be assigned to a player
		 * regardless of the value of this field.
		 */
		bool m_bMoreCapabilitiesPerClassAssignedToPlayer = false;
		/** When only one human player is active, tells whether capabilities are
		 * automatically assigned to it. If it's the case events generated by
		 * the capabilities according to m_bAllCapabilityClasses, m_aCapabilityClasses
		 * and m_bMoreCapabilitiesPerClassAssignedToPlayer are sent to that player.
		 * The default is `false`.
		 */
		bool m_bCapabilitiesAutoAssignedToActivePlayer = false;
		/** Tells whether the data in this structure instance makes sense.
		 * @return Whether it is valid.
		 */
		bool isValid() const noexcept
		{
			return (m_nMaxCapabilitiesExplicitlyAssignedToPlayer >= 0);
		}
		/** Tells whether a certain capability class can be assigned to players.
		 * Note that The stmi::PlaybackCapability is not included in this check.
		 * @param oClass The capability class.
		 * @return Whether devices providing the capability can be assigned to players.
		 */
		bool canBeAssigned(const stmi::Capability::Class& oClass) const noexcept
		{
			return m_bAllCapabilityClasses
				|| (std::find(m_aCapabilityClasses.begin(), m_aCapabilityClasses.end(), oClass) != m_aCapabilityClasses.end());
		}
	};
	/** The key action class.
	 * Can be used by the preferences dialog.
	 */
	class KeyAction final
	{
	public:
		/** Construct a key action.
		 * Having more than one name for the same action allows to mix different games into one.
		 *
		 * Names must start with alphanumeric character.
		 *
		 * Ex. The hardware key stmi::HK_UP can be assigned to a tetris block with key action
		 * name "Rotate" and to a cursor that responds to a key action named "MoveUp".
		 *
		 * @param aKeyActionName The non empty names that identify the same key action.
		 * @param sDescription The description as it should appear in the preferences dialog.
		 * @param aDefaultClassKeys The capability classes and their default keys.
		 */
		KeyAction(const std::vector<std::string>& aKeyActionName, const std::string& sDescription
					, const std::vector< std::pair<stmi::Capability::Class, std::vector<stmi::HARDWARE_KEY> > >& aDefaultClassKeys) noexcept;
		/** Construct a key action.
		 * The name must start with alphanumeric character.
		 * @param sKeyActionName The name of the key action. Must be unique.
		 * @param sDescription The description of the key action.
		 * @param oCapaClass The device's capability class for which the default keys are defined.
		 * @param aDefaultClassKeys The keys that can be used as defaults by a preferences dialog.
		 */
		KeyAction(const std::string& sKeyActionName, const std::string& sDescription
				, const stmi::Capability::Class& oCapaClass, const std::vector<stmi::HARDWARE_KEY>& aDefaultClassKeys) noexcept;
		/** Names associated with the key action.
		 */
		const std::vector<std::string>& getNames() const noexcept { return m_aKeyActionName; }
		/** Description of the key action.
		 */
		const std::string& getDescription() const noexcept { return m_sDescription; }
		/** Capability classes that have default keys for this key action.
		 */
		const std::vector<stmi::Capability::Class>& getClasses() const noexcept { return m_aCapaClasses; }
		/** Default keys of a capability class for this key action.
		 */
		const std::vector<stmi::HARDWARE_KEY>& getClassDefaultKeys(const stmi::Capability::Class& oClass) const noexcept;
	private:
		friend class StdConfig;
		std::vector<std::string> m_aKeyActionName;
		std::string m_sDescription;
		// map from class to vector of keys
		std::vector<stmi::Capability::Class> m_aCapaClasses; // Unique classes
		std::vector< std::vector<stmi::HARDWARE_KEY> > m_aClassDefaultKeys; // Size: m_aCapaClasses.size()
	};

	struct Init
	{
		std::string m_sAppName; /**< The application name. Cannot be empty and only contain alphanum and '-' and '_'. */
		std::string m_sAppVersion; /**< The application version. Cannot be empty. */
		shared_ptr<stmi::DeviceManager> m_refDeviceManager; /**< The device manager, source of input events and playback capability provider. Cannot be null. */
		AppConstraints m_oAppConstraints; /**< Constraints. */
		CapabilityAssignment m_oCapabilityAssignment; /**< How capabilities are assigned to players. */
		std::vector<KeyAction> m_aKeyActions; /**< The key actions. Names must be unique. */
		std::vector< shared_ptr<Option> > m_aOptions; /**< The (possibly interrelated) options. Option names starting with an underscore are reserved (not allowed here). */
		int32_t m_nMaxGamePlayedHistory = 10; /**< The maximum number of latest game names played. Can be 0. Default is 10. */
		bool m_bSoundEnabled = true; /**< Whether game plays sounds. Default is true. */
		bool m_bSoundPerPlayerAllowed = false; /**< Whether the game allows per player sound output. Must be false if m_bSoundEnabled is false. Default is false. */
		bool m_bTestMode = false; /**< Whether test mode. Default is false. */
	};
	/** Constructor.
	 *
	 * An option cannot be slave of an option that is not also in Init::m_aOptions.
	 *
	 * An AI BoolOption is always defined for each type of owner and created automatically
	 * if not in Init::m_aOptions. If you need to create an AI option use StdConfig::createAIOption().
	 * AI options should be passed with Init::m_aOptions only if AI players are allowed,
	 * that is if `getAppConstraints().allowsAI() == true`.
	 * If created automatically AI options are placed before all other options
	 * (passed in Init::m_aOptions).
	 *
	 * Sound options are always defined and created automatically if not in Init::m_aOptions.
	 * If you need to create a sound option use StdConfig::createSoundEnabledOption(),
	 * StdConfig::createPerPlayerSoundOption(), StdConfig::createSoundVolumeOption().
	 *
	 * Sound options should only be passed in Init::m_aOptions if `Init::m_bSoundEnabled == true`.
	 * The option created by StdConfig::createSoundEnabledOption() should only be passed
	 * if `Init::m_bSoundPerPlayerAllowed == true`. The option created by StdConfig::createSoundVolumeOption()
	 * for players should only be passed if `Init::m_bSoundPerPlayerAllowed == true`.
	 *
	 * @param oInit The initialization structure.
	 */
	explicit StdConfig(StdConfig::Init&& oInit) noexcept;

	const std::string& getAppName() const noexcept override { return m_sAppName; }
	const std::string& getAppVersion() const noexcept override { return m_sAppVersion; }

	/** Get the device manager.
	 * The device manager is the source of the input events.
	 * @return The device manager. Cannot be null.
	 */
	shared_ptr<stmi::DeviceManager> getDeviceManager() const noexcept override { return m_refDeviceManager; }

	const AppConstraints& getAppConstraints() const noexcept override { return m_oAppConstraints; }

	/** How capabilities are assigned (or not) to players.
	 * @return The mode.
	 */
	inline const CapabilityAssignment& getCapabilityAssignment() const noexcept { return m_oCapabilityAssignment; }

	int32_t getTotKeyActions() const noexcept override;
	int32_t getKeyActionId(const std::string& sKeyActionName) const noexcept override;

	bool isEventAssignedToActivePlayer() const noexcept override
	{
		return m_oCapabilityAssignment.m_bCapabilitiesAutoAssignedToActivePlayer;
	}
	bool soundEnabled() const noexcept override { return m_bSoundEnabled; }
	bool canPlayPerPlayerSounds() const noexcept override { return m_bSoundPerPlayerAllowed; }
	bool isTestMode() const noexcept override
	{
		return m_bTestMode;
	}
	/** The maximum number of games in the recently played games history.
	 * @return The maximum number of remembered games. Is &gt;= 0.
	 */
	int32_t getMaxGamePlayedHistory() const noexcept
	{
		return m_nMaxGamePlayedHistory;
	}

	/** Get the default playback capability.
	 * @return The playback capability or null if none found.
	 */
	shared_ptr<stmi::PlaybackCapability> getDefaultPlayback() const noexcept;

	/** Get Key Action data.
	 * @param nKeyActionId The key action id. Cannot be a negative number.
	 * @return The key action object or undefined if not a valid id.
	 */
	const KeyAction& getKeyAction(int32_t nKeyActionId) const noexcept;

	const NamedObjIndex< shared_ptr<Option> >& getOptions(OwnerType eOwner) const noexcept override;
	const shared_ptr<Option>& getOption(OwnerType eOwner, const std::string& sName) const noexcept override;

	int32_t getTotVisibleOptions() const noexcept;
	int32_t getTotVisibleOptions(OwnerType eOwner) const noexcept;

	/** The AI option name.
	 * This name is the same for OwnerType::GAME, OwnerType::TEAM and OwnerType::PLAYER
	 * options.
	 *
	 * The option, if passed to the constructor with m_aOptions, must be
	 * a BoolOption, not be a slave.
	 * @return The AI option name.
	 */
	static const std::string& getAIOptionName() noexcept;
	/** The sound enabled option name.
	 * @return The option name.
	 */
	static const std::string& getSoundEnabledOptionName() noexcept;
	/** The "per player sound" option name.
	 * @return The option name.
	 */
	static const std::string& getPerPlayerSoundOptionName() noexcept;
	/** The sound volume option name.
	 * @return The option name.
	 */
	static const std::string& getSoundVolumeOptionName() noexcept;
	/** Create an AI option to pass to the constructor StdConfig::StdConfig().
	 *
	 * This function should only be used if `getAppConstraints().allowsAI() == true`.
	 *
	 * If an AI option for a certain owner type is not passed to the constructor it
	 * is created automatically with some default description string. This
	 * function allows you to create the option and set the description string
	 * and possibly use it as a master option for another slave option (ex. 'AISmartness').
	 *
	 * If the owner is game, the created option is readonly and invisible and the
	 * default value is true iff AI players are allowed.
	 *
	 * If the owner is team, the option is readonly. It is visible iff AI players
	 * are allowed. The default value should be ignored since a team should be
	 * considered AI only if all its players are AI.
	 *
	 * If the owner is player, the option is modifiable and visible iff AI players
	 * are allowed. The default value is `false` (human player).
	 *
	 * @param eOwnerType The owner type.
	 * @param sAIOptionDesc The description of the option.
	 * @return The AI option for a certain owner type.
	 */
	static shared_ptr<BoolOption> createAIOption(OwnerType eOwnerType, const std::string& sAIOptionDesc) noexcept;

	/** Create the SoundEnabled option to pass to the constructor of StdConfig.
	 * The created option is owned by game and is read-only and invisible.
	 * Its default value is set to bSoundEnabled. It must be the same as
	 * Init::m_bSoundEnabled.
	 *
	 * This function allows you to create the option to possibly use it as
	 * a master option for another slave option (ex. 'EnableCountdownSounds').
	 *
	 * @param bSoundEnabled Whether sound is enabled or not.
	 * @return The unique SoundEnabled option.
	 */
	static shared_ptr<BoolOption> createSoundEnabledOption(bool bSoundEnabled) noexcept;
	/** Create the PerPlayerSound option to pass to the constructor of StdConfig.
	 *
	 * @param sOptionDesc The description of the option.
	 * @return The PerPlayerSound game option.
	 */
	static shared_ptr<BoolOption> createPerPlayerSoundOption(const std::string& sOptionDesc) noexcept;
	/** Create a SoundVolume option to pass to the constructor of StdConfig.
	 * @param eOwnerType The owner type. Must either OwnerTyoe::GAME or OwnerTyoe::PLAYER.
	 * @param sOptionDesc The description of the option.
	 * @param refPerPlayerSound The master PerPlayerSound option possibly created with createSoundEnabledOption(). Cannot be null
	 * @param refAIPlayer The master AI option if `eOwnerType == OwnerTyoe::PLAYER` and AI allowed, otherwise must be null.
	 * @return The SoundVolume option.
	 */
	static shared_ptr<IntOption> createSoundVolumeOption(OwnerType eOwnerType, const std::string& sOptionDesc
														, const shared_ptr<BoolOption>& refPerPlayerSound, const shared_ptr<BoolOption>& refAIPlayer) noexcept;

private:
	static shared_ptr<BoolOption> createAIOption(OwnerType eOwnerType, const std::string& sAIOptionDesc, bool bAllowAI) noexcept;
	static shared_ptr<BoolOption> createPerPlayerSoundOption(const std::string& sOptionDesc, bool bAllowPerPlayerSound) noexcept;
	static shared_ptr<IntOption> createSoundVolumeOption(OwnerType eOwnerType, const std::string& sOptionDesc
														, const shared_ptr<BoolOption>& refPerPlayerSound, const shared_ptr<BoolOption>& refAIPlayer
														, bool bAllowSound) noexcept;

	void checkAIOptions(const std::vector< shared_ptr<Option> >& aOptions, bool bAllowAI
						, bool& bAIGameDefined, bool& bAITeamDefined, shared_ptr<BoolOption>& refAIOptionPlayer) noexcept;
	void checkSoundOptions(const std::vector< shared_ptr<Option> >& aOptions, bool& bSoundEnabledDefined
							, shared_ptr<BoolOption>& refPerPlayerSound, bool& bGameVolumeDefined, bool& bPlayerVolumeDefined) noexcept;
	NamedObjIndex< shared_ptr<Option> >& getOptionsInternal(OwnerType eOwnerType) noexcept;
	void addOptions(const std::vector< shared_ptr<Option> >& aOptions) noexcept;
	void addOption(const shared_ptr<Option>& refOption) noexcept;
	void checkOptions(OwnerType eOwnerType) noexcept;
	#ifndef NDEBUG
	void checkMasterOptionExists(const shared_ptr<Option>& refMaster) const noexcept;
	void checkKeyActions() const noexcept;
	#endif //NDEBUG

private:
	std::string m_sAppName;
	std::string m_sAppVersion;
	shared_ptr<stmi::DeviceManager> m_refDeviceManager;

	// The sound mgmt capability of the device manager or null if sound disabled
	shared_ptr<stmi::SndMgmtCapability> m_refSndMgmtCapability;

	AppConstraints m_oAppConstraints;

	const CapabilityAssignment m_oCapabilityAssignment;

	std::vector<KeyAction> m_aKeyActions;
	//
	NamedObjIndex< shared_ptr<Option> > m_oGameOptions;
	NamedObjIndex< shared_ptr<Option> > m_oTeamOptions;
	NamedObjIndex< shared_ptr<Option> > m_oPlayerOptions;

	int32_t m_nMaxGamePlayedHistory;
	bool m_bSoundEnabled;
	bool m_bSoundPerPlayerAllowed;

	bool m_bTestMode;
private:
	StdConfig() = delete;
};

} // namespace stmg

#endif	/* STMG_STD_CONFIG_H */

