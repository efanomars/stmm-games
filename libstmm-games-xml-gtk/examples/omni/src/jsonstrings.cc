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
 * File:   jsonstrings.cc
 */

#include "jsonstrings.h"

namespace stmg
{

const std::string jas::s_sConfConstraints                       = "constraints";
const std::string jas::s_sConfConstraintsTeamsMin                 = "teamsMin";
const std::string jas::s_sConfConstraintsTeamsMax                 = "teamsMax";
const std::string jas::s_sConfConstraintsMatesPerTeamMax          = "matesPerTeamMax";
const std::string jas::s_sConfConstraintsPlayersMax               = "playersMax";
const std::string jas::s_sConfConstraintsAIMatesPerTeamMax        = "aiMatesPerTeamMax";
const std::string jas::s_sConfConstraintsAllowMixedAIHumanTeam    = "allowMixedAIHumanTeam";
const std::string jas::s_sConfDm                                = "deviceManager";
const std::string jas::s_sConfDmEnableEventClasses                = "enableEventClasses";
const std::string jas::s_sConfDmEnDisableEventClasses             = "enDisableEventClasses";
const std::string jas::s_sConfDmPlugins                           = "plugins";
const std::string jas::s_sConfDmPluginsEnablePlugins                = "enablePlugins";
const std::string jas::s_sConfDmPluginsEnDisablePlugins             = "enDisablePlugins";
const std::string jas::s_sConfDmPluginsGroups                       = "groups";
const std::string jas::s_sConfDmPluginsVerbose                      = "verbose";
const std::string jas::s_sConfAssign                            = "assign";
const std::string jas::s_sConfAssignAllCapabilityClasses          = "allCapabilityClasses";
const std::string jas::s_sConfAssignCapabilityClasses             = "capabilityClasses";
const std::string jas::s_sConfAssignMaxCapabilitiesExplicitlyAssignedToPlayer = "maxCapabilitiesExplicitlyAssignedToPlayer";
const std::string jas::s_sConfAssignMoreCapabilitiesPerClassAssignedToPlayer  = "moreCapabilitiesPerClassAssignedToPlayer";
const std::string jas::s_sConfAssignCapabilitiesAutoAssignedToActivePlayer    = "capabilitiesAutoAssignedToActivePlayer";
const std::string jas::s_sConfKeyActions                        = "keyActions";
const std::string jas::s_sConfKeyActionsKeyActionNames            = "keyActionNames";
const std::string jas::s_sConfKeyActionsKeyActionDescription      = "keyActionDescription";
const std::string jas::s_sConfKeyActionsKeyActionDefaultClassKeys = "keyActionDefaultClassKeys";
const std::string jas::s_sConfKeyActionsCapabilityClassId           = "capabilityClassId";
const std::string jas::s_sConfKeyActionsDefaultKeys                 = "defaultKeys";
const std::string jas::s_sConfOptions                           = "options";
const std::string jas::s_sConfOptionsOptionType                   = "optionType";
const std::string jas::s_sConfOptionsOptionTypeInt                  = "int";
const std::string jas::s_sConfOptionsOptionTypeBool                 = "bool";
const std::string jas::s_sConfOptionsOptionTypeEnum                 = "enum";
const std::string jas::s_sConfOptionsOwnerType                    = "ownerType";
const std::string jas::s_sConfOptionsOwnerTypeGame                  = "GAME";
const std::string jas::s_sConfOptionsOwnerTypeTeam                  = "TEAM";
const std::string jas::s_sConfOptionsOwnerTypePlayer                = "PLAYER";
const std::string jas::s_sConfOptionsOptionName                   = "name";
const std::string jas::s_sConfOptionsDefaultValue                 = "defaultValue";
const std::string jas::s_sConfOptionsDescription                  = "description";
const std::string jas::s_sConfOptionsReadOnly                     = "readOnly";
const std::string jas::s_sConfOptionsVisible                      = "visible";
const std::string jas::s_sConfOptionsMasters                      = "masters";
const std::string jas::s_sConfOptionsMastersMasterName              = "masterName";
const std::string jas::s_sConfOptionsMastersMasterValues            = "masterValues";
const std::string jas::s_sConfOptionsIntOptionMin                 = "min";
const std::string jas::s_sConfOptionsIntOptionMax                 = "max";
const std::string jas::s_sConfOptionsEnumOptionEnums              = "enums";
const std::string jas::s_sConfOptionsEnumOptionEnumValue            = "enumValue";
const std::string jas::s_sConfOptionsEnumOptionEnumName             = "enumName";
const std::string jas::s_sConfOptionsEnumOptionEnumDesc             = "enumDesc";
const std::string jas::s_sConfMaxGamePlayedHistory              = "maxGamePlayedHistory";
const std::string jas::s_sConfSoundEnabled                      = "soundEnabled";
const std::string jas::s_sConfSoundPerPlayerAllowed             = "soundPerPlayerAllowed";

} // namespace stmg
