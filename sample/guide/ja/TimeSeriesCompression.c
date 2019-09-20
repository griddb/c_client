#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

void main(int argc, char *argv[]){

	GSGridStore *store;
	GSContainer *container;
	GSTimeSeries *timeseries;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[2];
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
	// 時系列コンテナを作成する
	//===============================================
	{
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	GSTimeSeriesProperties timeProp = GS_TIME_SERIES_PROPERTIES_INITIALIZER;
	GSColumnCompression comp = GS_COLUMN_COMPRESSION_INITIALIZER;

	// (1)コンテナ種別を設定する
	info.type = GS_CONTAINER_TIME_SERIES;

	// (2)ロウキーを設定する
	info.rowKeyAssigned = GS_TRUE;

	// (3)カラム圧縮を設定する
	comp.columnName = "value";
	comp.width = 0.7;

	timeProp.compressionMethod = GS_COMPRESSION_HI;
	timeProp.compressionList = &comp;
	timeProp.compressionListSize = 1;

	info.timeSeriesProperties = &timeProp;

	// (4)カラム情報を定義する
	columnInfo.name = "date";
	columnInfo.type = GS_TYPE_TIMESTAMP;
	columnInfoList[0] = columnInfo;

	columnInfo.name = "value";
	columnInfo.type = GS_TYPE_DOUBLE;
	columnInfoList[1] = columnInfo;

	info.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info.columnInfoList = columnInfoList;

	// (5)時系列コンテナを作成する
	ret = gsPutTimeSeriesGeneral(store, "SampleC_Compression", &info, GS_FALSE, &timeseries);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsPutTimeSeriesGeneral\n");
		goto LABEL_ERROR;
	}

	fprintf(stdout, "Create TimeSeries & Set Compression name=SampleC_Compression\n");
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
