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
 * File:   stdpreferences.cc
 */

#include "stdpreferences.h"

#include "appconstraints.h"
#include "option.h"
#include "ownertype.h"
#include "stdconfig.h"
#include "util/namedobjindex.h"

#include <stmm-input-au/playbackcapability.h>

#include <stmm-input/device.h>
#include <stmm-input/hardwarekey.h>
#include <stmm-input-ev/devicemgmtevent.h>
#include <stmm-input/callifs.h>
#include <stmm-input/capability.h>
#include <stmm-input/devicemanager.h>

#include <vector>
#include <cassert>
#include <algorithm>
//#include <iostream>
#include <iterator>
#include <type_traits>

namespace stmi { class Event; }

namespace stmg
{

static constexpr int32_t s_nMaxTeamNameLength = 12;
static constexpr int32_t s_nMaxPlayerNameLength = 12;

Variant StdPreferences::Player::getOptionValue(const std::string& sName) const noexcept
{
	assert(m_p0Team != nullptr);
	auto p0Prefs = m_p0Team->m_p0Prefs;
	assert(p0Prefs != nullptr);
	const NamedObjIndex< shared_ptr<Option> >& oOptions = p0Prefs->m_refStdConfig->getOptions(OwnerType::PLAYER);
	const int32_t nIdx = oOptions.getIndex(sName);
	if (nIdx < 0) {
		return Variant{};
	}
	assert(nIdx < static_cast<int32_t>(m_aOption.size()));
	return m_aOption[nIdx];
}
bool StdPreferences::Player::setName(const std::string& sNewName) noexcept
{
	assert(m_p0Team != nullptr);
	StdPreferences* p0Prefs = m_p0Team->m_p0Prefs;
	assert(p0Prefs != nullptr);
	assert(p0Prefs->m_bEditMode);
	auto itOld = p0Prefs->m_oNamedPlayers.find(m_sName);
	assert(itOld != p0Prefs->m_oNamedPlayers.end());
	if (m_sName == sNewName) {
		return true; //---------------------------------------------------------
	}
	std::unordered_map<std::string, shared_ptr<Player> >::const_iterator itNew = p0Prefs->m_oNamedPlayers.find(sNewName);
	if (itNew != p0Prefs->m_oNamedPlayers.end()) {
		return false; //--------------------------------------------------------
	}
	p0Prefs->m_oNamedPlayers.erase(itOld);
	// Set new name
	m_sName = sNewName;

	p0Prefs->m_oNamedPlayers.insert(std::make_pair(sNewName, shared_from_this()));
	return true;
}
std::pair<stmi::Capability*, stmi::HARDWARE_KEY> StdPreferences::Player::getKeyValue(int32_t nKeyActionId) const noexcept
{
//std::cout << "getKeyValue(key action id=" << nKeyActionId << ")" << '\n';
	if (isAI()) {
		return std::make_pair(nullptr, stmi::HK_NULL);
	}
	assert(m_p0Team != nullptr);
	auto p0Prefs = m_p0Team->m_p0Prefs;
	assert(p0Prefs != nullptr);
	assert((nKeyActionId >= 0) && (nKeyActionId < p0Prefs->getStdConfig()->getTotKeyActions()));
	auto& oTuple = m_aKeyActionCapaHK[nKeyActionId];
	const int32_t& nClassIdx = std::get<0>(oTuple);
	const int32_t& nCapaIdx = std::get<1>(oTuple);
	stmi::Capability* p0Capability = p0Prefs->getCapabilityFromIdxs(nClassIdx, nCapaIdx);
//std::cout << "   nClassIdx=" << nClassIdx << "  nCapaIdx=" << nCapaIdx << "  p0Capability=" << reinterpret_cast<int64_t>(p0Capability) << '\n';
	//assert(p0Capability != nullptr);
	return std::make_pair(p0Capability, std::get<2>(oTuple));
}
bool StdPreferences::Player::setKeyValue(int32_t nKeyActionId, stmi::Capability* p0Capability, stmi::HARDWARE_KEY eKey) noexcept
{
	assert(!isAI());
	assert(p0Capability != nullptr);
	assert((eKey != stmi::HK_NULL) && stmi::HardwareKeys::isValid(eKey));
	assert(m_p0Team != nullptr);
	auto p0Prefs = m_p0Team->m_p0Prefs;
	assert(p0Prefs != nullptr);
	assert(p0Prefs->m_bEditMode);
	assert((nKeyActionId >= 0) && (nKeyActionId < p0Prefs->getStdConfig()->getTotKeyActions()));
	auto& oTuple = m_aKeyActionCapaHK[nKeyActionId];

	const int32_t nOldClassIdx = std::get<0>(oTuple);
	const int32_t nOldCapaIdx = std::get<1>(oTuple);
	const stmi::Capability* p0OldCapability = p0Prefs->getCapabilityFromIdxs(nOldClassIdx, nOldCapaIdx);
	assert(p0OldCapability != nullptr);
	stmi::HARDWARE_KEY eOldKey = std::get<2>(oTuple);
	if ((p0OldCapability == p0Capability) && (eOldKey == eKey)) {
		// unchanged, no swap with another key action necessary
		return false; //--------------------------------------------------------
	}
	int32_t nClassIdx;
	int32_t nCapaIdx;
	#ifndef NDEBUG
	const bool bFound =
	#endif //NDEBUG
	p0Prefs->getIdxsFromCapability(p0Capability, nClassIdx, nCapaIdx);
	assert(bFound);
	auto& oHKPlayerKeyAction = p0Prefs->m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_oHKPlayerKeyAction;
	auto itOtherPlayer = oHKPlayerKeyAction.find(eKey);
	const bool bKeyAlreadyInUse = (itOtherPlayer != oHKPlayerKeyAction.end());

	shared_ptr<Player> refSelf = shared_from_this();
	if (bKeyAlreadyInUse) {
		// swap with other (player,keyaction) using the eKey
		std::pair<shared_ptr<Player>,int32_t>& oOtherPair = itOtherPlayer->second;
		shared_ptr<Player>& refOtherPlayer = oOtherPair.first;
		int32_t& nOtherKeyActionId = oOtherPair.second;
		assert((refOtherPlayer != refSelf) || (nOtherKeyActionId != nKeyActionId));
		//
		auto& oOtherTuple = refOtherPlayer->m_aKeyActionCapaHK[nOtherKeyActionId];
		std::swap(oTuple, oOtherTuple);

		auto& oHKOldPlayerKeyAction = p0Prefs->m_aCapabilityClassData[nOldClassIdx].m_aCapabilityData[nOldCapaIdx].m_oHKPlayerKeyAction;
		auto itOldPlayer = oHKOldPlayerKeyAction.find(eOldKey);
		assert(itOldPlayer != oHKOldPlayerKeyAction.end());
		itOldPlayer->second = oOtherPair;

		refOtherPlayer = refSelf;
		nOtherKeyActionId = nKeyActionId;

	} else {
		std::get<0>(oTuple) = nClassIdx;
		std::get<1>(oTuple) = nCapaIdx;
		std::get<2>(oTuple) = eKey;

		auto& oHKOldPlayerKeyAction = p0Prefs->m_aCapabilityClassData[nOldClassIdx].m_aCapabilityData[nOldCapaIdx].m_oHKPlayerKeyAction;
		oHKOldPlayerKeyAction.erase(eOldKey);
		oHKPlayerKeyAction.emplace(eKey, std::make_pair(refSelf, nKeyActionId));
	}
	return bKeyAlreadyInUse;
}
bool StdPreferences::Player::setAI(bool bIsAI) noexcept
{
	assert(m_p0Team != nullptr);
	auto p0Prefs = m_p0Team->m_p0Prefs;
	assert(p0Prefs != nullptr);
	assert(p0Prefs->m_bEditMode);
	if (bIsAI == m_aOption[m_nAIOptionIdx].getBool()) {
		return true; //---------------------------------------------------------
	}
	const AppConstraints& oAppConstraints = p0Prefs->getStdConfig()->getAppConstraints();
	const int32_t nTotAIMates = m_p0Team->getTotAIMates();
	if (bIsAI) {
		// from human to ai
		if (nTotAIMates == oAppConstraints.m_nAIMatesPerTeamMax) {
			// the team already has reached maximum number of ai mates
			return false; //----------------------------------------------------
		}
		if (! oAppConstraints.m_bAllowMixedAIHumanTeam) {
			assert(nTotAIMates == 0);
			if (m_p0Team->getTotMates() > 1) {
				// this team has at least another human: either remove the other mates
				// or make the operation fail
				// For now let's just fail (TODO)
				return false; //------------------------------------------------
			}
		}
	} else {
		// from ai to human
		if (! oAppConstraints.m_bAllowMixedAIHumanTeam) {
			const int32_t nTotMates = m_p0Team->getTotMates();
			assert(nTotAIMates == nTotMates);
			if (nTotMates > 1) {
				// this team has at least another ai: either remove the other mates
				// or make the operation fail
				// For now let's just fail (TODO)
				return false; //------------------------------------------------
			}
		}
	}
	#ifndef NDEBUG
	auto& refOption = p0Prefs->m_refStdConfig->getOption(OwnerType::PLAYER, p0Prefs->m_refStdConfig->getAIOptionName());
	#endif //NDEBUG
	assert(!refOption->isReadonly());
	m_aOption[m_nAIOptionIdx].setBool(bIsAI);
	if (bIsAI) { // from human to AI
		// Remove all assigned capabilities
		while (!m_aCapabilities.empty()) {
			auto& oPair = m_aCapabilities[0];
			int32_t& nClassId = oPair.first;
			int32_t& nCapaId = oPair.second;
			shared_ptr<stmi::Capability> refCapability;
			#ifndef NDEBUG
			const bool bFound =
			#endif //NDEBUG
			p0Prefs->getCapabilityFromIdxs(nClassId, nCapaId, refCapability);
			assert(bFound);
			unassignCapability(refCapability);
		}
		// Remove all key actions!
		const int32_t nTotKeyActions = static_cast<int32_t>(m_aKeyActionCapaHK.size());
		for (int32_t nKeyActionId = 0; nKeyActionId < nTotKeyActions; ++nKeyActionId) {
			auto& oTuple = m_aKeyActionCapaHK[nKeyActionId];
			int32_t& nClassId = std::get<0>(oTuple);
			int32_t& nCapaId = std::get<1>(oTuple);
			stmi::HARDWARE_KEY& eKey = std::get<2>(oTuple);
			assert(p0Prefs->getCapabilityFromIdxs(nClassId, nCapaId) != nullptr);
			auto& oHKPlayerKeyAction = p0Prefs->m_aCapabilityClassData[nClassId].m_aCapabilityData[nCapaId].m_oHKPlayerKeyAction;
			auto itFind = oHKPlayerKeyAction.find(eKey);
			assert(itFind != oHKPlayerKeyAction.end());
			#ifndef NDEBUG
			auto& oPair = itFind->second;
			const shared_ptr<Player>& refTempPlayer = oPair.first;
			const int32_t& nTempKeyActionId = oPair.second;
			#endif //NDEBUG
			assert(refTempPlayer.get() == this);
			assert(nTempKeyActionId == nKeyActionId);
			//
			oHKPlayerKeyAction.erase(itFind);
			nClassId = -1;
			nCapaId = -1;
			eKey = stmi::HK_NULL;
		}
	} else { // from AI to human
		// Since AI players have no keys assigned to key actions
		// need to fill with defaults
		p0Prefs->m_bUndefinedKeyActions = true;
		p0Prefs->populatePlayersKeyActions();
	}
	p0Prefs->recalcStuff();
	return true;
}
bool StdPreferences::Player::setOptionValue(const std::string& sOptionName, const Variant& oValue) noexcept
{
	assert(m_p0Team != nullptr);
	auto p0Prefs = m_p0Team->m_p0Prefs;
	assert(p0Prefs != nullptr);
	const NamedObjIndex< shared_ptr<Option> >& oOptions = p0Prefs->m_refStdConfig->getOptions(OwnerType::PLAYER);
	const int32_t nIdx = oOptions.getIndex(sOptionName);
	assert((nIdx >= 0) && (nIdx < static_cast<int32_t>(m_aOption.size())));
	if (nIdx == m_nAIOptionIdx) {
		assert(oValue.getType() == Variant::TYPE_BOOL);
		return setAI(oValue.getBool()); //--------------------------------------
	}
	auto& refOption = oOptions.getObj(nIdx);
	assert(!refOption->isReadonly());
	if (! refOption->isValidValue(oValue)) {
		return false; //--------------------------------------------------------
	}
	m_aOption[nIdx] = oValue;
	return true; //-------------------------------------------------------------
}
bool StdPreferences::Player::assignCapability(const shared_ptr<stmi::Capability>& refCapability) noexcept
{
	assert(refCapability);
	assert(!isAI());
	assert(m_p0Team != nullptr);
	auto p0Prefs = m_p0Team->m_p0Prefs;
	assert(p0Prefs != nullptr);
	assert(p0Prefs->m_bEditMode);
	auto& oCapabilityAssignment = p0Prefs->m_refStdConfig->getCapabilityAssignment();
	const stmi::Capability::Class& oCapaClass = refCapability->getCapabilityClass();
	const bool bIsPlayback = (oCapaClass == stmi::PlaybackCapability::getClass());
	if (bIsPlayback ) {
		if (! p0Prefs->m_refStdConfig->canPlayPerPlayerSounds()) {
			return false; //----------------------------------------------------
		}
	} else {
		if (oCapabilityAssignment.m_nMaxCapabilitiesExplicitlyAssignedToPlayer == 0) {
			// don't even try to assign
			return false; //----------------------------------------------------
		}
	}
	int32_t nClassIdx;
	int32_t nCapaIdx;
	const bool bFound = p0Prefs->getIdxsFromCapability(refCapability.get(), nClassIdx, nCapaIdx);
	if (!bFound) {
		// unknown capability (the device manager doesn't support it)
		return false; //--------------------------------------------------------
	}
	if (! bIsPlayback) {
		const bool bGoodClass = oCapabilityAssignment.canBeAssigned(oCapaClass);
		if (!bGoodClass) {
			// capability class is not among those that can be assigned
			return false; //----------------------------------------------------
		}
	}
	// Note that m_aCapabilityClassData is stable (no classes are added after initialization)
	// and m_aCapabilityData is stable within the scope of this function call,
	// capabilities cannot be added or removed since only
	// setEditMode and device manager callbacks do.
	auto& refCapaPlayer = p0Prefs->m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_refPlayer;
	if (refCapaPlayer && (refCapaPlayer.get() == this)) {
		// capability already assigned to this player
		return false; //--------------------------------------------------------
	}
	int32_t nPlaybackIdx = -1;
	const int32_t nPlaybackClassIdx = p0Prefs->getCapabilityClassIdx(stmi::PlaybackCapability::getClass());
	shared_ptr<stmi::Capability> refSwapCapability;
	int32_t nIdx = 0;
	for (const auto& oPair : m_aCapabilities) {
		const int32_t nOtherClassIdx = oPair.first;
		const int32_t nOtherCapaIdx = oPair.second;
		if (nOtherClassIdx == nClassIdx) {
			assert(nOtherCapaIdx != nCapaIdx);
			if (bIsPlayback || ! oCapabilityAssignment.m_bMoreCapabilitiesPerClassAssignedToPlayer) {
				// only one capability for this class is allowed
				shared_ptr<stmi::Capability> refOtherCapability;
				#ifndef NDEBUG
				const bool bFound =
				#endif
				p0Prefs->getCapabilityFromIdxs(nClassIdx, nOtherCapaIdx, refOtherCapability);
				assert(bFound);
				#ifndef NDEBUG
				const bool bRemoved =
				#endif //NDEBUG
				unassignCapability(refOtherCapability);
				assert(bRemoved);
				// This capability can be used if another player is robbed of
				// its capability
				refSwapCapability = refOtherCapability;
			}
		}
		if (nPlaybackClassIdx == nOtherClassIdx) {
			// this is only used if assigning a non playback capability
			assert(nPlaybackIdx < 0); // There can only be one
			nPlaybackIdx = nIdx;
		}
		++nIdx;
	}
	if (refCapaPlayer) {
		// capability assigned to another player, unassign it first
		auto refSavePlayer = refCapaPlayer;
		#ifndef NDEBUG
		const bool bRemoved =
		#endif //NDEBUG
		refSavePlayer->unassignCapability(refCapability);
		assert(bRemoved);
		assert(! refCapaPlayer);
		if (refSwapCapability) {
			// give him what you had
			#ifndef NDEBUG
			const bool bAdded =
			#endif //NDEBUG
			refSavePlayer->assignCapability(refSwapCapability);
			assert(bAdded);
		}
	}
	//
	m_aCapabilities.push_back(std::make_pair(nClassIdx, nCapaIdx));
	refCapaPlayer = shared_from_this(); // sets the player of the capability
	//
	if (! bIsPlayback) {
		const int32_t nTotCapas = static_cast<int32_t>(m_aCapabilities.size()) - ((nPlaybackIdx >= 0) ? 1 : 0);
		if (nTotCapas > oCapabilityAssignment.m_nMaxCapabilitiesExplicitlyAssignedToPlayer) {
			assert(nTotCapas > 1); // since here oCapabilityAssignment.m_nMaxCapabilitiesExplicitlyAssignedToPlayer is > 0
			// we need to remove one of the other capabilities assigned to the player,
			// but not the playback!
			const int32_t nToRemoveIdx = ((nTotCapas - 2 != nPlaybackIdx) ? nTotCapas - 2 : nTotCapas - 3);
			assert(nToRemoveIdx >= 0);
			const auto& oPair = m_aCapabilities[nToRemoveIdx];
			const int32_t nRemoveClassIdx = oPair.first;
			const int32_t nRemoveCapaIdx = oPair.first;
			shared_ptr<stmi::Capability> refRemoveCapability;
			#ifndef NDEBUG
			const bool bFound =
			#endif
			p0Prefs->getCapabilityFromIdxs(nRemoveClassIdx, nRemoveCapaIdx, refRemoveCapability);
			assert(bFound);
			#ifndef NDEBUG
			const bool bRemoved =
			#endif //NDEBUG
			unassignCapability(refRemoveCapability);
			assert(bRemoved);
		}
	}
//std::cout << "   refPlayer->get()=" << refPlayer->get() << "  refCapability=" << reinterpret_cast<int64_t>(refCapability.get()) << '\n';
	p0Prefs->recalcStuff();
	return true;
}
bool StdPreferences::Player::unassignCapability(const shared_ptr<stmi::Capability>& refCapability) noexcept
{
	assert(refCapability);
	assert(m_p0Team != nullptr);
	auto p0Prefs = m_p0Team->m_p0Prefs;
	assert(p0Prefs != nullptr);
	assert(p0Prefs->m_bEditMode);
	int32_t nClassIdx;
	int32_t nCapaIdx;
	const bool bFound = p0Prefs->getIdxsFromCapability(refCapability.get(), nClassIdx, nCapaIdx);
	if (!bFound) {
		// unknown capability
		return false; //--------------------------------------------------------
	}
	auto oPair = std::make_pair(nClassIdx, nCapaIdx);
	auto itFind = std::find(m_aCapabilities.begin(), m_aCapabilities.end(), oPair);
	if (itFind == m_aCapabilities.end()) {
		// capability not assigned to this player
		return false; //--------------------------------------------------------
	}
	auto& refPlayer = p0Prefs->m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_refPlayer;
	assert(refPlayer == shared_from_this());
	refPlayer.reset();
	m_aCapabilities.erase(itFind);
	p0Prefs->recalcStuff();
	return true;
}
std::vector< shared_ptr<stmi::Capability> > StdPreferences::Player::getCapabilities() const noexcept
{
	assert(m_p0Team != nullptr);
	auto p0Prefs = m_p0Team->m_p0Prefs;
	assert(p0Prefs != nullptr);
	std::vector< shared_ptr<stmi::Capability> > aCapabilities;
	for (auto& oPair : m_aCapabilities) {
		const int32_t& nClassIdx = oPair.first;
		const int32_t& nCapaIdx = oPair.second;
		aCapabilities.push_back(p0Prefs->m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_refCapability);
	}
	return aCapabilities;
}
shared_ptr<stmi::Capability> StdPreferences::Player::getCapability(const stmi::Capability::Class& oClass) const noexcept
{
	return getCapabilityInternal(oClass);
}
static const shared_ptr<stmi::Capability> s_sEmptyCapability{};
const shared_ptr<stmi::Capability>& StdPreferences::Player::getCapabilityInternal(const stmi::Capability::Class& oClass) const noexcept
{
	assert(m_p0Team != nullptr);
	auto p0Prefs = m_p0Team->m_p0Prefs;
	const int32_t nFindClassIdx = p0Prefs->getCapabilityClassIdx(oClass);
	for (auto& oPair : m_aCapabilities) {
		const int32_t& nClassIdx = oPair.first;
		const int32_t& nCapaIdx = oPair.second;
		if (nFindClassIdx == nClassIdx) {
			return p0Prefs->m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_refCapability;
		}
	}
	return s_sEmptyCapability;
}
bool StdPreferences::Team::isHumanPriv() const noexcept
{
	for (auto& refPlayer : m_aTeammate)
	{
		if (!refPlayer->isAI()) {
			return true;
		}
	}
	return false;
}
const shared_ptr<AppPreferences::PrefPlayer> StdPreferences::Team::getMate(int32_t nMate) const noexcept
{
	return getMateFull(nMate);
}
const shared_ptr<StdPreferences::Player>& StdPreferences::Team::getMateFull(int32_t nMate) const noexcept
{
	assert((nMate >= 0) && (nMate < getTotMates()));
	return m_aTeammate[nMate];
}
Variant StdPreferences::Team::getOptionValue(const std::string& sName) const noexcept
{
	assert(m_p0Prefs != nullptr);
	const NamedObjIndex< shared_ptr<Option> >& oOptions = m_p0Prefs->m_refStdConfig->getOptions(OwnerType::TEAM);
	const int32_t nIdx = oOptions.getIndex(sName);
	if (nIdx < 0) {
		return Variant();
	}
	assert(nIdx < static_cast<int32_t>(m_aOption.size()));
	if (nIdx == m_nAIOptionIdx) {
		// It's calculated!
		return Variant{ isAI() };
	}
	return m_aOption[nIdx];
}
bool StdPreferences::Team::setName(const std::string& sNewName) noexcept
{
	assert(m_p0Prefs != nullptr);
	std::unordered_map<std::string, shared_ptr<Team> >::const_iterator itOld = m_p0Prefs->m_oNamedTeams.find(m_sName);
	assert(itOld != m_p0Prefs->m_oNamedTeams.end());
	if (m_sName == sNewName) {
		return true; //---------------------------------------------------------
	}
	std::unordered_map<std::string, shared_ptr<Team> >::const_iterator itNew = m_p0Prefs->m_oNamedTeams.find(sNewName);
	if (itNew != m_p0Prefs->m_oNamedTeams.end()) {
		return false; //--------------------------------------------------------
	}
	m_p0Prefs->m_oNamedTeams.erase(itOld);
	// Set new name
	m_sName = sNewName;
	m_p0Prefs->m_oNamedTeams.insert(std::make_pair(sNewName, shared_from_this()));
	return true;
}
int32_t StdPreferences::Team::getTotAIMates() const noexcept
{
	int32_t nTotAI = 0;
	for (auto& refPlayer : m_aTeammate)
	{
		if (refPlayer->isAI()) {
			++nTotAI;
		}
	}
	return nTotAI;
}
bool StdPreferences::Team::setTotMates(int32_t nTotMates) noexcept
{
	assert(m_p0Prefs != nullptr);
	const int32_t nOldTotMates = getTotMates();
	const int32_t nDiff = nTotMates - nOldTotMates;
	const int32_t nOldTotPlayers = m_p0Prefs->getTotPlayers();
	return m_p0Prefs->setTotPlayers(nOldTotPlayers + nDiff, m_nTeam);
}
bool StdPreferences::Team::setOptionValue(const std::string& sOptionName, const Variant& oValue) noexcept
{
	assert(m_p0Prefs != nullptr);
	const NamedObjIndex< shared_ptr<Option> >& oOptions = m_p0Prefs->m_refStdConfig->getOptions(OwnerType::TEAM);
	const int32_t nIdx = oOptions.getIndex(sOptionName);
	assert((nIdx >= 0) && (nIdx < static_cast<int32_t>(m_aOption.size())));
	auto& refOption = oOptions.getObj(nIdx);
	if (nIdx == m_nAIOptionIdx) {
		assert(oValue.getType() == Variant::TYPE_BOOL);
		assert(refOption->isReadonly());
	}
	assert(!refOption->isReadonly());
	if (! refOption->isValidValue(oValue)) {
		return false;
	}
	m_aOption[nIdx] = oValue;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
StdPreferences::StdPreferences(const shared_ptr<StdConfig>& refStdConfig) noexcept
: m_refAppConfig(refStdConfig)
, m_refStdConfig(refStdConfig)
, m_bEditMode(true)
, m_bUndefinedKeyActions(true)
{
	assert(refStdConfig);
	assert(refStdConfig->getAppConstraints().isValid());
	// initialize game options with default
	const NamedObjIndex< shared_ptr<Option> >& oOptions = refStdConfig->getOptions(OwnerType::GAME);
	const int32_t nTotOptions = oOptions.size();
	for (int32_t nIdx = 0; nIdx < nTotOptions; ++nIdx) {
		const shared_ptr<Option>& refOption = oOptions.getObj(nIdx);
		m_aOption.push_back(refOption->getDefaultValue());
	}
	initCreatePlayers();
	initCreateCapabilities();
	initListenToDeviceMgmt();
	populatePlayersKeyActions();
	recalcStuff();
}
StdPreferences::StdPreferences(const StdPreferences& oSource) noexcept
: m_refAppConfig(oSource.m_refAppConfig)
, m_refStdConfig(oSource.m_refStdConfig)
{
	operator=(oSource);
	initListenToDeviceMgmt();
	setEditMode(true);
}
StdPreferences& StdPreferences::operator=(const StdPreferences& oSource) noexcept
{
	assert(m_refStdConfig == oSource.m_refStdConfig);
	assert(m_refAppConfig == oSource.m_refAppConfig);

	m_bEditMode = oSource.m_bEditMode;
	m_bUndefinedKeyActions = oSource.m_bUndefinedKeyActions;

	// copy teams
	m_aTeam.clear();
	for (auto& refCurTeam : oSource.m_aTeam) {
		auto refNewTeam = std::make_shared<Team>(*refCurTeam); // shallow copy
		m_aTeam.push_back(refNewTeam);
		refNewTeam->m_p0Prefs = this;
		refNewTeam->m_aTeammate.clear();
		for (auto& refCurMate : refCurTeam->m_aTeammate) {
			auto refNewMate = std::make_shared<Player>(*refCurMate); // (shallow) copy
			refNewTeam->m_aTeammate.push_back(refNewMate);
			refNewMate->m_p0Team = refNewTeam.get();
		}
	}
	m_aFreeTeams.clear(); // don't copy
	m_aFreePlayers.clear(); // don't copy
	m_oNamedTeams = oSource.m_oNamedTeams;
	for (auto& oSourcePair : m_oNamedTeams) {
		auto& refTeam = oSourcePair.second;
		const int32_t nTeam = refTeam->get();
		refTeam = m_aTeam[nTeam];
	}
	m_oNamedPlayers = oSource.m_oNamedPlayers;
	for (auto& oSourcePair : m_oNamedPlayers) {
		auto& refPlayer = oSourcePair.second;
		const int32_t nTeam = refPlayer->getTeam()->get();
		const int32_t nMate = refPlayer->getMate();
		refPlayer = m_aTeam[nTeam]->m_aTeammate[nMate];

	}
	m_aPlayerNrToPlayer = oSource.m_aPlayerNrToPlayer;
	for (auto& refPlayer : m_aPlayerNrToPlayer) {
		const int32_t nTeam = refPlayer->getTeam()->get();
		const int32_t nMate = refPlayer->getMate();
		refPlayer = m_aTeam[nTeam]->m_aTeammate[nMate];
	}
	m_aOption = oSource.m_aOption;
	// copy capability data
	m_aCapabilityClass = oSource.m_aCapabilityClass;
	m_aCapabilityClassData = oSource.m_aCapabilityClassData;
	// make the copy deep
	for (auto& oCapabilityClassData : m_aCapabilityClassData) {
		for (auto& oCapabilityData : oCapabilityClassData.m_aCapabilityData) {
			for (auto& oPair : oCapabilityData.m_oHKPlayerKeyAction) {
				auto& oValuePair = oPair.second;
				auto& refPlayer = oValuePair.first;
				const int32_t nTeam = refPlayer->getTeam()->get();
				const int32_t nMate = refPlayer->getMate();
				refPlayer = m_aTeam[nTeam]->m_aTeammate[nMate];
			}
			auto& refPlayer = oCapabilityData.m_refPlayer;
			if (refPlayer) {
				const int32_t nTeam = refPlayer->getTeam()->get();
				const int32_t nMate = refPlayer->getMate();
				refPlayer = m_aTeam[nTeam]->m_aTeammate[nMate];
			}
		}
	}

	if (oSource.m_refDefaultPlayer) {
		const int32_t nTeam = oSource.m_refDefaultPlayer->getTeam()->get();
		const int32_t nMate = oSource.m_refDefaultPlayer->getMate();
		m_refDefaultPlayer = m_aTeam[nTeam]->m_aTeammate[nMate];
	} else {
		m_refDefaultPlayer.reset();
	}
	// Note: initListenToDeviceMgmt() must not be called since done in constructor
	return *this;
}
void StdPreferences::initCreatePlayers() noexcept
{
//std::cout << "StdPreferences::initCreatePlayers" << '\n';
	const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
	const int32_t nMinTeams = oAppConstraints.m_nTeamsMin;
	for (int32_t nCount = 0; nCount < nMinTeams; ++nCount) {
		shared_ptr<Team> refTeam = teamAlloc();
		shared_ptr<Player> refPlayer = playerAlloc();
		playerAddToTeam(refPlayer, refTeam);
	}
}
void StdPreferences::initCreateCapabilities() noexcept
{
	const shared_ptr<stmi::DeviceManager>& refDM = m_refStdConfig->getDeviceManager();
	// Create capabilities
	m_aCapabilityClass = refDM->getDeviceCapabilityClasses();
	// TODO Following is debugging the DeviceManager: remove!
	#ifndef NDEBUG
	for (auto& oClass : m_aCapabilityClass) {
		assert(!oClass.isDeviceManagerCapability());
	}
	#endif //NDEBUG
	m_aCapabilityClassData.resize(m_aCapabilityClass.size());
	int32_t nClassIdx = 0;
	for (auto& oClass : m_aCapabilityClass) {
		auto& oCapabilityClassData = m_aCapabilityClassData[nClassIdx];
		oCapabilityClassData.m_nTotAssignedCapabilities = -77; // set by recalcStuff()
		std::vector<int32_t> aDevices = refDM->getDevicesWithCapabilityClass(oClass);
		const auto nTotCapas = aDevices.size();
		oCapabilityClassData.m_aCapabilityData.resize(nTotCapas);
		int32_t nCapaIdx = 0;
		for (int32_t nDeviceId : aDevices) {
			auto refDevice = refDM->getDevice(nDeviceId);
			auto refCapa = refDevice->getCapability(oClass);
			oCapabilityClassData.m_aCapability.push_back(refCapa.get());
			auto& oCapabilityData = oCapabilityClassData.m_aCapabilityData[nCapaIdx];
			oCapabilityData.m_bRuntimeRemoved = false;
			oCapabilityData.m_bRuntimeAdded = false;
			oCapabilityData.m_refCapability = refCapa;
			++nCapaIdx;
		}
		++nClassIdx;
	}
}
void StdPreferences::initListenToDeviceMgmt() noexcept
{
	const shared_ptr<stmi::DeviceManager>& refDM = m_refStdConfig->getDeviceManager();
	// Create device manager listener
	m_refDeviceMgmtListener = std::make_shared<stmi::EventListener>(
	[this](const shared_ptr<stmi::Event>& refEvent)
	{
		auto p0MgmtEvent = static_cast<stmi::DeviceMgmtEvent*>(refEvent.get());
		if (p0MgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED) {
			deviceRemoved(p0MgmtEvent->getDevice());
		} else if (p0MgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED) {
			deviceAdded(p0MgmtEvent->getDevice());
		} else {
			//TODO for now we assume that a changed device cannot remove or add capabilities
		}
	});
	m_refCallIfDeviceMgmt = std::make_shared<stmi::CallIfEventClass>(stmi::DeviceMgmtEvent::getClass());
	refDM->addEventListener(m_refDeviceMgmtListener, m_refCallIfDeviceMgmt);
}
void StdPreferences::deviceAdded(const shared_ptr<stmi::Device>& refDevice) noexcept
{
	const int32_t nTotClasses = static_cast<int32_t>(m_aCapabilityClass.size());
	for (int32_t nClassIdx = 0; nClassIdx < nTotClasses; ++nClassIdx) {
		auto refCapa = refDevice->getCapability(m_aCapabilityClass[nClassIdx]);
		if (!refCapa) {
			continue; //for -------
		}
		auto& oCapabilityClassData = m_aCapabilityClassData[nClassIdx];
		std::vector<stmi::Capability*>& aCapability = oCapabilityClassData.m_aCapability;
		int32_t nCapaIdx = -1;
		if (!m_bEditMode) {
			auto itRemoved = std::find(aCapability.begin(), aCapability.end(), nullptr);
			if (itRemoved != aCapability.end()) {
				nCapaIdx = std::distance(aCapability.begin(), itRemoved);
			}
		}
		if (nCapaIdx >= 0) {
			// runtime mode and
			// there is a removed capability slot for the class,
			// fill it with the newly added
			aCapability[nCapaIdx] = refCapa.get();
			CapabilityData& oCapabilityData = oCapabilityClassData.m_aCapabilityData[nCapaIdx];
			assert(!oCapabilityData.m_bRuntimeAdded);
			assert(oCapabilityData.m_bRuntimeRemoved);
			oCapabilityData.m_bRuntimeRemoved = false;
			oCapabilityData.m_refCapability = refCapa;
			// Note: m_oHKPlayerKeyAction and m_refPlayer are kept as they are!
		} else {
			// add the new one marking it as runtime added if in runtime mode
			aCapability.push_back(refCapa.get());
			nCapaIdx = static_cast<int32_t>(oCapabilityClassData.m_aCapabilityData.size());
			oCapabilityClassData.m_aCapabilityData.resize(nCapaIdx + 1);
			CapabilityData& oCapabilityData = oCapabilityClassData.m_aCapabilityData[nCapaIdx];
			oCapabilityData.m_bRuntimeAdded = !m_bEditMode; // !!! in runtime mode this is set to true!
			oCapabilityData.m_bRuntimeRemoved = false;
			oCapabilityData.m_refCapability = refCapa;
			if (m_bEditMode) {
				recalcStuff();
			}
		}
	}
}
void StdPreferences::removeReferencesToCapability(int32_t nClassIdx, int32_t nCapaIdx, CapabilityData& oCapabilityData) noexcept
{
	for (auto& oPair : oCapabilityData.m_oHKPlayerKeyAction) {
		#ifndef NDEBUG
		const stmi::HARDWARE_KEY& eKey = oPair.first;
		#endif //NDEBUG
		auto& oPlayerActionPair = oPair.second;
		shared_ptr<Player>& refPlayer = oPlayerActionPair.first;
		int32_t& nKeyAction = oPlayerActionPair.second;
		auto& oTuple = refPlayer->m_aKeyActionCapaHK[nKeyAction];
		int32_t& nTempClassIdx = std::get<0>(oTuple);
		int32_t& nTempCapaIdx = std::get<1>(oTuple);
		stmi::HARDWARE_KEY& eTempKey = std::get<2>(oTuple);
		assert(nClassIdx == nTempClassIdx);
		assert(nCapaIdx == nTempCapaIdx);
		assert(eKey == eTempKey);
		// Reset player's key action
		nTempClassIdx = -1;
		nTempCapaIdx = -1;
		eTempKey = stmi::HK_NULL;
		m_bUndefinedKeyActions = true;
	}
	oCapabilityData.m_oHKPlayerKeyAction.clear();
	//
	if (oCapabilityData.m_refPlayer) {
		auto& aCapabilities = oCapabilityData.m_refPlayer->m_aCapabilities;
		auto itFindCapa = std::find(aCapabilities.begin(), aCapabilities.end(), std::make_pair(nClassIdx, nCapaIdx));
		assert(itFindCapa != aCapabilities.end());
		// Remove assignment from player
		aCapabilities.erase(itFindCapa);
		//
		oCapabilityData.m_refPlayer.reset();
	}
}
void StdPreferences::moveReferencesToCapability(int32_t nClassIdx, int32_t nRemovedCapaIdx, int32_t nLastCapaIdx
												, CapabilityData& oCapabilityData, CapabilityData& oLastCapabilityData) noexcept
{
	for (auto& oPair : oLastCapabilityData.m_oHKPlayerKeyAction) {
		#ifndef NDEBUG
		const stmi::HARDWARE_KEY& eKey = oPair.first;
		#endif //NDEBUG
		auto& oPlayerActionPair = oPair.second;
		shared_ptr<Player>& refPlayer = oPlayerActionPair.first;
		int32_t& nKeyAction = oPlayerActionPair.second;
		auto& oTuple = refPlayer->m_aKeyActionCapaHK[nKeyAction];
		#ifndef NDEBUG
		int32_t& nTempClassIdx = std::get<0>(oTuple);
		#endif //NDEBUG
		int32_t& nTempCapaIdx = std::get<1>(oTuple);
		#ifndef NDEBUG
		stmi::HARDWARE_KEY& eTempKey = std::get<2>(oTuple);
		#endif //NDEBUG
		assert(nClassIdx == nTempClassIdx);
		assert(nLastCapaIdx == nTempCapaIdx);
		assert(eKey == eTempKey);
		// Point to capability's new position within class
		nTempCapaIdx = nRemovedCapaIdx;
	}
	oCapabilityData.m_oHKPlayerKeyAction.swap(oLastCapabilityData.m_oHKPlayerKeyAction);
	assert(oLastCapabilityData.m_oHKPlayerKeyAction.empty());
	//
	if (oLastCapabilityData.m_refPlayer) {
		auto& aCapabilities = oLastCapabilityData.m_refPlayer->m_aCapabilities;
		auto itFindCapa = std::find(aCapabilities.begin(), aCapabilities.end(), std::make_pair(nClassIdx, nLastCapaIdx));
		assert(itFindCapa != aCapabilities.end());
		// Point to capability's new position within class
		itFindCapa->second = nRemovedCapaIdx;
	}
	oCapabilityData.m_refPlayer.swap(oLastCapabilityData.m_refPlayer);
	assert(!oLastCapabilityData.m_refPlayer);
	oCapabilityData.m_refCapability.swap(oLastCapabilityData.m_refCapability);
}
void StdPreferences::deviceRemoved(const shared_ptr<stmi::Device>& refDevice) noexcept
{
	const int32_t nTotClasses = static_cast<int32_t>(m_aCapabilityClass.size());
	for (int32_t nClassIdx = 0; nClassIdx < nTotClasses; ++nClassIdx) {
		auto refCapa = refDevice->getCapability(m_aCapabilityClass[nClassIdx]);
		if (!refCapa) {
			continue; //for -------
		}
		auto& oCapabilityClassData = m_aCapabilityClassData[nClassIdx];
		std::vector<stmi::Capability*>& aCapability = oCapabilityClassData.m_aCapability;
		auto itFind = std::find(aCapability.begin(), aCapability.end(), refCapa.get());
		if (itFind == aCapability.end()) {
			continue; //for -------
		}
		const int32_t nRemoveCapaIdx = std::distance(aCapability.begin(), itFind);
		CapabilityData& oCapabilityData = oCapabilityClassData.m_aCapabilityData[nRemoveCapaIdx];
		if ((!m_bEditMode) && !oCapabilityData.m_bRuntimeAdded) {
			// runtime mode: just mark the slot as removed, it might be refilled in deviceAdded()
			aCapability[nRemoveCapaIdx] = nullptr;
			oCapabilityData.m_refCapability.reset();
			oCapabilityData.m_bRuntimeRemoved = true;
			// Don't reset oCapabilityData.m_refPlayer
			// Don't clear oCapabilityData.m_oHKPlayerKeyAction
		} else {
			// Either in edit mode or
			// the capability being removed at runtime was also added at runtime
			// which means it isn't used in key actions or assigned to a player.

			if (m_bEditMode) {
				// Remove all key actions associated with the capability
				// and remove it from the player it is assigned to, if any.
				removeReferencesToCapability(nClassIdx, nRemoveCapaIdx, oCapabilityData);
			} else { // runtime added
				assert(!oCapabilityData.m_refPlayer);
				assert(oCapabilityData.m_oHKPlayerKeyAction.empty());
			}
			//
			const int32_t nTotCapas = static_cast<int32_t>(aCapability.size());
			const int32_t nLastCapaIdx = nTotCapas - 1;
			if (nRemoveCapaIdx < nLastCapaIdx) {
				// Since the removed is not the last capability, the last needs to be swapped
				// to the removed capability slot.
				// Note: if the removed capability was runtime added, then also the last is
				// which means that there are no references to either
				CapabilityData& oLastCapabilityData = oCapabilityClassData.m_aCapabilityData[nLastCapaIdx];
				if (m_bEditMode) {
					moveReferencesToCapability(nClassIdx, nRemoveCapaIdx, nLastCapaIdx, oCapabilityData, oLastCapabilityData);
					aCapability[nRemoveCapaIdx] = aCapability[nLastCapaIdx];
					//
				} else { // runtime mode
					assert(!oCapabilityData.m_bRuntimeRemoved);
					assert(oCapabilityData.m_bRuntimeAdded);
					assert(!oCapabilityData.m_refPlayer);
					assert(oCapabilityData.m_oHKPlayerKeyAction.empty());
					assert(!oLastCapabilityData.m_bRuntimeRemoved);
					assert(oLastCapabilityData.m_bRuntimeAdded);
					assert(!oLastCapabilityData.m_refPlayer);
					assert(oLastCapabilityData.m_oHKPlayerKeyAction.empty());
				}
				//
			} else {
				// It's the last, can be removed
			}
			// Remove the last
			aCapability.pop_back();
			oCapabilityClassData.m_aCapabilityData.pop_back();
		}
	}
	if (m_bEditMode) {
		// nedded because removeReferencesToCapability() might have set m_bUndefinedKeyActions
		populatePlayersKeyActions();
		recalcStuff();
	}
}
void StdPreferences::setEditMode(bool bInEditMode) noexcept
{
	if ((m_bEditMode && bInEditMode) || ((!m_bEditMode) && !bInEditMode)) {
		return;
	}
	if (!bInEditMode) {
		// to runtime mode
		m_bEditMode = false;
		return; //--------------------------------------------------------------
	}
	m_bEditMode = true;
	// runtime mode to edit mode.
	const int32_t nTotClasses = static_cast<int32_t>(m_aCapabilityClass.size());
	for (int32_t nClassIdx = 0; nClassIdx < nTotClasses; ++nClassIdx) {
		// Compact vector of capabilities by swapping the last not m_bRuntimeRemoved with
		// the first m_bRuntimeRemoved repeatedly.
		auto& oCapabilityClassData = m_aCapabilityClassData[nClassIdx];
		std::vector<stmi::Capability*>& aCapability = oCapabilityClassData.m_aCapability;
		// Remove all references (key actions, player) to m_bRuntimeRemoved capabilities
		const int32_t nTotCapas = static_cast<int32_t>(aCapability.size());
//std::cout << "setEditMode  nClassIdx=" << nClassIdx << "  nTotCapas=" << nTotCapas << '\n';
		for (int32_t nCapaIdx = 0; nCapaIdx < nTotCapas; ++nCapaIdx) {
			CapabilityData& oCapabilityData = oCapabilityClassData.m_aCapabilityData[nCapaIdx];
			if (aCapability[nCapaIdx] == nullptr) {
//std::cout << "setEditMode  null nCapaIdx=" << nCapaIdx << '\n';
				removeReferencesToCapability(nClassIdx, nCapaIdx, oCapabilityData);
				m_bUndefinedKeyActions = true;
				assert(oCapabilityData.m_bRuntimeRemoved);
				oCapabilityData.m_bRuntimeRemoved = false;
			}
			oCapabilityData.m_bRuntimeAdded = false;
		}
		int32_t nLastCapaIdx = nTotCapas - 1;
		while (nLastCapaIdx >= 0) {
//std::cout << "setEditMode  nLastCapaIdx=" << nLastCapaIdx << '\n';
			if (aCapability[nLastCapaIdx] != nullptr) {
//std::cout << "setEditMode  not null nLastCapaIdx=" << nLastCapaIdx << '\n';
				auto itFindRemoved = std::find(aCapability.begin(), aCapability.end(), nullptr);
				if (itFindRemoved == aCapability.end()) {
					// all m_bRuntimeRemoved slots removed
					break; //while -------
				}
				const int32_t nRemovedCapaIdx = std::distance(aCapability.begin(), itFindRemoved);
//std::cout << "setEditMode  nRemovedCapaIdx=" << nRemovedCapaIdx << "  nLastCapaIdx=" << nLastCapaIdx << '\n';
				CapabilityData& oRemovedCapabilityData = oCapabilityClassData.m_aCapabilityData[nRemovedCapaIdx];
				CapabilityData& oLastCapabilityData = oCapabilityClassData.m_aCapabilityData[nLastCapaIdx];
				// adjust references to last capability to removed capability
				moveReferencesToCapability(nClassIdx, nRemovedCapaIdx, nLastCapaIdx, oRemovedCapabilityData, oLastCapabilityData);
				assert(! oLastCapabilityData.m_refCapability);
				aCapability[nRemovedCapaIdx] = aCapability[nLastCapaIdx];
			}
			// remove last
			aCapability.pop_back();
			oCapabilityClassData.m_aCapabilityData.pop_back();
			--nLastCapaIdx;
		}
	}
	// since removeReferencesToCapability() might have set m_bUndefinedKeyActions
	populatePlayersKeyActions();
	recalcStuff();
}
int32_t StdPreferences::getCapabilityClassIdx(const stmi::Capability::Class& oClass) const noexcept
{
	auto itFindClass = std::find(m_aCapabilityClass.begin(), m_aCapabilityClass.end(), oClass);
	if (itFindClass == m_aCapabilityClass.end()) {
		return -1; //-----------------------------------------------------------
	}
	const int32_t nClassIdx = std::distance(m_aCapabilityClass.begin(), itFindClass);
	return nClassIdx;
}
bool StdPreferences::getCapabilityFromIdxs(int32_t nClassIdx, int32_t nCapaIdx
											, shared_ptr<stmi::Capability>& refCapability) const noexcept
{
	if ((nClassIdx < 0) || (nClassIdx >= static_cast<int32_t>(m_aCapabilityClass.size()))) {
		return false;
	}
	auto& aCapabilityData = m_aCapabilityClassData[nClassIdx].m_aCapabilityData;
	if ((nCapaIdx < 0) || (nCapaIdx >= static_cast<int32_t>(aCapabilityData.size()))) {
		return false;
	}
	refCapability = aCapabilityData[nCapaIdx].m_refCapability;
	return true;
}
stmi::Capability* StdPreferences::getCapabilityFromIdxs(int32_t nClassIdx, int32_t nCapaIdx) const noexcept
{
	shared_ptr<stmi::Capability> refCapability;
	const bool bFound = getCapabilityFromIdxs(nClassIdx, nCapaIdx, refCapability);
	if (!bFound) {
		return nullptr;
	}
	return refCapability.get();
}
bool StdPreferences::getIdxsFromCapability(stmi::Capability const* p0Capability, int32_t& nClassIdx, int32_t& nCapaIdx) const noexcept
{
	assert(p0Capability != nullptr);
	const stmi::Capability::Class& oClass = p0Capability->getCapabilityClass();
	const int32_t nTempClassIdx = getCapabilityClassIdx(oClass);
	if (nTempClassIdx < 0) {
		return false; //--------------------------------------------------------
	}
	auto& aCapability = m_aCapabilityClassData[nTempClassIdx].m_aCapability;
	auto itFind = std::find(aCapability.begin(), aCapability.end(), p0Capability);
	if (itFind == aCapability.end()) {
		return false; //--------------------------------------------------------
	}
	nClassIdx = nTempClassIdx;
	nCapaIdx = std::distance(aCapability.begin(), itFind);
	assert(!m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_bRuntimeRemoved);
	return true;
}
bool StdPreferences::getIdxsFromCapabilityId(int32_t nCapabilityId, int32_t& nClassIdx, int32_t& nCapaIdx) const noexcept
{
	for (auto itClass = m_aCapabilityClassData.begin(); itClass != m_aCapabilityClassData.end(); ++itClass) {
		const CapabilityClassData& oCapabilityClassData = *itClass;
		for (auto itCapa = oCapabilityClassData.m_aCapabilityData.begin(); itCapa != oCapabilityClassData.m_aCapabilityData.end(); ++itCapa) {
			const CapabilityData& oCapabilityData = *itCapa;
			if (oCapabilityData.m_refCapability && (oCapabilityData.m_refCapability->getId() == nCapabilityId)) {
				assert(!oCapabilityData.m_bRuntimeRemoved);
				nClassIdx = std::distance(m_aCapabilityClassData.begin(), itClass);
				nCapaIdx = std::distance(oCapabilityClassData.m_aCapabilityData.begin(), itCapa);
				return true;
			}
		}
	}
	return false;
}

shared_ptr<StdPreferences::Team> StdPreferences::teamAlloc() noexcept
{
	const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
	const int32_t nMaxTeams = oAppConstraints.getMaxTeams();
	static const std::string s_sTeamNameBase = "Team";
	const int32_t nSizeOfMaxTeamNr = static_cast<int32_t>(std::to_string(nMaxTeams).size());
	const std::string sTeamNameBase = s_sTeamNameBase.substr(0, getMaxTeamNameLength() - 1 - nSizeOfMaxTeamNr);

	const NamedObjIndex< shared_ptr<Option> >& oOptions = m_refStdConfig->getOptions(OwnerType::TEAM);
	const int32_t nTotOptions = oOptions.size();
	shared_ptr<Team> refFreeTeam;
	if (m_aFreeTeams.empty()) {
		refFreeTeam = std::make_shared<Team>();
		refFreeTeam->m_sName = sTeamNameBase;
		// create options array
		refFreeTeam->m_aOption.resize(nTotOptions);
	} else {
		refFreeTeam = m_aFreeTeams.back();
		m_aFreeTeams.pop_back();
	}
	if (m_oNamedTeams.find(refFreeTeam->m_sName) != m_oNamedTeams.end()) {
		for (int32_t nTryTeamNr = 1; nTryTeamNr <= nMaxTeams; ++nTryTeamNr) {
			const std::string sTeamName = sTeamNameBase + " " + std::to_string(nTryTeamNr);
			if (m_oNamedTeams.find(sTeamName) == m_oNamedTeams.end()) {
				refFreeTeam->m_sName = sTeamName;
				break; //for ---
			}
		}
	}
	const int32_t nTotTeams = static_cast<int32_t>(m_aTeam.size());
	refFreeTeam->m_nTeam = nTotTeams;
	assert(refFreeTeam->m_aTeammate.empty());
	refFreeTeam->m_p0Prefs = this;
	// insert
	m_aTeam.push_back(refFreeTeam);
	m_oNamedTeams.emplace(refFreeTeam->m_sName, refFreeTeam);
	// defaults
	for (int32_t nIdx = 0; nIdx < nTotOptions; ++nIdx) {
		const shared_ptr<Option>& refOption = oOptions.getObj(nIdx);
		const Variant oDefault = refOption->getDefaultValue();
		refFreeTeam->m_aOption[nIdx] = oDefault;
		if (refOption->getName() == m_refStdConfig->getAIOptionName()) {
			refFreeTeam->m_nAIOptionIdx = nIdx;
			assert(oDefault.getType() == Variant::TYPE_BOOL);
		}
	}

	return refFreeTeam;
}
void StdPreferences::teamFree(const shared_ptr<Team>& refTeam) noexcept
{
	assert(refTeam);
	#ifndef NDEBUG
	const int32_t nTotTeams = static_cast<int32_t>(m_aTeam.size());
	#endif //NDEBUG
	assert(m_aTeam[nTotTeams - 1] == refTeam); // Only removal of last team is allowed
	assert(refTeam->m_nTeam == nTotTeams - 1);

	m_aTeam.pop_back();
	refTeam->m_p0Prefs = nullptr;
	refTeam->m_nTeam = -1;
	// free all mates
	for (auto& refMate : refTeam->m_aTeammate) {
		playerFree(refMate);
	}
	refTeam->m_aTeammate.clear();
	m_oNamedTeams.erase(refTeam->m_sName);
	m_aFreeTeams.push_back(refTeam);
}
void StdPreferences::populatePlayersKeyActions() noexcept
{
//std::cout << "populatePlayersKeyActions m_bUndefinedKeyActions=" << m_bUndefinedKeyActions << '\n';
	assert(m_bEditMode);
	if (!m_bUndefinedKeyActions) {
		return;
	}
	m_bUndefinedKeyActions = false;
	const int32_t nTotTeams = static_cast<int32_t>(m_aTeam.size());
//std::cout << "populatePlayersKeyActions nTotTeams=" << nTotTeams << '\n';
	for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
		Team& oTeam = *(m_aTeam[nTeam]);
		const int32_t nTotMates = static_cast<int32_t>(oTeam.m_aTeammate.size());
//std::cout << "populatePlayersKeyActions nTotMates=" << nTotMates << '\n';
		for (int32_t nMate = 0; nMate < nTotMates; ++nMate) {
			shared_ptr<Player>& refPlayer = oTeam.m_aTeammate[nMate];
//std::cout << "populatePlayersKeyActions nTeam=" << nTeam << " nMate=" << nMate << '\n';
			if (refPlayer->isAI()) {
				continue; //for nMate ---------
			}
			int32_t nPriorityClassIdx = -1;
			int32_t nPriorityCapaIdx = -1;
			int32_t nPriorityDefaultIdx = -1;
			int32_t nKeyActionId = 0;
			for (auto& oTuple : refPlayer->m_aKeyActionCapaHK) {
				int32_t& nCapaIdx = std::get<1>(oTuple);
				stmi::HARDWARE_KEY& eKey = std::get<2>(oTuple);
				if ((nCapaIdx < 0) || (eKey == stmi::HK_NULL)) {
					int32_t& nClassIdx = std::get<0>(oTuple);
					// undefined
					bool bDefined = populatePlayerKeyAction(refPlayer, nKeyActionId, nClassIdx, nCapaIdx, eKey
														, nPriorityClassIdx, nPriorityCapaIdx, nPriorityDefaultIdx);
					if (!bDefined) {
//std::cout << "populatePlayersKeyActions !bDefined" << '\n';
						m_bUndefinedKeyActions = true;
					} else {
						auto& oCapabilityClassData = m_aCapabilityClassData[nClassIdx];
						auto& oCapabilityData = oCapabilityClassData.m_aCapabilityData[nCapaIdx];
//std::cout << "populatePlayersKeyActions defined nKeyActionId=" << nKeyActionId << "  nClassIdx=" << nClassIdx << " nCapaIdx=" << nCapaIdx
						oCapabilityData.m_oHKPlayerKeyAction.emplace(eKey, std::make_pair(refPlayer, nKeyActionId));
						refPlayer->m_aKeyActionCapaHK[nKeyActionId] = std::make_tuple(nClassIdx, nCapaIdx, eKey);
					}
				}
				++nKeyActionId;
			}
		}
	}
}
bool StdPreferences::populatePlayerKeyAction(const shared_ptr<Player>& refPlayer, int32_t nKeyActionId
											, int32_t& nClassIdx, int32_t& nCapaIdx, stmi::HARDWARE_KEY& eKey
											, int32_t& nPriorityClassIdx, int32_t& nPriorityCapaIdx, int32_t& nPriorityDefaultIdx) noexcept
{
//std::cout << "StdPreferences::populatePlayerKeyAction nPlayer=" << refPlayer->get() << "  nKeyActionId=" << nKeyActionId << '\n';
	const StdConfig::KeyAction& oKeyAction =  m_refStdConfig->getKeyAction(nKeyActionId);
	// nMode == 0: try to set all defaults with nPriorityDefaultIdx (>=0) for nPriorityClassIdx to nPriorityCapaIdx
	// nMode == 1: try to set all defaults with all priorities (nPriorityDefaultIdx < 0) for nPriorityClassIdx to nPriorityCapaIdx
	// nMode == 2: try to set defaults (all idxs) for all capabilities assigned to this player (Player::m_aCapabilities)
	// nMode == 3: try to set defaults (all idxs) for all capabilities not assigned to a different player (!CapabilityData::m_refPlayer)
	// nMode == 4: try to set defaults (all idxs) for all capabilities assigned to a different player (CapabilityData::m_refPlayer != refPlayer)
	// nMode == 5: take the first capability found during nMode 0..4 and find an unused key (random)
	int32_t nRandomClassIdx = -1;
	int32_t nRandomCapaIdx = -1;
	for (int32_t nMode = 0; nMode < 5; ++nMode) {
		if ((nMode == 0) || (nMode == 1)) {
			if (nPriorityCapaIdx >= 0) {
				assert(nPriorityClassIdx >= 0);
				if (nRandomCapaIdx < 0) {
					nRandomClassIdx = nPriorityClassIdx;
					nRandomCapaIdx = nPriorityCapaIdx;
				}
				auto& aDefaultKeys = oKeyAction.getClassDefaultKeys(m_aCapabilityClass[nPriorityClassIdx]);
				const int32_t nTotDefaults = static_cast<int32_t>(aDefaultKeys.size());
				for (int32_t nDefKeyIdx = 0; nDefKeyIdx < nTotDefaults; ++nDefKeyIdx) {
					if (((nMode == 0) && (nDefKeyIdx == nPriorityDefaultIdx)) || (nMode == 1)) {
						stmi::HARDWARE_KEY eDefKey = aDefaultKeys[nDefKeyIdx];
						if ((eDefKey != stmi::HK_NULL) && !capabilityKeyInUse(nPriorityClassIdx, nPriorityCapaIdx, eDefKey)) {
							//
							nClassIdx = nPriorityClassIdx;
							nCapaIdx = nPriorityCapaIdx;
							eKey = eDefKey;
							nPriorityDefaultIdx = nDefKeyIdx;
							return true; //-------------------------------------
						}
					}
				}
			}
		} else if ((nMode >= 2) && (nMode <= 4)) {
			const int32_t nTotClasses = static_cast<int32_t>(m_aCapabilityClass.size());
			for (int32_t nCurClassIdx = 0; nCurClassIdx < nTotClasses; ++nCurClassIdx) {
				const CapabilityClassData& oClassData = m_aCapabilityClassData[nCurClassIdx];
				const int32_t nTotCapas = static_cast<int32_t>(oClassData.m_aCapability.size());
				auto& aDefaultKeys = oKeyAction.getClassDefaultKeys(m_aCapabilityClass[nCurClassIdx]);
				const int32_t nTotDefaults = static_cast<int32_t>(aDefaultKeys.size());
				for (int32_t nDefKeyIdx = 0; nDefKeyIdx < nTotDefaults; ++nDefKeyIdx) {
					stmi::HARDWARE_KEY eDefKey = aDefaultKeys[nDefKeyIdx];
					for (int32_t nCurCapaIdx = 0; nCurCapaIdx < nTotCapas; ++nCurCapaIdx) {
						if (nRandomCapaIdx < 0) {
							nRandomClassIdx = nCurClassIdx;
							nRandomCapaIdx = nCurCapaIdx;
						}
						const CapabilityData& oCapaData = oClassData.m_aCapabilityData[nCurCapaIdx];
						if (((nMode == 2) && (oCapaData.m_refPlayer == refPlayer))
								|| ((nMode == 3) && !oCapaData.m_refPlayer)
								|| ((nMode == 4) && (oCapaData.m_refPlayer != refPlayer))) {
							if ((eDefKey != stmi::HK_NULL) && !capabilityKeyInUse(nCurClassIdx, nCurCapaIdx, eDefKey)) {
								//
								nClassIdx = nCurClassIdx;
								nCapaIdx = nCurCapaIdx;
								eKey = eDefKey;
								nPriorityClassIdx = nCurClassIdx;
								nPriorityCapaIdx = nCurCapaIdx;
								nPriorityDefaultIdx = nDefKeyIdx;
								return true; //---------------------------------
							}
						}
					}
				}
			}
		}
	}
	//assert(nMode == 5);
	if (nRandomCapaIdx < 0) {
		return false; //--------------------------------------------------------
	}
	auto eTempKey = findFreeKey(nRandomClassIdx, nRandomCapaIdx);
	if (eTempKey == stmi::HK_NULL) {
		return false; //--------------------------------------------------------
	}
	nClassIdx = nRandomClassIdx;
	nCapaIdx = nRandomCapaIdx;
	eKey = eTempKey;
	nPriorityClassIdx = nRandomClassIdx;
	nPriorityCapaIdx = nRandomCapaIdx;
	nPriorityDefaultIdx = -1;
	return true;
}
bool StdPreferences::capabilityKeyInUse(int32_t nClassIdx, int32_t nCapaIdx, stmi::HARDWARE_KEY eKey) const noexcept
{
//std::cout << "capabilityKeyInUse  nClassIdx=" << nClassIdx << "  nCapaIdx=" << nCapaIdx << "  eKey=" << static_cast<int32_t>(eKey) << '\n';
	auto& oCapabilityClassData = m_aCapabilityClassData[nClassIdx];
	auto& oCapabilityData = oCapabilityClassData.m_aCapabilityData[nCapaIdx];
	auto itFindDefa = oCapabilityData.m_oHKPlayerKeyAction.find(eKey);
	return (itFindDefa != oCapabilityData.m_oHKPlayerKeyAction.end());
}
stmi::HARDWARE_KEY StdPreferences::findFreeKey(int32_t nClassIdx, int32_t nCapaIdx) noexcept
{
	auto& oAllKeys = stmi::HardwareKeys::get();
	for (auto eKey : oAllKeys) {
		if (!capabilityKeyInUse(nClassIdx, nCapaIdx, eKey)) {
			return eKey;
		}
	}
	return stmi::HK_NULL;
}
shared_ptr<StdPreferences::Player> StdPreferences::playerAlloc() noexcept
{
	const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
	const int32_t nMaxPlayers = oAppConstraints.getMaxPlayers();

	static const std::string s_sPlayerNameBase = "Player";
	const int32_t nSizeOfMaxPlayerNr = static_cast<int32_t>(std::to_string(nMaxPlayers).size());
	const std::string sPlayerNameBase = s_sPlayerNameBase.substr(0, getMaxPlayerNameLength() - 1 - nSizeOfMaxPlayerNr);

	shared_ptr<Player> refFreePlayer;
	const NamedObjIndex< shared_ptr<Option> >& oOptions = m_refStdConfig->getOptions(OwnerType::PLAYER);
	const int32_t nTotOptions = oOptions.size();
	if (m_aFreePlayers.empty()) {
		refFreePlayer = std::make_shared<Player>();
		refFreePlayer->m_sName = sPlayerNameBase;
		// create options array
		refFreePlayer->m_aOption.resize(nTotOptions);
		//
		const int32_t nTotKeyActionIds = m_refStdConfig->getTotKeyActions();
		for (int32_t nKeyAction = 0; nKeyAction < nTotKeyActionIds; ++nKeyAction) {
			refFreePlayer->m_aKeyActionCapaHK.emplace_back(-1, -1, stmi::HK_NULL);
		}
	} else {
		refFreePlayer = m_aFreePlayers.back();
		m_aFreePlayers.pop_back();
	}
	// Make sure name is not in use, modify it if it is
	if (m_oNamedPlayers.find(refFreePlayer->m_sName) != m_oNamedPlayers.end()) {
		for (int32_t nTryPlayerNr = 1; nTryPlayerNr < nMaxPlayers; ++nTryPlayerNr) {
			const std::string sPlayerName = sPlayerNameBase + " " + std::to_string(nTryPlayerNr);
			if (m_oNamedPlayers.find(sPlayerName) == m_oNamedPlayers.end()) {
				refFreePlayer->m_sName = sPlayerName;
				break;
			}
		}
	}
	m_oNamedPlayers.emplace(refFreePlayer->m_sName, refFreePlayer);
	// Set options to default value
	refFreePlayer->m_nAIOptionIdx = -88;
	for (int32_t nIdx = 0; nIdx < nTotOptions; ++nIdx) {
		const shared_ptr<Option>& refOption = oOptions.getObj(nIdx);
		const Variant oDefault = refOption->getDefaultValue();
		refFreePlayer->m_aOption[nIdx] = oDefault;
		if (refOption->getName() == m_refStdConfig->getAIOptionName()) {
			refFreePlayer->m_nAIOptionIdx = nIdx;
			assert(oDefault.getType() == Variant::TYPE_BOOL);
			if (! oDefault.getBool()) {
				// human player: the key actions have to be populated
				m_bUndefinedKeyActions = true;
			}
		}
	}
	assert(refFreePlayer->m_nAIOptionIdx >= 0);
	return refFreePlayer;
}
void StdPreferences::playerFree(const shared_ptr<Player>& refPlayer) noexcept
{
	assert(refPlayer);
	refPlayer->m_p0Team = nullptr;
	refPlayer->m_nPlayer = -1;
	refPlayer->m_nMate = -1;
	for (auto& oPair : refPlayer->m_aCapabilities) {
		const int32_t nClassIdx = oPair.first;
		const int32_t nCapaIdx = oPair.second;
		assert(getCapabilityFromIdxs(nClassIdx, nCapaIdx) != nullptr);
		assert(m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_refPlayer == refPlayer);
		m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_refPlayer.reset();
	}
	refPlayer->m_aCapabilities.clear();
	//
	for (auto& oTuple : refPlayer->m_aKeyActionCapaHK) {
		int32_t& nCapaIdx = std::get<1>(oTuple);
		stmi::HARDWARE_KEY& eKey = std::get<2>(oTuple);
		if ((nCapaIdx >= 0) && (eKey != stmi::HK_NULL)) {
			int32_t& nClassIdx = std::get<0>(oTuple);
			// defined
			#ifndef NDEBUG
			auto nTotErased =
			#endif //NDEBUG
			m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_oHKPlayerKeyAction.erase(eKey);
			assert(nTotErased == 1);
			// reset
			nClassIdx = -1;
			nCapaIdx = -1;
			eKey = stmi::HK_NULL;
		}
	}
	m_oNamedPlayers.erase(refPlayer->m_sName);
	m_aFreePlayers.push_back(refPlayer);
}
void StdPreferences::playerAddToTeam(const shared_ptr<Player>& refPlayer, const shared_ptr<Team>& refTeam) noexcept
{
	assert(refPlayer);
	assert(refTeam);
	assert(refPlayer->m_p0Team == nullptr);
	const int32_t nTotTeammates = static_cast<int32_t>(refTeam->m_aTeammate.size());
	refTeam->m_aTeammate.push_back(refPlayer);

	refPlayer->m_nMate = nTotTeammates;
	refPlayer->m_nPlayer = -77; // set by recalcStuff())
	refPlayer->m_p0Team = refTeam.get();
	refPlayer->m_aKeyActionCapaHK.resize(m_refStdConfig->getTotKeyActions(), std::make_tuple(-1, -1, stmi::HK_NULL));
	// initialize options with default value
	auto& oOptions = m_refStdConfig->getOptions(OwnerType::PLAYER);
	const int32_t nTotOptions = oOptions.size();
	refPlayer->m_aOption.resize(nTotOptions);
	for (int32_t nIdx = 0; nIdx < nTotOptions; ++nIdx) {
		refPlayer->m_aOption[nIdx] = oOptions.getObj(nIdx)->getDefaultValue();
	}
}
void StdPreferences::playerRemoveFromTeam(const shared_ptr<Player>& refPlayer, const shared_ptr<Team>& refTeam) noexcept
{
	assert(refPlayer);
	assert(refTeam);
	assert(refPlayer->m_p0Team == refTeam.get());

	const int32_t nTeammate = refPlayer->m_nMate;
	const int32_t nTeam = refTeam->m_nTeam;
	const int32_t nTotTeammates = refTeam->getTotMates();

	// Remove from teammates
	m_aTeam[nTeam]->m_aTeammate.erase(m_aTeam[nTeam]->m_aTeammate.begin() + nTeammate);
	for (int32_t nNextTeammate = nTeammate; nNextTeammate < nTotTeammates - 1; ++nNextTeammate) {
		assert(m_aTeam[nTeam]->m_aTeammate[nNextTeammate]->m_nMate == nNextTeammate + 1);
		m_aTeam[nTeam]->m_aTeammate[nNextTeammate]->m_nMate = nNextTeammate;
	}
	refPlayer->m_p0Team = nullptr;
	refPlayer->m_nPlayer = -88;
	refPlayer->m_nMate = -1;
	// this function expects recalcStuff() to be called
	// because it invalidated m_aPlayerNrToPlayer
}
void StdPreferences::recalcStuff() noexcept
{
	assert(m_bEditMode);
	m_refDefaultPlayer.reset();
	//
	m_aPlayerNrToPlayer.clear();
	shared_ptr<Player> refFirstHumanPlayer;
	int32_t nTotHumans = 0;
	int32_t nPlayer = 0;
	for (auto& refTeam : m_aTeam) {
		for (auto& refMate : refTeam->m_aTeammate) {
			refMate->m_nPlayer = nPlayer;
			m_aPlayerNrToPlayer.push_back(refMate);
			if (!refMate->isAI()) {
				if (!refFirstHumanPlayer) {
					refFirstHumanPlayer = refMate;
				}
				++nTotHumans;
			}
			++nPlayer;
		}
	}
	if (m_refStdConfig->getCapabilityAssignment().m_bCapabilitiesAutoAssignedToActivePlayer) {
		if (nTotHumans == 1) {
			m_refDefaultPlayer = refFirstHumanPlayer;
		}
	}
	// Recalc tot assigned capabilities per class
	for (auto& oCapabilityClassData : m_aCapabilityClassData) {
		int32_t nTotAssignedCapas = 0;
		for (auto& oCapabilityData : oCapabilityClassData.m_aCapabilityData) {
			if (oCapabilityData.m_refPlayer) {
				++nTotAssignedCapas;
			}
		}
		oCapabilityClassData.m_nTotAssignedCapabilities = nTotAssignedCapas;
	}
}

int32_t StdPreferences::getMaxTeamNameLength() const noexcept
{
	return s_nMaxTeamNameLength;
}
int32_t StdPreferences::getMaxPlayerNameLength() const noexcept
{
	return s_nMaxPlayerNameLength;
}
int32_t StdPreferences::getTotTeams() const noexcept
{
	return static_cast<int32_t>(m_aTeam.size());
}
const shared_ptr<AppPreferences::PrefTeam> StdPreferences::getTeam(int32_t nTeam) const noexcept
{
	assert((nTeam >= 0) && (nTeam < getTotTeams()));
	return m_aTeam[nTeam];
}
const shared_ptr<StdPreferences::Team>& StdPreferences::getTeamFull(int32_t nTeam) const noexcept
{
	assert((nTeam >= 0) && (nTeam < getTotTeams()));
	return m_aTeam[nTeam];
}
const shared_ptr<AppPreferences::PrefTeam> StdPreferences::getTeam(const std::string& sTeamName) const noexcept
{
	return getTeamFull(sTeamName);
}
const shared_ptr<StdPreferences::Team>& StdPreferences::getTeamFull(const std::string& sTeamName) const noexcept
{
	static shared_ptr<StdPreferences::Team> s_refEmptyTeam{};
	auto itFind = m_oNamedTeams.find(sTeamName);
	if (itFind == m_oNamedTeams.end()) {
		return s_refEmptyTeam;
	}
	return itFind->second;
}
std::vector<std::string> StdPreferences::getTeamNames() const noexcept
{
	std::vector<std::string> aNames;
	for (auto& refTeam : m_aTeam) {
		aNames.push_back(refTeam->getName());
	}
	return aNames;
}
int32_t StdPreferences::getTotPlayers() const noexcept
{
	return static_cast<int32_t>(m_aPlayerNrToPlayer.size());
}
const shared_ptr<AppPreferences::PrefPlayer> StdPreferences::getPlayer(int32_t nPlayer) const noexcept
{
	assert((nPlayer >= 0) && (nPlayer < getTotPlayers()));
	return m_aPlayerNrToPlayer[nPlayer];
}
const shared_ptr<StdPreferences::Player>& StdPreferences::getPlayerFull(int32_t nPlayer) const noexcept
{
	assert((nPlayer >= 0) && (nPlayer < getTotPlayers()));
	return m_aPlayerNrToPlayer[nPlayer];
}
const shared_ptr<AppPreferences::PrefPlayer> StdPreferences::getPlayer(const std::string& sPlayerName) const noexcept
{
	return getPlayerFull(sPlayerName);
}
const shared_ptr<StdPreferences::Player>& StdPreferences::getPlayerFull(const std::string& sPlayerName) const noexcept
{
	static shared_ptr<StdPreferences::Player> s_refEmptyPlayer{};
	auto itFind = m_oNamedPlayers.find(sPlayerName);
	if (itFind == m_oNamedPlayers.end()) {
		return s_refEmptyPlayer;
	}
	return itFind->second;
}
void StdPreferences::getTeammate(int32_t nPlayer, int32_t& nTeam, int32_t& nMate) const noexcept
{
	assert((nPlayer >= 0) && (nPlayer < getTotPlayers()));
	const Player& oPlayer = *(m_aPlayerNrToPlayer[nPlayer]);
	nMate = oPlayer.getMate();
	nTeam = oPlayer.getTeam()->get();
}
 int32_t StdPreferences::getTotHumanPlayers() const noexcept
{
	return getTotPlayers() - getTotAIPlayers();
}
int32_t StdPreferences::getTotAIPlayers() const noexcept
{
	int32_t nTotAI = 0;
	for (auto& refTeam : m_aTeam) {
		for (auto& refMate : refTeam->m_aTeammate) {
			if (refMate->isAI()) {
				++nTotAI;
			}
		}
	}
	return nTotAI;
}
int32_t StdPreferences::getTotHumanTeams() const noexcept
{
	return getTotTeams() - getTotAITeams();
}
int32_t StdPreferences::getTotAITeams() const noexcept
{
	int32_t nTotAI = 0;
	for (auto& refTeam : m_aTeam) {
		if (refTeam->isAI()) {
			++nTotAI;
		}
	}
	return nTotAI;
}
bool StdPreferences::playerMoveUp(const std::string& sPlayerName) noexcept
{
//std::cout << "playerMoveUp() sPlayerName=" << sPlayerName << '\n';
	assert(m_bEditMode);
	std::unordered_map<std::string, shared_ptr<Player> >::const_iterator it = m_oNamedPlayers.find(sPlayerName);
	assert(it != m_oNamedPlayers.end());
	const shared_ptr<Player>& refPlayer = it->second;
	const int32_t nTeammate = refPlayer->m_nMate;
	assert(refPlayer->m_p0Team != nullptr);
	const int32_t nTeam = refPlayer->m_p0Team->m_nTeam;

	if (nTeammate > 0) {
		m_aTeam[nTeam]->m_aTeammate[nTeammate]->m_nMate = nTeammate - 1;
		m_aTeam[nTeam]->m_aTeammate[nTeammate - 1]->m_nMate = nTeammate;
		m_aTeam[nTeam]->m_aTeammate[nTeammate].swap(m_aTeam[nTeam]->m_aTeammate[nTeammate - 1]);
	} else if (nTeam > 0) {
		//return playerTeamUp(sPlayerName);
		const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
		auto& refPrecTeam = m_aTeam[nTeam - 1];
		auto& refCurTeam = m_aTeam[nTeam];
		const int32_t nPrecTotAIMates = refPrecTeam->getTotAIMates();
		const int32_t nPrecTotMates = refPrecTeam->getTotMates();
		const int32_t nCurTotAIMates = refCurTeam->getTotAIMates();
		const int32_t nCurTotMates = refCurTeam->getTotMates();
		const int32_t nLast = nPrecTotMates - 1;
		// following are references into the vectors!
		// (while refPlayer is a ref into m_oNamedPlayers!)
		auto& refPrecPlayer = refPrecTeam->m_aTeammate[nLast];
		auto& refCurPlayer = refCurTeam->m_aTeammate[0];
		const bool bPrecPlayerAI = refPrecPlayer->isAI();
		const bool bCurPlayerAI = refCurPlayer->isAI();
		if (bPrecPlayerAI != bCurPlayerAI) {
			// swap might violate some constraint
			if (!oAppConstraints.m_bAllowMixedAIHumanTeam) {
				assert((nPrecTotMates == nPrecTotAIMates) || (nPrecTotAIMates == 0));
				assert((nCurTotMates == nCurTotAIMates) || (nCurTotAIMates == 0));
				if ((nCurTotMates > 1) || (nPrecTotMates > 1)) {
					// !m_bAllowMixedAIHumanTeam would be violated
					return false; //--------------------------------------------
				}
			}
			if (bCurPlayerAI && (nPrecTotAIMates == oAppConstraints.m_nAIMatesPerTeamMax)) {
				return false; //--------------------------------------------
			} else if (bPrecPlayerAI && (nCurTotAIMates == oAppConstraints.m_nAIMatesPerTeamMax)) {
				return false; //--------------------------------------------
			}
		}
		std::swap(refCurPlayer->m_nMate, refPrecPlayer->m_nMate);
		std::swap(refCurPlayer->m_p0Team, refPrecPlayer->m_p0Team);
		refCurPlayer.swap(refPrecPlayer);
	} else {
		// already on top
		return false;
	}
	recalcStuff();
	return true;
}
bool StdPreferences::playerMoveDown(const std::string& sPlayerName) noexcept
{
//std::cout << "playerMoveDown() sPlayerName=" << sPlayerName << '\n';
	assert(m_bEditMode);
	std::unordered_map<std::string, shared_ptr<Player> >::const_iterator it = m_oNamedPlayers.find(sPlayerName);
	assert(it != m_oNamedPlayers.end());
	const shared_ptr<Player>& refPlayer = it->second;
	const int32_t nTeammate = refPlayer->m_nMate;
	assert(refPlayer->m_p0Team != nullptr);
	const int32_t nTeam = refPlayer->m_p0Team->m_nTeam;

	const int32_t nTotTeammates = static_cast<int32_t>(m_aTeam[nTeam]->m_aTeammate.size());
	const int32_t nTotTeams = static_cast<int32_t>(m_aTeam.size());
	if (nTeammate < nTotTeammates - 1) {
		m_aTeam[nTeam]->m_aTeammate[nTeammate]->m_nMate = nTeammate + 1;
		m_aTeam[nTeam]->m_aTeammate[nTeammate + 1]->m_nMate = nTeammate;
		m_aTeam[nTeam]->m_aTeammate[nTeammate].swap(m_aTeam[nTeam]->m_aTeammate[nTeammate + 1]);
	} else if (nTeam < nTotTeams - 1) {
		const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
		auto& refCurTeam = m_aTeam[nTeam];
		auto& refNextTeam = m_aTeam[nTeam + 1];
		const int32_t nCurTotAIMates = refCurTeam->getTotAIMates();
		const int32_t nCurTotMates = refCurTeam->getTotMates();
		const int32_t nLast = nTeammate;
		const int32_t nNextTotAIMates = refNextTeam->getTotAIMates();
		const int32_t nNextTotMates = refNextTeam->getTotMates();
		// following are references into the vectors!
		// (while refPlayer is a ref into m_oNamedPlayers!)
		auto& refNextPlayer = refNextTeam->m_aTeammate[0];
		auto& refCurPlayer = refCurTeam->m_aTeammate[nLast];
		const bool bCurPlayerAI = refCurPlayer->isAI();
		const bool bNextPlayerAI = refNextPlayer->isAI();
		if (bNextPlayerAI != bCurPlayerAI) {
			// swap might violate some constraint
			if (!oAppConstraints.m_bAllowMixedAIHumanTeam) {
				assert((nCurTotMates == nCurTotAIMates) || (nCurTotAIMates == 0));
				assert((nNextTotMates == nNextTotAIMates) || (nNextTotAIMates == 0));
				if ((nCurTotMates > 1) || (nNextTotMates > 1)) {
					// !m_bAllowMixedAIHumanTeam would be violated
					return false; //--------------------------------------------
				}
			}
			if (bCurPlayerAI && (nNextTotAIMates == oAppConstraints.m_nAIMatesPerTeamMax)) {
				return false; //--------------------------------------------
			} else if (bNextPlayerAI && (nCurTotAIMates == oAppConstraints.m_nAIMatesPerTeamMax)) {
				return false; //--------------------------------------------
			}
		}
		std::swap(refCurPlayer->m_nMate, refNextPlayer->m_nMate);
		std::swap(refCurPlayer->m_p0Team, refNextPlayer->m_p0Team);
		refCurPlayer.swap(refNextPlayer);
	} else {
		// already on bottom
		return false;
	}
	recalcStuff();
	return true;
}
bool StdPreferences::playerTeamUp(const std::string& sPlayerName) noexcept
{
//std::cout << "playerTeamUp() sPlayerName=" << sPlayerName << '\n';
	assert(m_bEditMode);
	std::unordered_map<std::string, shared_ptr<Player> >::const_iterator it = m_oNamedPlayers.find(sPlayerName);
	assert(it != m_oNamedPlayers.end());
	const shared_ptr<Player>& refPlayer = it->second;
	assert(refPlayer->m_p0Team != nullptr);
	const int32_t nTeam = refPlayer->m_p0Team->m_nTeam;

	if (nTeam == 0) {
		return false; // -------------------------------------------------------
	}

	const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
	const int32_t nMaxTeammates = oAppConstraints.getMaxTeammates();
	const int32_t nTotTeammates = static_cast<int32_t>(m_aTeam[nTeam]->m_aTeammate.size());
	#ifndef NDEBUG
	const int32_t nTotAITeammates = m_aTeam[nTeam]->getTotAIMates();
	#endif //NDEBUG
	if (!oAppConstraints.m_bAllowMixedAIHumanTeam) {
		assert((nTotTeammates == nTotAITeammates) || (nTotAITeammates == 0));
	}
	const int32_t nTotTeammatesPrec = static_cast<int32_t>(m_aTeam[nTeam - 1]->m_aTeammate.size());
	const int32_t nTotAITeammatesPrec = m_aTeam[nTeam - 1]->getTotAIMates();
	if (!oAppConstraints.m_bAllowMixedAIHumanTeam) {
		assert((nTotTeammatesPrec == nTotAITeammatesPrec) || (nTotAITeammatesPrec == 0));
	}
	if (! ((nTotTeammates > 1) && (nTotTeammatesPrec < nMaxTeammates)) ) {
		return false; // -------------------------------------------------------
	}
	const bool bAI = refPlayer->isAI();
	bool bInvertAI = false;
	if (bAI) {
		assert(oAppConstraints.m_nAIMatesPerTeamMax > 0);
		if (! oAppConstraints.m_bAllowMixedAIHumanTeam) {
			if (nTotAITeammatesPrec == 0) {
				// it would become mixed, change to Human
				bInvertAI = true;
			} else if (nTotAITeammatesPrec == oAppConstraints.m_nAIMatesPerTeamMax) {
				// cannot add because max reached and cannot convert to human
				// because mixed not allowed
				return false; //------------------------------------------------
			}
		} else {
			if (nTotAITeammatesPrec == oAppConstraints.m_nAIMatesPerTeamMax) {
				bInvertAI = true;
			}
		}
	} else {
		if (! oAppConstraints.m_bAllowMixedAIHumanTeam) {
			if (nTotAITeammatesPrec > 0) {
				// Would have to convert a human player to AI but
				// disallowed because information (key actions, etc.) would be lost
				return false; // -----------------------------------------------
			}
		}
	}
	playerRemoveFromTeam(refPlayer, m_aTeam[nTeam]);
	playerAddToTeam(refPlayer, m_aTeam[nTeam - 1]);
	if (bInvertAI) {
		#ifndef NDEBUG
		const bool bDone =
		#endif
		refPlayer->setAI(!bAI);
		assert(bDone);
//std::cout << "StdPreferences::playerTeamUp  populatePlayersKeyActions()" << '\n';
		m_bUndefinedKeyActions = true;
		populatePlayersKeyActions();
	}
	recalcStuff();
	return true;
}
bool StdPreferences::playerTeamDown(const std::string& sPlayerName) noexcept
{
//std::cout << "playerTeamDown() sPlayerName=" << sPlayerName << '\n';
	assert(m_bEditMode);
	std::unordered_map<std::string, shared_ptr<Player> >::const_iterator it = m_oNamedPlayers.find(sPlayerName);
	assert(it != m_oNamedPlayers.end());
	const shared_ptr<Player>& refPlayer = it->second;
	assert(refPlayer->m_p0Team != nullptr);
	const int32_t nTeam = refPlayer->m_p0Team->m_nTeam;

	const int32_t nTotTeams = static_cast<int32_t>(m_aTeam.size());
	if (nTeam == nTotTeams - 1) {
		return false; // -------------------------------------------------------
	}

	const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
	const int32_t nMaxTeammates = oAppConstraints.getMaxTeammates();
	const int32_t nTotTeammates = static_cast<int32_t>(m_aTeam[nTeam]->m_aTeammate.size());
	#ifndef NDEBUG
	const int32_t nTotAITeammates = m_aTeam[nTeam]->getTotAIMates();
	#endif //NDEBUG
	if (!oAppConstraints.m_bAllowMixedAIHumanTeam) {
		assert((nTotTeammates == nTotAITeammates) || (nTotAITeammates == 0));
	}
	const int32_t nTotTeammatesNext = static_cast<int32_t>(m_aTeam[nTeam + 1]->m_aTeammate.size());
	const int32_t nTotAITeammatesNext = m_aTeam[nTeam + 1]->getTotAIMates();
	if (!oAppConstraints.m_bAllowMixedAIHumanTeam) {
		assert((nTotTeammatesNext == nTotAITeammatesNext) || (nTotAITeammatesNext == 0));
	}
	if (! ((nTotTeammates > 1) && (nTotTeammatesNext < nMaxTeammates)) ) {
		return false; // ------------------------------------------
	}

	const bool bAI = refPlayer->isAI();
	bool bInvertAI = false;
	if (bAI) {
		assert(oAppConstraints.m_nAIMatesPerTeamMax > 0);
		if (! oAppConstraints.m_bAllowMixedAIHumanTeam) {
			if (nTotAITeammatesNext == 0) {
				// it would become mixed, change to Human
				bInvertAI = true;
			} else if (nTotAITeammatesNext == oAppConstraints.m_nAIMatesPerTeamMax) {
				// cannot add because max reached and cannot convert to human
				// because mixed not allowed
				return false; //------------------------------------------------
			}
		} else {
			if (nTotAITeammatesNext == oAppConstraints.m_nAIMatesPerTeamMax) {
				// convert to human
				bInvertAI = true;
			}
		}
	} else {
		if (! oAppConstraints.m_bAllowMixedAIHumanTeam) {
			if (nTotAITeammatesNext > 0) {
				// Would have to convert a human player to AI but
				// disallowed because information (key actions, etc.) would be lost
				return false; // -----------------------------------------------
			}
		}
	}
	playerRemoveFromTeam(refPlayer, m_aTeam[nTeam]);
	playerAddToTeam(refPlayer, m_aTeam[nTeam + 1]);
	if (bInvertAI) {
		#ifndef NDEBUG
		const bool bDone =
		#endif
		refPlayer->setAI(!bAI);
		assert(bDone);
		if (!bAI) {
			m_bUndefinedKeyActions = true;
			populatePlayersKeyActions();
		}
	}
	recalcStuff();
	return true;
}
bool StdPreferences::setTotTeams(int32_t nTotTeams) noexcept
{
	assert(m_bEditMode);
	assert(nTotTeams > 0);
	const int32_t nTargetTotTeams = nTotTeams;
	const int32_t nOldTotTeams = getTotTeams();
	if (nTotTeams == nOldTotTeams) {
		return true; //---------------------------------------------------------
	}
	const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
	const int32_t nMaxTeams = oAppConstraints.getMaxTeams();
	const int32_t nMaxPlayers = oAppConstraints.getMaxPlayers();
	nTotTeams = std::min(nTotTeams, nMaxTeams);

	if (nTotTeams > nOldTotTeams) {
		const int32_t nAddedTeams = nTotTeams - nOldTotTeams;
		const int32_t nTotPlayers = getTotPlayers();
		const int32_t nMissingNewPlayers = std::max(0, nTotPlayers + nAddedTeams - nMaxPlayers);
		const int32_t nNewPlayers = std::max(0, nAddedTeams - nMissingNewPlayers);
		// add the teams that don't need to steal players
		for (int32_t nPlayer = 0; nPlayer < nNewPlayers; ++nPlayer) {
			shared_ptr<Team> refTeam = teamAlloc();
			shared_ptr<Player> refPlayer = playerAlloc();
			playerAddToTeam(refPlayer, refTeam);
		}
		if (nMissingNewPlayers > 0) {
			// We have to steal players from existing teams and spread them
			// to the added teams.
			// We start from the last player of the last team
			int32_t nTeam = nOldTotTeams;
			int32_t nStolen = 0;
			while (nStolen < nMissingNewPlayers) {
				--nTeam;
				assert(nTeam >= 0);
				do {
					const int32_t nTotMates = static_cast<int32_t>(m_aTeam[nTeam]->m_aTeammate.size());
					if (nTotMates <= 1) {
						// We have to leave at least one mate in the team
						break; //do while
					}
					const int32_t nMate = nTotMates - 1;
					shared_ptr<Player> refPlayer = m_aTeam[nTeam]->m_aTeammate[nMate];
					playerRemoveFromTeam(refPlayer, m_aTeam[nTeam]);
					shared_ptr<Team> refTeam = teamAlloc();
					playerAddToTeam(refPlayer, refTeam);
					++nStolen;
				} while (nStolen < nMissingNewPlayers);
			}
		}
	} else {
		const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
		const int32_t nTeamsToDelete = ((nTotTeams < oAppConstraints.m_nTeamsMin) ? (nOldTotTeams - oAppConstraints.m_nTeamsMin) : (nOldTotTeams - nTotTeams));
		if (nTeamsToDelete <= 0) {
			// We already know there's no team we can remove
			return false; //----------------------------------------------------
		}
		for (int32_t nDeleted = 0; nDeleted < nTeamsToDelete; ++nDeleted) {
			const int32_t nLastTeam = static_cast<int32_t>(m_aTeam.size()) - 1;
			shared_ptr<Team> refTeam = m_aTeam[nLastTeam];
			teamFree(refTeam);
		}
	}
	populatePlayersKeyActions();
	recalcStuff();
	const bool bTargetNrOfTeamsReached = (nTargetTotTeams == getTotTeams());
	return bTargetNrOfTeamsReached;
}
bool StdPreferences::setTotPlayers(int32_t nPlayers) noexcept
{
	return setTotPlayers(nPlayers, -1);
}
bool StdPreferences::setTotPlayers(int32_t nTotPlayers, int32_t nForceTeam) noexcept
{
	assert(m_bEditMode);
	assert(nTotPlayers > 0);
	const int32_t nTargetTotPlayers = nTotPlayers;
	const int32_t nOldTotPlayers = getTotPlayers();
	if (nTargetTotPlayers == nOldTotPlayers) {
		return true; //---------------------------------------------------------
	}
	const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
	const int32_t nMaxTeams = oAppConstraints.getMaxTeams();
	const int32_t nMaxPlayers = oAppConstraints.getMaxPlayers();
	const int32_t nMaxTeammates = oAppConstraints.getMaxTeammates();
	nTotPlayers = std::min(nTotPlayers, nMaxPlayers);

	if (nTotPlayers > nOldTotPlayers) {
		int32_t nToAdd = nTotPlayers - nOldTotPlayers;
		while (nToAdd > 0) {
			int32_t nTargetTeam = -1;
			const int32_t nTotTeams = static_cast<int32_t>(m_aTeam.size());
			int32_t nFromTeam = nTotTeams - 1;
			int32_t nToTeam = 0;
			if (nForceTeam >= 0) {
				assert(nForceTeam < nTotTeams);
				nFromTeam = nForceTeam;
				nToTeam = nForceTeam;
			}
			for (int32_t nTeam = nFromTeam; nTeam >= nToTeam; --nTeam) {
				if (static_cast<int32_t>(m_aTeam[nTeam]->m_aTeammate.size()) == nMaxTeammates) {
					// There is no place
					continue; // for -----------------------------
				}
				// If the team is human we can certainly add a player
				// If the team is AI there might be limitations
				if (!oAppConstraints.m_bAllowMixedAIHumanTeam) {
					const bool bTeamIsAI = m_aTeam[nTeam]->isAI();
					if (bTeamIsAI) {
						// We can only add an AI player to this team
						const int32_t nTotAITeammates = m_aTeam[nTeam]->getTotAIMates();
						if (nTotAITeammates == oAppConstraints.m_nAIMatesPerTeamMax) {
							// But max AI players already reached! impossible to add here
							//TODO maybe could turn all the mates of the team to human!?
							continue; // for -----------------------------
						}
					}
				}
				nTargetTeam = nTeam;
				break; //for -----------------------------
			}
			if (nTargetTeam == -1) {
				if (nForceTeam >= 0) {
					break; // while ------------------------------------
				}
				// add team if possible
				if (nTotTeams == nMaxTeams) {
					// can't add a team, maximum reached!
					break; // while ------------------------------------
				}
				shared_ptr<Team> refNewTeam = teamAlloc();
				assert(refNewTeam->m_nTeam == nTotTeams);
				assert(nTotTeams < nMaxTeams);
				nTargetTeam = nTotTeams;
			}
			shared_ptr<Team> refTeam = m_aTeam[nTargetTeam];
			shared_ptr<Player> refPlayer = playerAlloc();
			playerAddToTeam(refPlayer, refTeam);
			--nToAdd;
		}
	} else {
		int32_t nToDelete = nOldTotPlayers - nTotPlayers;
		while (nToDelete > 0) {
			int32_t nTargetTeam = -1;
			const int32_t nTotTeams = static_cast<int32_t>(m_aTeam.size());
			int32_t nFromTeam = nTotTeams - 1;
			int32_t nToTeam = 0;
			if (nForceTeam >= 0) {
				assert(nForceTeam < nTotTeams);
				nFromTeam = nForceTeam;
				nToTeam = nForceTeam;
			}
			for (int32_t nTeam = nFromTeam; nTeam >= nToTeam; --nTeam) {
				if (m_aTeam[nTeam]->m_aTeammate.size() > 1) {
					nTargetTeam = nTeam;
					break; //for ------------------------------------
				}
			}
			if (nTargetTeam == -1) {
				// If the forced team is defined stop here, otherwise
				// at this point all teams contain just one player.
				// So to eliminate a player we also have to remove its team.
				if (nForceTeam >= 0) {
					break; // while ------------------------------------
				}
				if (nTotTeams == oAppConstraints.m_nTeamsMin) {
					// Cannot remove a team, minimum reached
					break; //while ------------------------------------
				}
				// Remove last team
				shared_ptr<Team> refTeam = m_aTeam[nTotTeams - 1];
				teamFree(refTeam);
			} else {
				// Remove the last mate of the team
				shared_ptr<Team>& refTeam = m_aTeam[nTargetTeam];
				const int32_t nTotTeammates = refTeam->getTotMates();
				shared_ptr<Player> refPlayer = m_aTeam[nTargetTeam]->m_aTeammate[nTotTeammates-1];
				playerRemoveFromTeam(refPlayer, refTeam);
				playerFree(refPlayer);
			}
			--nToDelete;
		}
	}
	populatePlayersKeyActions();
	recalcStuff();
	const bool bTargetNrOfPlayersReached = (nTargetTotPlayers == getTotPlayers());
	return bTargetNrOfPlayersReached;
}
bool StdPreferences::getPlayerKeyActionFromCapabilityKey(int32_t nCapabilityId, stmi::HARDWARE_KEY eKey
														, shared_ptr<PrefPlayer>& refPlayer, int32_t& nKeyActionId) const noexcept
{
	std::unordered_map<stmi::HARDWARE_KEY, std::pair< shared_ptr<Player>, int32_t > >::const_iterator itFindKey;
	const bool bFound = getPlayerKeyActionFromCapabilityKey(nCapabilityId, eKey, itFindKey);
	if (!bFound) {
		return false; //--------------------------------------------------------
	}
	auto& oPair = itFindKey->second;
	nKeyActionId = oPair.second;
	refPlayer = oPair.first;
	return true;
}
bool StdPreferences::getPlayerKeyActionFromCapabilityKey(int32_t nCapabilityId, stmi::HARDWARE_KEY eKey
														, int32_t& nTeam, int32_t& nMate, int32_t& nKeyActionId) const noexcept
{
	std::unordered_map<stmi::HARDWARE_KEY, std::pair< shared_ptr<Player>, int32_t > >::const_iterator itFindKey;
	const bool bFound = getPlayerKeyActionFromCapabilityKey(nCapabilityId, eKey, itFindKey);
	if (!bFound) {
		return false;
	}
	auto& oPair = itFindKey->second;
	nKeyActionId = oPair.second;
	auto& refPlayer = oPair.first;
	nMate = refPlayer->m_nMate;
	nTeam = refPlayer->m_p0Team->m_nTeam;
	return true;
}
bool StdPreferences::getPlayerKeyActionFromCapabilityKey(int32_t nCapabilityId, stmi::HARDWARE_KEY eKey
														, std::unordered_map<stmi::HARDWARE_KEY
														, std::pair< shared_ptr<Player>, int32_t > >::const_iterator& itFindKey) const noexcept
{
	assert(eKey != stmi::HK_NULL);
	int32_t nClassIdx;
	int32_t nCapaIdx;
	const bool bFoundCapa = getIdxsFromCapabilityId(nCapabilityId, nClassIdx, nCapaIdx);
	if (!bFoundCapa) {
		return false; //--------------------------------------------------------
	}
	auto& oHKPlayerKeyAction = m_aCapabilityClassData[nClassIdx].m_aCapabilityData[nCapaIdx].m_oHKPlayerKeyAction;
	itFindKey = oHKPlayerKeyAction.find(eKey);
	if (itFindKey == oHKPlayerKeyAction.end()) {
		return false; //--------------------------------------------------------
	}
	return true;
}
bool StdPreferences::getCapabilityPlayer(int32_t nCapabilityId
										, shared_ptr<PrefPlayer>& refPlayer) const noexcept
{
	const shared_ptr<Player>& refTempPlayer = getCapabilityPlayer(nCapabilityId);
	if (!refTempPlayer) {
		return false;
	}
	refPlayer = refTempPlayer;
	return true;
}
bool StdPreferences::getCapabilityPlayerFull(int32_t nCapabilityId, shared_ptr<Player>& refPlayer) const noexcept
{
	const shared_ptr<Player>& refTempPlayer = getCapabilityPlayer(nCapabilityId);
	if (!refTempPlayer) {
		return false;
	}
	refPlayer = refTempPlayer;
	return true;
}
bool StdPreferences::getCapabilityPlayer(int32_t nCapabilityId
										, int32_t& nTeam, int32_t& nMate) const noexcept
{
	const shared_ptr<StdPreferences::Player>& refPlayer = getCapabilityPlayer(nCapabilityId);
	if (!refPlayer) {
		return false;
	}
	nTeam = refPlayer->m_p0Team->m_nTeam;
	nMate = refPlayer->m_nMate;
	return true;
}
const shared_ptr<StdPreferences::Player>& StdPreferences::getCapabilityPlayer(int32_t nCapabilityId) const noexcept
{
	static const shared_ptr<StdPreferences::Player> s_refEmptyPlayer{};

	int32_t nClassIdx;
	int32_t nCapaIdx;
	const bool bFound = getIdxsFromCapabilityId(nCapabilityId, nClassIdx, nCapaIdx);
	if (!bFound) {
		// Non managed capability
		return s_refEmptyPlayer; //---------------------------------------------
	}
	auto& oCapabilityClassData = m_aCapabilityClassData[nClassIdx];
	auto& oCapabilityData = oCapabilityClassData.m_aCapabilityData[nCapaIdx];
	if (oCapabilityData.m_refPlayer) {
		return oCapabilityData.m_refPlayer; //----------------------------------
	}
	// The capability is not explicitly assigned to a player
	if (!m_refDefaultPlayer) {
		// No default player
		return s_refEmptyPlayer; //---------------------------------------------
	}
	const StdConfig::CapabilityAssignment& oCapabilityAssignment = m_refStdConfig->getCapabilityAssignment();
	if (oCapabilityAssignment.m_bCapabilitiesAutoAssignedToActivePlayer) {
		return s_refEmptyPlayer; //---------------------------------------------
	}
	if (oCapabilityAssignment.m_bMoreCapabilitiesPerClassAssignedToPlayer) {
		// All valid capabilities are automatically assigned to the default player
		return m_refDefaultPlayer; //-------------------------------------------
	}
	if (oCapabilityClassData.m_nTotAssignedCapabilities == 0) {
		// No capabilities are explicitly assigned to the default player
		// for this class.
		// The default is considered the capability with index 0.
		if (nCapaIdx == 0) {
			return m_refDefaultPlayer; //------------------------------------
		}
	}
	return s_refEmptyPlayer;
}
std::vector<stmi::Capability::Class> StdPreferences::getCapabilityClasses() const noexcept
{
	return m_aCapabilityClass;
}
int32_t StdPreferences::getCapabilityClassDeviceIdx(stmi::Capability const* p0Capability) const noexcept
{
	assert(p0Capability != nullptr);
	int32_t nClassIdx;
	int32_t nCapaIdx;
	const bool bFound = getIdxsFromCapability(p0Capability, nClassIdx, nCapaIdx);
	if (!bFound) {
		return -1; //-----------------------------------------------------------
	}
	return nCapaIdx;
}
shared_ptr<stmi::Capability> StdPreferences::getCapabilityFromClassDeviceIdx(const stmi::Capability::Class& oCapaClass, int32_t nDeviceIdx) const noexcept
{
	shared_ptr<stmi::Capability> refCapability;
	const int32_t nClassIdx = getCapabilityClassIdx(oCapaClass);
	if (nClassIdx < 0) {
		return shared_ptr<stmi::Capability>{}; //-------------------------------
	}
	const bool bFound = getCapabilityFromIdxs(nClassIdx, nDeviceIdx, refCapability);
	if (!bFound) {
		return shared_ptr<stmi::Capability>{}; //-------------------------------
	}
	return refCapability;
}
Variant StdPreferences::getOptionValue(const std::string& sOptionName) const noexcept
{
	const NamedObjIndex< shared_ptr<Option> >& oOptions = m_refStdConfig->getOptions(OwnerType::GAME);
	const int32_t nIdx = oOptions.getIndex(sOptionName);
	if (nIdx < 0) {
		return Variant{};
	}
	assert(nIdx < static_cast<int32_t>(m_aOption.size()));
	return m_aOption[nIdx];
}
bool StdPreferences::setOptionValue(const std::string& sOptionName, const Variant& oValue) noexcept
{
//std::cout << "StdPreferences::setOptionValue  sOptionName=" << sOptionName << '\n';
	const NamedObjIndex< shared_ptr<Option> >& oOptions = m_refStdConfig->getOptions(OwnerType::GAME);
	const int32_t nIdx = oOptions.getIndex(sOptionName);
	assert((nIdx >= 0) && (nIdx < static_cast<int32_t>(m_aOption.size())));
	auto& refOption = oOptions.getObj(nIdx);
	assert(!refOption->isReadonly());
	if (! refOption->isValidValue(oValue)) {
		return false; //--------------------------------------------------------
	}
	m_aOption[nIdx] = oValue;
	return true;
}

} // namespace stmg
