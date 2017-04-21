#include "uuid_utils.h"
#include "uuid/uuid.h"

void UUIDUtils::generate(UUIDValue &out) {
	uuid_generate(out);
}
