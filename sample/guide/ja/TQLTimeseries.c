#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[]){

	/* 定数 */
	// コンテナ名
	const GSChar * containerName = "SampleC_TQLTimeseries";
	
	// 登録するロウのデータ (コンテナのロウ数 4)
	const int rowCount = 4;
	const GSChar *dateList[4] = {"2018-12-01T10:00:00.000Z",
						"2018-12-01T10:10:00.000Z",
						"2018-12-01T10:20:00.000Z",
						"2018-12-01T10:40:00.000Z" };
	const int value1List[4] = { 1, 3, 2, 4 };
	const double value2List[4] = { 10.3, 5.7, 8.2, 4.5 };
	
	
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
    GSRow * rowList[4];
    const void * const * rowObj;
	GSBool status;
	GSTimestamp timestamp;
	
	// 検索用
	GSQuery *query;
	GSRowSet *rs;
	GSAggregationResult *aggregationResult;
	GSTimestamp date;
	int32_t value1;
	double value2;
	GSChar buf[GS_TIME_STRING_SIZE_MAX];
	
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
		info0.type = GS_CONTAINER_TIME_SERIES;
		info0.rowKeyAssigned = GS_TRUE;

		columnInfo.name = "date";
		columnInfo.type = GS_TYPE_TIMESTAMP;
		columnInfoList[0] = columnInfo;

		columnInfo.name = "value1";
		columnInfo.type = GS_TYPE_INTEGER;
		columnInfoList[1] = columnInfo;

		columnInfo.name = "value2";
		columnInfo.type = GS_TYPE_DOUBLE;
		columnInfoList[2] = columnInfo;

		info0.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
		info0.columnInfoList = columnInfoList;

		// 時系列コンテナを作成する
		ret = gsPutContainerGeneral(store, containerName, &info0, GS_FALSE, &container);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsPutContainerGeneral\n");
			goto LABEL_ERROR;
		}
		printf("Sample data generation: Create Collection name=%s\n", containerName);
		printf("Sample data generation:  column=(%s, %s, %s)\n", columnInfoList[0].name, columnInfoList[1].name, columnInfoList[2].name);

		for ( i = 0; i < rowCount; i++ ){
			// 空のロウオブジェクトを生成する
			gsCreateRowByContainer(container, &row);

			status = gsParseTime(dateList[i], &timestamp);
			if ( status == GS_FALSE ){
				fprintf(stderr, "ERROR gsParseTime\n");
				goto LABEL_ERROR;
			}

			// (3)カラム値をセット
			gsSetRowFieldByTimestamp(row, 0, timestamp);
			gsSetRowFieldByInteger(row, 1, value1List[i]);
			gsSetRowFieldByDouble(row, 2, value2List[i]);

			rowList[i] = row;
			
			printf("Sample data generation:  row=(%s, %d, %lf)\n", dateList[i], value1List[i], value2List[i]);
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
	// 時系列特有の集計演算
	//===============================================
	{
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
		
		//------------------------------------
		// 重み付き平均 TIME_AVG
		//------------------------------------
		// (1)TQLで集計演算の実行
		printf("TQL query : %s\n", "SELECT TIME_AVG(value1)");
		ret = gsQuery(container, "SELECT TIME_AVG(value1)", &query);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsQuery\n");
			goto LABEL_ERROR;
		}
		ret = gsFetch(query, GS_FALSE, &rs);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsFetch\n");
			goto LABEL_ERROR;
		}

		// (2)結果を取得する
		while (gsHasNextRow(rs)) {
			double value;

			// (3)集計演算の結果を取得する
			ret = gsGetNextAggregation(rs, &aggregationResult);
			if ( !GS_SUCCEEDED(ret) ){
				fprintf(stderr, "ERROR gsGetNextRow\n");
				goto LABEL_ERROR;
			}

			gsGetAggregationValueAsDouble(aggregationResult, &value, NULL);

			printf("TQL result: %lf\n", value);
		}
	}

	//===============================================
	// 時系列特有の選択演算
	//===============================================
	{
		//------------------------------------
		// TIME_NEXT
		//------------------------------------
		// (1)TQLで集計演算の実行
		printf("TQL query : %s\n", "SELECT TIME_NEXT(*, TIMESTAMP('2018-12-01T10:10:00.000Z'))");
		
		ret = gsQuery(container, "SELECT TIME_NEXT(*, TIMESTAMP('2018-12-01T10:10:00.000Z'))", &query);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsQuery\n");
			goto LABEL_ERROR;
		}
		ret = gsFetch(query, GS_FALSE, &rs);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsFetch\n");
			goto LABEL_ERROR;
		}

		// (2)結果を取得する
		while (gsHasNextRow(rs)) {

			// 空のロウを作成
			gsCreateRowByContainer(container, &row);

			// ロウを取得する
			ret = gsGetNextRow(rs, row);
			if ( !GS_SUCCEEDED(ret) ){
				fprintf(stderr, "ERROR gsGetNextRow\n");
				goto LABEL_ERROR;
			}

			// 値を取得する
			gsGetRowFieldAsTimestamp(row, 0, &date);
			gsGetRowFieldAsInteger(row, 1, &value1);
			gsGetRowFieldAsDouble(row, 2, &value2);

			// GSTimestampを文字列に変換する
			gsFormatTime(date, buf, GS_TIME_STRING_SIZE_MAX);

			printf("TQL result: row=(%s, %d, %lf)\n", buf, value1, value2);
		}
	}


	//===============================================
	// 時系列特有の補間演算
	//===============================================
	{
		//------------------------------------
		// TIME_INTERPOLATED
		//------------------------------------
		// (1)TQLで集計演算の実行
		printf("TQL query : %s\n", "SELECT TIME_INTERPOLATED(value1, TIMESTAMP('2018-12-01T10:30:00.000Z'))");

		ret = gsQuery(container, "SELECT TIME_INTERPOLATED(value1, TIMESTAMP('2018-12-01T10:30:00.000Z'))", &query);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsQuery\n");
			goto LABEL_ERROR;
		}
		ret = gsFetch(query, GS_FALSE, &rs);
		if ( !GS_SUCCEEDED(ret) ){
			fprintf(stderr, "ERROR gsFetch\n");
			goto LABEL_ERROR;
		}

		// (2)結果を取得する
		while (gsHasNextRow(rs)) {

			// 空のロウを作成
			gsCreateRowByContainer(container, &row);

			// ロウを取得する
			ret = gsGetNextRow(rs, row);
			if ( !GS_SUCCEEDED(ret) ){
				fprintf(stderr, "ERROR gsGetNextRow\n");
				goto LABEL_ERROR;
			}

			// 値を取得する
			gsGetRowFieldAsTimestamp(row, 0, &date);
			gsGetRowFieldAsInteger(row, 1, &value1);
			gsGetRowFieldAsDouble(row, 2, &value2);

			// GSTimestampを文字列に変換する
			gsFormatTime(date, buf, GS_TIME_STRING_SIZE_MAX);

			printf("TQL result: row=(%s, %d, %lf)\n", buf, value1, value2);
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
