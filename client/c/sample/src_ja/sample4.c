#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

// コンテナ情報を用いてスキーマ定義
int sample4(const char *args[5]) {
	static const int8_t personLob[] = { 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 };
	static const GSBool update = GS_TRUE;

	GSGridStore *store;
	GSContainer *container;
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[4];
	GSRow *row;
	GSQuery *query;
	GSRowSet *rs;
	GSResult ret;

	const GSPropertyEntry props[] = {
			{ "notificationAddress", args[0] },
			{ "notificationPort", args[1] },
			{ "clusterName", args[2] },
			{ "user", args[3] },
			{ "password", args[4] } };
	const size_t propCount = sizeof(props) / sizeof(*props);

	// GridStoreインスタンスの取得
	gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);

	// コンテナ情報を作成
	columnInfo.name = "name";
	columnInfo.type = GS_TYPE_STRING;
	columnInfoList[0] = columnInfo;

	columnInfo.name = "status";
	columnInfo.type = GS_TYPE_BOOL;
	columnInfoList[1] = columnInfo;

	columnInfo.name = "count";
	columnInfo.type = GS_TYPE_LONG;
	columnInfoList[2] = columnInfo;

	columnInfo.name = "lob";
	columnInfo.type = GS_TYPE_BYTE_ARRAY;
	columnInfoList[3] = columnInfo;

	info.type = GS_CONTAINER_COLLECTION;
	info.name = "col01";
	info.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info.columnInfoList = columnInfoList;
	info.rowKeyAssigned = GS_TRUE;

	// コレクションの作成
	gsPutContainerGeneral(store, NULL, &info, GS_FALSE, &container);

	// カラムに索引を設定
	gsCreateIndex(container, "count", GS_INDEX_FLAG_DEFAULT);

	// 自動コミットモードをオフ
	gsSetAutoCommit(container, GS_FALSE);

	// Rowのデータを用意
	{
		const GSChar *name = "name01";
		GSBool status = GS_FALSE;
		int64_t count = 1;
		const int8_t *lobData = personLob;
		size_t lobSize = sizeof(personLob);

		gsCreateRowByStore(store, &info, &row);
		gsSetRowFieldByString(row, 0, name);
		gsSetRowFieldByBool(row, 1, status);
		gsSetRowFieldByLong(row, 2, count);
		gsSetRowFieldByByteArray(row, 3, lobData, lobSize);
	}

	// KV形式でRowを操作: RowKeyは"name01"
	gsPutRow(container, NULL, row, NULL);	// 登録
	gsGetRowByString(container, "name01", row, update, NULL);	// 取得(更新用にロック)
	gsDeleteRowByString(container, "name01", NULL);	// 削除

	// KV形式でRowを操作: RowKeyは"name02"
	gsPutRowByString(container, "name02", row, NULL);	// 登録(RowKeyを指定)

	// 不要ロウオブジェクトの解放
	gsCloseRow(&row);

	// トランザクションの確定(ロック解除)
	gsCommit(container);

	// コレクション内のRowを検索(クエリ使用)
	gsQuery(container, "select * where name = 'name02'", &query);

	// 検索したRowの取得と更新
	gsFetch(query, update, &rs);
	while (gsHasNextRow(rs)) {
		const GSChar *name;
		GSBool status;
		int64_t count;
		const int8_t *lobData;
		size_t lobSize;
		size_t i;

		gsCreateRowByContainer(container, &row);

		// 検索したRowの更新
		gsGetNextRow(rs, row);
		gsGetRowFieldAsString(row, 0, &name);
		gsGetRowFieldAsBool(row, 1, &status);
		gsGetRowFieldAsLong(row, 2, &count);
		gsGetRowFieldAsByteArray(row, 3, &lobData, &lobSize);
		count += 1;
		ret = gsUpdateCurrentRow(rs, row);
		if (!GS_SUCCEEDED(ret) || name == NULL) break;

		printf("Person:");
		printf(" name=%s", name);
		printf(" status=%s", status ? "true" : "false");
		printf(" count=%d", (int) count);
		printf(" lob=[");
		for (i = 0; i < lobSize; i++) {
			if (i > 0) {
				printf(", ");
			}
			printf("%d", (int) lobData[i]);
		}
		printf("]\n");
	}

	// トランザクションの確定
	ret = gsCommit(container);

	// コレクションの削除
	gsDropContainer(store, info.name);

	// リソースの解放
	gsCloseGridStore(&store, GS_TRUE);

	return (GS_SUCCEEDED(ret) ? EXIT_SUCCESS : EXIT_FAILURE);
}
