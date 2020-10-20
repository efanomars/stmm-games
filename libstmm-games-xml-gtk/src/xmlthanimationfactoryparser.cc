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
 * File:   xmlthanimationfactoryparser.cc
 */

#include "xmlthanimationfactoryparser.h"

#include "xmlthemeaniparser.h"

//#include <cassert>
//#include <iostream>

namespace stmg
{

XmlThAnimationFactoryParser::XmlThAnimationFactoryParser(const std::string& sAnimationFactoryName)
: m_sAnimationFactoryName(sAnimationFactoryName)
, m_p1Owner(nullptr)
{
}
const std::string& XmlThAnimationFactoryParser::getAnimationFactoryName() const
{
	return m_sAnimationFactoryName;
}
XmlConditionalParser* XmlThAnimationFactoryParser::getXmlConditionalParser()
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return nullptr;
	}
	#endif //NDEBUG
	return &(m_p1Owner->m_oXmlConditionalParser);
}
XmlTraitsParser* XmlThAnimationFactoryParser::getXmlTraitsParser()
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return nullptr;
	}
	#endif //NDEBUG
	return &(m_p1Owner->m_oXmlTraitsParser);
}
XmlThemeImageParser* XmlThAnimationFactoryParser::getXmlThemeImageParser()
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return nullptr;
	}
	#endif //NDEBUG
	return &(m_p1Owner->m_oXmlThemeImageParser);
}

} // namespace stmg
