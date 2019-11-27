#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[]){

	/* 変数 */
	GSGridStore *store;
	GSContainer *container;
	GSResult ret;
	int i;
	
	// コンテナ作成用
	GSContainerInfo info0 = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[3];	// カラム数3

	// コンテナ情報取得用
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	GSBool exist;
	
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
		ret = gsPutCollectionGeneral(store, "SampleC_Info", &info0, GS_FALSE, &container);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsPutCollectionGeneral\n");
			goto LABEL_ERROR;
		}
		printf("Sample data generation: Create Collection name=SampleC_Info\n");
		
		gsCloseContainer(&container, GS_TRUE);
	}
	
	
	//===============================================
	// コンテナ情報を取得する
	//===============================================

	// (1)コンテナ情報を取得する
	ret = gsGetContainerInfo(store, "SampleC_Info", &info, &exist);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetContainerInfo\n");
		goto LABEL_ERROR;
	}
	if ( !exist ){
		printf("Container not found\n");
		goto LABEL_ERROR;
	}

	// (2)コンテナ情報を表示する
	printf("Get ContainerInfo: \n    name=%s\n", info.name);
	if ( info.type == GS_CONTAINER_COLLECTION ){
		printf("    type=Collection\n");
	} else {
		printf("    type=TimeSeries\n");
	}
	printf("    rowKeyAssigned=%d\n", info.rowKeyAssigned);

	printf("    columnCount=%d\n", info.columnCount);
	for ( i = 0; i < info.columnCount; i++ ){
		printf("    column (%s, %d)\n", info.columnInfoList[i].name, info.columnInfoList[i].type);
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
