/* Phosphor Engine: A small but quite special game engine to create text
 *                  adventures.
 * by Mibi88
 *
 * This software is licensed under the BSD-3-Clause license:
 *
 * Copyright 2025 Mibi88
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <arena.h>

#include <stdlib.h>

int ph_arena_init(PHArena *arena, size_t chunk_size) {
    arena->chunk_size = chunk_size;

    arena->current_chunk_size = arena->chunk_size;

    arena->data = malloc(sizeof(void*));
    if(arena->data == NULL){
        return 1;
    }

    arena->data[0] = malloc(chunk_size);
    if(arena->data[0] == NULL){
        free(arena->data);
        arena->data = NULL;
        return 1;
    }

    arena->current_chunk = 0;

    arena->usage = 0;

    return 0;
}

void *ph_arena_alloc(PHArena *arena, size_t size, size_t num) {
    size_t start = arena->usage+(size-arena->usage%size);
    size_t end = start+size*num;

    if(arena->data == NULL) return NULL;

    if(end > arena->current_chunk_size){
        size_t new_chunk_size = size*num > arena->chunk_size ?
                                size*num : arena->chunk_size;
        void *new;

        new = realloc(arena->data, (arena->current_chunk+2)*sizeof(void*));
        if(new == NULL) return NULL;

        arena->data = new;

        arena->current_chunk++;

        arena->data[arena->current_chunk] = malloc(new_chunk_size);
        if(arena->data[arena->current_chunk] == NULL){
            arena->current_chunk--;
            return NULL;
        }

        /* We successfully allocated a new chunk */
        arena->usage = 0;
        arena->current_chunk_size = new_chunk_size;

        /* Recalculate the sizes */
        start = 0;
        end = start+size*num;
    }

    arena->usage = end;

    return (unsigned char*)arena->data[arena->current_chunk]+start;
}
void ph_arena_free(PHArena *arena) {
    size_t i;

    for(i=0;i<=arena->current_chunk;i++){
        free(arena->data[i]);
    }
    free(arena->data);
}
