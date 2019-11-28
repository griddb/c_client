#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[]){

	/* 定数 */
	// コンテナ名
	const GSChar * containerName = "SampleC_RemoveRowByRowkey";
	
	// 登録するロウのデータ (コンテナのロウ数 5)
	const int rowCount = 5;
	const GSChar * nameList[5] = {"notebook PC", "desktop PC", "keyboard", "mouse", "printer"};
	const int numberList[5] = {108, 72, 25, 45, 62};
	
	
	/* 変数 */
	GSGridStore *store = NULL;
	GSContainer *container;
	GSResult ret;
	int i;
	int j;
	
	// コンテナ作成用
	GSContainerInfo info0 = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[3];	// カラム数 3
	
	// ロウ登録用
	GSRow *row;
    GSRow * rowList[5];
    const void * const * rowObj;
	
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
	// データ準備 (コンテナ作成＆ロウ登録)
	//===============================================
	{
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

		// コレクションを作成する
		ret = gsPutContainerGeneral(store, containerName, &info0, GS_FALSE, &container);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsPutCollectionGeneral\n");
			goto LABEL_ERROR;
		}
		printf("Sample data generation: Create Collection name=%s\n", containerName);

		for ( i = 0; i < rowCount; i++ ){
			// 空のロウオブジェクトを生成する
			gsCreateRowByContainer(container, &row);

			// ロウオブジェクトに値をセットする
			gsSetRowFieldByInteger(row, 0, i);
			gsSetRowFieldByString(row, 1, nameList[i]);
			gsSetRowFieldByInteger(row, 2, numberList[i]);

			rowList[i] = row;
		}
	    rowObj = (void *)rowList;

		// 複数のロウを登録する
		ret = gsPutMultipleRows(container, rowObj, rowCount, NULL);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsPutMultipleRows\n");
			goto LABEL_ERROR;
		}
		printf("Sample data generation: Put Rows count=%d\n", rowCount);
		
		gsCloseContainer(&container, GS_TRUE);
	}
	
	
	//===============================================
	// ロウを削除する
	//===============================================
	// コンテナを取得する
	ret = gsGetContainerGeneral(store, containerName, &container);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetContainerGeneral\n");
		goto LABEL_ERROR;
	}
	if ( container == NULL ){
		fprintf(stderr, "ERROR Container not found. name=%s\n", containerName);
		goto LABEL_ERROR;
	}
	
	// (1) ロウキーを指定してロウを削除する
	ret = gsDeleteRowByInteger(container, 3, NULL);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsDeleteRowByInteger rowkey=3\n");
		goto LABEL_ERROR;
	}

	printf("Delete Row rowkey=3\n");

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
