//=================================================================================================
//　@outline	XMLノード用のクラス(※未完成)
//　@other		予め stl の string と vector をインクルードしておく必要があります
//=================================================================================================
#pragma once

namespace kurobee {

class XMLNode {

	// element
	std::string element_;

	// attribute
	struct AT {
		std::string name;
		std::string value;
		AT() {}
		AT(const char *_name, const char *_value) : name(_name), value(_value) {}
	}; std::vector<AT> attribute_;

	// text
	std::string text_;

	// prev( parent )
	XMLNode *pPrev_;

	// next( children )
	std::vector<XMLNode*> pNext_;

public:

	// constructor & destructor
	XMLNode() : element_(), attribute_(0), text_(), pPrev_(NULL), pNext_(0) {}
	~XMLNode() {}

	//=============================================================================================
	//　@outline	親ノードを結合＆取得
	//=============================================================================================
	void setPrevNode(XMLNode *pNode)
	{
		pPrev_ = pNode;
	}

	XMLNode *getPrevNode()
	{
		return pPrev_;
	}

	//=============================================================================================
	//　@outline	子ノードを結合＆取得
	//=============================================================================================
	void setNextNode(XMLNode *pNode)
	{
		pNext_.push_back( pNode );
	}

	XMLNode *getNextNode(const char *name, int no = 0)
	{
		for(size_t i = 0; i < pNext_.size(); i++)
		{
			if( pNext_[i]->element_ == name )
			{
				if( --no < 0 )
				{
					return pNext_[i];
				}
			}
		}
		return NULL;
	}

	// iterator
	XMLNode *getNextNode(size_t no)
	{
		if( no < pNext_.size() )
		{
			return pNext_[no];
		}
		return NULL;		
	}

	size_t getNextNodeSize()
	{
		return pNext_.size();
	}

	//=============================================================================================
	//　@outline	要素へのアクセサ
	//=============================================================================================
	const char *getElement()
	{
		return element_.c_str();
	}

	void setElement(const char *name)
	{
		element_ = name;
	}

	//=============================================================================================
	//　@outline	属性へのアクセサ
	//=============================================================================================
	void setAttribute(const char *name, const char *value)
	{
		for(size_t i = 0; i < attribute_.size(); i++)
		{
			if( attribute_[i].name == name )
			{
				attribute_[i].value = value;
				return;
			}
		}
		attribute_.push_back( AT(name, value) );
	}

	const char *getAttribute(const char *name)
	{
		for(size_t i = 0; i < attribute_.size(); i++)
		{
			if( attribute_[i].name == name )
			{
				return attribute_[i].value.c_str();
			}
		}
		return NULL;
	}

	//=============================================================================================
	//　@outline	文章へのアクセサ
	//=============================================================================================
	void setText(const char *text)
	{
		text_ = text;
	}
	
	const char *getText()
	{
		return text_.c_str();
	}

};

}//namespace
