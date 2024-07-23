#include "xps.h"

int main() {
    char* line;
    while ((line = xps_readline()) != nullptr) {
        if (xps_length(line) <= 20) {
            xps_writeline(line);
        } else {
            char* temp1 = xps_slice(line, 0, 10);
            char* temp2 = xps_from_cstr("...");
            char* temp3 = xps_slice(line, xps_length(line) - 7);
            char* temp4 = xps_concat(temp1, temp2);
            char* truncated_line = xps_concat(temp4, temp3);

            xps_writeline(truncated_line);
            xps_free(temp1);
            xps_free(temp2);
            xps_free(temp3);
            xps_free(temp4);
            xps_free(truncated_line);
        }
        xps_free(line);
    }
    return 0;
}