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
 * File:   xmlthanimationfactoryparser.h
 */

#ifndef STMG_XML_TH_ANIMATION_FACTORY_PARSER_H
#define STMG_XML_TH_ANIMATION_FACTORY_PARSER_H

#include <memory>
#include <string>

namespace stmg { class StdThemeAnimationFactory; }
namespace stmg { class ThemeCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlConditionalParser;
class XmlTraitsParser;
class XmlThemeImageParser;
class XmlThemeAniParser;

class XmlThAnimationFactoryParser
{
public:
	virtual ~XmlThAnimationFactoryParser() = default;
	/** Constructor.
	 * @param sAnimationFactoryName The animation factory name. Cannot be empty.
	 */
	XmlThAnimationFactoryParser(const std::string& sAnimationFactoryName);

	/** The animation factory name.
	 * @return The animation factory name. Is not empty.
	 */
	const std::string& getAnimationFactoryName() const;
	/** Parse the animation factory element.
	 * @param oCtx The theme context.
	 * @param p0Element The element. Cannot be null.
	 * @return The factory. Is not null.
	 * @throws std::runtime_error.
	 */
	virtual unique_ptr<StdThemeAnimationFactory> parseAnimationFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element) = 0;
protected:
	XmlConditionalParser* getXmlConditionalParser();
	XmlTraitsParser* getXmlTraitsParser();
	XmlThemeImageParser* getXmlThemeImageParser();

private:
	friend class XmlThemeAniParser;
	const std::string m_sAnimationFactoryName;
	XmlThemeAniParser* m_p1Owner;

private:
	XmlThAnimationFactoryParser(const XmlThAnimationFactoryParser& oSource) = delete;
	XmlThAnimationFactoryParser& operator=(const XmlThAnimationFactoryParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_TH_ANIMATION_FACTORY_PARSER_H */

