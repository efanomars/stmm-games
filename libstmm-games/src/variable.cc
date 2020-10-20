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
 * File:   variable.cc
 */

#include "variable.h"

#include "game.h"

#include "util/namedobjindex.h"
#include "util/util.h"

//#include <iostream>
#include <cassert>
#include <memory>

namespace stmg
{

Variable::Variable(const VariableType* p0VarType, Game* p0Game) noexcept
: m_p0VarType(p0VarType)
, m_nValue((assert(p0VarType != nullptr), p0VarType->m_nInitialValue))
, m_nLastChangeTime(0)
, m_nInhibitTimeMillisec(-1)
, m_p0Game(p0Game)
{
	assert(p0VarType != nullptr);
	assert(p0Game != nullptr);
}

int32_t Variable::calcElapsed() const noexcept
{
	int32_t nElapsed = m_nInhibitTimeMillisec;
	if (m_nInhibitTimeMillisec < 0) {
		nElapsed = static_cast<int32_t>(m_p0Game->gameElapsedMillisec());
		assert(nElapsed >= 0);
	}
	if (m_p0VarType->m_eTimeBase == VARIABLE_TIME_BASE_MIN) {
		nElapsed = nElapsed / 60 / 1000;
	} else if (m_p0VarType->m_eTimeBase == VARIABLE_TIME_BASE_SEC) {
		nElapsed = nElapsed / 1000;
	}
	if (m_p0VarType->m_bAddTime) {
		return nElapsed;
	} else {
		return -nElapsed;
	}
}
int32_t Variable::get() const noexcept
{
//std::cout << "Variable(" << reinterpret_cast<int64_t>(this) << ")::get m_nValue=" << m_nValue << '\n';
	int32_t nValue = m_nValue;
	if (m_p0VarType->m_bTimeRelative) {
		const int32_t nElapsed = calcElapsed();
		nValue += nElapsed;
	}
//std::cout << "Variable::get nValue=" << nValue << "  initial value was=" << m_p0VarType->m_nInitialValue << '\n';
	return nValue;
}
void Variable::set(int32_t nValue) noexcept
{
//std::cout << "Variable(" << reinterpret_cast<int64_t>(this) << ")::set m_nValue=" << m_nValue << "  gameElapsed=" << m_p0Game->gameElapsed() << '\n';
	assert(m_p0Game != nullptr);
	assert(m_p0Game->isInGameTick());
	assert(m_p0VarType != nullptr);
	if (m_p0VarType->m_bReadOnly) {
		return;
	}
	if (m_nInhibitTimeMillisec >= 0) {
		return; //--------------------------------------------------------------
	}
//std::cout << "Variable::set nValue=" << nValue << "  m_nLastChangeTime=" << m_nLastChangeTime << '\n';
	if (m_p0VarType->m_bTimeRelative) {
		const int32_t nElapsed = calcElapsed();
		nValue -= nElapsed;
	}
	m_nValue = nValue;
	m_nLastChangeTime = m_p0Game->gameElapsed();
}
void Variable::forceSet(int32_t nValue) noexcept
{
	assert(m_p0Game != nullptr);
	m_nValue = nValue;
	m_nLastChangeTime = m_p0Game->gameElapsed();
}
void Variable::inc(int32_t nInc) noexcept
{
	set(m_nValue + nInc);
}
void Variable::inhibit() noexcept
{
	assert(m_p0Game != nullptr);
	if (m_nInhibitTimeMillisec >= 0) {
		return; //--------------------------------------------------------------
	}
	m_nInhibitTimeMillisec = m_p0Game->gameElapsedMillisec();
}
bool Variable::isChanged() const noexcept
{
	assert(m_p0VarType != nullptr);
	if (m_p0VarType->m_bTimeRelative) {
		return true; //---------------------------------------------------------
	}
	assert(m_p0Game != nullptr);
	const int32_t nElapsed = m_p0Game->gameElapsed();
	if (m_p0Game->isInGameTick()) {
		return (nElapsed <= m_nLastChangeTime);
	} else {
		// asking from a view tick, => elapsed ticks was incremented
		return (nElapsed - 1 <= m_nLastChangeTime);
	}
}
std::string variableValueToString(int32_t nValue, Variable::VARIABLE_FORMAT eFormat) noexcept
{
	switch (eFormat) {
		case Variable::VARIABLE_FORMAT_NUMBER:
		{
			return std::to_string(nValue);
		}
		case Variable::VARIABLE_FORMAT_MILL:
		{
			return Util::intToMillString(nValue);
		}
		case Variable::VARIABLE_FORMAT_MIN_SEC:
		{
			return Util::secToMinSecString(nValue);
		}
		case Variable::VARIABLE_FORMAT_MIN_SEC_MILLISEC:
		{
			return Util::millisecToMinSecString(nValue);
		}
		default:
		{
			assert(false);
			return Util::s_sEmptyString;
		}
	}
}
std::string Variable::toFormattedString() const noexcept
{
	assert(m_p0VarType != nullptr);
	const int32_t nValue = get();
	return variableValueToString(nValue, m_p0VarType->m_eFormat);
}

Variable::Value Variable::getValue() const noexcept
{
	Value oValue;
	oValue.m_nValue = get();
	oValue.m_eFormat = m_p0VarType->m_eFormat;
	return oValue;
}
std::string Variable::Value::toFormattedString() const noexcept
{
	return variableValueToString(m_nValue, m_eFormat);
}
Variable::Value Variable::Value::create(int32_t nValue, VARIABLE_FORMAT eFormat) noexcept
{
	Variable::Value oVal;
	oVal.m_nValue = nValue;
	oVal.m_eFormat = eFormat;
	assert(oVal.isValid());
	return oVal;
}
bool Variable::Value::isValid() const noexcept
{
	return ((m_eFormat == VARIABLE_FORMAT_NUMBER) || (m_eFormat == VARIABLE_FORMAT_MILL)
			|| (m_eFormat == VARIABLE_FORMAT_MIN_SEC) || (m_eFormat == VARIABLE_FORMAT_MIN_SEC_MILLISEC));
}
void Variable::Variables::init(const NamedObjIndex<VariableType>& oTypes, Game* p0Game) noexcept
{
	assert(p0Game != nullptr);
	const int32_t nTotIds = oTypes.size();
	if (nTotIds > static_cast<int32_t>(m_aVar.capacity())) {
		m_aVar.reserve(nTotIds);
	}
	m_aVar.clear();
	for (int32_t nId = 0; nId < nTotIds; ++nId) {
		const VariableType& oType = oTypes.getObj(nId);
		m_aVar.emplace_back(&oType, p0Game);
		m_aVar.back().m_nValue = oType.m_nInitialValue;
	}
}
void Variable::Variables::inhibit() noexcept
{
	for (auto& oVar : m_aVar) {
		oVar.inhibit();
	}
}
Variable& Variable::Variables::getVariable(int32_t nId) noexcept
{
	assert((nId >= 0) && (nId < static_cast<int32_t>(m_aVar.size())));
	return m_aVar[nId];
}
const Variable& Variable::Variables::getVariable(int32_t nId) const noexcept
{
	assert((nId >= 0) && (nId < static_cast<int32_t>(m_aVar.size())));
	return m_aVar[nId];
}
int32_t Variable::Variables::getTotVariables() const noexcept
{
	return static_cast<int32_t>(m_aVar.size());
}

} // namespace stmg
