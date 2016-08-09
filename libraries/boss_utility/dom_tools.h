/* dom_tools.h    $Revision: 1.5 $ $Date: 2007/04/16 09:43:33 $
 Copyright (C) 2000 University of Bonn.
 http://www.ikp.uni-bonn.de/boss

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA  02111-1307, USA.
 */

/**
 @author Karlheinz Stoeber
 */

#ifndef DOM_TOOLS_H
#define DOM_TOOLS_H

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XercesVersion.hpp>

#if XERCES_VERSION_MAJOR == 3
#include <xercesc/sax/InputSource.hpp>
#else if XERCES_VERSION_MAJOR == 2
#include <xercesc/dom/DOMInputSource.hpp>
#endif

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <locale.h>

using namespace std;

ostream & operator <<(ostream & target, xercesc::DOMNode * toWrite);
ostream & operator <<(ostream & target, const XMLCh * x);

//xercesc::XMLTranscoder* UTF8Transcoder = NULL;
//xercesc::XMLTranscoder* UTF16Transcoder = NULL;

class BOSSString {
public:
	unsigned char* data;
	BOSSString & operator =(const XMLCh* source);
};

namespace BOSS {

typedef vector<xercesc::DOMNode*> DOMNodeVector;

//void BOSS::docOutput (xercesc::DOMNode * toWrite)
//ostream & operator << (ostream& target, const DOMString & s);
//string getAttrByName(const DOMNode & element, const DOMString & key);
bool hasAttribute(const xercesc::DOMNode* n, string as);
bool hasAttribute(const xercesc::DOMElement * e, string as);
string getAttrByName(const xercesc::DOMElement * e, string as);
string getAttrByName(const xercesc::DOMNode * n, string as);
//UnicodeString* getAttrByNameUni(const xercesc::DOMElement * e, string as);
void createLists(const xercesc::DOMNode * start_node, const char* node_name,
		DOMNodeVector & list);
void setAttribute(xercesc::DOMElement* e, const char* name, const char* value);
void setAttribute(xercesc::DOMNode* n, const char* name, const char* value);
xercesc::DOMNodeList* getElementsByTagName(const xercesc::DOMElement* e,
		const char* name);
xercesc::DOMNodeList* getElementsByTagName(const xercesc::DOMDocument* d,
		const char* name);
xercesc::DOMElement* createElement(xercesc::DOMDocument* doc, const char* name);
bool compareNodeName(const xercesc::DOMNode* node, const string & name);
string getNodeName(const xercesc::DOMNode* node);
string getNodeValue(const xercesc::DOMNode* node);
//UnicodeString BOSS::getNodeValueUni(const xercesc::DOMNode* node);
void* getUserData(const xercesc::DOMNode* node);
void setUserData(const xercesc::DOMNode* node, void* data);

void initXML();
void initTranscoder();
xercesc::XMLTranscoder* initTranscoder(string name);
xercesc::DOMDocument* parseXMLFile(const char* xml_file);
xercesc::DOMDocument* parseXMLFromMemory(const string input);

#if XERCES_VERSION_MAJOR == 3

class MMFInputStream: public xercesc::BinInputStream {

public:
	MMFInputStream(const char *xml) {
		m_xml = xml;
		m_pStart = (XMLByte*) xml;
		m_pCurrent = m_pStart;
	}

	virtual ~MMFInputStream(void) {
	}

	// BinInputStream methods
public:

	virtual XMLFilePos curPos() const {
		return m_pCurrent - m_pStart;
	}

	virtual XMLSize_t readBytes(XMLByte* const toFill,
			const XMLSize_t maxToRead) {
		// init to bytes available
		unsigned int nRead = strlen(m_xml) - curPos();
		if (nRead > maxToRead)
			nRead = maxToRead;

		memcpy(toFill, m_pCurrent, nRead);
		m_pCurrent += nRead;
		return nRead;
	}

	virtual const XMLCh* getContentType() const {
		cerr << "chinga tu m****\n";
	}
	;

protected:

	const char* m_xml;
	XMLByte* m_pStart;
	XMLByte* m_pCurrent;

};

class MMFInputSource: public xercesc::InputSource {

public:

	MMFInputSource(const char *xml) {
		m_xml = xml;
	}

	virtual ~MMFInputSource(void) {
	}

	virtual xercesc::BinInputStream* makeStream() const {
		return new MMFInputStream(m_xml);
	}

	const XMLCh * getEncoding() const {
		return NULL;
	}
	const XMLCh * getPublicId() const {
		return NULL;
	}
	const XMLCh * getSystemId() const {
		return NULL;
	}
	const XMLCh * getBaseURI() const {
		return NULL;
	}
	void setEncoding(const XMLCh * const encodingStr) {
	}
	void setPublicId(const XMLCh * const publicId) {
	}
	void setSystemId(const XMLCh * const systemId) {
	}
	void setBaseURI(const XMLCh * const baseURI) {
	}
	void setIssueFatalErrorIfNotFound(const bool flag) {
	}
	bool getIssueFatalErrorIfNotFound() const {
		return false;
	}
	void release() {
	}

protected:

	const char* m_xml;

};

#else if XERCES_VERSION_MAJOR == 2
class MMFInputStream :
public xercesc::BinInputStream
{

public:
	MMFInputStream(const char *xml)
	{
		m_xml = xml;
		m_pStart = (XMLByte*) xml;
		m_pCurrent = m_pStart;
	}

	virtual ~MMFInputStream(void)
	{
	}

	// BinInputStream methods
public:

	virtual unsigned int curPos() const
	{
		return m_pCurrent - m_pStart;
	}

	virtual unsigned int readBytes(XMLByte* const toFill, const unsigned int maxToRead)
	{
		// init to bytes available
		unsigned int nRead = strlen(m_xml) - curPos();
		if (nRead > maxToRead)
		nRead = maxToRead;

		memcpy(toFill, m_pCurrent, nRead);
		m_pCurrent += nRead;
		return nRead;
	}

protected:

	const char* m_xml;
	XMLByte* m_pStart;
	XMLByte* m_pCurrent;

};

class MMFInputSource :
public xercesc::DOMInputSource
{

public:

	MMFInputSource(const char *xml)
	{
		m_xml = xml;
	}

	virtual ~MMFInputSource(void)
	{
	}

	virtual xercesc::BinInputStream* makeStream() const
	{
		return new MMFInputStream(m_xml);
	}

	const XMLCh * getEncoding () const {return NULL;}
	const XMLCh * getPublicId () const {return NULL;}
	const XMLCh * getSystemId () const {return NULL;}
	const XMLCh * getBaseURI () const {return NULL;}
	void setEncoding (const XMLCh *const encodingStr) {}
	void setPublicId (const XMLCh *const publicId) {}
	void setSystemId (const XMLCh *const systemId) {}
	void setBaseURI (const XMLCh *const baseURI) {}
	void setIssueFatalErrorIfNotFound (const bool flag) {}
	bool getIssueFatalErrorIfNotFound () const {return false;}
	void release () {}

protected:

	const char* m_xml;

};
#endif

}
;

#endif
