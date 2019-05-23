#include <stdio.h>
#include "client_sample.h"

int main(int argc, char **argv) {
	int needHelp;
	int exitCode;
	if (argc < 8) {
		needHelp = 1;
		exitCode = EXIT_FAILURE;
	}
	else {
		printf("run\n");
		exitCode = runClientSample(
				&needHelp,
				argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
	}

	if (needHelp) {
		const char *const names[] = { CLIENT_SAMPLE_ALL_NAMES };
		const char *const langs[] = { CLIENT_SAMPLE_ALL_LANGS };
		size_t i;

		printf("Usage: ");
		for (i = 0; i < sizeof(names) / sizeof(names); i++) {
			if (i > 0) {
				printf("|");
			}
			printf("%s", names[i]);
		}
		printf(" ");
		for (i = 0; i < sizeof(langs) / sizeof(langs); i++) {
			if (i > 0) {
				printf("|");
			}
			printf("%s", langs[i]);
		}
		printf(" (addr) (port) (clusterName) (user) (password)\n");
	}
	else if (exitCode == EXIT_SUCCESS) {
		printf("Succeeded\n");
	}
	else {
		printf("Failed\n");
	}

	return exitCode;
}
