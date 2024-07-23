#include "xps.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        const char* out = "USAGE: replace [old] [new]";
        xps_writeline(xps_from_cstr(out));
        return 1;
    }

    char* old_str = xps_from_cstr(argv[1]);
    char* new_str = xps_from_cstr(argv[2]);
    char* line;

    while ((line = xps_readline()) != nullptr) {
        size_t old_len = xps_length(old_str);
        size_t new_len = xps_length(new_str);
        size_t index = 0;

        while (index != XPS_NPOS) {
            index = xps_find(line, old_str, index);
            if (index != XPS_NPOS) {
                char* start = xps_slice(line, 0, index);
                char* before = xps_concat(start, new_str);
                xps_free(start);
                char* after = xps_slice(line, index + old_len, xps_length(line));
                char* new_line = xps_concat(before, after);
                xps_free(before);
                xps_free(after);
                xps_free(line);
                line = new_line;

                index += new_len;
            }
        }

        xps_writeline(line);
        xps_free(line);
    }

    xps_free(old_str);
    xps_free(new_str);

    return 0;
}