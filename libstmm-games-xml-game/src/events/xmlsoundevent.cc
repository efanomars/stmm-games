/*
 * File:   xmlsoundevent.cc
 *
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

#include "events/xmlsoundevent.h"

#include "gamectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/events/soundevent.h>
#include <stmm-games/event.h>
#include <stmm-games/named.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/namedindex.h>

//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

namespace stmg { class ConditionalCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventSoundNodeName = "SoundEvent";
static const std::string s_sEventSoundNameAttr = "name";
static const std::string s_sEventSoundListenerRelativeAttr = "listenerRealtive";
static const std::string s_sEventSoundLoopingAttr = "loop";
static const std::string s_sEventSoundVolumeAttr = "volume";
static const std::string s_sEventSoundPosXAttr = "posX";
static const std::string s_sEventSoundPosYAttr = "posY";
static const std::string s_sEventSoundPosZAttr = "posZ";

XmlSoundEventParser::XmlSoundEventParser()
: XmlEventParser(s_sEventSoundNodeName)
{
}

Event* XmlSoundEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventSound(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlSoundEventParser::parseEventSound(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlSoundEventParser::parseEventSound" << '\n';
	oCtx.addChecker(p0Element);
	SoundEvent::Init oSInit;
	parseEventBase(oCtx, p0Element, oSInit);
	//
	const auto oPairName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSoundNameAttr);
	if (oPairName.first) {
		const std::string& sName = oPairName.second;
		if (sName.empty()) {
			throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sEventSoundNameAttr);
		}
		oSInit.m_nSoundIdx = oCtx.named().sounds().addName(sName);
	}
	//
	const auto oPairOwner = getXmlConditionalParser().parseOwner(oCtx, p0Element);
	oSInit.m_nSoundTeam = oPairOwner.first;
	oSInit.m_nSoundMate = oPairOwner.second;

	const auto oPairListenerRelative = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSoundListenerRelativeAttr);
	if (oPairListenerRelative.first) {
		const std::string& sListenerRelative = oPairListenerRelative.second;
		oSInit.m_bListenerRelative = XmlUtil::strToBool(oCtx, p0Element, s_sEventSoundListenerRelativeAttr, sListenerRelative);
	}

	const auto oPairLooping = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSoundLoopingAttr);
	if (oPairLooping.first) {
		const std::string& sLooping = oPairLooping.second;
		oSInit.m_bLooping = XmlUtil::strToBool(oCtx, p0Element, s_sEventSoundLoopingAttr, sLooping);
	}

	const auto oPairVolume = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSoundVolumeAttr);
	if (oPairVolume.first) {
		const std::string& sVolume = oPairVolume.second;
		oSInit.m_fVolume = 0.01 * XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventSoundVolumeAttr, sVolume, false
																, true, 0, true, 100);
	}

	const auto oPairSoundPosX = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSoundPosXAttr);
	if (oPairSoundPosX.first) {
		const std::string& sSoundPosX = oPairSoundPosX.second;
		oSInit.m_oPosXY.m_fX = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventSoundPosXAttr, sSoundPosX, false
															, false, -1.0, false, -1.0);
	}
	const auto oPairSoundPosY = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSoundPosYAttr);
	if (oPairSoundPosY.first) {
		const std::string& sSoundPosY = oPairSoundPosY.second;
		oSInit.m_oPosXY.m_fY = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventSoundPosYAttr, sSoundPosY, false
															, false, -1.0, false, -1.0);
	}
	const auto oPairSoundPosZ = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSoundPosZAttr);
	if (oPairSoundPosZ.first) {
		const std::string& sSoundPosZ = oPairSoundPosZ.second;
		oSInit.m_fPosZ = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sEventSoundPosZAttr, sSoundPosZ, false
													, false, -1.0, false, -1.0);
	}

	oCtx.removeChecker(p0Element, true);
	auto refSoundEvent = std::make_unique<SoundEvent>(std::move(oSInit));
	return refSoundEvent;
}
int32_t XmlSoundEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "SET_SOUND_IDX") {
		nMsg = SoundEvent::MESSAGE_SET_SOUND_IDX;
	} else if (sMsgName == "PRELOAD") {
		nMsg = SoundEvent::MESSAGE_PRELOAD;
	} else if (sMsgName == "PLAY") {
		nMsg = SoundEvent::MESSAGE_PLAY;
	} else if (sMsgName == "PLAY_FREE") {
		nMsg = SoundEvent::MESSAGE_PLAY_FREE;
	} else if (sMsgName == "PLAY_FREE_ONCE") {
		nMsg = SoundEvent::MESSAGE_PLAY_FREE_ONCE;
	} else if (sMsgName == "PLAY_FREE_IDX") {
		nMsg = SoundEvent::MESSAGE_PLAY_FREE_IDX;
	} else if (sMsgName == "PLAY_FREE_IDX_ONCE") {
		nMsg = SoundEvent::MESSAGE_PLAY_FREE_IDX_ONCE;
	} else if (sMsgName == "PAUSE") {
		nMsg = SoundEvent::MESSAGE_PAUSE;
	} else if (sMsgName == "RESUME") {
		nMsg = SoundEvent::MESSAGE_RESUME;
	} else if (sMsgName == "STOP") {
		nMsg = SoundEvent::MESSAGE_STOP;
	} else if (sMsgName == "SET_VOL") {
		nMsg = SoundEvent::MESSAGE_SET_VOL;
	} else if (sMsgName == "SET_POS_X") {
		nMsg = SoundEvent::MESSAGE_SET_POS_X;
	} else if (sMsgName == "SET_POS_Y") {
		nMsg = SoundEvent::MESSAGE_SET_POS_Y;
	} else if (sMsgName == "SET_POS_Z") {
		nMsg = SoundEvent::MESSAGE_SET_POS_Z;
	} else if (sMsgName == "SET_POS_XY") {
		nMsg = SoundEvent::MESSAGE_SET_POS_XY;
	} else if (sMsgName == "ADD_TO_POS_X") {
		nMsg = SoundEvent::MESSAGE_ADD_TO_POS_X;
	} else if (sMsgName == "ADD_TO_POS_Y") {
		nMsg = SoundEvent::MESSAGE_ADD_TO_POS_Y;
	} else if (sMsgName == "ADD_TO_POS_Z") {
		nMsg = SoundEvent::MESSAGE_ADD_TO_POS_Z;
	} else if (sMsgName == "ADD_PERC_TO_POS_X") {
		nMsg = SoundEvent::MESSAGE_ADD_PERC_TO_POS_X;
	} else if (sMsgName == "ADD_PERC_TO_POS_Y") {
		nMsg = SoundEvent::MESSAGE_ADD_PERC_TO_POS_Y;
	} else if (sMsgName == "ADD_PERC_TO_POS_Z") {
		nMsg = SoundEvent::MESSAGE_ADD_PERC_TO_POS_Z;
	} else if (sMsgName == "RESTART") {
		nMsg = SoundEvent::MESSAGE_RESTART;
	} else if (sMsgName == "RESTART_RESET") {
		nMsg = SoundEvent::MESSAGE_RESTART_RESET;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}
int32_t XmlSoundEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
															, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "SOUND_FINISHED") {
		nListenerGroup = SoundEvent::LISTENER_GROUP_SOUND_FINISHED;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
