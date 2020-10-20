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
 * File:   option.cc
 */

#include "option.h"

#include <cassert>

#include <stdint.h>

namespace stmg
{

using MastersValues = std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>;

Option::Option(OwnerType eOwnerType, const std::string& sName, const Variant& oDefaultValue, const std::string& sDesc) noexcept
: Option(eOwnerType, sName, oDefaultValue, sDesc, false, true, {}, {})
{
}
Option::Option(OwnerType eOwnerType, const std::string& sName, const Variant& oDefaultValue, const std::string& sDesc
				, bool bReadOnly, bool bVisible
				, const shared_ptr<Option>& refMaster, const std::vector<Variant>& aMasterValues) noexcept
: Option(eOwnerType, sName, oDefaultValue, sDesc, bReadOnly, bVisible, (refMaster ? MastersValues{std::make_pair(refMaster, aMasterValues)} : MastersValues{}))
{
	if (! refMaster) {
		assert(aMasterValues.empty());
	}
}
Option::Option(OwnerType eOwnerType, const std::string& sName, const Variant& oDefaultValue, const std::string& sDesc
				, bool bReadOnly, bool bVisible
				, const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>& aMastersValues) noexcept
: m_eOwnerType(eOwnerType)
, m_sName(sName)
, m_oDefaultValue(oDefaultValue)
, m_sDesc(sDesc)
, m_bReadOnly(bReadOnly)
, m_bVisible(bVisible)
, m_aMastersValues(aMastersValues)
{
	assert((eOwnerType == OwnerType::GAME) || (eOwnerType == OwnerType::TEAM) || (eOwnerType == OwnerType::PLAYER));
	assert( ! sName.empty());
	#ifndef NDEBUG
	// check sName only contains a-zA-Z0-9 characters
	const int32_t nNameSize = static_cast<int32_t>(sName.size());
	for (int32_t nIdx = 0; nIdx < nNameSize; ++nIdx) {
		auto c = sName[nIdx];
		assert((c == '_') || ((c >= '0') && (c >= '9')) || ((c >= 'a') && (c >= 'z')) || ((c >= 'A') && (c >= 'Z')));
	}
	assert(! oDefaultValue.isNull());
	for (auto& oPair : m_aMastersValues) {
		// It's a slave option
		auto& refMaster = oPair.first;
		assert(refMaster);
		//assert(refMaster->m_bVisible); // The master of a slave option must be visible
		assert(refMaster->m_aMastersValues.empty()); // An option can't be both a master and a slave
		// The master's owner type must be superior to the owner type of the slave.
		if (eOwnerType == OwnerType::GAME) {
			assert(refMaster->m_eOwnerType == OwnerType::GAME);
		} else if (eOwnerType == OwnerType::TEAM) {
			assert((refMaster->m_eOwnerType == OwnerType::GAME) || (refMaster->m_eOwnerType == OwnerType::TEAM));
		}
		auto& aMasterValues = oPair.second;
		const int32_t nTotMasterValues = static_cast<int32_t>(aMasterValues.size());
		assert(nTotMasterValues > 0); // There must be a value of the master for which the slave is enabled
		for (int32_t nIdx = 0; nIdx < nTotMasterValues; ++nIdx) {
			auto& oVariant = aMasterValues[nIdx];
			assert(refMaster->isValidValue(oVariant)); // The value of the master is of the wrong type or out of range
			for (int32_t nIdx2 = nIdx + 1; nIdx2 < nTotMasterValues; ++nIdx2) {
				auto& oVariant2 = aMasterValues[nIdx2];
				assert(! (oVariant2 == oVariant)); // no duplicates allowed
			}
		}
	}
	#endif //NDEBUG
}

} // namespace stmg
