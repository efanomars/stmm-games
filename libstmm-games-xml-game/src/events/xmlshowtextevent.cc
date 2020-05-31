/*
 * File:   xmlshowtextevent.cc
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

#include "events/xmlshowtextevent.h"

#include "gamectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/game.h>
#include <stmm-games/level.h>
#include <stmm-games/apppreferences.h>
#include <stmm-games/event.h>
#include <stmm-games/events/showtextevent.h>
#include <stmm-games/levelanimation.h>
#include <stmm-games/named.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/util/util.h>

#include <vector>
//#include <cassert>
//#include <iostream>

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

#include <cstdint>
#include <utility>

namespace stmg { class ConditionalCtx; }

namespace stmg
{

static const std::string s_sEventShowTextNodeName = "ShowTextEvent";
static const std::string s_sEventShowTextTokenAttr = "token";
	static const std::string s_sEventShowTextTokenAttrDefault = "%%";
static const std::string s_sEventShowTextLineNodeName = "Line";
static const std::string s_sEventShowTextTypeAttr = "type";
static const std::string s_sEventShowTextFontSizeInTilesAttr = "fontSizeInTiles";
static const std::string s_sEventShowTextDurationAttr = "duration";
static const std::string s_sEventShowTextRefSysAttr = "refSys";
	static const std::string s_sEventShowTextRefSysAttrBoard = "BOARD";
	static const std::string s_sEventShowTextRefSysAttrShow = "SHOW";
	static const std::string s_sEventShowTextRefSysAttrSubshow = "SUBSHOW";
static const std::string s_sEventShowTextPosXAttr = "posX";
static const std::string s_sEventShowTextPosYAttr = "posY";
static const std::string s_sEventShowTextSizeWAttr = "width";
static const std::string s_sEventShowTextSizeHAttr = "height";
static const std::string s_sEventShowTextPosZHAttr = "posZ";

XmlShowTextEventParser::XmlShowTextEventParser()
: XmlEventParser(s_sEventShowTextNodeName)
{
}

Event* XmlShowTextEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventShowText(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlShowTextEventParser::parseEventShowText(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlShowTextEventParser::parseEventShowText" << '\n';
	Level& oLevel = oCtx.level();

	oCtx.addChecker(p0Element);
	ShowTextEvent::Init oSTInit;
	parseEventBase(oCtx, p0Element, oSTInit);

	std::string sToken;
	const auto oPairToken = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventShowTextTokenAttr);
	if (oPairToken.first) {
		sToken = oPairToken.second;
	}
	if (sToken.empty()) {
		sToken = s_sEventShowTextTokenAttrDefault;
	}

	const auto oPairAniName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventShowTextTypeAttr);
	if (oPairAniName.first) {
		const std::string& sName = oPairAniName.second;
		oSTInit.m_nAnimationNamedIdx = oCtx.named().animations().addName(sName);
	}

	const auto oPairTextSizeInTiles = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventShowTextFontSizeInTilesAttr);
	if (oPairTextSizeInTiles.first) {
		const std::string& sTextSizeInTiles = oPairTextSizeInTiles.second;
		oSTInit.m_fTextSize = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventShowTextFontSizeInTilesAttr, sTextSizeInTiles, true
																	, true, 0.0, false, -1.0);
	}

	const auto oPairDuration = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventShowTextDurationAttr);
	if (oPairDuration.first) {
		const std::string& sDuration = oPairDuration.second;
		// -1: forever, 0:theme default
		oSTInit.m_nDuration = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventShowTextDurationAttr, sDuration, false
																	, true, -1, false, -1);
	}

	static std::vector<char const *> s_aRefSysEnumString{s_sEventShowTextRefSysAttrBoard.c_str(), s_sEventShowTextRefSysAttrShow.c_str(), s_sEventShowTextRefSysAttrSubshow.c_str()};
	static const std::vector<LevelAnimation::REFSYS> s_aRefSysEnumValue{LevelAnimation::REFSYS_BOARD, LevelAnimation::REFSYS_SHOW, LevelAnimation::REFSYS_SUBSHOW};
	//
	const int32_t nIdxRefSys = getXmlConditionalParser().getEnumAttributeValue(oCtx, p0Element, s_sEventShowTextRefSysAttr, s_aRefSysEnumString);
	if (nIdxRefSys >= 0) {
		oSTInit.m_eRefSys = s_aRefSysEnumValue[nIdxRefSys];
	}
	if (oSTInit.m_eRefSys == LevelAnimation::REFSYS_SUBSHOW) {
		if (!oLevel.subshowMode()) {
			throw XmlCommonErrors::error(oCtx, p0Element, s_sEventShowTextRefSysAttr, "Subshow text needs Subshow mode.");
		}
		// add the level player (TODO level team if subshow shows team!?)
		const auto oPairOwner = getXmlConditionalParser().parseOwner(oCtx, p0Element, OwnerType::PLAYER);
		const int32_t nTeam = oPairOwner.first;
		const int32_t nMate = oPairOwner.second;
		if (nMate < 0) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, XmlConditionalParser::s_sConditionalOwnerMateAttr);
		}
		// calculate level player
		const bool bATIOL = oCtx.game().isAllTeamsInOneLevel();
		int32_t nLevelPlayer;
		if (bATIOL) {
			auto refPlayer = oCtx.appPreferences()->getTeam(nTeam)->getMate(nMate);
			nLevelPlayer = refPlayer->get();
		} else {
			nLevelPlayer = nMate;
		}
		oSTInit.m_eRefSys = static_cast<LevelAnimation::REFSYS>(static_cast<int32_t>(oSTInit.m_eRefSys) + nLevelPlayer);
	}
	const auto oPairShowTextPosX = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventShowTextPosXAttr);
	if (oPairShowTextPosX.first) {
		const std::string& sShowTextPosX = oPairShowTextPosX.second;
		oSTInit.m_oRect.m_fX = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventShowTextPosXAttr, sShowTextPosX, false
																	, false, -1.0, false, -1.0);
	}
	const auto oPairShowTextPosY = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventShowTextPosYAttr);
	if (oPairShowTextPosY.first) {
		const std::string& sShowTextPosY = oPairShowTextPosY.second;
		oSTInit.m_oRect.m_fY = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventShowTextPosYAttr, sShowTextPosY, false
																	, false, -1.0, false, -1.0);
	}
	const auto oPairShowTextSizeW = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventShowTextSizeWAttr);
	if (oPairShowTextSizeW.first) {
		const std::string& sShowTextSizeW = oPairShowTextSizeW.second;
		oSTInit.m_oRect.m_fW = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventShowTextSizeWAttr, sShowTextSizeW, false
																	, true, 0.0, false, -1.0);
	}
	const auto oPairShowTextSizeH = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventShowTextSizeHAttr);
	if (oPairShowTextSizeH.first) {
		const std::string& sShowTextSizeH = oPairShowTextSizeH.second;
		oSTInit.m_oRect.m_fH = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventShowTextSizeHAttr, sShowTextSizeH, false
																	, true, 0.0, false, -1.0);
	}
	const auto oPairZPos = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventShowTextPosZHAttr);
	if (oPairZPos.first) {
		const std::string& sZPos = oPairZPos.second;
		oSTInit.m_nZ = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventShowTextPosZHAttr, sZPos, false
															, false, -1, false, -1);
	}

	bool bAddedOne = false;
	std::vector<std::string> aLine;
	getXmlConditionalParser().visitNamedElementChildren(oCtx, p0Element, s_sEventShowTextLineNodeName, [&](const xmlpp::Element* p0Line)
	{
		oCtx.addChecker(p0Line);
		std::string sLine;
		const xmlpp::TextNode* p0LineText = p0Line->get_child_text();
		if (p0LineText != nullptr) {
			sLine = p0LineText->get_content();
		}
		Util::strTextToLines(sLine, aLine);
		bAddedOne = true;
		oCtx.removeChecker(p0Line, true);
	});
	if (!bAddedOne) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sEventShowTextLineNodeName);
	}

	//TODO as s_cXXX static class members
	static const Glib::ustring::value_type cInputId = 'i';
		static const Glib::ustring::value_type cInputMsgId = 'm';
		static const Glib::ustring::value_type cInputValueId = 'v';
		static const Glib::ustring::value_type cInputValueXId = 'x';
		static const Glib::ustring::value_type cInputValueYId = 'y';
		static const Glib::ustring::value_type cInputTickId = 't';
		static const Glib::ustring::value_type cInputEventId = 'e';
	static const Glib::ustring::value_type cVarNameId = 'v';
		static const Glib::ustring::value_type cVarNameGameId = 'g';
		static const Glib::ustring::value_type cVarNameTeamId = 't';
		static const Glib::ustring::value_type cVarNameMateId = 'm';
		static const Glib::ustring::value_type cVarNamePlayerId = 'p';
	//static const Glib::ustring::value_type cNameId = 'n';
	//	static const Glib::ustring::value_type cTeamId = 't';
	//	static const Glib::ustring::value_type cMateId = 'm';
	//	static const Glib::ustring::value_type cPlayerId = 'p';
	static const Glib::ustring::value_type cIntervalId = 'd';
		static const Glib::ustring::value_type cIntervalGameId = 'g';
		static const Glib::ustring::value_type cIntervalLevelId = 'l';

	std::vector<std::string> aSobstLines;
	std::vector<ShowTextEvent::SobstItem> aSobsts;

	const int32_t nTokenLen = sToken.length();
	if (nTokenLen > 0) {
		const int32_t nTotLines = static_cast<int32_t>(aLine.size());
		aSobstLines.resize(nTotLines, "");
		for (int32_t nLine = 0; nLine < nTotLines; ++nLine) {
			const std::string& sLine = aLine[nLine];
			const int32_t nLineLen = sLine.length();
			std::string& sDestLine = aSobstLines[nLine];

			int32_t nCurPos = 0;
			while (true) {
//std::cout << "parseEventShowText nCurPos=" << nCurPos << '\n';
				const auto nStartPos = sLine.find(sToken, nCurPos);
//std::cout << "parseEventShowText nStartPos=" << nStartPos << '\n';
				if (nStartPos == std::string::npos) {
					sDestLine.append(sLine.substr(nCurPos, nLineLen - nCurPos));
					break; // while -----
				}
				sDestLine.append(sLine.substr(nCurPos, nStartPos - nCurPos));

				const auto nEndPos = sLine.find(sToken, nStartPos + nTokenLen);
//std::cout << "parseEventShowText nEndPos=" << nEndPos << '\n';
				if (nEndPos == std::string::npos) {
					// error: ignore
					break; // while -----
				}
				const std::string sCom = sLine.substr(nStartPos + nTokenLen, nEndPos - (nStartPos + nTokenLen));
//std::cout << "parseEventShowText sCom=" << sCom << '\n';

				if (sCom.length() > 0) {
					ShowTextEvent::SobstItem oSobstItem;
					oSobstItem.m_nLine = nLine;
					oSobstItem.m_nPos = sDestLine.length();
					bool bValidSobst = false;

					const std::string::value_type cCom = sCom[0];
					std::string sRest = sCom.substr(1, sCom.length() - 1);
					switch (cCom) {
						case cInputId:
						{
							oSobstItem.m_nParam1 = -1;
							if (!sRest.empty()) {
								const std::string::value_type cInput = sRest[0];
								if (cInput == cInputMsgId) {
									oSobstItem.m_nParam1 = ShowTextEvent::SOBST_INPUT_TYPE_MSG;
								} else if (cInput == cInputValueId) {
									oSobstItem.m_nParam1 = ShowTextEvent::SOBST_INPUT_TYPE_VALUE;
								} else if (cInput == cInputValueXId) {
									oSobstItem.m_nParam1 = ShowTextEvent::SOBST_INPUT_TYPE_VALUE_X;
								} else if (cInput == cInputValueYId) {
									oSobstItem.m_nParam1 = ShowTextEvent::SOBST_INPUT_TYPE_VALUE_Y;
								} else if (cInput == cInputTickId) {
									oSobstItem.m_nParam1 = ShowTextEvent::SOBST_INPUT_TYPE_TICK;
								} else if (cInput == cInputEventId) {
									oSobstItem.m_nParam1 = ShowTextEvent::SOBST_INPUT_TYPE_EVENT;
								} else {
									throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
																			"Unknown subcommand %1", std::string{cInput, 1}));
								}
							}
							oSobstItem.m_eSobstType = ShowTextEvent::SOBST_TYPE_INPUT;
							bValidSobst = true;
						};
						break; //switch (cCom)
						case cVarNameId:
						{
							oSobstItem.m_nParam1 = -1;
							if (!sRest.empty()) {
								OwnerType eOwnerType = OwnerType::GAME;
								const std::string::value_type cOwnerType = sRest[0];
								sRest = sRest.substr(1, sRest.length() - 1);
								if (cOwnerType == cVarNameGameId) {
									oSobstItem.m_nParam3 = -1;
									oSobstItem.m_nParam4 = -1;
								} else if (cOwnerType == cVarNameTeamId) {
									eOwnerType = OwnerType::TEAM;
									if (!sRest.empty()) {
										if ((sRest[0] >= '0') && (sRest[0] <= '9')) {
											oSobstItem.m_nParam3 = static_cast<int32_t>(sRest[0] - '0');
											sRest = sRest.substr(1, sRest.length() - 1);
										} else if (oLevel.getTotLevelTeams() == 1) {
											oSobstItem.m_nParam3 = 0;
											oSobstItem.m_nParam4 = -1;
										} else {
											throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
																			"Team missing after %1", std::string{cOwnerType, 1}));
										}
									}
								} else if (cOwnerType == cVarNameMateId) {
									eOwnerType = OwnerType::PLAYER;
									if (!sRest.empty()) {
										if ((sRest[0] >= '0') && (sRest[0] <= '9')) {
											oSobstItem.m_nParam3 = static_cast<int32_t>(sRest[0] - '0');
											sRest = sRest.substr(1, sRest.length() - 1);
											const int32_t nTotLevelTeams = oLevel.getTotLevelTeams();
											if ((sRest[0] >= '0') && (sRest[0] <= '9')) {
												oSobstItem.m_nParam4 = static_cast<int32_t>(sRest[0] - '0');
												sRest = sRest.substr(1, sRest.length() - 1);
											} else if (nTotLevelTeams == 1) {
												oSobstItem.m_nParam4 = oSobstItem.m_nParam3;
												oSobstItem.m_nParam3 = 0;
											} else {
												throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
																				"Team missing after %1", std::string{cOwnerType, 1}));
											}
											if (oSobstItem.m_nParam3 >= nTotLevelTeams) {
												throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
																					"Wrong level team %1", oSobstItem.m_nParam3));
											} else if (oSobstItem.m_nParam4 >= oCtx.game().prefs().getTeam(oSobstItem.m_nParam3)->getTotMates()) {
												throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
																					"Wrong mate %1", oSobstItem.m_nParam4));
											}
										} else if (oLevel.getTotLevelTeams() == 1) {
											oSobstItem.m_nParam3 = 0;
											if (oLevel.prefs().getTeam(0)->getTotMates() == 1) {
												oSobstItem.m_nParam4 = 0;
											} else {
												throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
																				"Team missing after %1", std::string{cOwnerType, 1}));
											}
										} else {
											throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
																			"Team missing after %1", std::string{cOwnerType, 1}));
										}
									}
								} else if (cOwnerType == cVarNamePlayerId) {
									eOwnerType = OwnerType::PLAYER;
									if (!sRest.empty()) {
										if ((sRest[0] >= '0') && (sRest[0] <= '9')) {
											const int32_t nLevelPlayer = static_cast<int32_t>(sRest[0] - '0');
											if (nLevelPlayer < oLevel.getTotLevelPlayers()) {
												if (oCtx.game().isAllTeamsInOneLevel()) {
													auto refPlayer = oLevel.prefs().getPlayer(nLevelPlayer);
													oSobstItem.m_nParam3 = refPlayer->getTeam()->get();
													oSobstItem.m_nParam4 = refPlayer->getMate();
												} else {
													oSobstItem.m_nParam3 = oLevel.getLevel();
													oSobstItem.m_nParam4 = nLevelPlayer;
												}
											} else {
												throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
																										"Wrong player %1", nLevelPlayer));
											}
										}
									}
								}
								const auto& sVarName = sRest;
								const auto oPairIdxOwner = oCtx.game().variableIdAndOwner(sVarName);
								const int32_t nVarIdx = oPairIdxOwner.first;
								const OwnerType eCheckOwnerType = oPairIdxOwner.second;
								if (nVarIdx < 0) {
									throw XmlCommonErrors::errorAttrVariableNotDefined(oCtx, p0Element, Util::s_sEmptyString, sVarName);
								}
								if (eCheckOwnerType != eOwnerType) {
									throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
																							"Variable %1 has wrong owner type %2", sVarName, cOwnerType));
								}
								if (nVarIdx >= 0) {
									oSobstItem.m_eSobstType = ShowTextEvent::SOBST_TYPE_VAR_ID;
									oSobstItem.m_nParam1 = static_cast<int32_t>(eOwnerType);
									oSobstItem.m_nParam2 = nVarIdx;
									bValidSobst = true;
								}
							}
						};
						break; //switch (cCom)
						case cIntervalId:
						{
							if (sRest.length() == 1) {
								if (sRest[0] == cIntervalGameId) {
									oSobstItem.m_nParam1 = ShowTextEvent::SOBST_INTERVAL_TYPE_GAME;
								} else if (sRest[0] == cIntervalLevelId) {
									oSobstItem.m_nParam1 = ShowTextEvent::SOBST_INTERVAL_TYPE_LEVEL;
								}
								bValidSobst = true;
								oSobstItem.m_eSobstType = ShowTextEvent::SOBST_TYPE_INTERVAL;
							}
						};
						break; //switch
					} //switch
					if (bValidSobst) {
						aSobsts.push_back(oSobstItem);
					}
				}
				nCurPos = nEndPos + nTokenLen;
			} //while
//std::cout << "parseEventShowText sDestLine=" << sDestLine << '\n';
		} //for
	} else {
		aSobstLines = aLine;
	}
	oCtx.removeChecker(p0Element, true);
	oSTInit.m_aSobstLines = std::move(aSobstLines);
	oSTInit.m_aSobsts = std::move(aSobsts);
	return std::make_unique<ShowTextEvent>(std::move(oSTInit));
}
int32_t XmlShowTextEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "STOP_ANIMATION") {
		nMsg = ShowTextEvent::MESSAGE_STOP_ANIMATION;
	} else if (sMsgName == "START_ANIMATION_AT") {
		nMsg = ShowTextEvent::MESSAGE_START_ANIMATION_AT;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}


} // namespace stmg
