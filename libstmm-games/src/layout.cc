/*
 * File:   layout.cc
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

#include "layout.h"
#include "apppreferences.h"
#include "containerwidget.h"
#include "gamewidget.h"
#include "widgets/levelshowwidget.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>


namespace stmg
{

Layout::Layout(const shared_ptr<GameWidget>& refRootGW
				, const shared_ptr<LevelShowWidget>& refReferenceLevelShow
				, bool bAllTeamsInOneLevel, bool bSubshows
				, const shared_ptr<AppPreferences>& refPrefs) noexcept
: m_refRootGW(refRootGW)
, m_refReferenceLevelShow(refReferenceLevelShow)
, m_refPrefs(refPrefs)
, m_bAllTeamsInOneLevel(bAllTeamsInOneLevel)
, m_bSubshows(bSubshows)
, m_bValid(false)
, m_sErrorString("")
{
	assert(refRootGW);
	assert(refPrefs);
	checkAndGet(*refPrefs);
}
void Layout::reInit(const shared_ptr<GameWidget>& refRootGW
					, const shared_ptr<LevelShowWidget>& refReferenceLevelShow
					, bool bAllTeamsInOneLevel, bool bSubshows
					, const shared_ptr<AppPreferences>& refPrefs) noexcept
{
	assert(refRootGW);
	assert(refPrefs);
	m_refRootGW = refRootGW;
	m_refReferenceLevelShow = refReferenceLevelShow;
	m_refPrefs = refPrefs;
	m_bAllTeamsInOneLevel = bAllTeamsInOneLevel;
	m_bSubshows = bSubshows;
	m_bValid = false;
	m_sErrorString.clear();
	m_oNamedWidgets.clear();
	checkAndGet(*refPrefs);
}
#ifndef NDEBUG
void Layout::dump(int32_t nIndentSpaces) const noexcept
{
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "Layout adr:" << reinterpret_cast<int64_t>(this) << '\n';
	std::cout << sIndent << "  " << "m_bAllTeamsInOneLevel: " << m_bAllTeamsInOneLevel << '\n';
	std::cout << sIndent << "  " << "m_bSubshows:           " << m_bSubshows << '\n';
	std::cout << sIndent << "  " << "m_bValid:              " << m_bValid << '\n';
	m_refRootGW->dump(nIndentSpaces + 2);
}
#endif //NDEBUG
void Layout::checkAndGet(const AppPreferences& oAppPreferences) noexcept
{
	std::vector< shared_ptr<LevelShowWidget> > aLSW;
	std::string sDuplicateName;
	//
	traverse(m_refRootGW, aLSW, sDuplicateName);
	//
	if (!sDuplicateName.empty()) {
		m_sErrorString = "More than one widget has name '" + sDuplicateName + "'";
		return; //--------------------------------------------------------------
	}
	const int32_t nTotLSWs = static_cast<int32_t>(aLSW.size());
//std::cout << "Layout::checkAndGet nTotLSWs=" << nTotLSWs << '\n';
	if (nTotLSWs == 0) {
		m_sErrorString = "No LevelShowWidget was defined in layout";
		return; //--------------------------------------------------------------
	}
	if (!m_refReferenceLevelShow) {
		m_refReferenceLevelShow = aLSW[0];
	}
	bool bReferenceLevelShowFound = false;
	const int32_t nTotTeams = oAppPreferences.getTotTeams();
	assert(nTotTeams > 0);
	if (m_bSubshows) {
		const int32_t nTotPlayers = oAppPreferences.getTotPlayers();
		assert(nTotPlayers > 0);
		if (nTotPlayers != nTotLSWs) {
			m_sErrorString = "Number of Subshow LevelShowWidget defined in layout not same as number of players";
			return; //----------------------------------------------------------
		}
		std::vector< int32_t > aLSWPlayer;
		for (auto& refLSW : aLSW) {
			if (m_refReferenceLevelShow == refLSW) {
				bReferenceLevelShowFound = true;
			}
			const int32_t nTeam = refLSW->getTeam();
			if (nTeam < 0) {
				m_sErrorString = "Subshow LevelShowWidget defined in layout without team number";
				return; //--------------------------------------------------
			}
			if (nTeam >= nTotTeams) {
				m_sErrorString = "Subshow LevelShowWidget defined in layout with wrong team number";
				return; //------------------------------------------------------
			}
			const auto refPrefTeam = oAppPreferences.getTeam(nTeam);
			const int32_t nMate = refLSW->getMate();
			if (nMate < 0) {
				m_sErrorString = "Subshow LevelShowWidget defined in layout without mate number";
				return; //------------------------------------------------------
			}
			const auto refPrefPlayer = refPrefTeam->getMate(nMate);
			const int32_t nPlayer = refPrefPlayer->get();
			aLSWPlayer.push_back(nPlayer);
		}
		std::sort(aLSWPlayer.begin(), aLSWPlayer.end());
		for (int32_t nIdx = 0; nIdx < nTotPlayers; ++nIdx) {
			if (aLSWPlayer[nIdx] != nIdx) {
				m_sErrorString = "Not all players have a LevelShowWidget in layout assigned to them";
				return; //------------------------------------------------------
			}
		}
	} else { // Shows
		if (m_bAllTeamsInOneLevel) {
			if (nTotLSWs != 1) {
				m_sErrorString = "More than one LevelShowWidget defined in layout";
				return; //------------------------------------------------------
			}
			auto& refLSW = aLSW[0];
			if (refLSW->getTeam() != -1) {
				if ((nTotTeams == 1) && (refLSW->getTeam() == 0)) {
					refLSW->setTeam(-1);
				} else {
					m_sErrorString = "LevelShowWidget wrongly assigned to a team";
					return; //--------------------------------------------------
				}
			}
			if (m_refReferenceLevelShow == refLSW) {
				bReferenceLevelShowFound = true;
			}
		} else {
			if (nTotLSWs != nTotTeams) {
				m_sErrorString = "Number of LevelShowWidget defined in layout doesn't match with number of teams";
				return; //------------------------------------------------------
			}
			std::vector< int32_t > aLSWTeam;
			for (auto& refLSW : aLSW) {
				if (m_refReferenceLevelShow == refLSW) {
					bReferenceLevelShowFound = true;
				}
				const int32_t nTeam = refLSW->getTeam();
				if ((nTotTeams == 1) && (nTeam < 0)) {
					refLSW->setTeam(0);
				} else if ((nTeam < 0) || (nTeam >= nTotTeams)) {
					m_sErrorString = "Show LevelShowWidget defined in layout with wrong team number";
					return; //--------------------------------------------------
				}
				aLSWTeam.push_back(nTeam);
			}
			std::sort(aLSWTeam.begin(), aLSWTeam.end());
			for (int32_t nIdx = 0; nIdx < nTotLSWs; ++nIdx) {
				if (aLSWTeam[nIdx] != nIdx) {
					m_sErrorString = "Not all teams have a LevelShowWidget in layout assigned to them";
					return; //--------------------------------------------------
				}
			}
		}
	}
	if (!bReferenceLevelShowFound) {
		m_sErrorString = "Couldn't find the given reference LevelShowWidget in the layout";
		return; //--------------------------------------------------------------
	}
	m_bValid = true;
	//
	traverseSetLayout(m_refRootGW);
	traverseOnAddedToLayout(m_refRootGW);
}
std::string Layout::getMangledName(const std::string& sName, int32_t nTeam, int32_t nMate) const noexcept
{
	return sName + "||" + std::to_string(nTeam) + "||" + std::to_string(nMate);
}
void Layout::traverse(const shared_ptr<GameWidget>& refGW, std::vector< shared_ptr<LevelShowWidget> >& aLSW, std::string& sDuplicateName) noexcept
{
//std::cout << "Layout::traverse" << '\n';
	assert(refGW);
	const std::string sName = refGW->getName();
	const int32_t nTeam = refGW->getTeam();
	const int32_t nMate = refGW->getMate();
	assert(nTeam >= -1);
	assert(nMate >= -1);
	assert((nMate < 0) || (nTeam >= 0));
	if (!sName.empty()) {
		const auto sMangledName = getMangledName(sName, nTeam, nMate);
//std::cout << "Layout::traverse  widget sName=" << sName << "  sMangledName=" << sMangledName << '\n';
		if (m_oNamedWidgets.getIndex(sMangledName) >= 0) {
			sDuplicateName = sName;
			return; //----------------------------------------------------------
		}
		m_oNamedWidgets.addNamedObj(sMangledName, refGW);
	}
	auto refLSW = std::dynamic_pointer_cast<LevelShowWidget>(refGW);
	if (refLSW) {
		aLSW.emplace_back(refLSW);
	} else {
		auto refCW = std::dynamic_pointer_cast<ContainerWidget>(refGW);
		if (refCW) {
			const std::vector< shared_ptr<GameWidget> >& aSubGW = refCW->getChildren();
			for (auto& refSubGW : aSubGW) {
				traverse(refSubGW, aLSW, sDuplicateName);
			}
		}
	}
}
void Layout::setGame(Game* p0Game) noexcept
{
	m_oGameProxy.m_p0Game = p0Game;
	traverseSetGameProxy(m_refRootGW);
	if (p0Game != nullptr) {
		traverseOnAddedToGame(m_refRootGW);
	}
}
void Layout::traverseSetGameProxy(const shared_ptr<GameWidget>& refGW) noexcept
{
	refGW->setGameProxy(&m_oGameProxy);
	auto refCW = std::dynamic_pointer_cast<ContainerWidget>(refGW);
	if (refCW) {
		const std::vector< shared_ptr<GameWidget> >& aSubGW = refCW->getChildren();
		for (auto& refSubGW : aSubGW) {
			traverseSetGameProxy(refSubGW);
		}
	}
}
void Layout::traverseOnAddedToGame(const shared_ptr<GameWidget>& refGW) noexcept
{
	refGW->onAddedToGame();
	auto refCW = std::dynamic_pointer_cast<ContainerWidget>(refGW);
	if (refCW) {
		const std::vector< shared_ptr<GameWidget> >& aSubGW = refCW->getChildren();
		for (auto& refSubGW : aSubGW) {
			traverseOnAddedToGame(refSubGW);
		}
	}
}
void Layout::traverseSetLayout(const shared_ptr<GameWidget>& refGW) noexcept
{
	refGW->setLayout(this);
	auto refCW = std::dynamic_pointer_cast<ContainerWidget>(refGW);
	if (refCW) {
		const std::vector< shared_ptr<GameWidget> >& aSubGW = refCW->getChildren();
		for (auto& refSubGW : aSubGW) {
			traverseSetLayout(refSubGW);
		}
	}
}
void Layout::traverseOnAddedToLayout(const shared_ptr<GameWidget>& refGW) noexcept
{
	refGW->onAddedToLayout();
	auto refCW = std::dynamic_pointer_cast<ContainerWidget>(refGW);
	if (refCW) {
		const std::vector< shared_ptr<GameWidget> >& aSubGW = refCW->getChildren();
		for (auto& refSubGW : aSubGW) {
			traverseOnAddedToLayout(refSubGW);
		}
	}
}
const shared_ptr<GameWidget>& Layout::getWidgetNamed(const std::string& sName, int32_t nTeam, int32_t nMate) const noexcept
{
	assert((nMate < 0) || (nTeam >= 0));
	const auto sMangledName = getMangledName(sName, nTeam, nMate);
//std::cout << "Layout::getWidgetNamed  sMangledName=" << sMangledName << '\n';
	const int32_t nIdx = m_oNamedWidgets.getIndex(sMangledName);
	if (nIdx < 0) {
//std::cout << "Layout::getWidgetNamed  NOT FOUND" << '\n';
		static shared_ptr<GameWidget> s_refEmpty{};
		return s_refEmpty;
	}
	return m_oNamedWidgets.getObj(nIdx);
}

} // namespace stmg
