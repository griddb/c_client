#include "gridstore.h"
#include <stdlib.h>
#include <stdio.h>

// Operation on Collection data
int sample4(const char *addr, const char *port, const char *clusterName, const char *user, const char *passwd) {
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
			{ "password", passwd } };
	const size_t propCount = sizeof(props) / sizeof(*props);

	// Get a GridStore instance
	gsGetGridStore(gsGetDefaultFactory(), props, propCount, &store);

	columnInfo.name = "name";
	columnInfo.type = GS_TYPE_STRING;
	columnInfoList[0] = columnInfo;

	columnInfo.name = "status";
	columnInfo.type = GS_TYPE_BOOL;
	columnInfoList[1] = columnInfo;

	columnInfo.name = "count";
	columnInfo.type = GS_TYPE_LONG;
	columnInfoList[2] = columnInfo;

	columnInfo.name = "blob";
	columnInfo.type = GS_TYPE_BLOB;
	columnInfoList[3] = columnInfo;

	info.type = GS_CONTAINER_COLLECTION;
	info.name = "col01";
	info.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info.columnInfoList = columnInfoList;
	info.rowKeyAssigned = GS_TRUE;

	// Create a Collection (Delete if schema setting is NULL)
	gsPutContainerGeneral(store, NULL, &info, GS_FALSE, &container);

	// Set the autocommit mode to OFF
	gsSetAutoCommit(container, GS_FALSE);

	// Set an index on the Row-key Column
	gsCreateIndex(container, "name", GS_INDEX_FLAG_DEFAULT);

	// Set an index on the Column
	gsCreateIndex(container, "count", GS_INDEX_FLAG_DEFAULT);

	// Prepare data for a Row
	{
		const GSChar *name = "name01";
		GSBool status = GS_FALSE;
		int64_t count = 1;
		GSBlob lob;
		lob.data = personLob;
		lob.size = sizeof(personLob);

		gsCreateRowByStore(store, &info, &row);
		gsSetRowFieldByString(row, 0, name);
		gsSetRowFieldByBool(row, 1, status);
		gsSetRowFieldByLong(row, 2, count);
		gsSetRowFieldByBlob(row, 3, &lob);
	}

	// Operate a Row on a K-V basis: RowKey = "name01"
	gsPutRow(container, NULL, row, NULL);	// Add a Row
	gsGetRowByString(container, "name01", row, update, NULL);	// Obtain the Row (acquiring a lock for update)
	gsDeleteRowByString(container, "name01", NULL);	// Delete the Row

	// Operate a Row on a K-V basis: RowKey = "name02"
	gsPutRowByString(container, "name02", row, NULL);	// Add a Row (specifying RowKey)

	gsCloseRow(&row);

	// Commit the transaction (Release the lock)
	gsCommit(container);

	// Search the Collection for a Row
	gsQuery(container, "select * where name = 'name02'", &query);

	// Fetch and update the searched Row
	gsFetch(query, update, &rs);
	while (gsHasNextRow(rs)) {
		const GSChar *name;
		GSBool status;
		int64_t count;
		GSBlob lob;
		size_t i;

		gsCreateRowByContainer(container, &row);

		// Update the searched Row
		gsGetNextRow(rs, row);
		gsGetRowFieldAsString(row, 0, &name);
		gsGetRowFieldAsBool(row, 1, &status);
		gsGetRowFieldAsLong(row, 2, &count);
		gsGetRowFieldAsBlob(row, 3, &lob);
		count += 1;
		ret = gsUpdateCurrentRow(rs, row);
		if (ret != GS_RESULT_OK) break;

		printf("Person:");
		printf(" name=%s", name);
		printf(" status=%s", status ? "true" : "false");
		printf(" count=%d", (int) count);
		printf(" lob=[");
		for (i = 0; i < lob.size; i++) {
			if (i > 0) {
				printf(", ");
			}
			printf("%d", (int) ((const int8_t*) lob.data)[i]);
		}
		printf("]\n");
	}

	// Commit the transaction
	ret = gsCommit(container);

	gsDropContainer(store, info.name);

	//Release the resource
	gsCloseGridStore(&store, GS_TRUE);

	return (GS_SUCCEEDED(ret) ? EXIT_SUCCESS : EXIT_FAILURE);
}

void main(int argc,char *argv[]){ sample4(argv[1],argv[2],argv[3],argv[4],argv[5]);}
