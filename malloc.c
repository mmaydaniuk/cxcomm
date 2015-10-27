/***********************************************************************
**
** This file is part of the CXCOMM project.
**
** Copyright (C) 2015 Michael Maydaniuk.  All rights reserved.
** 
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
** 02110-1301  USA
**
** 
**
**********************************************************************/
#include "malloc.h"
#include "debug.h"

char *heap_start = NULL;

struct malloc_chunk *
find_free_block(struct malloc_chunk **last, size_t size) {
  struct malloc_chunk *p = (struct malloc_chunk*) heap_start;
  
  // traverse the list
  while (p && !(p->is_free && p->size >= size)) {
    *last = p;
     p = p->fd;
  }
  
  return p;
}

struct malloc_chunk *
request_space(struct malloc_chunk* last, size_t size) {
  struct malloc_chunk *block;
  block = (struct malloc_chunk*) sbrk(0);
  char* request = (char*) sbrk(size + MALLOC_CHUNK_SIZE);
 
  if (request == (char*) -1) {
	// if sbrk fails return -1
    return NULL; 
  }

  if (last) { // NULL on first request.
    last->fd = block;
  }
  block->size = size;
  block->fd = NULL;
  block->is_free = 0;

  return block;
}

char *
malloc(size_t size) {
  struct malloc_chunk *block;
 
  if (size <= 0) {
    return NULL;
  }

  if (!heap_start) { // First call.
    block = request_space(NULL, size);
    if (!block) {
      return NULL;
    }
    heap_start = (char*) block;
  } else {
    struct malloc_chunk *last = (struct malloc_chunk*) heap_start;
    
    block = find_free_block(&last, size);
    
    if (!block) { // Failed to find free block.
      block = request_space(last, size);
      if (!block) {
        return NULL;
      }
    } else {      
	  // Found a block we can re-use
		/*
      if ((block->size - size) > (31+MALLOC_CHUNK_SIZE)) {
		 
		  // if the block we are re-using is considerably larger (32 bytes)
		  // than what we request, then split it.
		  int old_size = block->size;
		  block->size = size;
		  // save next ptr
		  struct malloc_chunk *next = (struct malloc_chunk*) block->fd;
		  char* p = (char*) block;
		  p = p + (MALLOC_CHUNK_SIZE + size);
		  
		  block->fd = (struct malloc_chunk*) p;
		  
		  // point to next free byte
		  //(char*) block->fd = (char*) block + size + MALLOC_CHUNK_SIZE;
		  
		  // create a new block header for the remainder of the block
		  struct malloc_chunk *split = (struct malloc_chunk*) block->fd;
		  
		  split->size = old_size - MALLOC_CHUNK_SIZE - size;
		  split->is_free = 1;
		  split->fd = next;
		  
		  
		  
	  }
	  */
      block->is_free = 0;
    }
  }

  return(block+1);
}

struct malloc_chunk *
get_block_ptr(char* ptr) {
  return (struct malloc_chunk*) ptr - 1;
}

void 
free(char *ptr) {
  if (!ptr) {
    return;
  }

  // TODO: merge blocks
  struct malloc_chunk* block_ptr = get_block_ptr(ptr);

  block_ptr->is_free = 1;
  
}

void
malloc_print_blocks() {
	struct malloc_chunk *p = (struct malloc_chunk*) heap_start;
  
    // traverse the list
	while (p) {
		if (p->is_free == 0) {
			printf("Used Block address = %x size = %d\n",p, p->size);
		} else {
			printf("Free Block address = %x size = %d\n",p, p->size);
		}
		p = p->fd;
	}
}

char *
calloc(size_t nelem, size_t elsize) {
  size_t size = nelem * elsize;
  char *ptr = malloc(size);
  memset(ptr, 0x72, size);
  return ptr;
}

