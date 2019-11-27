#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[]) {

	/* 定数 */
	// コンテナ名
	const GSChar *const containerNameList[2] = { "SampleC_MultiGet1", "SampleC_MultiGet2" };
	const size_t containerCount = 2;

	// 登録するロウデータ (各コンテナのロウ数5)
	const int rowCount = 5;
	const GSChar * nameList[5] = { "notebook PC", "desktop PC", "keyboard", "mouse", "printer" };
	const int numberList[2][5] = { { 108, 72, 25, 45, 62 }, { 50, 11, 208, 23, 153 } };

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
	GSRow * rowList[5];
	const void * const * rowObj;

	// 入力用の変数
	GSRowKeyPredicate *predicate;
	GSRowKeyPredicateEntry predEntryValueList[2];
	const GSRowKeyPredicateEntry *predEntryList[2];

	// 出力用の変数
	const GSContainerRowEntry *outEntryList;
	size_t outEntryCount;
	GSContainerRowEntry outEntry = GS_CONTAINER_ROW_ENTRY_INITIALIZER;
	GSRow *allRowList[2][5];
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


	// GridStoreインスタンスを取得する
	ret = gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);
	if (!GS_SUCCEEDED(ret)) {
		fprintf(stderr, "ERROR gsGetGridStore\n");
		goto LABEL_ERROR;
	}
	// コンテナ作成や取得などの操作を行うと、クラスタに接続される
	ret = gsGetContainerGeneral(store, "containerName", &container);
	if (!GS_SUCCEEDED(ret)) {
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
	info0.rowKeyAssigned = GS_TRUE;

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

	for (i = 0; i < containerCount; i++) {
		// コレクションを作成する
		ret = gsPutContainerGeneral(store, containerNameList[i], &info0, GS_FALSE, &container);
		if (!GS_SUCCEEDED(ret)) {
			fprintf(stderr, "ERROR gsPutCollectionGeneral\n");
			goto LABEL_ERROR;
		}
		printf("Sample data generation: Create Collection name=%s\n", containerNameList[i]);

		// 複数のロウを登録する
		for (j = 0; j < rowCount; j++) {
			gsCreateRowByContainer(container, &row);
			gsSetRowFieldByInteger(row, 0, j);
			gsSetRowFieldByString(row, 1, nameList[j]);
			gsSetRowFieldByInteger(row, 2, numberList[i][j]);

			rowList[j] = row;
		}
		rowObj = (void *)rowList;
		ret = gsPutMultipleRows(container, rowObj, rowCount, NULL);
		if (!GS_SUCCEEDED(ret)) {
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

		// GSRowKeyPredicateを生成
		gsCreateRowKeyPredicate(store, GS_TYPE_INTEGER, &predicate);

		// 個別条件を設定 (ロウキーの値が0または2)
		gsAddPredicateKeyByInteger(predicate, 0);
		gsAddPredicateKeyByInteger(predicate, 2);

		// GSRowKeyPredicateEntryに条件を格納する
		predEntryValueList[i].containerName = containerNameList[i];
		predEntryValueList[i].predicate = predicate;

		predEntryList[i] = &predEntryValueList[i];
	}

	// (2)複数コンテナからロウを一括取得する
	ret = gsGetMultipleContainerRows(store, predEntryList, containerCount, &outEntryList, &outEntryCount);
	if (!GS_SUCCEEDED(ret)) {
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

			printf("MultiGet: container=%s, id=%d, productName=%s, count=%d\n", outEntry.containerName, id, productName, count);
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
		for (i = 0; i < stackSize; i++) {
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
