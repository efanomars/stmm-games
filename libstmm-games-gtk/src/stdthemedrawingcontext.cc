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
 * File:   stdthemedrawingcontext.cc
 */

#include "stdthemedrawingcontext.h"
#include "stdthemecontext.h"
#include "stdtheme.h"

#include <stmm-games/util/basictypes.h>

#include <cassert>

namespace stmg
{

StdThemeDrawingContext::StdThemeDrawingContext(Theme::RuntimeVariablesEnv* p0RuntimeVariablesEnv) noexcept
: m_p1Owner(nullptr)
, m_p0RuntimeVariablesEnv(p0RuntimeVariablesEnv)
, m_p0SelectedImage(nullptr)
{
}
void StdThemeDrawingContext::reInit(Theme::RuntimeVariablesEnv* p0RuntimeVariablesEnv) noexcept
{
	m_p1Owner = nullptr;
	m_p0RuntimeVariablesEnv = p0RuntimeVariablesEnv;
	m_p0SelectedImage = nullptr;
	m_aStdThemeNameIdxToRuntimeId.clear();
}
NSize StdThemeDrawingContext::getTileSize() const noexcept
{
	return m_p1Owner->getTileSize();
}
const Glib::RefPtr<Pango::Context>& StdThemeDrawingContext::getFontContext() noexcept
{
	return m_p1Owner->getFontContext();
}

Image* StdThemeDrawingContext::getSelectedImage() noexcept
{
	return m_p0SelectedImage;
}
void StdThemeDrawingContext::setSelectedImage(Image* p0Image) noexcept
{
	m_p0SelectedImage = p0Image;
}
std::pair<bool, int32_t> StdThemeDrawingContext::getVariableValue(int32_t nVarId) noexcept
{
	assert(nVarId >= 0);
	if (nVarId >= static_cast<int32_t>(m_aStdThemeNameIdxToRuntimeId.size())) {
		StdTheme* p0StdTheme = m_p1Owner->m_p1Owner;
		#ifndef NDEBUG
		const int32_t nTotNames = p0StdTheme->getTotVariableIndexes();
		assert(nVarId < nTotNames);
		#endif //NDEBUG
		const std::string& sVarName = p0StdTheme->getVariableNameFromIndex(nVarId);
		m_aStdThemeNameIdxToRuntimeId.resize(nVarId + 1, -1);
		if (m_p0RuntimeVariablesEnv == nullptr) {
			return std::make_pair(false, -1); //--------------------------------
		}
		m_aStdThemeNameIdxToRuntimeId[nVarId] = m_p0RuntimeVariablesEnv->getVariableIdFromName(sVarName);
	}
	const int32_t& nVarIdx = m_aStdThemeNameIdxToRuntimeId[nVarId];
	if (nVarIdx < 0) {
		return std::make_pair(false, -1);
	}
	return std::make_pair(true, m_p0RuntimeVariablesEnv->getVariableValue(nVarIdx));
}

} // namespace stmg
