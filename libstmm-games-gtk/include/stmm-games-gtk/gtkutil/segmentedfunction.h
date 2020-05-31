/*
 * File:  segmentedfunction.h
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

#ifndef STMG_SEGMENTED_FUNCTION_H
#define STMG_SEGMENTED_FUNCTION_H

#include <vector>

#include <stdint.h>


namespace stmg
{

/** Segmented function: concatenation of "touching" segments.
 * The function is limited to:
 *
 *     Segment: fA * fX + fB, fFromX   with fA >= 0.0, fB >= 0.0, fFromX >= 0, fX >= 0.
 *
 * This helper class can be used to calculate layouts.
 */
class SegmentedFunction
{
public:
	/** Empty constructor.
	 * Calls SegmentedFunction(0.0, 0.0)
	 */
	SegmentedFunction() noexcept;
	/** Constructor.
	 * Creates a one segment function with segment `y = fA * x + fB`.
	 * Since this class doesn't check overflows `fA` and `fB` shouldn't be
	 * too big.
	 * @param fA Must be &gt;= 0.0.
	 * @param fB Must be &gt;= 0.0.
	 */
	SegmentedFunction(double fA, double fB) noexcept;

	/** Evaluate the function for a given x.
	 * Beware! No overflow check is performed so don't try it with too big or too small numbers.
	 * @param fX Must be &gt;= 0.0.
	 * @return The result (fY) of the function. Always &gt;= 0.0.
	 */
	double eval(double fX) const noexcept;
	/** Evaluate the inverse for a given y.
	 * @param fY Must be &gt;= 0.0.
	 * @return The highest value of fX for which `eval(fX) == fY`, or -1.0 if there's no inverse.
	 */
	double evalInverse(double fY) const noexcept;

	/** Multiply function by a factor.
	 * @param fFactor Must be &gt;= 0.0.
	 */
	void mul(double fFactor) noexcept;

	/** Calculate sum of two functions.
	 * @param oF1 First function.
	 * @param oF2 Second function.
	 * @return Result function.
	 */
	static SegmentedFunction add(const SegmentedFunction& oF1, const SegmentedFunction& oF2) noexcept;
	/** Calculate maximum of two functions.
	 * @param oF1 First function.
	 * @param oF2 Second function.
	 * @return Result function.
	 */
	static SegmentedFunction max(const SegmentedFunction& oF1, const SegmentedFunction& oF2) noexcept;

	inline bool operator==(const SegmentedFunction& oSF) const noexcept
	{
		return (m_aSegments == oSF.m_aSegments);
	}

#ifndef NDEBUG
	void dump(int32_t nIndentSpaces) const noexcept;
	void dump() const noexcept { dump(0); }
#endif //NDEBUG
private:
	explicit SegmentedFunction(bool bCreateZeroSegment) noexcept;

private:
	class Segment
	{
	public:
		Segment(double fFromX, double fA, double fB) noexcept;
		inline double getFromX() const noexcept { return m_fFromX; }
		inline double getA() const noexcept { return m_fA; }
		inline double getB() const noexcept { return m_fB; }
		inline bool operator==(const Segment& oS) const noexcept
		{
			return (m_fFromX == oS.m_fFromX) && (m_fA == oS.m_fA) && (m_fB == oS.m_fB);
		}
	private:
		friend class SegmentedFunction;

		double m_fFromX;
		double m_fA;
		double m_fB;
	private:
		Segment() = delete;
	};

	std::vector<Segment> m_aSegments;

	static const double s_fInfinity;
};

} // namespace stmg

#endif	/* STMG_SEGMENTED_FUNCTION_H */

