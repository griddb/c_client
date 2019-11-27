#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

void main(int argc, char *argv[]){

	GSGridStore *store;
	GSCollection *collection;
	GSQuery *query;
	GSRowSet *rs;
	GSRow *row;
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[3];
	GSResult ret;
	size_t stackSize;
	GSResult errorCode;
	GSChar errMsgBuf1[1024], errMsgBuf2[1024];	// エラーメッセージを格納するバッファ
	int i;

	//===============================================
	// クラスタに接続する
	//===============================================
	// 接続情報を指定する (マルチキャスト方式)
	const GSPropertyEntry props[] = {
		{ "notificationAddress", "239.0.0.1" },
		{ "notificationPort", "31999" },
		{ "clusterName", "myCluster" },
		{ "database", "public" },
		{ "user", "admin" },
		{ "password", "admin" },
		{ "applicationName", "SampleC" }
	};

	
	// 接続情報を指定する (固定リスト方式)
	/*
	const GSPropertyEntry props[] = {
		{ "notificationMember", "192.168.10.110:10001" },
		{ "clusterName", "myCluster" },
		{ "database", "public" },
		{ "user", "admin" },
		{ "password", "admin" },
		{ "applicationName", "SampleC" }
	};
	*/
	
	const size_t propCount = sizeof(props) / sizeof(*props);
	const int32_t rowKeyColumnList[] = { 0, 1 };
	const size_t rowKeyColumnCount = sizeof(rowKeyColumnList) / sizeof(*rowKeyColumnList);
	const GSChar *productType;
	int32_t productId;
	const GSChar *productName;

	// GridStoreインスタンスを取得する
	ret = gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetGridStore\n");
		goto LABEL_ERROR;
	}
	// コンテナ作成や取得などの操作を行うと、クラスタに接続される
	ret = gsGetContainerGeneral(store, "SampleC_CompositKey", &collection);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetContainerGeneral\n");
		goto LABEL_ERROR;
	}

	//===============================================
	// コレクションを作成する
	//===============================================
	// コンテナ種別を設定する
	info.type = GS_CONTAINER_COLLECTION;

	// ロウキーありの場合は設定する
	info.rowKeyColumnList = rowKeyColumnList;
	info.rowKeyColumnCount = rowKeyColumnCount;

	// カラム情報を定義する
	columnInfo.name = "productType";
	columnInfo.type = GS_TYPE_STRING;
	columnInfoList[0] = columnInfo;

	columnInfo.name = "productId";
	columnInfo.type = GS_TYPE_INTEGER;
	columnInfoList[1] = columnInfo;

	columnInfo.name = "productaName";
	columnInfo.type = GS_TYPE_STRING;
	columnInfoList[2] = columnInfo;

	// カラム情報をコンテナ情報オブジェクトに設定する
	info.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info.columnInfoList = columnInfoList;

	// コレクションを作成する
	fprintf(stdout, "Create Collection CompositKey\n");
	ret = gsPutContainerGeneral(store, "CompositKey", &info, GS_FALSE, &collection);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsPutContainerGeneral\n");
		goto LABEL_ERROR;
	}
	
	// 空のロウオブジェクトの作成
	ret = gsCreateRowByContainer(collection, &row);
	if (!GS_SUCCEEDED(ret)) {
		fprintf(stderr, "ERROR gsCreateRowByContainer\n");
		goto LABEL_ERROR;
	}

	// カラム値をセット
	gsSetRowFieldByString(row, 0, "Food");
	gsSetRowFieldByInteger(row, 1, 1);
	gsSetRowFieldByString(row, 2, "apple");

	// ロウを登録する
	ret = gsPutRow(collection, NULL, row, NULL);
	if (!GS_SUCCEEDED(ret)) {
		fprintf(stderr, "ERROR gsPutRow\n");
		goto LABEL_ERROR;
	}

	printf("Put Row\n");

	ret = gsQuery(collection, "SELECT *", &query);
	if (!GS_SUCCEEDED(ret)) {
		fprintf(stderr, "ERROR gsQuery\n");
		goto LABEL_ERROR;
	}
	ret = gsFetch(query, GS_FALSE, &rs);
	if (!GS_SUCCEEDED(ret)) {
		fprintf(stderr, "ERROR gsFetch\n");
		goto LABEL_ERROR;
	}

	// 空のロウを作成
	gsCreateRowByContainer(collection, &row);

	// 結果を取得する
	while (gsHasNextRow(rs)) {
		// ロウを取得する
		ret = gsGetNextRow(rs, row);
		if (!GS_SUCCEEDED(ret)) {
			fprintf(stderr, "ERROR gsGetNextRow\n");
			goto LABEL_ERROR;
		}

		gsGetRowFieldAsString(row, 0, &productType);
		gsGetRowFieldAsInteger(row, 1, &productId);
		gsGetRowFieldAsString(row, 2, &productName);

		printf("id=%s, productName=%d, count=%s\n", productType, productId, productName);
	}
	
	//===============================================
	// 終了処理
	//===============================================
	// リソースを解放する
	gsCloseGridStore(&store, GS_TRUE);
	
	printf("success!\n");

	return;


LABEL_ERROR:
	//===============================================
	// エラー処理
	//===============================================
	// エラーコードとエラーメッセージを出力する
	stackSize = gsGetErrorStackSize(store);
	for ( i = 0; i < stackSize; i++ ){
		errorCode = gsGetErrorCode(store, i);
		gsFormatErrorMessage(store, i, errMsgBuf1, sizeof(errMsgBuf1));
		gsFormatErrorLocation(store, i, errMsgBuf2, sizeof(errMsgBuf2));
		printf("[%d] %s (%s)\n", errorCode, errMsgBuf1, errMsgBuf2);
	}

	// リソースを解放する
	gsCloseGridStore(&store, GS_TRUE);
	return;

}
