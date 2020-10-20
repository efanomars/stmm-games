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
 * File:   xmlnewrowsparser.h
 */

#ifndef STMG_XML_NEW_ROWS_PARSER_H
#define STMG_XML_NEW_ROWS_PARSER_H

#include "xmlutile/xmlidtilesparser.h"

#include <stmm-games/util/intset.h>
#include <stmm-games/utile/newrows.h>

#include <string>
#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class GameCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlNewRowsParser
{
public:
	XmlNewRowsParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser)
	: m_oXmlConditionalParser(oXmlConditionalParser)
	, m_oXmlTraitsParser(oXmlTraitsParser)
	{
	}
	NewRows parseNewRows(GameCtx& oCtx, const xmlpp::Element* p0Element);
private:
	NewRows::NewRowGen parseNewRow(GameCtx& oCtx, const xmlpp::Element* p0Element
										, const XmlIdTilesParser::IdTiles& oIdTiles, const std::vector<std::string>& aRandomIds);
	unique_ptr<NewRows::Distr> parseDistrFixed(GameCtx& oCtx, const xmlpp::Element* p0Element
													, const XmlIdTilesParser::IdTiles& oXmlIdTiles);
	unique_ptr<NewRows::Distr> parseDistrSingle(GameCtx& oCtx, const xmlpp::Element* p0Element);
	unique_ptr<NewRows::Distr> parseDistrRand(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::vector<std::string>& aRandomIds);

	IntSet parsePositions(GameCtx& oCtx, const xmlpp::Element* p0Element);
	int32_t parseLeaveEmpty(GameCtx& oCtx, const xmlpp::Element* p0Element, int32_t nDefault);
	int32_t parseFill(GameCtx& oCtx, const xmlpp::Element* p0Element, int32_t nDefault);
private:
	XmlConditionalParser& m_oXmlConditionalParser;
	XmlTraitsParser& m_oXmlTraitsParser;
private:
	XmlNewRowsParser(const XmlNewRowsParser& oSource) = delete;
	XmlNewRowsParser& operator=(const XmlNewRowsParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_NEW_ROWS_PARSER_H */
