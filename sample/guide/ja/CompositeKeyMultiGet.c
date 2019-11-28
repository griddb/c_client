#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

#define CONTAINER_COUNT 2
#define ROW_COUNT 5

void main(int argc, char *argv[]){

	/* 定数 */
	// コンテナ名
	const GSChar *const containerNameList[CONTAINER_COUNT] = { "SampleC_CompositeMultiGet1", "SampleC_CompositeMultiGet2" };
	const size_t containerCount = CONTAINER_COUNT;

	// 登録するロウデータ (各コンテナのロウ数5)
	const int rowCount = ROW_COUNT;
	const GSChar * nameList[ROW_COUNT] = { "notebook PC", "desktop PC", "keyboard", "mouse", "printer" };
	const int numberList[CONTAINER_COUNT][ROW_COUNT] = { { 108, 72, 25, 45, 62 }, { 50, 11, 208, 23, 153 } };

	/* 変数 */
	GSGridStore *store = NULL;
	GSContainer *container;
	GSResult ret;
	int i;
	int j;
	
	// コンテナ作成用
	GSContainerInfo info0 = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[3];
	
	// ロウ登録用
	GSRow *row;
	GSRow * rowList[ROW_COUNT];
	const void * const * rowObj;
	
	// 入力用の変数
	GSRowKey *rowKey;
	GSRowKeyPredicate *predicate;
	GSRowKeyPredicateEntry predEntryValueList[CONTAINER_COUNT];
	const GSRowKeyPredicateEntry *predEntryList[CONTAINER_COUNT];

	// 出力用の変数
	const GSContainerRowEntry *outEntryList;
	size_t outEntryCount;
	GSContainerRowEntry outEntry = GS_CONTAINER_ROW_ENTRY_INITIALIZER;
	GSRow *allRowList[CONTAINER_COUNT][ROW_COUNT];
	int id;
	GSChar const *productName;
	int count;

	// エラー処理用
	size_t stackSize;
	GSResult errorCode;
	GSChar errMsgBuf1[1024], errMsgBuf2[1024];	// エラーメッセージを格納するバッファ
	
	
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

	/*
	// 接続情報を指定する (固定リスト方式)
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

	// 複合ロウキー設定
	const int32_t rowKeyColumnList[] = { 0, 1 };
	const size_t rowKeyColumnCount = sizeof(rowKeyColumnList) / sizeof(*rowKeyColumnList);

	GSBool exists;

	// GridStoreインスタンスを取得する
	ret = gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetGridStore\n");
		goto LABEL_ERROR;
	}
	// コンテナ作成や取得などの操作を行うと、クラスタに接続される
	ret = gsGetContainerGeneral(store, "containerName", &container);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetContainerGeneral\n");
		goto LABEL_ERROR;
	}
	gsCloseContainer(&container, GS_TRUE);
	printf("Connect to Cluster\n");
	
	
	//===============================================
	// データ準備 (コンテナ作成＆ロウ登録)
	//===============================================
	// コンテナ情報を設定する
	info0.type = GS_CONTAINER_COLLECTION;
	info0.rowKeyColumnList = rowKeyColumnList;
	info0.rowKeyColumnCount = rowKeyColumnCount;
	
	columnInfo.name = "id";
	columnInfo.type = GS_TYPE_INTEGER;
	columnInfoList[0] = columnInfo;

	columnInfo.name = "productName";
	columnInfo.type = GS_TYPE_STRING;
	columnInfoList[1] = columnInfo;

	columnInfo.name = "count";
	columnInfo.type = GS_TYPE_INTEGER;
	columnInfoList[2] = columnInfo;

	info0.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info0.columnInfoList = columnInfoList;

	for ( i = 0; i < containerCount; i++ ){
		// コレクションを作成する
		ret = gsPutContainerGeneral(store, containerNameList[i], &info0, GS_FALSE, &container);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsPutCollectionGeneral\n");
			goto LABEL_ERROR;
		}
		printf("Sample data generation: Create Collection name=%s\n", containerNameList[i]);

		// 複数のロウを登録する
		for ( j = 0; j < rowCount; j++ ){
			gsCreateRowByContainer(container, &row);
			gsSetRowFieldByInteger(row, 0, j);
			gsSetRowFieldByString(row, 1, nameList[j]);
			gsSetRowFieldByInteger(row, 2, numberList[i][j]);

			rowList[j] = row;
		}
		rowObj = (void *)rowList;
		ret = gsPutMultipleRows(container, rowObj, rowCount, NULL);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsPutMultipleRows\n");
			goto LABEL_ERROR;
		}
		printf("Sample data generation: Put Rows count=%d\n", rowCount);
		
		gsCloseContainer(&container, GS_TRUE);
	}
	
	
	//===============================================
	// 複数のコンテナから一括でロウを取得する
	//===============================================

	// (1)ロウ取得条件を構築する
	for (i = 0; i < containerCount; i++) {
		ret = gsCreateRowKeyByStore(store, &info0, &rowKey);
		if (!GS_SUCCEEDED(ret)) {
			fprintf(stderr, "ERROR gsCreateRowKeyByStore\n");
			goto LABEL_ERROR;
		}

		ret = gsSetRowFieldByInteger(rowKey, 0, 0);
		if (!GS_SUCCEEDED(ret)) {
			fprintf(stderr, "ERROR gsSetRowFieldByInteger\n");
			goto LABEL_ERROR;
		}

		ret = gsSetRowFieldByString(rowKey, 1, "notebook PC");
		if (!GS_SUCCEEDED(ret)) {
			fprintf(stderr, "ERROR gsSetRowFieldByInteger\n");
			goto LABEL_ERROR;
		}

		ret = gsCreateRowKeyPredicateGeneral(store, &info0, &predicate);
		if (!GS_SUCCEEDED(ret)) {
			fprintf(stderr, "ERROR gsCreateRowKeyPredicateGeneral\n");
			goto LABEL_ERROR;
		}

		ret = gsAddPredicateGeneralKey(predicate, rowKey);
		if (!GS_SUCCEEDED(ret)) {
			fprintf(stderr, "ERROR gsAddPredicateGeneralKey\n");
			goto LABEL_ERROR;
		}

		// GSRowKeyPredicateEntryに条件を格納する
		predEntryValueList[i].containerName = containerNameList[i];
		predEntryValueList[i].predicate = predicate;

		predEntryList[i] = &predEntryValueList[i];
	}

	// (2)複数コンテナからロウを一括取得する
	ret = gsGetMultipleContainerRows(store, predEntryList, containerCount, &outEntryList, &outEntryCount);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetMultipleContainerRows\n");
		goto LABEL_ERROR;
	}

	// (3)取得結果をコピーする
	// ※gsGetRowFieldAsStringなど、結果を取得する関数は、
	//   値の書き換えが行われうる仕様
	//   関数呼び出し前に結果はコピーしておく
	for (i = 0; i < outEntryCount; i++) {
		outEntry = outEntryList[i];
		for (j = 0; j < outEntry.rowCount; j++) {
			allRowList[i][j] = (GSRow*)outEntry.rowList[j];
		}
	}

	// (4)取得結果を出力する
	for (i = 0; i < outEntryCount; i++) {
		for (j = 0; j < outEntryList[i].rowCount; j++) {
			row = allRowList[i][j];

			gsGetRowFieldAsInteger(row, 0, &id);
			gsGetRowFieldAsString(row, 1, &productName);
			gsGetRowFieldAsInteger(row, 2, &count);

			printf("MultiGet: container=%s, id=%d, productName=%s, count=%d\n", outEntryList[i].containerName, id, productName, count);
		}
	}

	//===============================================
	// 終了処理
	//===============================================
	// リソースを解放する
	gsCloseGridStore(&store, GS_TRUE);
	printf("success!\n");
	return;


LABEL_ERROR:
	{
		int i;
		//===============================================
		// エラー処理
		//===============================================
		// エラーコードとエラーメッセージを出力する
		stackSize = gsGetErrorStackSize(store);
		for (i = 0; i < stackSize; i++ ){
			errorCode = gsGetErrorCode(store, i);
			gsFormatErrorMessage(store, i, errMsgBuf1, sizeof(errMsgBuf1));
			gsFormatErrorLocation(store, i, errMsgBuf2, sizeof(errMsgBuf2));
			fprintf(stderr, "[%d] %s (%s)\n", errorCode, errMsgBuf1, errMsgBuf2);
		}

		// リソースを解放する
		gsCloseGridStore(&store, GS_TRUE);
		return;
	}
}
