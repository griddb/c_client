#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

#define SAMPLE5_CONTAINER_COUNT 5
#define SAMPLE5_ROW_COUNT 2

// 複数コンテナ一括操作
int sample5(const char *args[5]) {
	static const int8_t personLob[] = { 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 };

	static const GSChar *const containerNameList[SAMPLE5_CONTAINER_COUNT] = {
			"col01", "col02", "col03", "col04", "col05"
	};
	static const GSChar *const keyList[SAMPLE5_ROW_COUNT] = {
			"name01", "name02"
	};

	static const size_t containerCount = SAMPLE5_CONTAINER_COUNT;
	static const size_t rowCount = SAMPLE5_ROW_COUNT;

	GSGridStore *store;
	GSContainer *containerList[SAMPLE5_CONTAINER_COUNT];
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[4];
	GSResult ret;
	int failed = 0;
	size_t i, j;

	const GSPropertyEntry props[] = {
			{ "notificationAddress", args[0] },
			{ "notificationPort", args[1] },
			{ "clusterName", args[2] },
			{ "user", args[3] },
			{ "password", args[4] } };
	const size_t propCount = sizeof(props) / sizeof(*props);

	// GridStoreインスタンスの取得
	gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);

	//コンテナ定義情報を作成
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
	info.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info.columnInfoList = columnInfoList;
	info.rowKeyAssigned = GS_TRUE;

	// コレクションの作成
	for (i = 0; i < containerCount; i++) {
		const GSChar *name = containerNameList[i];
		gsPutContainerGeneral(store, name, &info, GS_FALSE, &containerList[i]);
	}

	// 複数コレクションのRowを一括登録
	{
		void *allRowList[SAMPLE5_CONTAINER_COUNT][SAMPLE5_ROW_COUNT];
		GSContainerRowEntry inEntry = GS_CONTAINER_ROW_ENTRY_INITIALIZER;
		GSContainerRowEntry inEntryList[SAMPLE5_CONTAINER_COUNT];

		GSBool status = GS_FALSE;
		int64_t count = 1;
		const int8_t *lobData = personLob;
		size_t lobSize = sizeof(personLob);

		for (i = 0; i < containerCount; i++) {
			for (j = 0; j < rowCount; j++) {
				GSRow *row;
				const GSChar *name = keyList[j];

				gsCreateRowByStore(store, &info, &row);
				gsSetRowFieldByString(row, 0, name);
				gsSetRowFieldByBool(row, 1, status);
				gsSetRowFieldByLong(row, 2, count);
				gsSetRowFieldByByteArray(row, 3, lobData, lobSize);
				count++;

				allRowList[i][j] = row;
			}
			inEntry.containerName = containerNameList[i];
			inEntry.rowList = allRowList[i];
			inEntry.rowCount = rowCount;
			inEntryList[i] = inEntry;
		}

		ret = gsPutMultipleContainerRows(store, inEntryList, containerCount);
		failed |= !GS_SUCCEEDED(ret);

		for (i = 0; i < containerCount; i++) {
			for (j = 0; j < rowCount; j++) {
				GSRow *row = allRowList[i][j];
				gsCloseRow(&row);
			}
		}
	}

	// 複数コレクションのRowを一括取得
	{
		GSRowKeyPredicateEntry predEntry =
				GS_ROW_KEY_PREDICATE_ENTRY_INITIALIZER;
		GSRowKeyPredicateEntry predEntryValueList[SAMPLE5_CONTAINER_COUNT];
		const GSRowKeyPredicateEntry *predEntryList[SAMPLE5_CONTAINER_COUNT];
		const GSContainerRowEntry *outEntryList;
		size_t outEntryCount;
		GSRow *allRowList[SAMPLE5_CONTAINER_COUNT][SAMPLE5_ROW_COUNT];

		// 取得条件を構築
		for (i = 0; i < containerCount; i++) {
			GSRowKeyPredicate *predicate;

			gsCreateRowKeyPredicate(store, GS_TYPE_STRING, &predicate);
			for (j = 0; j < rowCount; j++) {
				gsAddPredicateKeyByString(predicate, keyList[j]);
			}

			predEntry.containerName = containerNameList[i];
			predEntry.predicate = predicate;
			predEntryValueList[i] = predEntry;
			predEntryList[i] = &predEntryValueList[i];
		}

		ret = gsGetMultipleContainerRows(
				store, predEntryList, containerCount,
				&outEntryList, &outEntryCount);
		failed |= !GS_SUCCEEDED(ret);

		// 取得結果ロウ列をコピー
		for (i = 0; i < containerCount || i < outEntryCount; i++) {
			GSContainerRowEntry outEntry = GS_CONTAINER_ROW_ENTRY_INITIALIZER;
			if (i < outEntryCount) {
				outEntry = outEntryList[i];
			}
			for (j = 0; j < rowCount || j < outEntry.rowCount; j++) {
				GSRow *row = NULL;
				if (j < outEntry.rowCount) {
					row = (GSRow*) outEntry.rowList[j];
				}
				if (i < containerCount && j < rowCount) {
					allRowList[i][j] = row;
				}
				else {
					gsCloseRow(&row);
				}
			}
		}

		// 取得結果を出力
		for (i = 0; i < containerCount; i++) {
			for (j = 0; j < rowCount; j++) {
				GSRow *row = allRowList[i][j];
				const GSChar *name;
				int64_t count;

				gsGetRowFieldAsString(row, 0, &name);
				gsGetRowFieldAsLong(row, 2, &count);

				failed |= (name == NULL);
				if (failed) break;

				printf("Person[%d]:", (int) i);
				printf(" name=%s", name);
				printf(" count=%d\n", (int) count);

				gsCloseRow(&row);
			}
			gsCloseRowKeyPredicate(&predEntryValueList[i].predicate);
		}
	}

	// 複数コレクションのRowを一括検索(クエリ使用)
	{
		GSQuery *queryList[SAMPLE5_CONTAINER_COUNT];

		for (i = 0; i < containerCount; i++) {
			const GSChar *tql = "select * where count >= 0";
			gsQuery(containerList[i], tql, &queryList[i]);
		}

		ret = gsFetchAll(store, queryList, containerCount);
		failed |= !GS_SUCCEEDED(ret);

		for (i = 0; i < containerCount; i++) {
			GSRowSet *rs;
			GSRow *row;

			gsCreateRowByContainer(containerList[i], &row);
			gsGetRowSet(queryList[i], &rs);

			while (gsHasNextRow(rs)) {
				const GSChar *name;
				GSBool status;
				int64_t count;
				const int8_t *lobData;
				size_t lobSize;

				ret = gsGetNextRow(rs, row);
				failed |= !GS_SUCCEEDED(ret);
				if (failed) break;

				gsGetRowFieldAsString(row, 0, &name);
				gsGetRowFieldAsBool(row, 1, &status);
				gsGetRowFieldAsLong(row, 2, &count);
				gsGetRowFieldAsByteArray(row, 3, &lobData, &lobSize);
				count += 1;

				failed |= (name == NULL);
				if (failed) break;

				printf("Person[%d]:", (int) i);
				printf(" name=%s", name);
				printf(" status=%s", status ? "true" : "false");
				printf(" count=%d", (int) count);
				printf(" lob=[");
				for (j = 0; j < lobSize; j++) {
					if (j > 0) {
						printf(", ");
					}
					printf("%d", (int) lobData[j]);
				}
				printf("]\n");
			}

			gsCloseRow(&row);
			gsCloseRowSet(&rs);
			gsCloseQuery(&queryList[i]);
		}
	}

	// コレクションの削除
	for (i = 0; i < containerCount; i++) {
		const GSChar *name = containerNameList[i];
		gsDropContainer(store, name);
	}

	// リソースの解放
	gsCloseGridStore(&store, GS_TRUE);

	return (failed ? EXIT_FAILURE : EXIT_SUCCESS);
}
