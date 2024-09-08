// artkey.cpp
#include "artkey.hpp"

namespace duckart {

template <>
ARTKey ARTKey::CreateARTKey(string_t value) {
    auto str_len = value.GetSize();

    auto len = str_len + 1;
    auto data = new data_t[len];
    std::memcpy(data, value.GetData(), str_len);
    
     // keys must not be prefixes of other keys https://db.in.tum.de/~leis/papers/ART.pdf  
     // https://github.com/duckdb/duckdb/blob/main/src/execution/index/art/art_key.cpp#L46  
    data[str_len] = '\0';

    return ARTKey(data, len);
}

template <>
ARTKey ARTKey::CreateARTKey(const char* value) {
    /*
    auto len = std::strlen(value);
    auto data = new data_t[len];
    std::memcpy(data, value, len);
    return ARTKey(data, len);
    */
    return ARTKey::CreateARTKey(string_t(value));
}

template <>
void ARTKey::CreateARTKey(ARTKey& key, string_t value) {
    /*
    auto len = value.GetSize();
    key.data = new data_t[len];
    key.len = len;
    std::memcpy(key.data, value.GetData(), len);
    */
    key = ARTKey::CreateARTKey<string_t>(value);
}

bool ARTKey::operator==(const ARTKey &k) const {
	if (len != k.len) {
		return false;
	}
	for (uint32_t i = 0; i < len; i++) {
		if (data[i] != k.data[i]) {
			return false;
		}
	}
	return true;
}

} // namespace duckar
