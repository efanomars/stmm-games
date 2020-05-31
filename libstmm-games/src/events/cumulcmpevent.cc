/*
 * File:   cumulcmpevent.cc
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

#include "events/cumulcmpevent.h"

#include <cassert>
#include <utility>
//#include <iostream>


namespace stmg
{

CumulCmpEvent::CumulCmpEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
, m_nLeft(m_oData.m_nInitialLeft)
, m_nRight(m_oData.m_nInitialRight)
{
}
void CumulCmpEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	m_nLeft = m_oData.m_nInitialLeft;
	m_nRight = m_oData.m_nInitialRight;
}

void CumulCmpEvent::trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept
{
//std::cout << "CumulCmpEvent::trigger  nMsg=" << nMsg << "  nValue=" << nValue << '\n';
	if (p0TriggeringEvent == nullptr) {
		return;
	}
	bool bCompare = false;
	//TODO check std::numeric_limits::min and max
	switch (nMsg) {
	case MESSAGE_CUMUL_COMPARE:
		{
			bCompare = true;
		}
		break;
	case MESSAGE_CUMUL_SET:
		{
			m_nLeft = nValue;
			m_nRight = nValue;
		}
		break;
	case MESSAGE_CUMUL_SET_LEFT:
		{
			m_nLeft = nValue;
		}
		break;
	case MESSAGE_CUMUL_SET_RIGHT:
		{
			m_nRight = nValue;
		}
		break;
	case MESSAGE_CUMUL_SET_LEFT_COMPARE:
		{
			m_nLeft = nValue;
			bCompare = true;
		}
		break;
	case MESSAGE_CUMUL_SET_RIGHT_COMPARE:
		{
			m_nRight = nValue;
			bCompare = true;
		}
		break;
	case MESSAGE_CUMUL_ADD_TO:
		{
			m_nLeft += nValue;
			m_nRight += nValue;
		}
		break;
	case MESSAGE_CUMUL_ADD_TO_LEFT:
		{
			m_nLeft += nValue;
		}
		break;
	case MESSAGE_CUMUL_ADD_TO_RIGHT:
		{
			m_nRight += nValue;
		}
		break;
	case MESSAGE_CUMUL_ADD_TO_LEFT_COMPARE:
		{
			m_nLeft += nValue;
			bCompare = true;
		}
		break;
	case MESSAGE_CUMUL_ADD_TO_RIGHT_COMPARE:
		{
			m_nRight += nValue;
			bCompare = true;
		}
		break;
	case MESSAGE_CUMUL_MULT:
		{
			m_nLeft = m_nLeft * nValue;
			m_nRight = m_nRight * nValue;
		}
		break;
	case MESSAGE_CUMUL_MULT_LEFT:
		{
			m_nLeft = m_nLeft * nValue;
		}
		break;
	case MESSAGE_CUMUL_MULT_RIGHT:
		{
			m_nRight = m_nRight * nValue;
		}
		break;
	case MESSAGE_CUMUL_MULT_LEFT_COMPARE:
		{
			m_nLeft = m_nLeft * nValue;
			bCompare = true;
		}
		break;
	case MESSAGE_CUMUL_MULT_RIGHT_COMPARE:
		{
			m_nRight = m_nRight * nValue;
			bCompare = true;
		}
		break;
	case MESSAGE_CUMUL_DIV:
		{
			m_nLeft = ((nValue == 0) ? 0 : m_nLeft / nValue);
			m_nRight = ((nValue == 0) ? 0 : m_nRight / nValue);
		}
		break;
	case MESSAGE_CUMUL_DIV_LEFT:
		{
			m_nLeft = ((nValue == 0) ? 0 : m_nLeft / nValue);
		}
		break;
	case MESSAGE_CUMUL_DIV_RIGHT:
		{
			m_nRight = ((nValue == 0) ? 0 : m_nRight / nValue);
		}
		break;
	case MESSAGE_CUMUL_DIV_LEFT_COMPARE:
		{
			m_nLeft = ((nValue == 0) ? 0 : m_nLeft / nValue);
			bCompare = true;
		}
		break;
	case MESSAGE_CUMUL_DIV_RIGHT_COMPARE:
		{
			m_nRight = ((nValue == 0) ? 0 : m_nRight / nValue);
			bCompare = true;
		}
		break;
	case MESSAGE_CUMUL_RESET:
		{
			m_nLeft = 0;
			m_nRight = 0;
			return; //----------------------------------------------------------
		}
		break;
	case MESSAGE_CUMUL_RESET_LEFT:
		{
			m_nLeft = 0;
			return; //----------------------------------------------------------
		}
		break;
	case MESSAGE_CUMUL_RESET_RIGHT:
		{
			m_nRight = 0;
			return; //----------------------------------------------------------
		}
		break;
	case MESSAGE_CUMUL_SET_TO_INITIAL:
		{
			m_nLeft = m_oData.m_nInitialLeft;
			m_nRight = m_oData.m_nInitialRight;
			return; //----------------------------------------------------------
		}
		break;
	case MESSAGE_CUMUL_SET_TO_INITIAL_LEFT:
		{
			m_nLeft = m_oData.m_nInitialLeft;
			return; //----------------------------------------------------------
		}
		break;
	case MESSAGE_CUMUL_SET_TO_INITIAL_RIGHT:
		{
			m_nRight = m_oData.m_nInitialRight;
			return; //----------------------------------------------------------
		}
		break;
	default:
		{
			return; //----------------------------------------------------------
		}
		break;
	}
	const int32_t nLeft = m_nLeft;
	const int32_t nRight = m_nRight;
	if (bCompare) {
		switch (m_oData.m_eOnCompareType) {
		case ON_COMPARE_RESET_BOTH:
			{
				m_nLeft = 0;
				m_nRight = 0;
			}
			break;
		case ON_COMPARE_RESET_LEFT:
			{
				m_nLeft = 0;
			}
			break;
		case ON_COMPARE_RESET_RIGHT:
			{
				m_nRight = 0;
			}
			break;
		case ON_COMPARE_SET_TO_INITIAL_BOTH:
			{
				m_nLeft = 0;
				m_nRight = 0;
			}
			break;
		case ON_COMPARE_SET_TO_INITIAL_LEFT:
			{
				m_nLeft = 0;
			}
			break;
		case ON_COMPARE_SET_TO_INITIAL_RIGHT:
			{
				m_nRight = 0;
			}
			break;
		case ON_COMPARE_COPY_LEFT_TO_RIGHT:
			{
				m_nRight = m_nLeft;
			}
			break;
		case ON_COMPARE_COPY_RIGHT_TO_LEFT:
			{
				m_nLeft = m_nRight;
			}
			break;
		case ON_COMPARE_UNCHANGED:
			{
			}
			break;
		default:
			{
				assert(false);
			}
		}
//std::cout << "CumulCmpEvent::trigger  LISTENER_GROUP_COMPARED nLeft=" << nLeft << '\n';
//std::cout << "CumulCmpEvent::trigger  LISTENER_GROUP_COMPARED nRight=" << nRight << '\n';
		const int32_t nDiff = nLeft - nRight;
		if (nDiff == 0) {
			informListeners(LISTENER_GROUP_COMPARED_EQUAL, nLeft);
		} else {
			informListeners(LISTENER_GROUP_COMPARED_NOT_EQUAL, nDiff);
		}
		informListeners(LISTENER_GROUP_COMPARED, nDiff);
	}
}

} // namespace stmg
