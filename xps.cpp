#include "xps.h"

size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}
size_t strlen(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
        ++length;
    }
    return length;
}
char* xps_to_cstr(const char* xpStr){
    size_t length = xps_length(xpStr);
    char* cStr = new char[length +1];

    for(size_t i = 0; i < length; ++i){
        cStr[i] = xps_getchar(xpStr, i);
    }
    cStr [length] = '\0';

    return cStr;
}
void copy_xps_string(const char* source, char* destination, size_t dest_offset, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        xps_setchar(destination, dest_offset + i, xps_getchar(source, i));
    }
}
void set_last_chunk_len(char* xp_string, size_t last_chunk_len) {
    size_t num_chunks = (xps_length(xp_string) + 14) / 15;
    xp_string[(num_chunks - 1) * 16] = last_chunk_len;
}
int xps_compare(const char* lhs, const char* rhs) {
    while(true){
       auto leftChunk = static_cast<unsigned char> (*lhs++);
       auto rightChunk = static_cast<unsigned char> (*rhs++);

       int cLength = min(leftChunk, rightChunk);

       for (int index = 0; index < cLength; ++index, ++lhs, ++rhs){
        if (*lhs < *rhs){
            return -1;
        }
        else if (*lhs > *rhs){
            return 1;
        }
       }
      if (leftChunk < rightChunk) return -1;
      if (leftChunk > rightChunk) return 1;
      if ((leftChunk < 0x0F) || (rightChunk < 0x0F)) {
        if (leftChunk == rightChunk) {
            return 0;
        }
      }
    }
}
char* xps_concat(const char* lhs, const char* rhs) {
   char* lhsC = xps_to_cstr(lhs);
   char* rhsC = xps_to_cstr(rhs);

   size_t newLen = strlen(lhsC) + strlen(rhsC);

   char* newCstr = new char[newLen +1];

   for (size_t i = 0; i < strlen(lhsC); i++){
    newCstr[i] = lhsC[i];
   }
   for (size_t j = 0; j < strlen(rhsC); j++){
    newCstr[strlen(lhsC)+ j] = rhsC[j];
   }

   newCstr[newLen] = '\0';

   char* newXPSTR = xps_from_cstr(newCstr);

   xps_free(lhsC);
   xps_free(rhsC);
   xps_free(newCstr);
   

   return newXPSTR;
}
size_t xps_find(const char* str, const char* substr) {
    size_t str_len = xps_length(str);
    size_t substr_len = xps_length(substr);

    if (substr_len == 0) {
        return 0;
    }

    for (size_t i = 0; i < str_len; ++i) {
        if (xps_getchar(str, i) == xps_getchar(substr, 0)) {
            size_t j = 0;

            while (j < substr_len && i + j < str_len && xps_getchar(str, i + j) == xps_getchar(substr, j)) {
                ++j;
            }

            if (j == substr_len) {
                return i;
            }
        }
    }

    return XPS_NPOS;
}
size_t xps_find(const char* str, const char* substr, size_t start) {
    size_t str_len = xps_length(str);
    size_t substr_len = xps_length(substr);

    if (substr_len == 0) {
        return start; 
    }

    for (size_t i = start; i < str_len; ++i) {
        if (xps_getchar(str, i) == xps_getchar(substr, 0)) {
            size_t j = 0;

            while (j < substr_len && i + j < str_len && xps_getchar(str, i + j) == xps_getchar(substr, j)) {
                ++j;
            }

            if (j == substr_len) {
                return i;
            }
        }
    }

    return XPS_NPOS;
}
void xps_free(char* str) {
    delete[] str;
}
char* xps_from_cstr(const char* cstr) {
   size_t inputLen = strlen(cstr);
   size_t encodedLen = inputLen + (inputLen / 15) + (inputLen % 15 ? 2 : 1);
   char* encodedStr= new char[encodedLen];
   char* nextChar = encodedStr;
   size_t index = 0;

   while(index < inputLen){
    size_t remainingChars = inputLen - index;
    size_t chunkSize = min(remainingChars, static_cast<size_t>(15));
    *nextChar++ = static_cast<char>(chunkSize);
    for(size_t charIndex = 0;charIndex < chunkSize; ++charIndex, ++index){
        *nextChar++ = cstr[index];
    }
   }

   *nextChar = 0;
   return encodedStr;
}
char xps_getchar(const char* str, size_t index) {
    const char* current_chunk = str;

    while (*current_chunk != 0 && index >= static_cast<size_t>(*current_chunk)) {

        index -= static_cast<size_t>(*current_chunk);
        current_chunk += static_cast<size_t>(*current_chunk) + 1;
    }

    return current_chunk[index + 1]; 
}

size_t xps_length(const char* str) {
    size_t length = 0;
    const char* ptr = str;

    if (!ptr || *ptr == 0) {
        return length;
    }

    while (true) {
        size_t chunk_length = static_cast<size_t>(*ptr);

        if (chunk_length <= 15) {
            length += chunk_length;

            if (chunk_length < 15 || !*(ptr + chunk_length + 1)) {
                break;
            }
            ptr += chunk_length + 1; 
        } else {

            break;
        }
    }

    return length;
}
void xps_setchar(char* str, size_t index, char c) {
    size_t chunk_index = index / 15;
    size_t char_index = index % 15;

    size_t xp_index = chunk_index * 16 + char_index + 1; 

    str[xp_index] = c;
}
char* xps_slice(const char* str, size_t start) {

    size_t str_len = xps_length(str);

    if (start >= str_len) {
        char* empty_string = new char[1];
        empty_string[0] = 0;
        return empty_string;
    }

    size_t slice_len = str_len - start;
    size_t num_chunks = (slice_len + 14) / 15; 

    char* slice_xp_string = new char[num_chunks * 16];

    for (size_t i = 0; i < slice_len; ++i) {
        xps_setchar(slice_xp_string, i, xps_getchar(str, start + i));
    }

    size_t last_chunk_len = slice_len % 15;
    if (last_chunk_len == 0) last_chunk_len = 15;
    slice_xp_string[(num_chunks - 1) * 16] = last_chunk_len;

    return slice_xp_string;
}

char* xps_slice(const char* str, size_t start, size_t stop) {
    size_t str_len = xps_length(str);

    if (start >= str_len || start >= stop) {
        char* empty_string = new char[1];
        empty_string[0] = 0; 
        return empty_string;
    }
    stop = (stop < str_len) ? stop : str_len; 

    size_t slice_len = stop - start;
    size_t num_chunks = (slice_len) / 15 + 1;

    char* slice_xp_string = new char[num_chunks + slice_len];

    for (size_t i = 0; i < slice_len; ++i) {
        xps_setchar(slice_xp_string, i, xps_getchar(str, start + i));
    }
    for (size_t i = 0; i < num_chunks; ++i){
    size_t chunk_index = i * 16; 
    size_t chunk_length = (i == num_chunks - 1) ? 15 : 15; 
    slice_xp_string[chunk_index] = chunk_length; 
}
    size_t last_chunk_len = slice_len % 15;
    // if (last_chunk_len == 0) last_chunk_len = 15;
    slice_xp_string[(num_chunks - 1) * 16] = last_chunk_len;

    return slice_xp_string;
}