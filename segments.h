/*  
 *      segments.h
 *      by Robin Park, Sang Woo Kim 11/17/2017
 *      COMP40 HW6
 *
 *      Purpose: Header file for segments module
 */

#ifndef SEGMENTS_INCLUDED
#define SEGMENTS_INCLUDED

#include <inttypes.h>

typedef uint32_t Umsegment_Id;
typedef uint32_t word;

void initialize_sequences(FILE *fp, long long word_count);
Umsegment_Id map_segment(int word_count);
void unmap_segment(Umsegment_Id id);
void push_word(Umsegment_Id seg_id, int offset, word value);
word retrieve_word(Umsegment_Id seg_id, int offset);
unsigned replace_segment(Umsegment_Id seg_id);
void free_segments();

#endif