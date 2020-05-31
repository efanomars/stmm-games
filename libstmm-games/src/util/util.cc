/*
 * File:   util.cc
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

#include "util/util.h"

#include <string>
//#include <iostream>
#include <cassert>

#include <ctype.h>
#include <stddef.h>


namespace stmg
{

namespace Util
{

namespace Private
{
void assertTrue(bool
								#ifndef NDEBUG
								bCond
								#endif //NDEBUG
								)
{
	assert(bCond);
}

std::string stringComposeSobst(const std::string& sFormat, const std::vector<std::string>& aParams) noexcept
{
	const int32_t nTotParams = static_cast<int32_t>(aParams.size());
	std::string sResult(sFormat);
	std::string::size_type nFindPos = 0;
	while (true) {
		nFindPos = sResult.find("%", nFindPos);
		if (nFindPos == std::string::npos) {
			break;
		}
		const auto nSize = sResult.size();
		if (nFindPos == nSize - 1) {
			break;
		}
		const auto cNr = sResult[nFindPos + 1];
		if (cNr == '%') {
			sResult.replace(nFindPos, 1, "");
			++nFindPos;
		} else if ((cNr >= '1') && (cNr <= '9')) {
			const int32_t nIdx = static_cast<int32_t>(cNr) - static_cast<int32_t>('1');
			if (nIdx < nTotParams) {
				const auto& sParam = aParams[nIdx];
				sResult.replace(nFindPos, 2, sParam);
				nFindPos += sParam.size();
			} else {
				nFindPos += 2;
			}
		} else {
			nFindPos += 2;
		}
	}
	return sResult;
}
} // namespace Private

std::string strStrip(const std::string& sStr) noexcept
{
	std::string sRes;
	std::string::const_iterator it1 = sStr.begin();
	do {
		if (it1 == sStr.end()) {
			return sRes;
		}
		if (! std::isspace(*it1)) {
			break;
		}
		++it1;
	} while (true);
	std::string::const_iterator it2 = sStr.end();
	do {
		--it2;
		if (! std::isspace(*it2)) {
			break;
		}
	} while (true);

	do {
		sRes.append(1, *it1);
		if (it1 == it2) {
			break;
		}
		++it1;
	} while (true);
	return sRes;
}
bool strContainsWhitespace(const std::string& sStr) noexcept
{
	for (const auto& c : sStr) {
		if (std::isspace(c)) {
			return true; //-----------------------------------------------------
		}
	}
	return false;
}

void strTextToLines(const std::string& sText, std::vector<std::string>& aLines) noexcept
{
	static const std::string::value_type cCR = 13;
	static const std::string::value_type cLF = 10;
	static const std::string sCRLF = std::string(1,cCR) + std::string(1,cLF);
	static const auto nCRLFSize = sCRLF.size();
	const size_t nTextEnd = sText.size();
	size_t nStart = 0;
	while (nStart < nTextEnd) {
		size_t nSkip = 0;
		size_t nEnd = nTextEnd;
		size_t nFoundCRLF = sText.find(sCRLF, nStart);
		if (nFoundCRLF != std::string::npos) {
			nEnd = nFoundCRLF; nSkip = nCRLFSize;
		}
		size_t nFoundLF = sText.find(cLF, nStart);
		if ((nFoundLF != std::string::npos) && (nFoundLF < nEnd)) {
			nEnd = nFoundLF; nSkip = 1;
		}
		size_t nFoundCR = sText.find(cCR, nStart);
		if ((nFoundCR != std::string::npos) && (nFoundLF < nEnd)) {
			nEnd = nFoundCR; nSkip = 1;
		}
		aLines.push_back(sText.substr(nStart, nEnd - nStart));
		nStart = nEnd + nSkip;
	}
}

int32_t strUTF8SizeInCodePoints(const std::string& sUtf8) noexcept
{
	// copied from Ángel José Riesgo: http://www.nubaria.com/en/blog/?p=371
	const unsigned char cFirstBitMask = 128; // 10000000
	//const unsigned char cSecondBitMask = 64; // 01000000
	const unsigned char cThirdBitMask = 32;  // 00100000
	const unsigned char cFourthBitMask = 16; // 00010000
	const unsigned char cFifthBitMask = 8; // 00001000
	const unsigned char cSixthBitMask = 4; // 00000100

	int32_t nCodePointSize = 0;

	const size_t nSize = sUtf8.size();
	size_t nPos = 0;
	while (nPos < nSize) {
		uint32_t nOffset = 1;
		const unsigned char c0 = sUtf8[nPos];
		if ((c0 & cFirstBitMask) != 0) { // length >= 2
			if ((c0 & cThirdBitMask) != 0) { // length >= 3
				if ((c0 & cFourthBitMask) != 0) { // length >= 4
					if ((c0 & cFifthBitMask) != 0) { // length >= 5
						if ((c0 & cSixthBitMask) != 0) { // length >= 6
							// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
							nOffset = 6;
						} else {
							// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
							nOffset = 5; 
						}
					} else {
						// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
						nOffset = 4;
					}
				} else {
					// 1110xxxx 10xxxxxx 10xxxxxx
					nOffset = 3;
				}
			} else {
				// 110xxxxx 10xxxxxx
				nOffset = 2;
			}
		} else {
			// 0xxxxxxx
		}
		nPos += nOffset;
		++nCodePointSize;
	}
	return nCodePointSize;
}

std::pair<bool, std::string> strToBool(const std::string& sStr)
{
//std::cout << "XmlCommonParser::strIsTrueOrFalse    str=" << str << '\n';
	std::string sTF = strStrip(sStr);
	if ( (sTF == "T") || (sTF == "t") || (sTF == "TRUE") || (sTF == "True") || (sTF == "true")
			|| (sTF == "Y") || (sTF == "y") || (sTF == "YES") || (sTF == "Yes") || (sTF == "yes") ) {
		return std::make_pair(true, "");
	} else if ( (sTF == "F") || (sTF == "f") || (sTF == "FALSE") || (sTF == "False") || (sTF == "false")
			|| (sTF == "N") || (sTF == "n") || (sTF == "NO") || (sTF == "No") || (sTF == "no") ) {
		return std::make_pair(false, "");
	}
	return std::make_pair(false, "Not a well formed bool value");
}

std::string intToMillString(int32_t nValue) noexcept
{
	std::string sValue;
	const bool bNeg = (nValue < 0);
	if (bNeg) {
		nValue = -nValue;
	}
	int32_t nZerosToAdd = 0;
	do {
		const std::string s1000 = Util::stringCompose("%1", nValue % 1000);
		if (sValue.empty()) {
			sValue = s1000;
		} else {
			sValue = s1000 + "'" + std::string(nZerosToAdd, '0') + sValue;
		}
		nZerosToAdd = 3 - s1000.length();
		nValue = nValue / 1000;
	} while (nValue > 0);
	if (bNeg) {
		sValue.insert(0,"-");
	}
	return sValue;
}

std::string millisecToMinSecString(int32_t nValue) noexcept
{
	const int32_t nSeconds = nValue / 1000;
	const bool bNeg = (nValue < 0);
	if (bNeg) {
		nValue = -nValue;
	}
	const int32_t nMillisec = nValue % 1000;
	std::string sMillisec = "00" + std::to_string(nMillisec);
	sMillisec = sMillisec.substr(sMillisec.size() - 3);
	return secToMinSecString(nSeconds) + "." + sMillisec;
}
std::string secToMinSecString(int32_t nValue) noexcept
{
	std::string sValue;
	const bool bNeg = (nValue < 0);
	if (bNeg) {
		nValue = -nValue;
	}
	const int32_t nMin = nValue / 60;
	const int32_t nSec = nValue % 60;
	sValue = std::to_string(nMin) + ":";
	if (nSec < 10) {
		sValue.append(1, '0');
	}
	sValue.append(std::to_string(nSec));
	if (bNeg) {
		sValue.insert(0,"-");
	}
	return sValue;
}

} // namespace Util

} // namespace stmg
