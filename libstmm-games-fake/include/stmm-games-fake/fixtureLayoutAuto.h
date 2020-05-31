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
 */
/*
 * File:   fixtureLayoutAuto.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_LAYOUT_AUTO_H
#define STMG_TESTING_FIXTURE_LAYOUT_AUTO_H

#include "fixtureStdPreferences.h"

#include "fixturevariantLayout.h"
#include "fixturevariantVariables.h"

#ifdef STMG_TESTING_COPY_FROM_FAKE
#include "layout.h"
#include "gamewidget.h"
#include "stdpreferences.h"
#include "util/namedobjindex.h"
#include "variable.h"
#include "widgets/boxwidget.h"
#include "widgets/levelshowwidget.h"
#include "widgets/varwidget.h"
#else
#include <stmm-games/layout.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/stdpreferences.h>
#include <stmm-games/util/namedobjindex.h>
#include <stmm-games/variable.h>
#include <stmm-games/widgets/boxwidget.h>
#include <stmm-games/widgets/levelshowwidget.h>
#include <stmm-games/widgets/varwidget.h>
#endif

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

class LayoutAutoFixture : public StdPreferencesFixture
						, public FixtureVariantLayoutTeamDistribution
						, public FixtureVariantLayoutShowMode
						, public FixtureVariantLayoutCreateVarWidgetsFromVariables
						, public FixtureVariantLayoutCreateActionWidgetsFromKeyActions
						, public FixtureVariantVariablesGame
						, public FixtureVariantVariablesTeam
						, public FixtureVariantVariablesPlayer
{
protected:
	void setup() override
	{
		StdPreferencesFixture::setup();
		//
		const bool bAllTeamsInOneLevel = FixtureVariantLayoutTeamDistribution::getAllTeamsInOneLevel();
		const bool bIsSubshowMode = !FixtureVariantLayoutShowMode::getIsShowMode();

		m_refLayout = std::make_shared<Layout>(createRootWidget(bAllTeamsInOneLevel, bIsSubshowMode), shared_ptr<LevelShowWidget>{}
												, bAllTeamsInOneLevel, bIsSubshowMode, m_refPrefs);
	}
	void teardown() override
	{
		m_refLayout.reset();
		StdPreferencesFixture::teardown();
	}
public:
	//
	shared_ptr<Layout> m_refLayout;
private:
	shared_ptr<GameWidget> createVarBox(const NamedObjIndex<Variable::VariableType>& oVarTypes, int32_t nTeam, int32_t nMate, bool bVertical)
	{
		BoxWidget::Init oInit;
		oInit.m_nTeam = nTeam;
		oInit.m_nMate = nMate;
		const int32_t nTotVars = oVarTypes.size();
		for (int32_t nIdx = 0; nIdx < nTotVars; ++nIdx) {
			VarWidget::Init oVarInit;
			oVarInit.m_nTeam = nTeam;
			oVarInit.m_nMate = nMate;
			oVarInit.m_sName = oVarTypes.getName(nIdx);
			oVarInit.m_fReferenceWFactor = 0.1;
			oVarInit.m_fReferenceHFactor = 0.1;
			oVarInit.m_nVarId = nIdx;
			oInit.m_aChildWidgets.push_back(std::make_shared<VarWidget>(std::move(oVarInit)));
		}
		oInit.setSwitchable(bVertical, false, true);
		return std::make_shared<BoxWidget>(std::move(oInit));
	}
	shared_ptr<GameWidget> createMateVarBox(int32_t nTeam, int32_t nMate)
	{
		return createVarBox(getVariablesPlayer(), nTeam, nMate, true);
	}
	shared_ptr<GameWidget> createTeamVarBox(int32_t nTeam)
	{
		if ((!getCreateVarWidgetsFromVariables()) || ((getVariablesTeam().size() == 0) && (getVariablesPlayer().size() == 0))) {
			return shared_ptr<GameWidget>{};
		}
		BoxWidget::Init oInit;
		std::vector<shared_ptr<GameWidget>>& aMateWidgets = oInit.m_aChildWidgets;
		const int32_t nTotMates = m_refPrefs->getTeamFull(nTeam)->getTotMates();
		if (getVariablesPlayer().size() > 0) {
			for (int32_t nMate = 0; nMate < nTotMates; ++nMate) {
				aMateWidgets.push_back(createMateVarBox(nTeam, nMate));
			}
		}
		if (getVariablesTeam().size() > 0) {
			aMateWidgets.push_back(createVarBox(getVariablesTeam(), nTeam, -1, true));
		}
		oInit.m_nTeam = nTeam;
		oInit.setSwitchable(true, false, true);
		return std::make_shared<BoxWidget>(std::move(oInit));
	}
	shared_ptr<GameWidget> createTeamLSBox(int32_t nTeam) // One team per level
	{
		LevelShowWidget::Init oLSInit;
		oLSInit.m_nTeam = nTeam;
		oLSInit.setExpand(true);
		oLSInit.setAlign(WidgetAxisFit::ALIGN_FILL);
		shared_ptr<GameWidget> refRes = std::make_shared<LevelShowWidget>(std::move(oLSInit));
		if (getVariablesPlayer().size() > 0) {
			BoxWidget::Init oBInit;
			oBInit.m_nTeam = nTeam;
			std::vector<shared_ptr<GameWidget>>& aMateWidgets = oBInit.m_aChildWidgets;
			const int32_t nTotMates = m_refPrefs->getTeamFull(nTeam)->getTotMates();
			for (int32_t nMate = 0; nMate < nTotMates; ++nMate) {
				aMateWidgets.push_back(createMateVarBox(nTeam, nMate));
			}
			oBInit.setSwitchable(true, false, true);
			auto refMatesVarsBox = std::make_shared<BoxWidget>(std::move(oBInit));
			BoxWidget::Init oBBInit;
			oBBInit.m_nTeam = nTeam;
			oBBInit.m_aChildWidgets = std::vector<shared_ptr<GameWidget>>{refRes, refMatesVarsBox};
			oBBInit.setSwitchable(false, false, true);
			refRes = std::make_shared<BoxWidget>(std::move(oBBInit));
		}
		if (getVariablesTeam().size() > 0) {
			BoxWidget::Init oBBBInit;
			oBBBInit.m_nTeam = nTeam;
			oBBBInit.m_aChildWidgets = std::vector<shared_ptr<GameWidget>>{refRes, createVarBox(getVariablesTeam(), nTeam, -1, false)};
			oBBBInit.setSwitchable(true, false, true);
			refRes = std::make_shared<BoxWidget>(std::move(oBBBInit));
		}
		assert(refRes);
		return refRes;
	}
	shared_ptr<GameWidget> createSubshowMateBox(int32_t nTeam, int32_t nMate)
	{
		LevelShowWidget::Init oLSInit;
		oLSInit.m_nTeam = nTeam;
		oLSInit.m_nMate = nMate;
		shared_ptr<GameWidget> refRes = std::make_shared<LevelShowWidget>(std::move(oLSInit));
		if (getVariablesPlayer().size() > 0) {
			BoxWidget::Init oBInit;
			oBInit.m_nTeam = nTeam;
			oBInit.m_nMate = nMate;
			oBInit.m_aChildWidgets = std::vector<shared_ptr<GameWidget>>{refRes, createMateVarBox(nTeam, nMate)};
			oBInit.setSwitchable(false, false, true);
			refRes = std::make_shared<BoxWidget>(std::move(oBInit));
		}
		return refRes;
	}
	shared_ptr<GameWidget> createSubshowTeamBox(int32_t nTeam)
	{
		std::vector<shared_ptr<GameWidget>> aMateWidgets;
		const int32_t nTotMates = m_refPrefs->getTeamFull(nTeam)->getTotMates();
		for (int32_t nMate = 0; nMate < nTotMates; ++nMate) {
			aMateWidgets.push_back(createSubshowMateBox(nTeam, nMate));
		}
		BoxWidget::Init oBInit;
		oBInit.m_nTeam = nTeam;
		oBInit.m_aChildWidgets = std::move(aMateWidgets);
		oBInit.setSwitchable(false, false, true);
		shared_ptr<GameWidget> refRes = std::make_shared<BoxWidget>(std::move(oBInit));
		if (getVariablesTeam().size() > 0) {
			BoxWidget::Init oBBInit;
			oBBInit.m_nTeam = nTeam;
			std::vector< shared_ptr<GameWidget> > aMatesTeam{refRes, createVarBox(getVariablesTeam(), nTeam, -1, false)};
			oBBInit.m_aChildWidgets = std::move(aMatesTeam);
			oBBInit.setSwitchable(true, false, true);
			refRes = std::make_shared<BoxWidget>(std::move(oBBInit));
		}
		return refRes;
	}
	shared_ptr<GameWidget> createGameWidget(const bool bAllTeamsInOneLevel, const bool bSubshow)
	{
		std::vector<shared_ptr<GameWidget>> aTeamWidgets;
		const int32_t nTotTeams = m_refPrefs->getTotTeams();
		if (bSubshow) {
			for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
				aTeamWidgets.push_back(createSubshowTeamBox(nTeam));
			}
		} else if (bAllTeamsInOneLevel) {
			if (getCreateVarWidgetsFromVariables() && ((getVariablesTeam().size() > 0) || (getVariablesPlayer().size() > 0))) {
				std::vector< shared_ptr<GameWidget> > aAllTeamsWidgets;
				for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
					aAllTeamsWidgets.push_back(createTeamVarBox(nTeam));
				}
				BoxWidget::Init oBInit;
				oBInit.m_nTeam = 0;
				oBInit.m_aChildWidgets = std::move(aAllTeamsWidgets);
				oBInit.setSwitchable(true, false, true);
				aTeamWidgets.push_back(std::make_shared<BoxWidget>(std::move(oBInit)));
			}
			LevelShowWidget::Init oLSInit;
			auto refLSW = std::make_shared<LevelShowWidget>(std::move(oLSInit));
			aTeamWidgets.push_back(refLSW);
		} else {
			for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
				aTeamWidgets.push_back(createTeamLSBox(nTeam));
			}
		}
		BoxWidget::Init oBBInit;
		oBBInit.m_aChildWidgets = std::move(aTeamWidgets);
		oBBInit.setSwitchable(false, false, true);
		shared_ptr<GameWidget> refRes = std::make_shared<BoxWidget>(std::move(oBBInit));
		return refRes;
	}
	shared_ptr<GameWidget> createRootWidget(const bool bAllTeamsInOneLevel, const bool bSubshow)
	{
		auto refGameWidget = createGameWidget(bAllTeamsInOneLevel, bSubshow);
		if (getCreateVarWidgetsFromVariables() && (getVariablesGame().size() > 0)) {
			auto refGameVarBox = createVarBox(getVariablesGame(), -1, -1, false);
			BoxWidget::Init oBInit;
			std::vector<shared_ptr<GameWidget>> aChildWidgets{refGameWidget, refGameVarBox};
			oBInit.m_aChildWidgets = std::move(aChildWidgets);
			oBInit.setSwitchable(true, false, true);
			return std::make_shared<BoxWidget>(std::move(oBInit));
		} else {
			return refGameWidget;
		}
	}
};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_LAYOUT_AUTO_H */
