/*
 * File:   ccsave.h
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

#ifndef STMG_CC_SAVE_H
#define STMG_CC_SAVE_H

#include <cairomm/context.h>
#include <cairomm/refptr.h>

namespace stmg
{

/** RAII save and restore for a Cairo::Context.
 */
class CCSave
{
public:
	/** Constructor.
	 * Saves the context.
	 * @param oCC The cairo context.
	 */
	explicit CCSave(Cairo::Context& oCC) noexcept
	: m_oCC(oCC)
	{
		m_oCC.save();
	}
	/** Constructor.
	 * Saves the context.
	 * @param refCC The cairo context. Cannot be null.
	 */
	explicit CCSave(const Cairo::RefPtr<Cairo::Context>& refCC) noexcept
	: m_oCC(*(refCC.operator->()))
	{
		m_oCC.save();
	}
	/** Destructor.
	 * Restores the context.
	 */
	~CCSave() noexcept
	{
		m_oCC.restore();
	}
private:
	Cairo::Context& m_oCC;
private:
	CCSave() = delete;
	CCSave(const CCSave& oSource) = delete;
	CCSave(CCSave&& oSource) = delete;
	CCSave& operator=(const CCSave& oSource) = delete;
	CCSave& operator=(CCSave&& oSource) = delete;
};


} // namespace stmg

#endif	/* STMG_CC_SAVE_H */

