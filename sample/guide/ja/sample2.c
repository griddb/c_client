#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	GSTimestamp timestamp;
	GSBool active;
	double voltage;
} Point;

GS_STRUCT_BINDING(Point,
	GS_STRUCT_BINDING_KEY(timestamp, GS_TYPE_TIMESTAMP)
	GS_STRUCT_BINDING_ELEMENT(active, GS_TYPE_BOOL)
	GS_STRUCT_BINDING_ELEMENT(voltage, GS_TYPE_DOUBLE));

// 時系列データの登録と範囲取得
int sample2(const char *addr, const char *port, const char *clusterName,
			const char *user, const char *password) {
	GSGridStore *store;
	GSTimeSeries *ts;
	Point point;
	GSTimestamp now;
	GSTimestamp before;
	GSQuery *query;
	GSRowSet *rs;
	GSResult ret = !GS_RESULT_OK;

	const GSPropertyEntry props[] = {
			{ "notificationAddress", addr },
			{ "notificationPort", port },
			{ "clusterName", clusterName },
			{ "user", user },
			{ "password", password } };
	const size_t propCount = sizeof(props) / sizeof(*props);

	// GridStoreインスタンスの取得
	gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);

	// 時系列の作成 (既存の場合は取得のみ)
	gsPutTimeSeries(store, "point01",
			GS_GET_STRUCT_BINDING(Point), NULL, GS_FALSE, &ts);

	// 時系列要素のデータを用意
	point.active = GS_FALSE;
	point.voltage = 100;

	// 時系列要素の登録(グリッドストア側で時刻設定)
	gsAppendTimeSeriesRow(ts, &point, NULL);

	// 指定区間の時系列の取得: 6時間前から直近まで
	now = gsCurrentTime();
	before = gsAddTime(now, -6, GS_TIME_UNIT_HOUR);

	gsQueryByTimeSeriesRange(ts, before, now, &query);
	ret = gsFetch(query, GS_FALSE, &rs);
	while (gsHasNextRow(rs)) {
		GSChar timeStr[GS_TIME_STRING_SIZE_MAX];

		ret = gsGetNextRow(rs, &point);
		if (!GS_SUCCEEDED(ret)) break;

		gsFormatTime(point.timestamp, timeStr, sizeof(timeStr));
		printf("Time=%s", timeStr);
		printf(" Active=%s", point.active ? "true" : "false");
		printf(" Voltage=%.1lf\n", point.voltage);
	}

	// リソースの解放
	gsCloseGridStore(&store, GS_TRUE);

	return (GS_SUCCEEDED(ret) ? EXIT_SUCCESS : EXIT_FAILURE);
}

void main(int argc,char *argv[]){ sample2(argv[1],argv[2],argv[3],argv[4],argv[5]);}
