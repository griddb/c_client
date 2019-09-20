#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[]){

	GSGridStore *store = NULL;
	GSContainer *container;
	GSCollection *collection;
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[2];
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
	info.type = GS_CONTAINER_COLLECTION;
	info.rowKeyAssigned = GS_TRUE;

	columnInfo.name = "id";
	columnInfo.type = GS_TYPE_INTEGER;
	columnInfoList[0] = columnInfo;

	columnInfo.name = "blob";
	columnInfo.type = GS_TYPE_BLOB;
	columnInfoList[1] = columnInfo;

	info.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info.columnInfoList = columnInfoList;

	ret = gsPutCollectionGeneral(store, "SampleC_BlobData", &info, GS_FALSE, &collection);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsPutCollectionGeneral\n");
		goto LABEL_ERROR;
	}
	fprintf(stdout, "Create Collection name=SampleC_BlobData\n");


	//===============================================
	// バイナリを登録する
	//===============================================
	{
		FILE* file;
		int i, size;
		char * data;
		GSBlob blob;

		// (1)バイナリファイルを読み込む
		file = fopen("BlobData.c", "rb");
		if( file == NULL ){
			fprintf(stderr, "ERROR file open\n");
			goto LABEL_ERROR;
		}
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		data = (char*)malloc(sizeof(char)*size);
		fseek(file,0,SEEK_SET);
		fread(data, sizeof(char), size, file);
		fclose(file); 

		// (2)空のロウオブジェクトの作成
		ret = gsCreateRowByContainer(collection, &row);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsCreateRowByContainer\n");
			goto LABEL_ERROR;
		}

		// (3)カラム値をセット
		gsSetRowFieldByInteger(row, 0, 0);

		blob.size = size;	// バイナリサイズ
		blob.data = data;	// バイナリデータ
		ret = gsSetRowFieldByBlob(row, 1, &blob);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsSetRowFieldByBlob\n");
			goto LABEL_ERROR;
		}

		// (4)ロウを登録する
		ret = gsPutRow(collection, NULL, row, NULL);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsPutRow\n");
			goto LABEL_ERROR;
		}

		gsCloseRow(&row);

		printf("Put Row (Blob)\n");
	}


	//===============================================
	// バイナリを取得する
	//===============================================
	{
		// (1)空のロウオブジェクトの作成
		gsCreateRowByContainer(collection, &row);

		// (2)ロウキーを指定してロウ取得
		ret = gsGetRowByInteger(collection, 0, row, GS_FALSE, NULL);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsGetRowByInteger\n");
			goto LABEL_ERROR;
		}

		// (3)ロウからバイナリ型データを取得
		GSBlob blob;
		gsGetRowFieldAsBlob(row, 1, &blob);

		printf("Get Row (Blob size=%d)\n", blob.size);
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
