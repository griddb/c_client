#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[]){

	/* 定数 */
	// コンテナ名
	const GSChar *const containerNameList[2] = { "SampleC_FetchAll1", "SampleC_FetchAll2" };
	const size_t containerCount = 2;
	
	// 各コンテナに対するTQL
	const GSChar *const tqlList[2] = { "select * where count > 60", 
										"select * where count > 100" };

	// 登録するロウのデータ (各コンテナのロウ数 5)
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
	GSColumnInfo columnInfoList[3];	// カラム数 3
	
	// ロウ登録用
	GSRow *row;
    GSRow * rowList[5];
    const void * const * rowObj;
	
	// FetchAll実行用
	GSContainer * containerList[2];
	GSQuery *queryList[2];
	GSRowSet *rs;
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
	// 複数のコンテナにTQLを実行する
	//===============================================
	
	// (1)各コンテナのTQLクエリを生成する
	for ( i = 0; i < containerCount; i++ ){
		ret = gsGetContainerGeneral(store, containerNameList[i], &containerList[i]);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsGetContainerGeneral\n");
			goto LABEL_ERROR;
		}
		if ( containerList[i] == NULL ){
			fprintf(stderr, "ERROR Container not found. name=%s\n", containerNameList[i]);
			goto LABEL_ERROR;
		}
		
		printf("FetchAll query : container=%s, tql=%s\n", containerNameList[i], tqlList[i]);
		ret = gsQuery(containerList[i], tqlList[i], &queryList[i]);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsQuery\n");
			goto LABEL_ERROR;
		}
	}
	

	// (2)複数コンテナにTQLを一括実行する
	ret = gsFetchAll(store, queryList, containerCount);
	if ( !GS_SUCCEEDED(ret) ){
		fprintf(stderr, "ERROR gsFetchAll\n");
		goto LABEL_ERROR;
	}

	// (3)結果を取得する
	for (i = 0; i < containerCount; i++) {

		// 空のロウオブジェクトを生成
		gsCreateRowByContainer(containerList[i], &row);

		// RowSetを取得
		ret = gsGetRowSet(queryList[i], &rs);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsGetRowSet\n");
			goto LABEL_ERROR;
		}

		// ロウを取得
		while (gsHasNextRow(rs)) {
			ret = gsGetNextRow(rs, row);
			if ( !GS_SUCCEEDED(ret) ){
				fprintf(stderr, "ERROR gsGetNextRow\n");
				goto LABEL_ERROR;
			}

			// 値を取得
			gsGetRowFieldAsInteger(row, 0, &id);
			gsGetRowFieldAsString(row, 1, &productName);
			gsGetRowFieldAsInteger(row, 2, &count);

			printf("FetchAll result: container=%s, row=(%d, \"%s\", %d)\n", containerNameList[i], id, productName, count);
		}

		gsCloseRow(&row);
		gsCloseRowSet(&rs);
		gsCloseQuery(&queryList[i]);
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
