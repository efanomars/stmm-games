/*
 * File:   tileselector.h
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

#ifndef STMG_TILE_SELECTOR_H
#define STMG_TILE_SELECTOR_H

#include "traitset.h"
#include "util/intset.h"

#include <memory>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg { class Tile; }

namespace stmg
{

using std::unique_ptr;

class TileSelector
{
public:
	class Operand;
	/** Constructor.
	 * @param refOperand The expression. Cannot be null.
	 */
	explicit TileSelector(unique_ptr<Operand> refOperand) noexcept;
	/** Empty constructor.
	 * The select() functions will always return false.
	 */
	TileSelector() noexcept = default;

	/** Whether the tile is selected by this instance.
	 * @param oTile The tile.
	 * @return Whether tile selected.
	 */
	bool select(const Tile& oTile) const noexcept;
	/** Whether the tile and skin are selected by this instance.
	 * @param oTile The tile.
	 * @param nSkin The skin.
	 * @return Whether tile selected.
	 */
	bool select(const Tile& oTile, int32_t nSkin) const noexcept;

	enum OPERAND_TYPE
	{
		OPERAND_TYPE_OPERATOR = 0
		, OPERAND_TYPE_TRAIT = 1
		, OPERAND_TYPE_SKIN = 2
	};
	//
	class Operand
	{
		friend class TileSelector;
	public:
		void dump(int32_t nIndentSpaces) const noexcept;
	protected:
		explicit Operand(OPERAND_TYPE eOperandType) noexcept;
		bool eval(const Tile& oTile, int32_t nSkin) const noexcept;
	private:
		OPERAND_TYPE m_eOperandType = OPERAND_TYPE_OPERATOR;
	};
	//
	class Skin : public Operand
	{
		friend class Operand;
	public:
		Skin(bool bComplement, unique_ptr<IntSet> refIntSet) noexcept;
		explicit Skin(unique_ptr<IntSet> refIntSet) noexcept;
		Skin& operator=(Skin&& oSource) noexcept
		{
			m_bComp = std::move(oSource.m_bComp);
			m_refIntSet = std::move(oSource.m_refIntSet);
			Operand::operator=(std::move(oSource));
			return *this;
		}
		void dump(int32_t nIndentSpaces) const noexcept;
	protected:
		bool eval(const Tile& oTile, int32_t nSkin) const noexcept;
	private:
		bool m_bComp;
		unique_ptr<IntSet> m_refIntSet;
	private:
		Skin() = delete;
		Skin(const Skin& oSource) = delete;
		Skin& operator=(const Skin& oSource) = delete;
	};
	//
	class Trait : public Operand
	{
		friend class Operand;
	public:
		Trait(bool bComplement, unique_ptr<TraitSet> refTraitSet) noexcept;
		explicit Trait(unique_ptr<TraitSet> refTraitSet) noexcept;
		Trait& operator=(Trait&& oSource) noexcept
		{
			m_bComp = std::move(oSource.m_bComp);
			m_refTraitSet = std::move(oSource.m_refTraitSet);
			Operand::operator=(std::move(oSource));
			return *this;
		}
		void dump(int32_t nIndentSpaces) const noexcept;
	protected:
		bool eval(const Tile& oTile, int32_t nSkin) const noexcept;
	private:
		bool m_bComp;
		unique_ptr<TraitSet> m_refTraitSet;
	private:
		Trait() = delete;
		Trait(const Trait& oSource) = delete;
		Trait& operator=(const Trait& oSource) = delete;
	};
	//
	class Operator : public Operand
	{
		friend class Operand;
	public:
		enum OP_TYPE {
			OP_TYPE_FIRST = 0
			, OP_TYPE_OR = 0
			, OP_TYPE_AND = 1
			, OP_TYPE_NOT = 2
			, OP_TYPE_LAST = 2
		};
		Operator(OP_TYPE eOperatorType, std::unique_ptr<Operand> refOperand) noexcept;
		Operator(OP_TYPE eOperatorType, std::unique_ptr<Operand> refOperand1, std::unique_ptr<Operand> refOperand2) noexcept;
		Operator(OP_TYPE eOperatorType, std::vector< std::unique_ptr<Operand> >& aOperands) noexcept;
		Operator& operator=(Operator&& oSource)
		{
			//*this = std::move(oSource);
			m_eType = std::move(oSource.m_eType);
			m_aOperands = std::move(oSource.m_aOperands);
			Operand::operator=(std::move(oSource));
			return *this;
		}
		inline int32_t totOperands() const noexcept { return static_cast<int32_t>(m_aOperands.size()); };
		void dump(int32_t nIndentSpaces) const noexcept;
	protected:
		bool eval(const Tile& oTile, int32_t nSkin) const noexcept;
	private:
		OP_TYPE m_eType;
		std::vector< unique_ptr<Operand> > m_aOperands;
	private:
		Operator() = delete;
		Operator(const Operator& oSource) = delete;
		Operator& operator=(const Operator& oSource) = delete;
	};

	#ifndef NDEBUG
	void dump(int32_t nIndentSpaces) const noexcept;
	#endif //NDEBUG
private:

	static inline bool xOr(bool b1, bool b2) noexcept { return (b1 && !b2) || (b2 && !b1); };
private:

	unique_ptr<Operand> m_refRoot;

	static constexpr int32_t s_nAnySkin = -1;
private:
	TileSelector(const TileSelector& oSource) = delete;
	TileSelector& operator=(const TileSelector& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_TILE_SELECTOR_H */

