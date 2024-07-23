#include "xps.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        char* usage = xps_from_cstr("USAGE: match [pattern]");
        xps_writeline(usage);
        xps_free(usage);
        return 1;
    }

    char* pattern = xps_from_cstr(argv[1]);
    const char* input;
    char* star = xps_from_cstr("*");
    char* match = xps_from_cstr("Match!");
    char* noMatch = xps_from_cstr("No match.");

    while((input = xps_readline()) != nullptr) {
        if (xps_find(pattern, star) == XPS_NPOS){
            if (xps_find(input, pattern) != XPS_NPOS){
                 xps_writeline(match);
            }
            else{
                xps_writeline(noMatch);
            }    
        }
        else {
            size_t start = 0;
            bool real_match = false;
            size_t initialchunk = 0;

            while(xps_find(pattern, star, start) != XPS_NPOS){
                 char* chunk_pattern = xps_slice(pattern, start, xps_find(pattern, star, start));

                if(xps_find(input, chunk_pattern, initialchunk) == XPS_NPOS){
                    real_match = true;
                    xps_free(chunk_pattern);
                    break;
                }

                size_t initialindex = initialchunk;
                initialchunk = xps_find(input, chunk_pattern, initialindex) + xps_length(chunk_pattern);

                xps_free(chunk_pattern);
                size_t temp = start;
                start = xps_find(pattern, star, temp) + 1;
            }

            char* chunk_pattern = xps_slice(pattern, start);

            if(xps_find(input, chunk_pattern, initialchunk) == XPS_NPOS){
                real_match = true;
            }
            

            xps_free(chunk_pattern);
            if(!real_match){
                xps_writeline(match);
            }
            else{
                xps_writeline(noMatch);
            }

        }
        delete[] input;
    }
    xps_free(match);
    xps_free(noMatch);
    xps_free(star);
    xps_free(pattern);
    return 0;
}
