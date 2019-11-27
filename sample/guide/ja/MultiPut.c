#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[]){

	/* 定数 */
	// コンテナ名
	const GSChar *const containerNameList[2] = { "SampleC_MultiPut1", "SampleC_MultiPut2" };
	const size_t containerCount = 2;

	// 登録するロウデータ(各コンテナに5つのロウ)
	const size_t rowCount = 5;
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
	
	// MultiPut用
	GSRow *row;
	void *allRowList[2][5];
	GSContainerRowEntry inEntry = GS_CONTAINER_ROW_ENTRY_INITIALIZER;
	GSContainerRowEntry inEntryList[2];

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
	// データ準備 (コンテナ作成)
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

	for ( i = 0; i < containerCount; i++ ){
		// コレクションを作成する
		ret = gsPutContainerGeneral(store, containerNameList[i], &info0, GS_FALSE, &container);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsPutContainerGeneral\n");
			goto LABEL_ERROR;
		}
		printf("Create Collection name=%s\n", containerNameList[i]);

		gsCloseContainer(&container, GS_TRUE);
	}
	
	
	//===============================================
	// 複数コンテナにロウを一括登録する
	//===============================================

	for ( i = 0; i < containerCount; i++ ){
		// (1)コンテナ取得
		ret = gsGetContainerGeneral(store, containerNameList[i], &container);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsGetContainerGeneral\n");
			return;
		}
		if ( container == NULL ){
			fprintf(stderr, "ERROR Container not found. name=%s\n", containerNameList[i]);
			goto LABEL_ERROR;
		}
		
		// (2)複数のロウを生成
		for ( j = 0; j < rowCount; j++ ){
			// 空のロウオブジェクト生成
			gsCreateRowByContainer(container, &row);

			// ロウオブジェクトに値をセット
			gsSetRowFieldByInteger(row, 0, j);
			gsSetRowFieldByString(row, 1, nameList[j]);
			gsSetRowFieldByInteger(row, 2, numberList[i][j]);
			allRowList[i][j] = row;
		}

		// (3)GSContainerRowEntryにコンテナ情報とロウをセット
		inEntry.containerName = containerNameList[i];
		inEntry.rowList = allRowList[i];
		inEntry.rowCount = rowCount;
		inEntryList[i] = inEntry;
	}

	// (4)複数コンテナにロウを一括登録
	ret = gsPutMultipleContainerRows(store, inEntryList, containerCount);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsPutMultipleContainerRows\n");
		return;
	}

	printf("Multi Put\n");

	for (i = 0; i < containerCount; i++) {
		for (j = 0; j < rowCount; j++) {
			GSRow *row = allRowList[i][j];
			gsCloseRow(&row);
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
	//===============================================
	// エラー処理
	//===============================================
	// エラーコードとエラーメッセージを出力する
	stackSize = gsGetErrorStackSize(store);
	for ( i = 0; i < stackSize; i++ ){
		errorCode = gsGetErrorCode(store, i);
		gsFormatErrorMessage(store, i, errMsgBuf1, sizeof(errMsgBuf1));
		gsFormatErrorLocation(store, i, errMsgBuf2, sizeof(errMsgBuf2));
		fprintf(stderr, "[%d] %s (%s)\n", errorCode, errMsgBuf1, errMsgBuf2);
	}

	// リソースを解放する
	gsCloseGridStore(&store, GS_TRUE);
	return;

}
