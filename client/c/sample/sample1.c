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

// Operation on Collection data
int sample1(const char *addr, const char *port, const char *clusterName, const char *user, const char *passwd) {
	static const int8_t personLob[] = { 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 };
	static const GSBool update = GS_TRUE;

	GSGridStore *store;
	GSCollection *col;
	GSQuery *query;
	GSRowSet *rs;
	Person person;
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

	// Create a Collection (Delete if schema setting is NULL)
	gsPutCollection(store, "col01",
			GS_GET_STRUCT_BINDING(Person), NULL, GS_FALSE, &col);

	// Set the autocommit mode to OFF
	gsSetAutoCommit(col, GS_FALSE);

	// Set an index on the Row-key Column
	gsCreateIndex(col, "name", GS_INDEX_FLAG_DEFAULT);

	// Set an index on the Column
	gsCreateIndex(col, "count", GS_INDEX_FLAG_DEFAULT);

	// Prepare data for a Row
	person.name = "name01";
	person.status = GS_FALSE;
	person.count = 1;
	person.lob = personLob;
	person.lobSize = sizeof(personLob);

	// Operate a Row on a K-V basis: RowKey = "name01"
	gsPutRow(col, NULL, &person, NULL);	// Add a Row
	gsGetRowForUpdate(col, &person.name, &person, NULL);	// Obtain the Row (acquiring a lock for update)
	gsDeleteRow(col, &person.name, NULL);	// Delete the Row

	// Operate a Row on a K-V basis: RowKey = "name02"
	gsPutRowByString(col, "name02", &person, NULL);	// Add a Row (specifying RowKey)

	// Commit the transaction (Release the lock)
	gsCommit(col);

	// Search the Collection for a Row
	gsQuery(col, "select * where name = 'name02'", &query);

	// Fetch and update the searched Row
	gsFetch(query, update, &rs);
	while (gsHasNextRow(rs)) {
		size_t i;

		// Update the searched Row
		gsGetNextRow(rs, &person);
		person.count += 1;
		ret = gsUpdateCurrentRow(rs, &person);
		if (ret != GS_RESULT_OK) break;

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

	// Commit the transaction
	ret = gsCommit(col);

	// Release the resource
	gsCloseGridStore(&store, GS_TRUE);

	return (GS_SUCCEEDED(ret) ? EXIT_SUCCESS : EXIT_FAILURE);
}

void main(int argc,char *argv[]){ sample1(argv[1],argv[2],argv[3],argv[4],argv[5]);}
