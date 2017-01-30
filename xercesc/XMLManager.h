//=================================================================================================
//　@outline	XMLノードのマネージャークラス(※未完成：とりあえず読み込みはできる)
//　@other		xercesのDOMを使用しています
//=================================================================================================
#pragma once
#include "XMLNode.h"
#include <xercesc/dom/DOMNode.hpp>

namespace kurobee {

class XMLManager {

	// ルートノードへのポインタ
	XMLNode *pRootNode_;
	
	// コピーコンストラクタ封じ
	explicit XMLManager(const XMLManager&) {}
	
	// DOMNode を XMLNode へ
	void loadNode(XMLNode *pXMLNode, xercesc::DOMNode *pDOMNode);
	void unloadNode();

public:

	// コンストラクタ＆デストラクタ
	XMLManager() : pRootNode_(NULL) {}
	~XMLManager() {}

	// xercesの初期化＆破棄処理
	static void init() throw(const char*);
	static void destroy();

	// xmlのロード＆アンロード
	bool load(const char *path) throw(const char*);
	void unload();

	// アクセサ
	XMLNode *getRootNode()
	{
		return pRootNode_;
	}
	
	XMLNode *getNode(const char *hierarchy)
	{
		
		// exp."system/config:1"(system内configの二つ目のNodeを取得)
		XMLNode *pNode = getRootNode();
		if(!__isNullPtr(pNode))
		{;
			int num = 0;
			char element[BUFFERSIZE], rest[BUFFERSIZE], buf[BUFFERSIZE];
			strcpy(buf, hierarchy);
			
			while(true)
			{
				// /が出るまで読み込む
				sscanf(buf, "%[^/]/%s", element, rest);
				// :が出るまで(ry
				strcpy(buf, element);
				sscanf(buf, "%[^:]:%d", element, &num);
				// 次のノードを取得
				pNode = pNode->getNextNode(element, num);
				// 条件分岐
				if(pNode == NULL || rest[0] == '\0') {
					break;
				}
				// 次の読み取り準備(初期化)
				strcpy(buf, rest);
				rest[0] = '\0';
				num = 0;
			}
		}
		return pNode;
	}

};

}//namespace
