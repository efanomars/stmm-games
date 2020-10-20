/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   xmlelapsedmapperparser.h
 */

#ifndef STMG_XML_ELAPSED_MAPPER_PARSER_H
#define STMG_XML_ELAPSED_MAPPER_PARSER_H

#include <stmm-games-gtk/gtkutil/elapsedmapper.h>

#include <string>

namespace stmg { class ConditionalCtx; }
namespace stmg { class XmlConditionalParser; }
namespace xmlpp { class Element; }

namespace stmg
{

class XmlElapsedMapperParser
{
public:
	explicit XmlElapsedMapperParser(XmlConditionalParser& oXmlConditionalParser);

	ElapsedMapper parseElapsedMapper(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
private:
	XmlConditionalParser& m_oXmlConditionalParser;
private:
	XmlElapsedMapperParser(const XmlElapsedMapperParser& oSource) = delete;
	XmlElapsedMapperParser& operator=(const XmlElapsedMapperParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_ELAPSED_MAPPER_PARSER_H */

