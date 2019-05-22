#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

// Schema definition using container information
int sample4(const char *addr, const char *port, const char *clusterName,
			const char *user, const char *password) {
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
			{ "notificationAddress", addr },
			{ "notificationPort", port },
			{ "clusterName", clusterName },
			{ "user", user },
			{ "password", password } };
	const size_t propCount = sizeof(props) / sizeof(*props);

	// Acquiring a GridStore instance
	gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);

	// Creating a container information
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

	// Creating a collection
	gsPutContainerGeneral(store, NULL, &info, GS_FALSE, &container);

	// Setting an index for a column
	gsCreateIndex(container, "count", GS_INDEX_FLAG_DEFAULT);

	// Setting auto-commit off
	gsSetAutoCommit(container, GS_FALSE);

	// Preparing row data
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

	// Operating a row in KV format: RowKey is "name01"
	gsPutRow(container, NULL, row, NULL);	// Registration
	gsGetRowByString(container, "name01", row, update, NULL);	// Aquisition (Locking to update)
	gsDeleteRowByString(container, "name01", NULL);	// Deletion

	// Operating a row in KV format: RowKey is "name02"
	gsPutRowByString(container, "name02", row, NULL);	// Registration (Specifying RowKey)

	// Release of an unnecessary row object
	gsCloseRow(&row);

	// Committing transaction (releasing lock)
	gsCommit(container);

	// Search rows in a container
	gsQuery(container, "select * where name = 'name02'", &query);

	// Fetching and updating retrieved rows
	gsFetch(query, update, &rs);
	while (gsHasNextRow(rs)) {
		const GSChar *name;
		GSBool status;
		int64_t count;
		const int8_t *lobData;
		size_t lobSize;
		size_t i;

		gsCreateRowByContainer(container, &row);

		// Updating a retrived row
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

	// Committing transaction
	ret = gsCommit(container);

	// Deleting a collection
	gsDropContainer(store, info.name);

	// Releasing resource
	gsCloseGridStore(&store, GS_TRUE);

	return (GS_SUCCEEDED(ret) ? EXIT_SUCCESS : EXIT_FAILURE);
}

void main(int argc,char *argv[]){ sample4(argv[1],argv[2],argv[3],argv[4],argv[5]);}