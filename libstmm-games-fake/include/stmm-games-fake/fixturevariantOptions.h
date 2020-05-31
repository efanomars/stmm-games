/*
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
 *
 */
/*
 * File:   fixturevariantOptions.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_VARIANT_OPTIONS_H
#define STMG_TESTING_FIXTURE_VARIANT_OPTIONS_H

#include "fixturevariantPlayers.h"

#include "options/booloption.h"
#include "options/enumoption.h"
#include "options/intoption.h"

#ifdef STMG_TESTING_COPY_FROM_FAKE
#include "stdconfig.h"
#include "ownertype.h"
#include "util/variant.h"
#else
#include <stmm-games/stdconfig.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/util/variant.h>
#endif

#include <string>
#include <tuple>
#include <vector>
#include <memory>
#include <cassert>

#include <stdint.h>

namespace stmg { class Option; }

namespace stmg
{

using std::shared_ptr;

namespace testing
{


///////////////////////////////////////////////////////////////////////////////
class FixtureVariantOptions_Game_None // Default
{
};
template< int32_t GameSpeedFROM, int32_t GameSpeedTO > // {1,2,3,4}, {2,3,4}
class FixtureVariantOptions_Game_GameSpeed
{
};
class FixtureVariantOptions_Game_AllowHint
{
};
class FixtureVariantOptions_Game_Difficulty
{
};

class FixtureVariantOptions_Team_AllowMateSwap
{
};

class FixtureVariantOptions_Player_Rotation
{
};
class FixtureVariantOptions_Player_AISlave_QI // from 1 to 3, default 1
{
};
class FixtureVariantOptions_Custom
{
public:
	virtual std::vector< shared_ptr<Option> > getCustomOptions() const = 0;
};

class FixtureVariantOptions
{
public:
	virtual ~FixtureVariantOptions() = default;
protected:
	std::vector< shared_ptr<Option> > getConfigOptions() const
	{
		std::vector< shared_ptr<Option> > aOptions;
		auto p0Custom = dynamic_cast<FixtureVariantOptions_Custom const*>(this);
		if (p0Custom != nullptr) {
			aOptions = p0Custom->getCustomOptions();
		}
		// GAME
		int32_t nGameSpeedFrom = -1;
		int32_t nGameSpeedTo = -1;
		if (dynamic_cast<FixtureVariantOptions_Game_GameSpeed<1, 2> const*>(this) != nullptr) {
			nGameSpeedFrom = 1;
			nGameSpeedTo = 2;
		} else if (dynamic_cast<FixtureVariantOptions_Game_GameSpeed<1,3> const*>(this) != nullptr) {
			nGameSpeedFrom = 1;
			nGameSpeedTo = 3;
		} else if (dynamic_cast<FixtureVariantOptions_Game_GameSpeed<1,4> const*>(this) != nullptr) {
			nGameSpeedFrom = 1;
			nGameSpeedTo = 4;
		} else if (dynamic_cast<FixtureVariantOptions_Game_GameSpeed<2,3> const*>(this) != nullptr) {
			nGameSpeedFrom = 2;
			nGameSpeedTo = 3;
		} else if (dynamic_cast<FixtureVariantOptions_Game_GameSpeed<2,4> const*>(this) != nullptr) {
			nGameSpeedFrom = 2;
			nGameSpeedTo = 4;
		} else if (dynamic_cast<FixtureVariantOptions_Game_GameSpeed<3,4> const*>(this) != nullptr) {
			nGameSpeedFrom = 3;
			nGameSpeedTo = 4;
		}
		if (nGameSpeedFrom > 0) {
			aOptions.push_back(std::make_shared<IntOption>(OwnerType::GAME, "GameSpeed", nGameSpeedFrom
															, "Game speed", nGameSpeedFrom, nGameSpeedTo));
		}
		if (dynamic_cast<FixtureVariantOptions_Game_AllowHint const*>(this) != nullptr) {
			aOptions.push_back(std::make_shared<BoolOption>(OwnerType::GAME, "AllowHint", false, "Allow hint"));
		}
		if (dynamic_cast<FixtureVariantOptions_Game_Difficulty const*>(this) != nullptr) {
			aOptions.push_back(std::make_shared<EnumOption>(OwnerType::GAME, "Difficulty", 0, "Difficulty"
				, std::vector< std::tuple<int32_t, std::string, std::string> >{ 
					std::tuple<int32_t, std::string, std::string>{0, "Easy", "Hurt me plenty"}
					, std::tuple<int32_t, std::string, std::string>{1, "Medium", "Manageable"}
					, std::tuple<int32_t, std::string, std::string>{2, "Hard", "I give up!"}
				}));
		}
		// TEAM
		if (dynamic_cast<FixtureVariantOptions_Team_AllowMateSwap const*>(this) != nullptr) {
			aOptions.push_back(std::make_shared<BoolOption>(OwnerType::TEAM, "AllowSwap", false, "Allow mate swap"));
		}
		// PLAYER
		if (dynamic_cast<FixtureVariantOptions_Player_Rotation const*>(this) != nullptr) {
			aOptions.push_back(std::make_shared<BoolOption>(OwnerType::PLAYER, "CWRotation", false, "Clockwise rotation"));
		}
		if (dynamic_cast<FixtureVariantOptions_Player_AISlave_QI const*>(this) != nullptr) {
			#ifndef NDEBUG
			const FixtureVariantAIMatesPerTeamMax* p0AIMates = dynamic_cast<FixtureVariantAIMatesPerTeamMax const*>(this);
			#endif //NDEBUG
			assert(p0AIMates != nullptr);
			assert(p0AIMates->getAIMatesPerTeamMax() > 0);
			shared_ptr<BoolOption> refMaster = StdConfig::createAIOption(OwnerType::PLAYER, "Computer AI");
			aOptions.push_back(refMaster);
			// In a UI (Preferences) QI option only visible if player's AI option is true
			aOptions.push_back(std::make_shared<IntOption>(OwnerType::PLAYER, "QI", 1
															, "Smartness", /*bReadOnly=*/false, /*bVisible=*/true
															, refMaster, std::vector<Variant>{true}, 1, 3));
			// In a UI (Preferences) Warp option only visible if player's AI option is false
			aOptions.push_back(std::make_shared<BoolOption>(OwnerType::PLAYER, "Warp", false
															, "Warping", /*bReadOnly=*/false, /*bVisible=*/true
															, refMaster, std::vector<Variant>{false}));
		}

		return aOptions;
	}
};


} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_VARIANT_OPTIONS_H */
