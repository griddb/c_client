#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	const GSChar *name;
	GSBool status;
	uint64_t count;
	const int8_t *lob;
	size_t lobSize;
} Person;

GS_STRUCT_BINDING(Person,
	GS_STRUCT_BINDING_KEY(name, GS_TYPE_STRING)
	GS_STRUCT_BINDING_ELEMENT(status, GS_TYPE_BOOL)
	GS_STRUCT_BINDING_ELEMENT(count, GS_TYPE_LONG)
	GS_STRUCT_BINDING_ARRAY(lob, lobSize, GS_TYPE_BYTE));

// コレクションデータの操作
int sample1(const char *args[5]) {
	static const int8_t personLob[] = { 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 };
	static const GSBool update = GS_TRUE;

	GSGridStore *store;
	GSCollection *col;
	GSQuery *query;
	GSRowSet *rs;
	Person person;
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

	// コレクションの作成
	gsPutCollection(store, "col01",
			GS_GET_STRUCT_BINDING(Person), NULL, GS_FALSE, &col);

	// カラムに索引を設定
	gsCreateIndex(col, "count", GS_INDEX_FLAG_DEFAULT);

	// 自動コミットモードをオフ
	gsSetAutoCommit(col, GS_FALSE);

	// Rowのデータを用意
	person.name = "name01";
	person.status = GS_FALSE;
	person.count = 1;
	person.lob = personLob;
	person.lobSize = sizeof(personLob);

	// KV形式でRowを操作: RowKeyは"name01"
	gsPutRow(col, NULL, &person, NULL);	// 登録
	gsGetRowForUpdate(col, &person.name, &person, NULL);	// 取得(更新用にロック)
	gsDeleteRow(col, &person.name, NULL);	// 削除

	// KV形式でRowを操作: RowKeyは"name02"
	gsPutRowByString(col, "name02", &person, NULL);	// 登録(RowKeyを指定)

	// トランザクションの確定(ロック解除)
	gsCommit(col);

	// コレクション内のRowを検索(クエリ使用)
	gsQuery(col, "select * where name = 'name02'", &query);

	// 検索したRowの取得と更新
	gsFetch(query, update, &rs);
	while (gsHasNextRow(rs)) {
		size_t i;

		// 検索したRowの更新
		gsGetNextRow(rs, &person);
		person.count += 1;
		ret = gsUpdateCurrentRow(rs, &person);
		if (!GS_SUCCEEDED(ret)) break;

		printf("Person:");
		printf(" name=%s", person.name);
		printf(" status=%s", person.status ? "true" : "false");
		printf(" count=%d", (int) person.count);
		printf(" lob=[");
		for (i = 0; i < person.lobSize; i++) {
			if (i > 0) printf(", ");
			printf("%d", (int) person.lob[i]);
		}
		printf("]\n");
	}

	// トランザクションの確定
	ret = gsCommit(col);

	// リソースの解放
	gsCloseGridStore(&store, GS_TRUE);

	return (GS_SUCCEEDED(ret) ? EXIT_SUCCESS : EXIT_FAILURE);
}
