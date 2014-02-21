/* 
 * Non-failing memory allocation routines.
 * Copyright (c) 1995 Markku Rossi.
 *
 * Author: Markku Rossi <mtr@iki.fi>
 */

/*
 * This file is part of GNU enscript.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "gsint.h"

/*
 * Global functions.
 */

void *
xmalloc (size_t size)
{
  void *ptr;

  ptr = malloc (size);
  if (ptr == NULL)
    fatal (_("xmalloc(): couldn't allocate %d bytes\n"), size);

  return ptr;
}


void *
xcalloc (size_t num, size_t size)
{
  void *ptr;

  ptr = calloc (num, size);
  if (ptr == NULL)
    fatal (_("xcalloc(): couldn't allocate %d bytes\n"), size);

  return ptr;
}


void *
xrealloc (void *ptr, size_t size)
{
  void *nptr;

  if (ptr == NULL)
    return xmalloc (size);

  nptr = realloc (ptr, size);
  if (nptr == NULL)
    fatal (_("xrealloc(): couldn't reallocate %d bytes\n"), size);

  return nptr;
}


void 
xfree (void *ptr)
{
  if (ptr == NULL)
    return;

  free (ptr);
}


char *
xstrdup (char *str)
{
  char *tmp;

  tmp = xmalloc (strlen (str) + 1);
  strcpy (tmp, str);

  return tmp;
}
