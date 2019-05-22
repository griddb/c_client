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

// Search and aggregation of time-series data
int sample3(const char *args[5]) {
	GSGridStore *store;
	GSTimeSeries *ts;
	GSQuery *query;
	GSRowSet *rs;
	GSResult ret = !GS_RESULT_OK;

	// Lower the consistency level because of read-only operation (default: IMMEDIATE)
	const GSPropertyEntry props[] = {
			{ "notificationAddress", args[0] },
			{ "notificationPort", args[1] },
			{ "clusterName", args[2] },
			{ "user", args[3] },
			{ "password", args[4] },
			{ "consistency", "EVENTUAL" } };
	const size_t propCount = sizeof(props) / sizeof(*props);

	// Acquiring a GridStore instance
	gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);

	// Obtain a TimeSeries
	// Use the Point class as in Sample 2
	gsGetTimeSeries(store, "point01", GS_GET_STRUCT_BINDING(Point), &ts);

	// Search for the locations whose voltage is not lower than a reference value, though not in action.
	gsQuery(ts,
			"select * from point01"
			" where not active and voltage > 50", &query);
	gsFetch(query, GS_FALSE, &rs);

	while (gsHasNextRow(rs)) {
		// Examine each abnormal point

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

		// Obtain the data around ten minutes before
		start = gsAddTime(hot, -10, GS_TIME_UNIT_MINUTE);
		gsGetRowByBaseTime(
				ts, start, GS_TIME_OPERATOR_NEXT, &startPoint, NULL);

		// Calculate the average of the data for 20 minutes around the point
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

	// Releasing resource
	gsCloseGridStore(&store, GS_TRUE);

	return (GS_SUCCEEDED(ret) ? EXIT_SUCCESS : EXIT_FAILURE);
}
