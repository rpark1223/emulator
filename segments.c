/*  
 *      segments.c
 *      by Robin Park, Sang Woo Kim 11/20/2017
 *
 *      Purpose: Implementation file for segments module
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <bitpack.h>
#include <uarray.h>
#include <seq.h>
#include "segments.h"

Seq_T mapped; /* contains all mapped segments */
Seq_T unmapped; /* contains identifiers for all unmapped segments */

void initialize_zero_segment(FILE *fp, long long word_count);

/*
 * purpose: initializes mapped and unmapped sequences
 * parameters: file pointer, number of words in file
 * returns: void
 */
void initialize_sequences(FILE *fp, long long word_count)
{
    mapped = Seq_new(20);
    unmapped = Seq_new(20);

    initialize_zero_segment(fp, word_count);
}

/*
 * purpose: reads all words from file into the zero segment
 * parameters: file pointer, number of words in file
 * returns: void
 */
void initialize_zero_segment(FILE *fp, long long word_count)
{
    map_segment(word_count);

    word new_word = 0;
    int c;
    for (int i = 0; i < word_count; i++) {
        for (unsigned j = 0; j < sizeof(word); j++) {
            c = fgetc(fp);
            if (c == EOF) {
                return;
            }
            new_word = Bitpack_newu(new_word, 8, 24 - (j * 8), c);
        }
        push_word(0, i, (word)new_word);
    }
}

/*
 * purpose: maps a new segment with the given number of words
 * parameters: desired length of new segment
 * returns: identifier of newly-mapped segment
 */
Umsegment_Id map_segment(int word_count)
{
    Umsegment_Id seg_id;
    UArray_T New_segment = UArray_new(word_count, sizeof(word));

    for (int i = 0; i < word_count; i++) { /* Initializes whole segment to 0 */
        word *initial_word = UArray_at(New_segment, i);
        *initial_word = 0;
    }

    if (Seq_length(unmapped) != 0) { /* Checks if there are reusable segments */
        seg_id = (Umsegment_Id)(uintptr_t)Seq_remlo(unmapped);
        assert(seg_id != 0);
        Seq_put(mapped, seg_id, New_segment);
    } else {
        Seq_addhi(mapped, New_segment);
        seg_id = Seq_length(mapped) - 1;
    }

    return seg_id;
}

/*
 * purpose: frees a segment within the mapped sequence
 * parameters: identifier of to-be-unmapped segment
 * returns: void
 */
void unmap_segment(Umsegment_Id id)
{
    assert(id != 0);
    assert(Seq_get(mapped, id) != NULL); /* Checks it hasn't been unmapped b4 */
    UArray_T remove = Seq_get(mapped, id);
    UArray_free(&remove);
    Seq_put(mapped, id, NULL);
    Seq_addlo(unmapped, (void *)(uintptr_t)id);
}

/*
 * purpose: places a word in the indicated [seg id][offset] pair
 * parameters: [seg id][offset] pair, value to be pushed
 * returns: void
 */
void push_word(Umsegment_Id seg_id, int offset, word value)
{
    UArray_T Segment = Seq_get(mapped, seg_id);
    assert(Segment != NULL);
    word *new_word = UArray_at(Segment, offset);
    *new_word = value;
}

/*
 * purpose: retrieves a word from the indicated [seg id][offset] pair
 * parameters: [seg id][offset] pair
 * returns: word
 */
word retrieve_word(Umsegment_Id seg_id, int offset)
{
    UArray_T Segment = Seq_get(mapped, seg_id);
    assert(Segment != NULL);
    return *(word *)(UArray_at(Segment, offset));
}

/*
 * purpose: duplicates the indicated segment and replaces the zero segment
 * parameters: identifier of segment to replace the zero segment
 * returns: length of new zero segment
 */
unsigned replace_segment(Umsegment_Id seg_id)
{
    if (seg_id == 0) {
        return UArray_length((UArray_T)Seq_get(mapped, 0));
    }

    UArray_T Segment = Seq_get(mapped, seg_id);
    UArray_T newSeg = UArray_copy(Segment, UArray_length(Segment));

    UArray_T remove = Seq_get(mapped, 0);
    UArray_free(&remove);
    Seq_put(mapped, 0, newSeg);

    return UArray_length(newSeg);
}

/*
 * purpose: frees all memory
 * parameters: none
 * returns: void
 */
void free_segments()
{
    for (int i = 0; i < Seq_length(mapped); i++) {
        if (Seq_get(mapped, i) != NULL) {
            UArray_T remove = Seq_get(mapped, i);
            UArray_free(&remove);
        }
    }

    Seq_free(&mapped);
    Seq_free(&unmapped);
}
