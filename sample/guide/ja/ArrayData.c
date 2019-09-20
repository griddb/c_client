#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[]){

	GSGridStore *store = NULL;
	GSContainer *container;
	GSCollection *collection;
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[3];
	GSRow *row;
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
	// コレクションを作成する
	//===============================================
	// (1)コンテナ種別を設定する
	info.type = GS_CONTAINER_COLLECTION;

	// (2)ロウキーありの場合は設定する
	info.rowKeyAssigned = GS_TRUE;

	// (3)カラム情報を定義する
	columnInfo.name = "id";
	columnInfo.type = GS_TYPE_INTEGER;
	columnInfoList[0] = columnInfo;

	columnInfo.name = "string_array";
	columnInfo.type = GS_TYPE_STRING_ARRAY;
	columnInfoList[1] = columnInfo;

	columnInfo.name = "integer_array";
	columnInfo.type = GS_TYPE_INTEGER_ARRAY;
	columnInfoList[2] = columnInfo;


	// (4)カラム情報をコンテナ情報オブジェクトに設定する
	info.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info.columnInfoList = columnInfoList;

	// (5)コレクションを作成する
	ret = gsPutCollectionGeneral(store, "SampleC_ArrayData", &info, GS_FALSE, &collection);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsPutCollectionGeneral\n");
		goto LABEL_ERROR;
	}

	fprintf(stdout, "Create Collection name=SampleC_ArrayData\n");



	//===============================================
	// 配列型のデータを登録する
	//===============================================
	{
		// (1)配列型のデータ
		const GSChar *stringArray[4] = { "Sales", "Development", "Marketing", "Research" };
		const int32_t integerArray[4] = { 39, 92, 18, 51 };


		// (2)空のロウオブジェクトの作成
		ret = gsCreateRowByContainer(collection, &row);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsCreateRowByContainer\n");
			goto LABEL_ERROR;
		}

		// (3)カラム値をセット
		gsSetRowFieldByInteger(row, 0, 0);
		gsSetRowFieldByStringArray(row, 1, stringArray, 4);
		gsSetRowFieldByIntegerArray(row, 2, integerArray, 4);

		// (4)ロウを登録する
		ret = gsPutRow(collection, NULL, row, NULL);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsPutRow\n");
			goto LABEL_ERROR;
		}

		gsCloseRow(&row);

		printf("Put Row (Array)\n");
	}


	//===============================================
	// 配列型のデータを取得する
	//===============================================
	{
		int32_t id;
		const GSChar * const * stringArray;
		const int32_t * integerArray;
		size_t stringCount;
		size_t integerCount;

		// (1)空のロウオブジェクトの作成
		gsCreateRowByContainer(collection, &row);

		// (2)ロウキーを指定してロウ取得
		ret = gsGetRowByInteger(collection, 0, row, GS_FALSE, NULL);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsGetRowByInteger\n");
			goto LABEL_ERROR;
		}

		// (3)ロウから配列型データを取得
		gsGetRowFieldAsInteger(row, 0, &id);
		gsGetRowFieldAsStringArray(row, 1, &stringArray, &stringCount);
		gsGetRowFieldAsIntegerArray(row, 2, &integerArray, &integerCount);

		printf("Get Row (id=%d,", id);
		
		printf("[");
		for( i = 0; i < stringCount; i++ ){
			if ( i != 0 ) printf(", ");
			printf("%s", *stringArray);
			stringArray++;
		}
		printf("],[");

		for( i = 0; i < integerCount; i++ ){
			if ( i != 0 ) printf(", ");
			printf("%d", *integerArray);
			integerArray++;
		}
		printf("])\n");

		gsCloseRow(&row);
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
