#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

void main(int argc, char *argv[]){

	GSGridStore *store;
	GSContainer *container;
	GSCollection *collection;
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[4];
	GSResult ret;
	size_t stackSize;
	GSResult errorCode;
	GSChar errMsgBuf1[1024], errMsgBuf2[1024];	// エラーメッセージを格納するバッファ
	int i;

	GSRow *row;
	GSPreciseTimestamp pts;//

	// データ取得用
	const GSChar *productName;
	int value;
	GSPreciseTimestamp pts1;//
	GSPreciseTimestamp pts2;//

	GSChar buf[GS_TIME_STRING_SIZE_MAX];
	GSTimestampFormatOption opt = GS_TIMESAMP_FORMAT_OPTION_INITIALIZER;

	//===============================================
	// [Connect]
	// クラスタに接続する
	//===============================================
	// 接続情報を指定する (マルチキャスト方式/Multicast method)
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
	// 接続情報を指定する (固定リスト方式/Fixed list method)
	const GSPropertyEntry props[] = {
		{ "notificationMember", "127.0.0.1:10001" },
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
	
	//===============================================
	// [Create Container]
	// コレクションを作成する
	//===============================================
	// (1)コンテナ種別を設定する
	info.type = GS_CONTAINER_COLLECTION;

	// (2)ロウキーありの場合は設定する
	info.rowKeyAssigned = GS_TRUE;

	// (3)カラム情報を定義する
	columnInfo.name = "time";
	columnInfo.type = GS_TYPE_TIMESTAMP;//
	columnInfo.options = GS_TYPE_OPTION_TIME_NANO;//
	columnInfoList[0] = columnInfo;

	columnInfo.name = "productName";
	columnInfo.type = GS_TYPE_STRING;
	columnInfo.options = 0;//
	columnInfoList[1] = columnInfo;

	columnInfo.name = "value";
	columnInfo.type = GS_TYPE_INTEGER;
	columnInfo.options = 0;//
	columnInfoList[2] = columnInfo;

	columnInfo.name = "time2";
	columnInfo.type = GS_TYPE_TIMESTAMP;//
	columnInfo.options = GS_TYPE_OPTION_TIME_MICRO;//
	columnInfoList[3] = columnInfo;

	// (4)カラム情報をコンテナ情報オブジェクトに設定する
	info.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info.columnInfoList = columnInfoList;

	// (5)コレクションを作成する
	ret = gsPutCollectionGeneral(store, "SampleC_collection1", &info, GS_FALSE, &collection);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsPutCollectionGeneral\n");
		goto LABEL_ERROR;
	}

	printf("Create Collection name=SampleC_collection1\n");

	//===============================================
	// [Put Row]
	// ロウを登録する
	//===============================================
	// (1)コンテナを取得する
	ret = gsGetContainerGeneral(store, "SampleC_collection1", &container);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetContainerGeneral\n");
		goto LABEL_ERROR;
	}
	if ( container == NULL ){
		fprintf(stderr, "ERROR Container not found. name=SampleC_PutRow\n");
		goto LABEL_ERROR;
	}	

	// (2)空のロウオブジェクトの作成
	ret = gsCreateRowByContainer(container, &row);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsCreateRowByContainer\n");
		goto LABEL_ERROR;
	}

	gsParsePreciseTime("2023-05-18T10:41:26.101123456Z", &pts, &opt);//

	// (3)カラム値をセット
	gsSetRowFieldByPreciseTimestamp(row, 0, &pts);//
	gsSetRowFieldByString(row, 1, "display");
	gsSetRowFieldByInteger(row, 2, 1);
	gsSetRowFieldByPreciseTimestamp(row, 3, &pts);//

	// (4)ロウを登録する
	ret = gsPutRowGeneral(container, NULL, row, NULL);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsPutRowGeneral\n");
		goto LABEL_ERROR;
	}

	printf("Put Row\n");	

	gsCloseContainer(&container, GS_TRUE);

	//===============================================
	// [Get Row]
	// ロウを取得する
	//===============================================
	// (1)コンテナを取得する
	ret = gsGetContainerGeneral(store, "SampleC_collection1", &container);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetContainerGeneral\n");
		goto LABEL_ERROR;
	}
	if ( container == NULL ){
		fprintf(stderr, "ERROR Container not found. name=%s\n", "SampleC_collection1");
		goto LABEL_ERROR;
	}

	// (2)空のロウオブジェクトの作成
	gsCreateRowByContainer(container, &row);

	// (3)ロウキーを指定してロウ取得
	ret = gsGetRow(container, &pts, row, NULL);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsGetRow\n");
		goto LABEL_ERROR;
	}

	// (4)ロウから値を取得
	gsGetRowFieldAsPreciseTimestamp(row, 0, &pts1);//
	gsGetRowFieldAsString(row, 1, &productName);
	gsGetRowFieldAsInteger(row, 2, &value);
	gsGetRowFieldAsPreciseTimestamp(row, 3, &pts2);//

	printf("Get Row (");
	gsFormatPreciseTime(&pts1, buf, GS_TIME_STRING_SIZE_MAX, &opt);//
	printf("time=%s, ", buf);
	printf("productName=%s, value=%d, ", productName, value);
	gsFormatPreciseTime(&pts2, buf, GS_TIME_STRING_SIZE_MAX, &opt);//
	printf("time2=%s)\n", buf);

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
