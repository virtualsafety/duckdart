// value.cpp
#include "value.hpp"

namespace duckart {

template <>
Value Value::CreateValue(string_t value) {
    auto str_len = value.GetSize();

    auto len = str_len + 1;
    auto data = new data_t[len];
    std::memcpy(data, value.GetData(), str_len);
    
     // keys must not be prefixes of other keys https://db.in.tum.de/~leis/papers/ART.pdf  
     // https://github.com/duckdb/duckdb/blob/main/src/execution/index/art/art_key.cpp#L46  
    data[str_len] = '\0';

    return Value(data, len);
}

template <>
Value Value::CreateValue(const char* value) {
    /*
    auto len = std::strlen(value);
    auto data = new data_t[len];
    std::memcpy(data, value, len);
    return Value(data, len);
    */
    return Value::CreateValue(string_t(value));
}

template <>
void Value::CreateValue(Value& key, string_t value) {
    /*
    auto len = value.GetSize();
    key.data = new data_t[len];
    key.len = len;
    std::memcpy(key.data, value.GetData(), len);
    */
    key = Value::CreateValue<string_t>(value);
}

bool Value::operator==(const Value &k) const {
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
