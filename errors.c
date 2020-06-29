#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

void handleError(ERROR error){
    switch (error) {
        case ERROR_OUT_OF_MEMORY:
            fprintf(stderr, "FATAL ERROR: Out of memory\n");
            exit(1);
    }
}