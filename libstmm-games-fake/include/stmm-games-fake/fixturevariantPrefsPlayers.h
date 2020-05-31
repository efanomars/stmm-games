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
 * File:   fixturevariantPrefsPlayers.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_VARIANT_PREFS_PLAYERS_H
#define STMG_TESTING_FIXTURE_VARIANT_PREFS_PLAYERS_H

#include "stdpreferences.h"

#ifdef STMG_TESTING_COPY_FROM_FAKE
#include "stdconfig.h"
#include "appconstraints.h"
#include "apppreferences.h"
#else
#include <stmm-games/stdconfig.h>
#include <stmm-games/appconstraints.h>
#include <stmm-games/apppreferences.h>
#endif

#include <memory>
#include <cassert>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

///////////////////////////////////////////////////////////////////////////////
template<int32_t TEAMS>
class FixtureVariantPrefsTeams // Default: FixtureVariantPrefsTeams<2>
{
};
///////////////////////////////////////////////////////////////////////////////
template <int32_t TEAM, int32_t MATES>
class FixtureVariantPrefsMates // Default: FixtureVariantPrefsMates<TEAM><2>
{
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantPrefsPlayers
{
public:
	virtual ~FixtureVariantPrefsPlayers() = default;
protected:
	int32_t getPrefsTotTeams()
	{
		if (dynamic_cast<FixtureVariantPrefsTeams<1>*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantPrefsTeams<3>*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantPrefsTeams<4>*>(this) != nullptr) {
			return 4;
		}
		return 2;
	}
	template<int32_t TEAM>
	int32_t getPrefsTotMates()
	{
		static_assert((TEAM >= 0) && (TEAM <= 3), "Wrong team number");
		if (dynamic_cast<FixtureVariantPrefsMates<TEAM,1>*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantPrefsMates<TEAM,3>*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantPrefsMates<TEAM,4>*>(this) != nullptr) {
			return 4;
		}
		return 2;
	}
	int32_t getPrefsTotMates(int32_t nTeam)
	{
		if (nTeam == 0) {
			return getPrefsTotMates<0>();
		} else if (nTeam == 1) {
			return getPrefsTotMates<1>();
		} else if (nTeam == 2) {
			return getPrefsTotMates<2>();
		} else if (nTeam == 3) {
			return getPrefsTotMates<3>();
		} else {
			assert(false);
			return -1;
		}
	}
	int32_t getPrefsTotPlayers()
	{
		const int32_t nTotTeams = getPrefsTotTeams();
		int32_t nTotPlayers = getPrefsTotMates<0>();
		if (nTotTeams > 1) {
			nTotPlayers += getPrefsTotMates<1>();
		} else if (nTotTeams > 2) {
			nTotPlayers += getPrefsTotMates<2>();
		} else if (nTotTeams > 3) {
			nTotPlayers += getPrefsTotMates<3>();
		}
		return nTotPlayers;
	}
	/** Try to fit desired player configuration into preferences*/
	void initStdPreferences(const shared_ptr<StdPreferences>& refPrefs)
	{
		#ifndef NDEBUG
		const int32_t nMaxPlayers = refPrefs->getStdConfig()->getAppConstraints().getMaxPlayers();
		#endif //NDEBUG
		const int32_t nMaxTeammates = refPrefs->getStdConfig()->getAppConstraints().getMaxTeammates();
		#ifndef NDEBUG
		const int32_t nMaxTeams = refPrefs->getStdConfig()->getAppConstraints().getMaxTeams();
		#endif //NDEBUG
		const int32_t nTotTeams = getPrefsTotTeams();
		assert(nTotTeams <= nMaxTeams);
		const int32_t nTotPlayers = getPrefsTotPlayers();
		assert(nTotPlayers <= nMaxPlayers);
		#ifndef NDEBUG
		bool bOk =
		#endif //NDEBUG
		refPrefs->setTotTeams(nTotTeams);
		assert(bOk);
		#ifndef NDEBUG
		bOk =
		#endif //NDEBUG
		refPrefs->setTotPlayers(nTotPlayers);
		assert(bOk);
		int32_t nCurTeam = 0;
		while (nCurTeam < nTotTeams) {
			int32_t nTargetTotMates = getPrefsTotMates(nCurTeam);
			assert(nTargetTotMates <= nMaxTeammates);
			while (refPrefs->getTeamFull(nCurTeam)->getTotMates() > nTargetTotMates) {
				// find following team that can receive a mate
				int32_t nNextTeam = nCurTeam + 1;
				while ((nNextTeam < nTotTeams) && (refPrefs->getTeamFull(nNextTeam)->getTotMates() == nMaxTeammates)) {
					// full
					++nNextTeam;
				}
				assert(nNextTeam < nTotTeams);
				--nNextTeam;
				while (nNextTeam >= nCurTeam) {
					#ifndef NDEBUG
					bOk =
					#endif //NDEBUG
					refPrefs->playerMoveDown(refPrefs->getTeamFull(nNextTeam)->getMate(0)->getName());
					assert(bOk);
					--nNextTeam;
				}
			}
			while (refPrefs->getTeamFull(nCurTeam)->getTotMates() < nTargetTotMates) {
				// find following team that can spare a mate
				int32_t nNextTeam = nCurTeam + 1;
				while ((nNextTeam < nTotTeams) && (refPrefs->getTeamFull(nNextTeam)->getTotMates() == 1)) {
					// only one mate
					++nNextTeam;
				}
				assert(nNextTeam < nTotTeams);
				while (nNextTeam > nCurTeam) {
					#ifndef NDEBUG
					bOk =
					#endif //NDEBUG
					refPrefs->playerMoveUp(refPrefs->getTeamFull(nNextTeam)->getMate(0)->getName());
					assert(bOk);
					--nNextTeam;
				}
			}
			++nCurTeam;
		}
	}
};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_VARIANT_PREFS_PLAYERS_H */
