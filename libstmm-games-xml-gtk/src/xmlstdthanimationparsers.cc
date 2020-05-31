/*
 * File:   xmlstdthanimationparsers.cc
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

#include "xmlstdthanimationparsers.h"

#include "xmlthanimationfactoryparser.h"

#include "themeanimations/xmlimageseqthanimationfactory.h"
#include "themeanimations/xmlexplosionthanimationfactory.h"
#include "themeanimations/xmlplaintextthanimationfactory.h"
#include "themeanimations/xmlstaticgridthanimationfactory.h"
#include "themeanimations/xmlbackgroundthanimationfactory.h"

namespace stmg
{

std::vector<unique_ptr<XmlThAnimationFactoryParser>> getXmlStdThAnimationParsers()
{
	std::vector<unique_ptr<XmlThAnimationFactoryParser>> aXmlThAnimationParsers;
	//
	aXmlThAnimationParsers.push_back(std::make_unique<XmlImageSeqThAnimationFactoryParser>());
	aXmlThAnimationParsers.push_back(std::make_unique<XmlExplosionThAnimationFactoryParser>());
	aXmlThAnimationParsers.push_back(std::make_unique<XmlPlainTextThAnimationFactoryParser>());
	aXmlThAnimationParsers.push_back(std::make_unique<XmlStaticGridThAnimationFactoryParser>());
	aXmlThAnimationParsers.push_back(std::make_unique<XmlBackgroundThAnimationFactoryParser>());

	return aXmlThAnimationParsers;
}

} // namespace stmg
