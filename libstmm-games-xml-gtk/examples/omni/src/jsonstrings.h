/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   inputstrings.h
 */

#ifndef STMG_OMNI_JSON_STRINGS_H
#define STMG_OMNI_JSON_STRINGS_H

#include <string>

namespace stmg
{

struct jas
{
static const std::string s_sConfConstraints;
static const std::string s_sConfConstraintsTeamsMin;
static const std::string s_sConfConstraintsTeamsMax;
static const std::string s_sConfConstraintsMatesPerTeamMax;
static const std::string s_sConfConstraintsPlayersMax;
static const std::string s_sConfConstraintsAIMatesPerTeamMax;
static const std::string s_sConfConstraintsAllowMixedAIHumanTeam;
static const std::string s_sConfDm;
static const std::string s_sConfDmEnableEventClasses;
static const std::string s_sConfDmEnDisableEventClasses;
static const std::string s_sConfDmPlugins;
static const std::string s_sConfDmPluginsEnablePlugins;
static const std::string s_sConfDmPluginsEnDisablePlugins;
static const std::string s_sConfDmPluginsGroups;
static const std::string s_sConfDmPluginsVerbose;
static const std::string s_sConfAssign;
static const std::string s_sConfAssignAllCapabilityClasses;
static const std::string s_sConfAssignCapabilityClasses;
static const std::string s_sConfAssignMaxCapabilitiesExplicitlyAssignedToPlayer;
static const std::string s_sConfAssignMoreCapabilitiesPerClassAssignedToPlayer;
static const std::string s_sConfAssignCapabilitiesAutoAssignedToActivePlayer;
static const std::string s_sConfKeyActions;
static const std::string s_sConfKeyActionsKeyActionNames;
static const std::string s_sConfKeyActionsKeyActionDescription;
static const std::string s_sConfKeyActionsKeyActionDefaultClassKeys;
static const std::string s_sConfKeyActionsCapabilityClassId;
static const std::string s_sConfKeyActionsDefaultKeys;
static const std::string s_sConfOptions;
static const std::string s_sConfOptionsOptionType;
static const std::string s_sConfOptionsOptionTypeInt;
static const std::string s_sConfOptionsOptionTypeBool;
static const std::string s_sConfOptionsOptionTypeEnum;
static const std::string s_sConfOptionsOwnerType;
static const std::string s_sConfOptionsOwnerTypeGame;
static const std::string s_sConfOptionsOwnerTypeTeam;
static const std::string s_sConfOptionsOwnerTypePlayer;
static const std::string s_sConfOptionsOptionName;
static const std::string s_sConfOptionsDefaultValue;
static const std::string s_sConfOptionsDescription;
static const std::string s_sConfOptionsReadOnly;
static const std::string s_sConfOptionsVisible;
static const std::string s_sConfOptionsMasters;
static const std::string s_sConfOptionsMastersMasterName;
static const std::string s_sConfOptionsMastersMasterValues;
static const std::string s_sConfOptionsIntOptionMin;
static const std::string s_sConfOptionsIntOptionMax;
static const std::string s_sConfOptionsEnumOptionEnums;
static const std::string s_sConfOptionsEnumOptionEnumValue;
static const std::string s_sConfOptionsEnumOptionEnumName;
static const std::string s_sConfOptionsEnumOptionEnumDesc;
static const std::string s_sConfMaxGamePlayedHistory;
static const std::string s_sConfSoundEnabled;
static const std::string s_sConfSoundPerPlayerAllowed;

};
} // namespace stmg

#endif /* STMG_OMNI_JSON_STRINGS_H */
