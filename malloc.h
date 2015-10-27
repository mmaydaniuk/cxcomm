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
#ifndef MALLOC_H
#define MALLOC_H

struct malloc_chunk {
	size_t 	size;
	char 		is_free;
	struct 	malloc_chunk *fd;
};

#define MALLOC_CHUNK_SIZE sizeof(struct malloc_chunk)

struct malloc_chunk *find_free_block(struct malloc_chunk **last, size_t size);
struct malloc_chunk *request_space(struct malloc_chunk* last, size_t size);
char *malloc(size_t size);
struct malloc_chunk *get_block_ptr(char *ptr);
void free(char *ptr);
char *calloc(size_t nelem, size_t elsize);
void malloc_print_blocks();
#endif
