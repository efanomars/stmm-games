/*
 * File:   gameproxy.cc
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

#include "gameproxy.h"
#include "game.h"

#include "util/basictypes.h"

//#include <cassert>
//#include <iostream>
#include <cstdint>

namespace stmg { class GameSound; }
namespace stmg { class Named; }
namespace stmg { class RandomSource; }
namespace stmg { class Variable; }
namespace stmi { class Event; }


namespace stmg
{

int32_t GameProxy::random(int32_t nFrom, int32_t nTo) noexcept
{
	return m_p0Game->random(nFrom, nTo);
}
RandomSource& GameProxy::getRandomSource() noexcept
{
	return *m_p0Game;
}
int64_t GameProxy::getGameId() const noexcept
{
	return m_p0Game->getGameId();
}
Named& GameProxy::getNamed() noexcept
{
	return m_p0Game->getNamed();
}
const Named& GameProxy::getNamed() const noexcept
{
	return m_p0Game->getNamed();
}
bool GameProxy::isAllTeamsInOneLevel() const noexcept
{
	return m_p0Game->isAllTeamsInOneLevel();
}
double GameProxy::gameInterval() const noexcept
{
	return m_p0Game->gameInterval();
}
int32_t GameProxy::gameElapsed() const noexcept
{
	return m_p0Game->gameElapsed();
}
double GameProxy::gameElapsedMillisec() const noexcept
{
	return m_p0Game->gameElapsedMillisec();
}
bool GameProxy::isInGameTick() const noexcept
{
	return m_p0Game->isInGameTick();
}
void GameProxy::changedInterval(int32_t nLevel) noexcept
{
	m_p0Game->changedInterval(nLevel);
}
void GameProxy::interrupt(INTERRUPT_TYPE eInterruptType) noexcept
{
	m_p0Game->interrupt(eInterruptType);
}
bool GameProxy::hasVariableId(int32_t nId, OwnerType eOwnerType) const noexcept
{
	return m_p0Game->hasVariableId(nId, eOwnerType);
}
std::pair<int32_t, OwnerType> GameProxy::variableIdAndOwner(const std::string& sName) const noexcept
{
	return m_p0Game->variableIdAndOwner(sName);
}
const Variable& GameProxy::variable(int32_t nId) const noexcept
{
	return m_p0Game->variable(nId);
}
Variable& GameProxy::variable(int32_t nId) noexcept
{
	return m_p0Game->variable(nId);
}
const Variable& GameProxy::variable(int32_t nId, int32_t nLevel, int32_t nLevelTeam, int32_t nMate) const noexcept
{
	return m_p0Game->variable(nId, nLevel, nLevelTeam, nMate);
}
Variable& GameProxy::variable(int32_t nId, int32_t nLevel, int32_t nLevelTeam, int32_t nMate) noexcept
{
	return m_p0Game->variable(nId, nLevel, nLevelTeam, nMate);
}
int32_t GameProxy::getPlayer(int32_t nLevel, int32_t nLevelPlayer) noexcept
{
	return m_p0Game->getPlayer(nLevel, nLevelPlayer);
}
Layout const& GameProxy::getLayout() const noexcept
{
	return *(m_p0Game->getLayout());
}
int32_t GameProxy::getGameVarIdGameOverTime() const noexcept
{
	return m_p0Game->getGameVarIdGameOverTime();
}
int32_t GameProxy::getGameVarIdWinnerTeam() const noexcept
{
	return m_p0Game->getGameVarIdWinnerTeam();
}
int32_t GameProxy::getTeamVarIdFinishedTime() const noexcept
{
	return m_p0Game->getTeamVarIdFinishedTime();
}
int32_t GameProxy::getTeamVarIdRank() const noexcept
{
	return m_p0Game->getTeamVarIdRank();
}
int32_t GameProxy::getTeamVarIdStatus() const noexcept
{
	return m_p0Game->getTeamVarIdStatus();
}
int32_t GameProxy::getPlayerVarIdFinishedTime() const noexcept
{
	return m_p0Game->getPlayerVarIdFinishedTime();
}
int32_t GameProxy::getPlayerVarIdStatus() const noexcept
{
	return m_p0Game->getPlayerVarIdStatus();
}
void GameProxy::othersSend(int32_t nSenderLevelInGame, int32_t nMsg, int32_t nValue) noexcept
{
	return m_p0Game->othersSend(nSenderLevelInGame, nMsg, nValue);
}
shared_ptr<GameSound> GameProxy::createSound(int32_t nSoundIdx, int32_t nTeam, int32_t nMate
											, FPoint oXYPos, double fZPos, bool bListenerRelative
											, double fVolume01, bool bLooping) noexcept
{
	return m_p0Game->createSound(nSoundIdx, nTeam, nMate, oXYPos, fZPos, bListenerRelative, fVolume01, bLooping);
}
void GameProxy::preloadSound(int32_t nSoundIdx) noexcept
{
	m_p0Game->preloadSound(nSoundIdx);
}
bool GameProxy::removeSound(const shared_ptr<GameSound>& refSound) noexcept
{
	return m_p0Game->removeSound(refSound);
}
void GameProxy::handleInput(int32_t nTeam, int32_t nMate, const shared_ptr<stmi::Event>& refEvent) noexcept
{
	return m_p0Game->handleInput(nTeam, nMate, refEvent);
}
void GameProxy::createKeyActionFromXYEvent(int32_t nLevel, int32_t nLevelTeam, int32_t nMate
											, int32_t nKeyActionId, const shared_ptr<stmi::Event>& refXYEvent) noexcept
{
	return m_p0Game->createKeyActionFromXYEvent(nLevel, nLevelTeam, nMate, nKeyActionId, refXYEvent);
}
int32_t GameProxy::getTotLevels() const noexcept
{
	return m_p0Game->getTotLevels();
}
Level& GameProxy::level(int32_t nLevel) noexcept
{
	return *(m_p0Game->level(nLevel));
}
Level const& GameProxy::level(int32_t nLevel) const noexcept
{
	return *(m_p0Game->level(nLevel));
}
const shared_ptr<const Highscore>& GameProxy::getPreGameHighscore() const noexcept
{
	return m_p0Game->getPreGameHighscore();
}
Highscore const& GameProxy::getInGameHighscore() const noexcept
{
	return m_p0Game->getInGameHighscore();
}

} // namespace stmg
