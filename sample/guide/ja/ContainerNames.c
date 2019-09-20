#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[]){

	GSGridStore *store = NULL;
	GSContainer *container;
	GSResult ret;
	size_t stackSize;
	GSResult errorCode;
	GSChar errMsgBuf1[1024], errMsgBuf2[1024];	// エラーメッセージを格納するバッファ
	int i, j;

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
	// コンテナ名一覧を取得する
	//===============================================
	{
		GSPartitionController * pc;
		int32_t pcCount;
		const GSChar *const * nameList;
		size_t nameCount;

		// (1)パーティションコントローラと、パーティション数を取得する
		ret = gsGetPartitionController(store, &pc);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsGetPartitionController\n");
			goto LABEL_ERROR;
		}
		gsGetPartitionCount(pc, &pcCount);


		// (2)パーティション数でループして、コンテナ名一覧を取得する
		for( i = 0; i < pcCount; i++) {
			ret = gsGetPartitionContainerNames(pc, i, 0, NULL, &nameList, &nameCount);
			if ( !GS_SUCCEEDED(ret) ){
				fprintf(stderr, "ERROR gsGetPartitionContainerNames\n");
				goto LABEL_ERROR;
			}
			
			for( j = 0; j < nameCount; j++ ){
				printf("%s\n", *nameList);
				nameList++;
			}
		}
	}

	//===============================================
	// 終了処理
	//===============================================
	// リソースを解放する
	gsCloseGridStore(&store, GS_TRUE);
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
