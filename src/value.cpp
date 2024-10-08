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
void Value::CreateValue(Value& val, string_t value) {
    /*
    auto len = value.GetSize();
    key.data = new data_t[len];
    key.len = len;
    std::memcpy(key.data, value.GetData(), len);
    */
    val = Value::CreateValue<string_t>(value);
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

//
template <>
string_t Value::ExtractValue(Value &val){
     if (val.len == 0 || val.data == nullptr) {
            return string_t("");  // Return an empty string if the Value is empty
        }

        // Subtract 1 from len to exclude the null terminator
        size_t str_len = val.len - 1;

        if (str_len <= 11) {
            // If the string fits in the inlined storage
            return string_t(reinterpret_cast<const char*>(val.data));
        } else {
            // If the string needs to use pointer storage
            string_t result("");  // Create an empty string_t
            result.value.pointer.length = str_len;
            memcpy(result.value.pointer.prefix, val.data, 4);
            
            // Allocate new memory for the string content
            result.value.pointer.ptr = new char[str_len + 1];
            memcpy(result.value.pointer.ptr, val.data, str_len);
            result.value.pointer.ptr[str_len] = '\0';  // Ensure null-termination

            return result;
}
}

} // namespace duckar
