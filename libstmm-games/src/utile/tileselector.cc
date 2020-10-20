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
 * File:   tileselector.cc
 */

#include "utile/tileselector.h"

#include <cassert>
#include <iostream>
#include <string>

namespace stmg { class Tile; }

namespace stmg
{

TileSelector::Operand::Operand(OPERAND_TYPE eOperandType) noexcept
: m_eOperandType(eOperandType)
{
}
void TileSelector::Operand::dump(int32_t nIndentSpaces) const noexcept
{
	switch (m_eOperandType) {
	case OPERAND_TYPE_OPERATOR:
		static_cast<TileSelector::Operator const*>(this)->dump(nIndentSpaces);
		break;
	case OPERAND_TYPE_TRAIT:
		static_cast<TileSelector::Trait const*>(this)->dump(nIndentSpaces);
		break;
	case OPERAND_TYPE_SKIN:
		static_cast<TileSelector::Skin const*>(this)->dump(nIndentSpaces);
		break;
	default:
		assert(false);
	}
}

bool TileSelector::Operand::eval(const Tile& oTile, int32_t nSkin) const noexcept
{
	switch (m_eOperandType) {
	case OPERAND_TYPE_OPERATOR:
		return static_cast<TileSelector::Operator const*>(this)->eval(oTile, nSkin);
		break;
	case OPERAND_TYPE_TRAIT:
		return static_cast<TileSelector::Trait const*>(this)->eval(oTile, nSkin);
		break;
	case OPERAND_TYPE_SKIN:
		return static_cast<TileSelector::Skin const*>(this)->eval(oTile, nSkin);
		break;
	default:
		assert(false);
		return false;
	}
}

TileSelector::TileSelector(unique_ptr<Operand> refOperand) noexcept
: m_refRoot((assert(refOperand), std::move(refOperand)))
{
}

bool TileSelector::select(const Tile& oTile) const noexcept
{
	return (m_refRoot ? m_refRoot->eval(oTile, s_nAnySkin) : false);
}
bool TileSelector::select(const Tile& oTile, int32_t nSkin) const noexcept
{
	assert((nSkin >= 0) || (nSkin == s_nAnySkin));
	return (m_refRoot ? m_refRoot->eval(oTile, nSkin) : false);
}
bool TileSelector::Operator::eval(const Tile& oTile, int32_t nSkin) const noexcept
{
//std::cout << "TileSelector::Operator::eval()" << '\n';
	// no operands means OR(is at least one true): false, AND(are all of them true): true, NOT: error !!!
	if (m_eType == Operator::OP_TYPE_OR) {
		for (auto& refOperand : m_aOperands) {
			if (refOperand->eval(oTile, nSkin)) {
				return true;
			}
		}
		return false;
	} else if (m_eType == Operator::OP_TYPE_AND) {
		for (auto& refOperand : m_aOperands) {
			if (!refOperand->eval(oTile, nSkin)) {
				return false;
			}
		}
		return true;
	} else if (m_eType == Operator::OP_TYPE_NOT) {
		assert(m_aOperands.size() == 1);
		const auto& refOperand = m_aOperands.front();
		return ! refOperand->eval(oTile, nSkin);
	} else {
		assert(false);
	}
	return false;
}
bool TileSelector::Trait::eval(const Tile& oTile, int32_t /*nSkin*/) const noexcept
{
//std::cout << "TileSelector::Trait::eval()" << '\n';
	const int32_t nIdx = m_refTraitSet->getIndexOfTileTraitValue(oTile);
	const bool bSelected = (nIdx >= -1);
	return xOr(m_bComp, bSelected);
}
bool TileSelector::Skin::eval(const Tile& /*oTile*/, int32_t nSkin) const noexcept
{
	if (nSkin == s_nAnySkin) {
		return true; //---------------------------------------------------------
	}
	const int32_t nIdx = m_refIntSet->getIndexOfValue(nSkin);
	const bool bSelected = (nIdx >= 0);
	return xOr(m_bComp, bSelected);
}


TileSelector::Trait::Trait(bool bComplement, unique_ptr<TraitSet> refTraitSet) noexcept
: Operand(OPERAND_TYPE_TRAIT)
, m_bComp(bComplement)
, m_refTraitSet((assert(refTraitSet), std::move(refTraitSet)))
{
}
TileSelector::Trait::Trait(unique_ptr<TraitSet> refTraitSet) noexcept
: Trait(false, std::move(refTraitSet))
{
}

TileSelector::Skin::Skin(bool bComplement, unique_ptr<IntSet> refIntSet) noexcept
: Operand(OPERAND_TYPE_SKIN)
, m_bComp(bComplement)
, m_refIntSet((assert(refIntSet), std::move(refIntSet)))
{
}
TileSelector::Skin::Skin(unique_ptr<IntSet> refIntSet) noexcept
: Skin(false, std::move(refIntSet))
{
}

TileSelector::Operator::Operator(OP_TYPE eOperatorType, std::unique_ptr<Operand> refOperand) noexcept
: Operand(OPERAND_TYPE_OPERATOR)
, m_eType(eOperatorType)
{
	assert((eOperatorType >= OP_TYPE_FIRST) && (eOperatorType <= OP_TYPE_LAST));
	assert(refOperand);
	m_aOperands.push_back(std::move(refOperand));
}
TileSelector::Operator::Operator(OP_TYPE eOperatorType, std::unique_ptr<Operand> refOperand1, std::unique_ptr<Operand> refOperand2) noexcept
: Operand(OPERAND_TYPE_OPERATOR)
, m_eType(eOperatorType)
{
	assert((eOperatorType >= OP_TYPE_FIRST) && (eOperatorType <= OP_TYPE_LAST));
	assert(refOperand1);
	assert(refOperand2);
	m_aOperands.push_back(std::move(refOperand1));
	m_aOperands.push_back(std::move(refOperand2));
}
TileSelector::Operator::Operator(OP_TYPE eOperatorType, std::vector< std::unique_ptr<Operand> >& aOperands) noexcept
: Operand(OPERAND_TYPE_OPERATOR)
, m_eType(eOperatorType)
{
	assert((eOperatorType >= OP_TYPE_FIRST) && (eOperatorType <= OP_TYPE_LAST));
	assert(! aOperands.empty());
	m_aOperands.swap(aOperands);
	#ifndef NDEBUG
	for (const auto& refOperand : m_aOperands) {
		assert(refOperand);
	}
	if (m_eType == Operator::OP_TYPE_NOT) {
		assert(m_aOperands.size() == 1);
	}
	#endif //NDEBUG
}
void TileSelector::Operator::dump(int32_t nIndentSpaces) const noexcept
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
void TileSelector::Trait::dump(int32_t nIndentSpaces) const noexcept
{
	static_cast<void>(nIndentSpaces);
	#ifndef NDEBUG
	if (m_bComp) {
		auto sIndent = std::string(nIndentSpaces, ' ');
		std::cout << sIndent << "Trait NOT" << '\n';
		nIndentSpaces += 2;
	}
	m_refTraitSet->dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}
void TileSelector::Skin::dump(int32_t nIndentSpaces) const noexcept
{
	static_cast<void>(nIndentSpaces);
	#ifndef NDEBUG
	if (m_bComp) {
		auto sIndent = std::string(nIndentSpaces, ' ');
		std::cout << sIndent << "Skin NOT" << '\n';
		nIndentSpaces += 2;
	}
	m_refIntSet->dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}
#ifndef NDEBUG
void TileSelector::dump(int32_t nIndentSpaces) const noexcept
{
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "TileSelector::dump()" << '\n';
	if (m_refRoot) {
		m_refRoot->dump(nIndentSpaces + 2);
	} else {
		std::cout << sIndent << "  (Empty)" << '\n';
	}
}
#endif //NDEBUG

} // namespace stmg
