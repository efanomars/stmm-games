/*
 * File:   segmentedfunction.cc
 *
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

#include "gtkutil/segmentedfunction.h"

#include <algorithm>
#include <string>
#include <limits>
#include <iostream>
#include <cassert>

namespace stmg
{

const double SegmentedFunction::s_fInfinity = std::numeric_limits<double>::max() / 2;

SegmentedFunction::SegmentedFunction() noexcept
: SegmentedFunction(true)
{
}
/** Constructor.
 * Creates a one segment function with segment `y = fA * x + fB`.
 * Since this class doesn't check overflows `fA` and `fB` shouldn't be
 * too big.
 * @param fA Must be \>= 0.0.
 * @param fB Must be \>= 0.0.
 */
SegmentedFunction::SegmentedFunction(double fA, double fB) noexcept
{
	assert(fA >= 0.0);
	assert(fB >= 0.0);
	m_aSegments.emplace_back(0.0, fA, fB);
}
SegmentedFunction::SegmentedFunction(bool bCreateZeroSegment) noexcept
{
	if (bCreateZeroSegment) {
		m_aSegments.emplace_back(0.0, 0.0, 0.0);
	}
}

double SegmentedFunction::eval(double fX) const noexcept
{
	auto it = m_aSegments.cbegin();
	std::vector<Segment>::const_iterator itLast;
	do {
		itLast = it;
		++it;
	} while ((it != m_aSegments.end()) && (fX >= it->m_fFromX));
	return itLast->m_fA * fX + itLast->m_fB;
}
double SegmentedFunction::evalInverse(double fY) const noexcept
{
	bool bExists = false;
	double fX = -1.0;
	auto it = m_aSegments.begin();
	assert(it != m_aSegments.end());
	while (true) {
		auto itNext = it;
		++itNext;
		double fToNX;
		const bool bEnd = (itNext == m_aSegments.end());
		if (bEnd) {
			fToNX = s_fInfinity;
		} else {
			fToNX = itNext->getFromX();
		}
		const double fFromX = it->getFromX();
		const double fA = it->getA();
		const double fB = it->getB();
		if (fA == 0.0) {
			if (fY == fB) {
				fX = fToNX;
				bExists = true;
			}
		} else {
			const double fNewX = (fY - fB) / fA;
			const bool bNewExists = ((fNewX >= fFromX) && (fNewX < fToNX));
			if (bExists) {
				if (bNewExists) {
					assert(fNewX >= fX);
					fX = fNewX;
				} else {
					// since eval(fX + fD) >= eval(fX) for all fD > 0.0
					break; // while -----
				}
			} else {
				if (bNewExists) {
					fX = fNewX;
					bExists = true;
				}
			}
		}
		if (bEnd) {
			break; // while -----
		}
		++it;
	}
	if (bExists && (fX >= 0.0)) {
		return fX;
	} else {
		return -1.0;
	}
}

SegmentedFunction SegmentedFunction::add(const SegmentedFunction& oF1, const SegmentedFunction& oF2) noexcept
{
	SegmentedFunction oFR{false};

	auto it1 = oF1.m_aSegments.begin();
	auto it2 = oF2.m_aSegments.begin();

	double fFromX = 0.0;
	double fLastFromX = std::numeric_limits<double>::lowest();

	assert(it1->getFromX() == 0.0);
	assert(it2->getFromX() == 0.0);

	while (true) {
		if (fFromX != fLastFromX) {
			oFR.m_aSegments.emplace_back(fFromX, it1->getA() + it2->getA(), it1->getB() + it2->getB());
//std::cout << "fFromX=" << fFromX << " fLastFromX=" << fLastFromX << ":  it1->getA()=" << it1->getA() << " it1->getB()=" << it1->getB() << "   it2->getA()=" << it2->getA() << " it2->getB()=" << it2->getB() << '\n';
		}
		fLastFromX = fFromX;

		auto it1Next = it1;
		++it1Next;
		auto it2Next = it2;
		++it2Next;

		const bool bEnd1 = (it1Next == oF1.m_aSegments.end());
		const bool bEnd2 = (it2Next == oF2.m_aSegments.end());
		if (bEnd1 && bEnd2) {
			break; // while -------------------------------------
		} else if (bEnd2) {
			fFromX = it1Next->getFromX();
			it1 = it1Next;
		} else if (bEnd1) {
			fFromX = it2Next->getFromX();
			it2 = it2Next;
		} else {
			const double fFromX1 = it1Next->getFromX();
			const double fFromX2 = it2Next->getFromX();
			if (fFromX1 < fFromX2) {
				fFromX = fFromX1;
				it1 = it1Next;
			} else if (fFromX1 > fFromX2) {
				fFromX = fFromX2;
				it2 = it2Next;
			} else {
				fFromX = fFromX1;
				it1 = it1Next;
				it2 = it2Next;
			}
		}
	}
	assert(oFR.m_aSegments.size() >= std::max(oF1.m_aSegments.size(), oF2.m_aSegments.size()));
	//TODO simplify if possible: if two segments are the same line fuse them in one segment
	return oFR;
}
SegmentedFunction SegmentedFunction::max(const SegmentedFunction& oF1, const SegmentedFunction& oF2) noexcept
{
	SegmentedFunction oFR{false};

	auto it1 = oF1.m_aSegments.begin();
	auto it2 = oF2.m_aSegments.begin();

	double fFromX = 0.0;

	assert(it1->getFromX() == 0.0);
	assert(it2->getFromX() == 0.0);

	do {
		auto it1Next = it1;
		++it1Next;
		auto it2Next = it2;
		++it2Next;
		const bool bEnd1 = (it1Next == oF1.m_aSegments.end());
		const bool bEnd2 = (it2Next == oF2.m_aSegments.end());
		double fToNX;
		if (bEnd1 && bEnd2) {
			fToNX = s_fInfinity;
		} else if (bEnd1) {
			fToNX = it2Next->getFromX();
			it1Next = it1;
		} else if (bEnd2) {
			fToNX = it1Next->getFromX();
			it2Next = it2;
		} else {
			const double fFromX1 = it1Next->getFromX();
			const double fFromX2 = it2Next->getFromX();
			if (fFromX1 < fFromX2) {
				fToNX = fFromX1;
				it2Next = it2;
			} else {
				fToNX = fFromX2;
				if (fFromX1 > fFromX2) {
					it1Next = it1;
				}
			}
		}
//std::cout << "  SF::max fFromX=" << fFromX << " fToNX=" << fToNX << '\n';
		const double fA1 = it1->getA();
		const double fA2 = it2->getA();
		const double fB1 = it1->getB();
		const double fB2 = it2->getB();
//std::cout << "  SF::max fA1=" << fA1 << " fB1=" << fB1 << "    fA2=" << fA2 << " fB2=" << fB2 << '\n';
		if (fA1 == fA2) {
			if (fB1 >= fB2) {
				oFR.m_aSegments.emplace_back(fFromX, fA1, fB1);
			} else {
				oFR.m_aSegments.emplace_back(fFromX, fA2, fB2);
			}
		} else {
			const double fIntersectX = (fB2 - fB1) / (fA1 - fA2);
//std::cout << "  SF::max fIntersectX=" << fIntersectX << '\n';
			if ((fIntersectX > fFromX) && (fIntersectX < fToNX)) {
				// create 2 segments
				if (fA1 < fA2) {
					oFR.m_aSegments.emplace_back(fFromX, fA1, fB1);
					if (fFromX != fIntersectX) {
						oFR.m_aSegments.emplace_back(fIntersectX, fA2, fB2);
					}
				} else {
					oFR.m_aSegments.emplace_back(fFromX, fA2, fB2);
					if (fFromX != fIntersectX) {
						oFR.m_aSegments.emplace_back(fIntersectX, fA1, fB1);
					}
				}
			} else {
				// no intersection within segment
				if (fIntersectX <= fFromX) {
					if (fA1 > fA2) {
						oFR.m_aSegments.emplace_back(fFromX, fA1, fB1);
					} else {
						oFR.m_aSegments.emplace_back(fFromX, fA2, fB2);
					}
				} else { // (fIntersectX >= fToNX)
					if (fA1 <= fA2) {
						oFR.m_aSegments.emplace_back(fFromX, fA1, fB1);
					} else {
						oFR.m_aSegments.emplace_back(fFromX, fA2, fB2);
					}
				}
			}
		}
		if (fToNX >= s_fInfinity) {
			break; //do ---------------------
		}
		fFromX = fToNX;
		it1 = it1Next;
		it2 = it2Next;
	} while (true);

	return oFR;
}
void SegmentedFunction::mul(double fFactor) noexcept
{
	assert(fFactor >= 0.0);
	if (fFactor == 0.0) {
		*this = SegmentedFunction{true};
		return; //--------------------------------------------------------------
	}
	for (auto& oSegment : m_aSegments) {
		oSegment.m_fA = oSegment.m_fA * fFactor;
		oSegment.m_fB = oSegment.m_fB * fFactor;
	}
}
#ifndef NDEBUG
void SegmentedFunction::dump(int32_t nIndentSpaces) const noexcept
{
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "SegmentedFunction addr: " << reinterpret_cast<int64_t>(this) << "  (" << m_aSegments.size() << " segments)" << '\n';
	for (auto& oSegment : m_aSegments) {
		std::cout << sIndent << "  fromX=" << oSegment.m_fFromX << "  A=" << oSegment.m_fA << "  B=" << oSegment.m_fB << '\n';
	}
}
#endif //NDEBUG

SegmentedFunction::Segment::Segment(double fFromX, double fA, double fB) noexcept
: m_fFromX(fFromX)
, m_fA(fA)
, m_fB(fB)
{
	assert(fFromX >= 0.0);
	assert(fA >= 0.0);
	assert(fB >= 0.0);
}

} // namespace stmg
