/*  
 *      main.c
 *      by Robin Park, Sang Woo Kim 11/20/2017
 *
 *      Purpose: contains main for um emulator program
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "um_emulator.h"

int main(int argc, char *argv[])
{
    struct stat sb;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    run(fp, sb.st_size);
    exit(EXIT_SUCCESS);
}
