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
 * File:   prefselector.h
 */

#ifndef STMG_PREF_SELECTOR_H
#define STMG_PREF_SELECTOR_H

#include "util/variantset.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg { class AppPreferences; }

namespace stmg
{

using std::unique_ptr;

class PrefSelector
{
public:
	class Operand;
	/** Constructor.
	 * @param refOperand The expression. Cannot be null.
	 */
	explicit PrefSelector(unique_ptr<Operand> refOperand) noexcept;
	/** Empty constructor.
	 * The select() functions will always return false.
	 */
	PrefSelector() noexcept = default;

	class Trait;

	/** Whether the preferences is selected by this instance.
	 * @param oPrefs The preferences.
	 * @return Whether tile selected.
	 */
	bool select(const AppPreferences& oPrefs) const noexcept;

	//
	class Operand
	{
		friend class PrefSelector;
	public:
		virtual ~Operand() noexcept = default;
		virtual void dump(int32_t nIndentSpaces) const noexcept = 0;
	protected:
		Operand() noexcept = default;
		virtual bool eval(const AppPreferences& oPrefs) const noexcept = 0;
	};
	//
	class OptionCond : public Operand
	{
	public:
		OptionCond(bool bComplement, std::string&& sGameOptionName, VariantSet&& oVariantSet) noexcept;
		OptionCond(std::string&& sGameOptionName, VariantSet&& oVariantSet) noexcept;
		OptionCond& operator=(OptionCond&& oSource) noexcept = default;
		void dump(int32_t nIndentSpaces) const noexcept override;
	protected:
		bool eval(const AppPreferences& oPrefs) const noexcept override;
	private:
		bool m_bComp;
		std::string m_sGameOptionName;
		VariantSet m_oVariantSet;
	private:
		OptionCond() = delete;
		OptionCond(const OptionCond& oSource) = delete;
		OptionCond& operator=(const OptionCond& oSource) = delete;
	};
	//
	class Operator : public Operand
	{
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
		Operator(OP_TYPE eOperatorType, std::vector< std::unique_ptr<Operand> >&& aOperands) noexcept;
		Operator& operator=(Operator&& oSource)
		{
			//*this = std::move(oSource);
			m_eType = std::move(oSource.m_eType);
			m_aOperands = std::move(oSource.m_aOperands);
			Operand::operator=(std::move(oSource));
			return *this;
		}
		inline int32_t totOperands() const noexcept { return static_cast<int32_t>(m_aOperands.size()); };
		void dump(int32_t nIndentSpaces) const noexcept override;
	protected:
		bool eval(const AppPreferences& oPrefs) const noexcept override;
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
	PrefSelector(const PrefSelector& oSource) = delete;
	PrefSelector& operator=(const PrefSelector& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_PREF_SELECTOR_H */

