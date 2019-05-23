#ifndef CLIENT_SAMPLE_H_
#define CLIENT_SAMPLE_H_

#include <stdlib.h>
#include <string.h>

#define CLIENT_SAMPLE_ALL_NAMES "sample1|sample2|sample3|sample4|sample5"
#define CLIENT_SAMPLE_ALL_LANGS "en|ja"

#define CLIENT_SAMPLE_DECLARE_FUNC(name, lang) \
	int sample ## name  ## _ ## lang(const char *args[5])

#define CLIENT_SAMPLE_DECLARE_FUNC_LIST(lang) \
	CLIENT_SAMPLE_DECLARE_FUNC(1, lang); \
	CLIENT_SAMPLE_DECLARE_FUNC(2, lang); \
	CLIENT_SAMPLE_DECLARE_FUNC(3, lang); \
	CLIENT_SAMPLE_DECLARE_FUNC(4, lang); \
	CLIENT_SAMPLE_DECLARE_FUNC(5, lang)

#ifdef __cplusplus
extern "C" {
#endif

CLIENT_SAMPLE_DECLARE_FUNC_LIST(en);
CLIENT_SAMPLE_DECLARE_FUNC_LIST(ja);

#ifdef __cplusplus
}
#endif

static int runClientSample(
		int *unmatched, const char *name, const char *lang,
		const char *addr, const char *port, const char *clusterName,
		const char *user, const char *password) {
	const char *args[] = { addr, port, clusterName, user, password };
	*unmatched = 0;
	if (strcmp(lang, "en") == 0) {
		if (strcmp(name, "sample1") == 0) {
			return sample1_en(args);
		}
		else if (strcmp(name, "sample2") == 0) {
			return sample2_en(args);
		}
		else if (strcmp(name, "sample3") == 0) {
			return sample3_en(args);
		}
		else if (strcmp(name, "sample4") == 0) {
			return sample4_en(args);
		}
		else if (strcmp(name, "sample5") == 0) {
			return sample5_en(args);
		}
	}
	else if (strcmp(lang, "ja") == 0) {
		if (strcmp(name, "sample1") == 0) {
			return sample1_ja(args);
		}
		else if (strcmp(name, "sample2") == 0) {
			return sample2_ja(args);
		}
		else if (strcmp(name, "sample3") == 0) {
			return sample3_ja(args);
		}
		else if (strcmp(name, "sample4") == 0) {
			return sample4_ja(args);
		}
		else if (strcmp(name, "sample5") == 0) {
			return sample5_ja(args);
		}
	}
	*unmatched = 1;
	return EXIT_FAILURE;
}

#endif
