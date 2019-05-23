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

// 時系列データの検索と集計
int sample3(const char *args[5]) {
	GSGridStore *store;
	GSTimeSeries *ts;
	GSQuery *query;
	GSRowSet *rs;
	GSResult ret = !GS_RESULT_OK;

	// 読み取りのみなので、一貫性レベルを緩和(デフォルトはIMMEDIATE)
	const GSPropertyEntry props[] = {
			{ "notificationAddress", args[0] },
			{ "notificationPort", args[1] },
			{ "clusterName", args[2] },
			{ "user", args[3] },
			{ "password", args[4] },
			{ "consistency", "EVENTUAL" } };
	const size_t propCount = sizeof(props) / sizeof(*props);

	// GridStoreインスタンスの取得
	gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);

	// 時系列の取得(既存の場合は取得のみ)
	// ※sample2と同じPoint構造体を使用
	gsGetTimeSeries(store, "point01", GS_GET_STRUCT_BINDING(Point), &ts);

	// 停止中にもかかわらず電圧が基準値以上の箇所を検索
	gsQuery(ts,
			"select * from point01"
			" where not active and voltage > 50", &query);
	gsFetch(query, GS_FALSE, &rs);

	while (gsHasNextRow(rs)) {
		// 各異常ポイントについて調査

		GSTimestamp hot;
		Point hotPoint;
		GSTimestamp start;
		Point startPoint;
		GSTimestamp end;
		GSAggregationResult *avg;
		double avgValue;
		GSChar hotStr[GS_TIME_STRING_SIZE_MAX];

		ret = gsGetNextRow(rs, &hotPoint);
		if (!GS_SUCCEEDED(ret)) break;
		hot = hotPoint.timestamp;

		// 10分前付近のデータを取得
		start = gsAddTime(hot, -10, GS_TIME_UNIT_MINUTE);
		gsGetRowByBaseTime(
				ts, start, GS_TIME_OPERATOR_NEXT, &startPoint, NULL);

		// 前後10分間の平均値を計算
		end = gsAddTime(hot, 10, GS_TIME_UNIT_MINUTE);
		gsAggregateTimeSeries(
				ts, start, end, "voltage", GS_AGGREGATION_AVERAGE, &avg);

		ret = gsGetAggregationValueAsDouble(avg, &avgValue, NULL);
		if (!GS_SUCCEEDED(ret)) break;

		gsFormatTime(hot, hotStr, sizeof(hotStr));

		printf("[Alert] %s", hotStr);
		printf(" start=%.1lf", startPoint.voltage);
		printf(" hot=%.1lf", hotPoint.voltage);
		printf(" avg=%.1lf\n", avgValue);
	}

	// リソースの解放
	gsCloseGridStore(&store, GS_TRUE);

	return (GS_SUCCEEDED(ret) ? EXIT_SUCCESS : EXIT_FAILURE);
}
