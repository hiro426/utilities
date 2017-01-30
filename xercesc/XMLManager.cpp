#include <stdafx.h>
using namespace std;

// Xerces_C (DOM)
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
using namespace xercesc;

#include <kurobee/utility/debug.h>
#include <kurobee/utility/extend.h>
#include <kurobee/utility/macro.h>
using namespace kurobee;

#include "XMLManager.h"

namespace kurobee {

//=================================================================================================
//　@outline
//=================================================================================================
void XMLManager::init() throw(const char*)
{
	try {
		XMLPlatformUtils::Initialize();
	} catch(...) {
		__throwError("xerces initialize error\n");
	}
}

//=================================================================================================
//　@outline
//=================================================================================================
void XMLManager::destroy()
{
	XMLPlatformUtils::Terminate();
}

//=================================================================================================
//　@outline
//=================================================================================================
bool XMLManager::load(const char *path) throw(const char*)
{
	pRootNode_ = new XMLNode();

	//xercesを用いて読み込みを行う
	static const XMLCh gLS[] = {chLatin_L, chLatin_S, chNull};
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(gLS);
	DOMLSParser* parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

	try {
		xercesc_3_1::DOMDocument* document = parser->parseURI(path);
		if(document == NULL) throw "";
		loadNode( pRootNode_, document );
	} catch(...) {
		// __throwError("XMLManager load error : file %s\n", path);
		__dbgPrintf("XMLManager load error : file %s\n", path);
		return false;
	}

	parser->release();

	return true;
}

//=================================================================================================
//　@outline
//=================================================================================================
void XMLManager::unload()
{
	unloadNode();
	delete pRootNode_;
	pRootNode_ = NULL;
}

//=================================================================================================
//　@outline
//=================================================================================================
void XMLManager::loadNode(XMLNode *pXMLNode, DOMNode *pDOMNode)
{
	if(pDOMNode == NULL)
		return;

	// read
	switch( pDOMNode->getNodeType() ) {

	case DOMNode::ELEMENT_NODE:
		{
			DOMElement *element = dynamic_cast<DOMElement*>(pDOMNode);
			char *element_name = XMLString::transcode( element->getTagName() );

			//_XMLNode *pNext = new _XMLNode();
			XMLNode *pNext = new XMLNode();

			// element
			pNext->setElement( element_name );

			// attribute
			DOMNamedNodeMap *pNodeMap = element->getAttributes();
			for(size_t i = 0; i < pNodeMap->getLength(); i++)
			{
				DOMAttr *attr = dynamic_cast<DOMAttr*>( pNodeMap->item(i) );
//				char *attr_name = XMLString::transcode( attr->getName() );
//				char *attr_value = XMLString::transcode( attr->getValue() );
				char attr_name[BUFFERSIZE], attr_value[BUFFERSIZE];
				_WideCharToMultiByte(attr_name, attr->getName(), BUFFERSIZE);
				_WideCharToMultiByte(attr_value, attr->getValue(), BUFFERSIZE);
				pNext->setAttribute( attr_name, attr_value );
//				XMLString::release( &attr_value );
//				XMLString::release( &attr_name );
			}

			pNext->setPrevNode( pXMLNode );
			pXMLNode->setNextNode( pNext );
			XMLString::release( &element_name );

			// 再帰
			DOMNode *child = pDOMNode->getFirstChild();
			while( child != NULL )
			{
				loadNode(pNext, child);
				child = child->getNextSibling();
			}
		}
		break;

	case DOMNode::TEXT_NODE:
		{
			DOMText *text = dynamic_cast<DOMText*>(pDOMNode);
			XMLCh *buf = new XMLCh[ XMLString::stringLen(text->getData())+1 ];
			XMLString::copyString(buf, text->getData());
			XMLString::trim(buf);
//			char *content = XMLString::transcode( buf );
			char content[BUFFERSIZE];
			_WideCharToMultiByte(content, buf, BUFFERSIZE);
			// copy
			pXMLNode->setText( content );

			//XMLString::release(&content);
			delete[] buf;
		}
		break;

	default:
		{
			// 再帰
			DOMNode *child = pDOMNode->getFirstChild();
			while( child != NULL ) {
				loadNode( pXMLNode, child );
				// 他の兄弟へ
				child = child->getNextSibling();
			}
		}
		break;

	}//switch

}//method

//=================================================================================================
//　@outline
//=================================================================================================
void XMLManager::unloadNode()
{
	vector<XMLNode*> vec_;
	// ローカル関数定義
	// クラスメソッド化することで実現させる
	struct VecManager {
		static void enumlate( vector<XMLNode*> &vec, XMLNode *pNode )
		{
			//if( pNode == NULL )
			//	return;

			//vec.push_back( pNode );
			size_t size = pNode->getNextNodeSize();
			for(size_t i = 0; i < size; i++)
			{
				XMLNode *pNext = pNode->getNextNode(i);
				vec.push_back( pNext );
				enumlate( vec, pNext );
			}
		}
	};
	VecManager::enumlate( vec_, pRootNode_ );

	for(size_t i = 0; i < vec_.size(); i++)
	{
		delete vec_[i];
	}
}

}//namespace
