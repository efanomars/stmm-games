/*
 * File:   appconfig.h
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

#ifndef STMG_APP_CONFIG_H
#define STMG_APP_CONFIG_H

#include "ownertype.h"

#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class AppConstraints; }
namespace stmg { class Option; }
namespace stmg { template <class T> class NamedObjIndex; }
namespace stmi { class DeviceManager; }

namespace stmg
{

using std::shared_ptr;

/** Read-only interface to application configuration data.
 */
class AppConfig
{
public:
	virtual ~AppConfig() noexcept = default;

	/** The name of the application.
	 * @return The application name. Cannot be empty.
	 */
	virtual const std::string& getAppName() const noexcept = 0;
	/** The version of the application.
	 * @return The application version. Cannot be empty.
	 */
	virtual const std::string& getAppVersion() const noexcept = 0;
	/** The application constraints.
	 * @return The constraints.
	 */
	virtual const AppConstraints& getAppConstraints() const noexcept = 0;
	/** The device manager that generates the input events.
	 * @return The device manager. Cannot be null.
	 */
	virtual shared_ptr<stmi::DeviceManager> getDeviceManager() const noexcept = 0;

	/** The total number of key actions.
	 * A valid key action id is a number from `0` to `getTotKeyActions()-1`.
	 * @return The total number of key actions.
	 */
	virtual int32_t getTotKeyActions() const noexcept = 0;
	/** Get to the key action id from its name.
	 * @param sKeyActionName The key action name.
	 * @return The key action id or -1 if not found.
	 */
	virtual int32_t getKeyActionId(const std::string& sKeyActionName) const noexcept = 0;

	/** Get the options for an owner type.
	 * @param eOwner The owner type.
	 * @return The options.
	 */
	virtual const NamedObjIndex< shared_ptr<Option> >& getOptions(OwnerType eOwner) const noexcept = 0;
	/** Get an option by name.
	 * @param eOwner The owner type.
	 * @param sName The name of the option. Cannot be empty.
	 * @return  The option or null if not defined.
	 */
	virtual const shared_ptr<Option>& getOption(OwnerType eOwner, const std::string& sName) const noexcept = 0;
	/** Tells whether events should be assigned to the active player.
	 * If this function returns `true`, the game, when it receives an event from a 
	 * device's capability that wasn't assigned to a player in the preferences,
	 * will try to assign it to the unique active human player.
	 * If more than one human player is active at the moment the event is received,
	 * it is dropped.
	 * 
	 * This is useful in turn based games where the two players use the same device
	 * (ex. the mouse). Be aware that it can cause inconsistencies. Example:
	 * if between the press and the release of a joystick button the active player
	 * is changed the release event is sent to a different player.
	 * 
	 * @return Whether to look for a target human player for an unassigned event.
	 */
	virtual bool isEventAssignedToActivePlayer() const noexcept = 0;
	/** Whether the game plays sounds.
	 * If this function returns false sounds are not played and the view preferences
	 * should not show widgets to choose the volume.
	 * @return Whether sounds activated.
	 */
	virtual bool soundEnabled() const noexcept = 0;
	/** Whether allows playback devices to be assigned to players.
	 * If true the game allows playback devices such as head phones to be assigned
	 * to each player for personalized output.
	 * Ignored if playsSounds() returns false.
	 * @return Whether sounds can be player specific.
	 */
	virtual bool canPlayPerPlayerSounds() const noexcept = 0;
	/** Whether in testing mode.
	 * This flag can be used to output additional information.
	 * @return Whether testing.
	 */
	virtual bool isTestMode() const noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_APP_CONFIG_H */

