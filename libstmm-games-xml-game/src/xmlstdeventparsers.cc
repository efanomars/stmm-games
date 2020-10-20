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
 * File:   xmlstdeventparsers.cc
 */

#include "xmlstdeventparsers.h"

#include "xmleventparser.h"

#include "events/xmlalarmsevent.h"
#include "events/xmlarrayevent.h"
#include "events/xmlbackgroundevent.h"
#include "events/xmlcumulcmpevent.h"
#include "events/xmldelayedqueueevent.h"
#include "events/xmllogevent.h"
#include "events/xmlothersevent.h"
#include "events/xmlpositionerevent.h"
#include "events/xmlrandomevent.h"
#include "events/xmlscrollerevent.h"
#include "events/xmlselectevent.h"
#include "events/xmlshowtextevent.h"
#include "events/xmlsoundevent.h"
#include "events/xmlspeedevent.h"
#include "events/xmlstaticgridevent.h"
#include "events/xmlsysevent.h"
#include "events/xmltileanimatorevent.h"
#include "events/xmlvariableevent.h"

#include <utility>

namespace stmg
{

std::vector<unique_ptr<XmlEventParser>> getXmlStdEventParsers()
{
	std::vector<unique_ptr<XmlEventParser>> aXmlEventParsers;
	//
	aXmlEventParsers.push_back(std::make_unique<XmlAlarmsEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlArrayEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlBackgroundEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlCumulCmpEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlDelayedQueueEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlLogEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlOthersSenderEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlOthersReceiverEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlPositionerEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlRandomEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlScrollerEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlSelectEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlShowTextEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlSoundEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlSpeedEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlStaticGridEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlSysEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlTileAnimatorEventParser>());
	aXmlEventParsers.push_back(std::make_unique<XmlVariableEventParser>());

	return aXmlEventParsers;
}

} // namespace stmg
