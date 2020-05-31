/*
 * File:   prefselector.cc
 *
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

#include "prefselector.h"

#include "apppreferences.h"
#include "util/variant.h"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace stmg
{

PrefSelector::PrefSelector(unique_ptr<Operand> refOperand) noexcept
: m_refRoot((assert(refOperand), std::move(refOperand)))
{
}

bool PrefSelector::select(const AppPreferences& oPrefs) const noexcept
{
	return (m_refRoot ? m_refRoot->eval(oPrefs) : false);
}
bool PrefSelector::Operator::eval(const AppPreferences& oPrefs) const noexcept
{
//std::cout << "PrefSelector::Operator::eval()" << '\n';
	// no operands means OR(is at least one true): false, AND(are all of them true): true, NOT: error !!!
	if (m_eType == Operator::OP_TYPE_OR) {
		for (auto& refOperand : m_aOperands) {
			if (refOperand->eval(oPrefs)) {
				return true;
			}
		}
		return false;
	} else if (m_eType == Operator::OP_TYPE_AND) {
		for (auto& refOperand : m_aOperands) {
			if (!refOperand->eval(oPrefs)) {
				return false;
			}
		}
		return true;
	} else if (m_eType == Operator::OP_TYPE_NOT) {
		assert(m_aOperands.size() == 1);
		const auto& refOperand = m_aOperands.front();
		return ! refOperand->eval(oPrefs);
	} else {
		assert(false);
	}
	return false;
}
bool PrefSelector::OptionCond::eval(const AppPreferences& oPrefs) const noexcept
{
//std::cout << "PrefSelector::Trait::eval()" << '\n';
	const Variant oValue = oPrefs.getOptionValue(m_sGameOptionName);
	const bool bSelected = m_oVariantSet.contains(oValue);
	return xOr(m_bComp, bSelected);
}

PrefSelector::OptionCond::OptionCond(bool bComplement, std::string&& sGameOptionName, VariantSet&& oVariantSet) noexcept
: m_bComp(bComplement)
, m_sGameOptionName(std::move(sGameOptionName))
, m_oVariantSet(std::move(oVariantSet))
{
	assert(! m_sGameOptionName.empty());
}
PrefSelector::OptionCond::OptionCond(std::string&& sGameOptionName, VariantSet&& oVariantSet) noexcept
: OptionCond(false, std::move(sGameOptionName), std::move(oVariantSet))
{
}

PrefSelector::Operator::Operator(OP_TYPE eOperatorType, std::unique_ptr<Operand> refOperand) noexcept
: m_eType(eOperatorType)
{
	assert((eOperatorType >= OP_TYPE_FIRST) && (eOperatorType <= OP_TYPE_LAST));
	assert(refOperand);
	m_aOperands.push_back(std::move(refOperand));
}
PrefSelector::Operator::Operator(OP_TYPE eOperatorType, std::unique_ptr<Operand> refOperand1, std::unique_ptr<Operand> refOperand2) noexcept
: m_eType(eOperatorType)
{
	assert((eOperatorType >= OP_TYPE_FIRST) && (eOperatorType <= OP_TYPE_LAST));
	assert(refOperand1);
	assert(refOperand2);
	m_aOperands.push_back(std::move(refOperand1));
	m_aOperands.push_back(std::move(refOperand2));
}
PrefSelector::Operator::Operator(OP_TYPE eOperatorType, std::vector< std::unique_ptr<Operand> >&& aOperands) noexcept
: m_eType(eOperatorType)
{
	assert((eOperatorType >= OP_TYPE_FIRST) && (eOperatorType <= OP_TYPE_LAST));
	assert(! aOperands.empty());
	m_aOperands = std::move(aOperands);
	#ifndef NDEBUG
	for (const auto& refOperand : m_aOperands) {
		assert(refOperand);
	}
	if (m_eType == Operator::OP_TYPE_NOT) {
		assert(m_aOperands.size() == 1);
	}
	#endif //NDEBUG
}
void PrefSelector::Operator::dump(int32_t nIndentSpaces) const noexcept
{
	static_cast<void>(nIndentSpaces);
	#ifndef NDEBUG
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << [&]()
	{
		if (m_eType == OP_TYPE_OR) {
			return "OR";
		} else if (m_eType == OP_TYPE_AND) {
			return "AND";
		} else {
			assert(m_eType == OP_TYPE_NOT);
			return "NOT";
		}
	}() << '\n';
	for (const auto& refOperand : m_aOperands) {
		refOperand->dump(nIndentSpaces + 2);
	}
	#endif //NDEBUG
}
void PrefSelector::OptionCond::dump(int32_t nIndentSpaces) const noexcept
{
	static_cast<void>(nIndentSpaces);
	#ifndef NDEBUG
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "Game Option " << m_sGameOptionName;
	if (m_bComp) {
		std::cout << sIndent << "NOT ";
	}
	std::cout << "in" << '\n';
	m_oVariantSet.dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}
#ifndef NDEBUG
void PrefSelector::dump(int32_t nIndentSpaces) const noexcept
{
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "PrefSelector::dump()" << '\n';
	if (m_refRoot) {
		m_refRoot->dump(nIndentSpaces + 2);
	} else {
		std::cout << sIndent << "  (Empty)" << '\n';
	}
}
#endif //NDEBUG

} // namespace stmg
