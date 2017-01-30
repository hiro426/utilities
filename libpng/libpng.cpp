//=================================================================================================
//　@outline	LibPng 実装部
//=================================================================================================
#include <stdio.h>
#include <stdlib.h>

// libpng
#include <libpng/png.h>

#include "libpng.h"

namespace libpng {

//=================================================================================================
//　@outline	PNG画像の読み込み
//　@argument	const char *filename	ロードするPNGファイルへのパス
//　@return		HPNGPROPERTY			データを格納している構造体へのポインタ(ハンドル)
//　@other		malloc() の failsafe を書くと、ソースが見辛くなるので省いています
//=================================================================================================
HPNGPROPERTY loadPng(const char *filename)
{
	// ファイルを開く
	FILE *fp = fopen(filename, "rb");
	if(fp == NULL) {
		return NULL;
	}

	// pnt_struct 作成
	png_struct *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL) {
		fclose(fp);
		return NULL;
	}
	
	// png_info 作成
	png_info *info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(fp);
		return NULL;
	}
	
	// libpngI/Oを初期化(fp登録)
	png_init_io(png_ptr, fp);

	// ヘッダーの読み込み
	png_read_info(png_ptr, info_ptr);
	
	// チャンク情報格納用
	png_uint_32 width, height;	// 画像サイズ
	int depth;					// 深度
	int colorType;				// カラータイプ
	
	// チャンク情報を取得
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &depth, &colorType, NULL, NULL, NULL);

	// パレット用
	int paletteSize = 0;
	unsigned int *palette = NULL;

	// パレットを保有しているなら
	if(colorType == PNG_COLOR_TYPE_PALETTE) {
		
		// パレット読み込み(自分で解放はしないこと！)
		png_colorp p = NULL;
		png_get_PLTE(png_ptr, info_ptr, &p, &paletteSize);
		
		// 透過情報読み込み(自分で解放はしないこと！)
		png_bytep pt = NULL;
		int nt = 0;
		png_color_16p vt = NULL;
		png_get_tRNS(png_ptr, info_ptr, &pt, &nt, &vt);
		
		// 読み込んだパレット情報を配列に格納する
		palette = reinterpret_cast<unsigned int*>(malloc(paletteSize*4));	// rgba各1byteずつ
		for(int i = 0; i < paletteSize; i++) {
			
			// variable
			unsigned int rgba = 0;
			
			// rgb
			rgba += p[i].red;	rgba <<= 8;
			rgba += p[i].green;	rgba <<= 8;
			rgba += p[i].blue;	rgba <<= 8;
			
			// alpha
			if(pt == NULL)	rgba += 0xff;
			else			rgba += pt[i];
			
			// substitution
			palette[i] = rgba;
			
		}
		
	}
	
	// データ部の読み込みのため 配列の配列を作成
	unsigned int rowByte = png_get_rowbytes(png_ptr, info_ptr);							// 1lineのbyte数
	png_bytep *ppimg = reinterpret_cast<png_bytep*>(malloc(height*sizeof(png_bytep)));	// column
	for(int i = 0; i < height; i++) {
		ppimg[i] = reinterpret_cast<png_bytep>(malloc(rowByte));						// row
	}

	// データ部の読み込み
	png_read_image(png_ptr, ppimg);
	
	// データ部をコピーするための容量確保 1次元配列
	png_byte *pData = NULL;
	pData = reinterpret_cast<png_byte*>(malloc(rowByte*height));
	
	// コピー(1行ずつコピーする)
	for(int i = 0; i < height; i++) {
		memcpy(pData+rowByte*i, ppimg[i], rowByte);
	}
	
	// データ部の読み込みのために確保した領域を開放
	for(int i = 0; i < height; i++) {
		free(ppimg[i]);
	}
	free(ppimg);
	
	// エンドデータの読み込み(必要ないのでNULL)
	png_read_end(png_ptr, NULL);

	// 破棄処理
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);		// 構造体の破棄
	fclose(fp);												// ファイルを閉じる
	
	// プロパティを作成して代入
	HPNGPROPERTY hProperty = reinterpret_cast<PNGPROPERTY*>(malloc(sizeof(PNGPROPERTY)));
	hProperty->width = width;
	hProperty->height = height;
	hProperty->depth = depth;
	hProperty->rowByte = rowByte;
	hProperty->colorType = colorType;
	hProperty->paletteSize = paletteSize;
	hProperty->pPalette = palette;
	hProperty->pData = pData;

	// 作成したプロパティを返す( succeed! )
	return hProperty;
}

//=================================================================================================
//　@outline	PNG画像の破棄
//　@argument	HPNGPROPERTY &hProperty		削除するハンドル
//=================================================================================================
void unloadPng(HPNGPROPERTY &hProperty)
{
	// アロー演算子を使用するためfailsafe
	if(hProperty != NULL) {
		// freeは引数がNULLの場合何もしない
		free(hProperty->pData);
		free(hProperty->pPalette);
		free(hProperty);
		hProperty = NULL;
	}
}

}//namespace
