//=================================================================================================
//　@outline	libpngを使用したPNGの読み込み
//				画像サイズやパレット、色情報を構造体へ格納します
//　@other		ガンマ値やインタレースには非対応
//=================================================================================================
#pragma once

namespace libpng {

// 読み込んだPNGデータの保存用
typedef struct PNGPROPERTY {
	
	// 画像サイズ
	int width, height;
	
	// デプス
	// 1...2色インデックス
	// 2...4色インデックス
	// 4...16色インデックス
	// 8...256色インデックス、フルカラー
	int depth;
	
	// 1lineのバイト数
	unsigned int rowByte;
	
	// カラータイプ
	// グレイスケール...PNG_COLOR_TYPE_GRAY
	// インデックスカラー...PNG_COLOR_TYPE_PALETTE
	// フルカラー(トゥルーカラー)...PNG_COLOR_TYPE_RGB
	// フルカラー(アルファチャンネル)...PNG_COLOR_TYPE_RGB_ALPHA
	// グレイスケール(アルファチャンネル)...PNG_COLOR_TYPE_GRAY_ALPHA
	// ※詳しくは libpng の png.h を参照してください。
	int colorType;
	
	// カラーマップサイズ(パレット色数)
	// パレット無しの場合...0
	unsigned int paletteSize;
	
	// パレット情報(argb順)
	// パレット無しの場合...NULL
	// アルファマスクを持たない場合アルファ値は0xff
	unsigned int *pPalette;
	
	// 画像データ部
	// 画像のフォーマットによって意味が異なります。
	// 1bitIndex...1dotあたり1bitなので1byteに8dot分のデータ(index)が入る
	// 2bitIndex...1dotあたり2bitなので1byteに4dot分のデータ(index)が入る
	// 4bitIndex...1dotあたり4bitなので1byteに2dot分のデータ(index)が入る
	// 8bitIndex...1dotあたり1byteなので1byteに1dot分のデータ(index)が入る
	// 24bitFullColor...1dotあたり3byteで表します(rgb順)
	// 32bitFullColor...1dotあたり4byteで表します(rgba順)
	unsigned char *pData;
	
} *HPNGPROPERTY;

// load&unload method
HPNGPROPERTY loadPng(const char *filename);
void unloadPng(HPNGPROPERTY &pProperty);

}//namespace
