
#include "config.h"
#include "uuidP.h"

#include "Rpc.h"
#undef uuid_t

#pragma warning(push,4)

#pragma comment(lib, "Rpcrt4.lib")

void uuid_generate(uuid_t out)
{
	UUID winUuid;
	struct uuid libUuid;

	UuidCreate(&winUuid);

	libUuid.time_low = winUuid.Data1;
	libUuid.time_mid = winUuid.Data2;
	libUuid.time_hi_and_version = winUuid.Data3;
	libUuid.clock_seq = winUuid.Data4[0] << CHAR_BIT | winUuid.Data4[1];
	memcpy(
			libUuid.node,
			winUuid.Data4 + sizeof(libUuid.clock_seq),
			sizeof(libUuid.node));

	uuid_pack(&libUuid, out);
}
