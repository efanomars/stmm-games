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
 * File:   fixtureGame.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_GAME_H
#define STMG_TESTING_FIXTURE_GAME_H

#include "fixtureLayoutAuto.h"
#include "fixtureGameOwner.h"

#include "fixturevariantLevelInit.h"
#include "fixturevariantLayout.h"

#include <stmm-games/game.h>
#include <stmm-games/level.h>
#include <stmm-games/named.h>
#include <stmm-games/util/namedobjindex.h>

#include <stmm-input-fake/fakedevicemanager.h>
#include <stmm-input/devicemanager.h>

#include <memory>
#include <string>
#include <utility>
#include <cassert>

#include <stdint.h>

namespace stmg { class AppPreferences; }
namespace stmi { class Event; }

namespace stmg
{

namespace testing
{

class GameFixture : public LayoutAutoFixture, public Game::CreateLevelCallback
					, public FixtureVariantLevelInit, public GameOwnerFixture
{
protected:
	void setup() override
	{
		LayoutAutoFixture::setup();
		GameOwnerFixture::setup();

		Level::Init oLevelInit;
		FixtureVariantLevelInit::getLevelInit(oLevelInit, FixtureVariantLayoutShowMode::getIsShowMode());
		Named oNamed; //TODO FixtureVariantNamed
		Game::Init oGameInit;
		oGameInit.m_sName = std::string{"Test"};
		oGameInit.m_p0GameOwner = this;
		oGameInit.m_oNamed = std::move(oNamed);
		oGameInit.m_oGameVariableTypes = getVariablesGame();
		oGameInit.m_oTeamVariableTypes = getVariablesTeam();
		oGameInit.m_oPlayerVariableTypes = getVariablesPlayer();
		oGameInit.m_refLayout = m_refLayout;
		m_refGame = std::make_shared<Game>(std::move(oGameInit), *this, oLevelInit);

		m_refEventListener = std::make_shared<stmi::EventListener>(
			[this](const shared_ptr<stmi::Event>& refEvent)
			{
				//auto p0Event = refEvent.get();
				//if (p0Event->getEventClass().isXYEvent()) {
				//	auto p0XYEvent = static_cast<stmi::XYEvent*>(p0Event);
				//	p0XYEvent->translateXY(- m_nDrawingAreaX, - m_nDrawingAreaY);
				//}
				m_refGame->handleInput(refEvent);
			});
		m_refDM->addEventListener(m_refEventListener);
	}
	void teardown() override
	{
		GameOwnerFixture::teardown();
		LayoutAutoFixture::teardown();
	}
public:
	shared_ptr<Level> createLevel(Game* p0Game, int32_t nLevel
									, const shared_ptr<AppPreferences>& refPreferences
									, const Level::Init& oInit) noexcept override
	{
		assert(p0Game != nullptr);
		return std::make_shared<Level>(p0Game, nLevel, refPreferences, oInit);
	}
public:
	shared_ptr<Game> m_refGame;
	shared_ptr<stmi::EventListener> m_refEventListener;
};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_GAME_H */
