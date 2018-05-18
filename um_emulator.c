/*  
 *      um_emulator.c
 *      by Robin Park, Sang Woo Kim 11/20/2017
 *
 *      Purpose: Implementation file for um_emulator module
 */

#include <assert.h>
#include <bitpack.h>
#include <inttypes.h>
#include "um_emulator.h"
#include "segments.h"

typedef uint32_t Um_instruction;
typedef uint32_t Register;

typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

Register *registers;
long long word_count;

void initialize_registers();
void process_instruction(Um_instruction word, FILE *fp);
void conditional_move(unsigned ra, unsigned rb, unsigned rc);
void segmented_load(unsigned ra, unsigned rb, unsigned rc);
void segmented_store(unsigned ra, unsigned rb, unsigned rc);
void add(unsigned ra, unsigned rb, unsigned rc);
void multiply(unsigned ra, unsigned rb, unsigned rc);
void divide(unsigned ra, unsigned rb, unsigned rc);
void nand(unsigned ra, unsigned rb, unsigned rc);
void halt(FILE *fp);
void map(unsigned rb, unsigned rc);
void unmap(unsigned rc);
void output(unsigned rc);
void input(unsigned rc);
void loadprog(unsigned rb, unsigned rc);
void loadval(unsigned ra, uint32_t value);

/*
 * purpose: runs the whole program
 * parameters: file pointer, file size in bytes
 * returns: void
 */
void run(FILE *fp, long long file_size)
{
    word_count = file_size / sizeof(word);
    initialize_sequences(fp, word_count);
    initialize_registers();

    do {
        assert(registers[8] < word_count);
        word retrieved = retrieve_word(0, registers[8]);
        registers[8]++;
        process_instruction(retrieved, fp);
    } while(1);
}

/*
 * purpose: initializes all registers to zero
 * parameters:
 * returns: void
 */
void initialize_registers()
{
    registers = malloc(sizeof(Register) * 9);

    for (int i = 0; i < 9; i++) {
        registers[i] = 0;
    }
}

/*
 * purpose: extracts opcode/register/value information from a word and calls
            corresponding instructions
 * parameters: word, file pointer
 * returns: void
 */
void process_instruction(Um_instruction word, FILE *fp)
{
    Um_opcode opcode = Bitpack_getu(word, 4, 28);
    unsigned ra = Bitpack_getu(word, 3, 6);
    unsigned rb = Bitpack_getu(word, 3, 3);
    unsigned rc = Bitpack_getu(word, 3, 0);

    switch(opcode) {
        case CMOV :
            conditional_move(ra, rb, rc);
            break;
        case SLOAD :
            segmented_load(ra, rb, rc);
            break;
        case SSTORE :
            segmented_store(ra, rb, rc);
            break;
        case ADD :
            add(ra, rb, rc);
            break;
        case MUL :
            multiply(ra, rb, rc);
            break;
        case DIV :
            divide(ra, rb, rc);
            break;
        case NAND :
            nand(ra, rb, rc);
            break;
        case HALT :
            halt(fp);
            break;
        case ACTIVATE :
            map(rb, rc);
            break;
        case INACTIVATE :
            unmap(rc);
            break;
        case OUT :
            output(rc);
            break;
        case IN :
            input(rc);
            break;
        case LOADP :
            loadprog(rb, rc);
            break;
        case LV :
            ra = Bitpack_getu(word, 3, 25);
            uint32_t value = Bitpack_getu(word, 25, 0);
            loadval(ra, value);
            break;
        default :
            fprintf(stderr, "Invalid instruction\n");
            exit(EXIT_FAILURE);
    }
}

void conditional_move(unsigned ra, unsigned rb, unsigned rc)
{
    if (registers[rc] != 0) {
        registers[ra] = registers[rb];
    }
}

void segmented_load(unsigned ra, unsigned rb, unsigned rc)
{
    registers[ra] = retrieve_word(registers[rb], registers[rc]);
}

void segmented_store(unsigned ra, unsigned rb, unsigned rc)
{
    push_word(registers[ra], registers[rb], registers[rc]);
}

void add(unsigned ra, unsigned rb, unsigned rc)
{
    registers[ra] = registers[rb] + registers[rc];
}

void multiply(unsigned ra, unsigned rb, unsigned rc)
{
    registers[ra] = registers[rb] * registers[rc];
}

void divide(unsigned ra, unsigned rb, unsigned rc)
{
    registers[ra] = registers[rb] / registers[rc];
}

void nand(unsigned ra, unsigned rb, unsigned rc)
{
    registers[ra] = ~(registers[rb] & registers[rc]);
}

void halt(FILE *fp)
{
    free_segments();
    free(registers);
    fclose(fp);
    exit(EXIT_SUCCESS);
}

void map(unsigned rb, unsigned rc)
{
    registers[rb] = map_segment(registers[rc]);
}

void unmap(unsigned rc)
{
    unmap_segment(registers[rc]);
}

void output(unsigned rc)
{
    assert(registers[rc] <= 255);
    printf("%c", registers[rc]);
}

void input(unsigned rc)
{
    int c = fgetc(stdin);

    if (c == EOF) {
        registers[rc] = ~0U;
        return;
    }

    assert((c >= 0) && (c <= 255));
    registers[rc] = c;
}

void loadprog(unsigned rb, unsigned rc)
{
    word_count = replace_segment(registers[rb]);
    registers[8] = registers[rc];
}

void loadval(unsigned ra, uint32_t value)
{
    registers[ra] = value;
}
