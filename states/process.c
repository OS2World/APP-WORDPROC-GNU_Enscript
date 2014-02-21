/* 
 * Process input according to the specified rules.
 * Copyright (c) 1997 Markku Rossi.
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

#include "defs.h"

/*
 * Global functions.
 */

void
process_file (fname)
     char *fname;
{
  Node *result;
  int return_seen = 0;

  start_state = NULL;
  current_fname = fname;

  /* Init buffer variables. */
  data_in_buffer = 0;
  bufpos = 0;
  eof_seen = 0;

  /* Enter build-in variables. */
  enter_system_variable ("filename", fname);

  /* Read in the first block of data. */
  data_in_buffer = fread (inbuf, 1, INBUFSIZE, ifp);
  if (data_in_buffer < INBUFSIZE)
    eof_seen = 1;

  if (start_state_arg)
    start_state = start_state_arg;

  /* Execute start block. */
  result = eval_statement_list (start_stmts, NULL, &return_seen);
  node_free (result);

  if (start_state == NULL)
    {
      /* No start state found, copy our input to output. */
      while (data_in_buffer)
	{
	  fwrite (inbuf, 1, data_in_buffer, ofp);
	  data_in_buffer = fread (inbuf, 1, INBUFSIZE, ifp);
	}
    }
  else
    {
      result = execute_state (start_state);
      node_free (result);
    }
}


Node *
execute_state (name)
     char *name;
{
  List *state;
  int to_read, got;
  ListItem *rule, *first_rule;
  unsigned int first_idx;
  unsigned int match_len;
  Node *result = nvoid;
  Cons *r;
  Node *exp;
  int return_seen = 0;
  int idx;

  /* Lookup state. */
  if (!strhash_get (ns_states, name, strlen (name), (void **) &state))
    {
      fprintf (stderr, _("%s: undefined state `%s'\n"), program, name);
      exit (1);
    }

  /* Begin rule? */
  for (rule = state->head; rule; rule = rule->next)
    {
      r = (Cons *) rule->data;
      if (r->car == RULE_BEGIN)
	{
	  node_free (result);
	  result = eval_statement_list ((List *) r->cdr, NULL, &return_seen);
	  if (return_seen)
	    goto out;
	  break;
	}
    }

  /* Execute this state. */
  while (1)
    {
      int eol;

      /* Do we have enough data? */
      if (bufpos >= data_in_buffer)
	{
	  if (eof_seen)
	    /* All done. */
	    break;
	  
	  /* Read more data. */
	  data_in_buffer = fread (inbuf, 1, INBUFSIZE, ifp);
	  if (data_in_buffer < INBUFSIZE)
	    eof_seen = 1;

	  bufpos = 0;
	  continue;
	}

      /* Find the end of the input line. */
      for (eol = bufpos; eol < data_in_buffer && inbuf[eol] != '\n'; eol++)
	;
      if (inbuf[eol] == '\n')
	eol++;
      if (eol >= data_in_buffer && !eof_seen && bufpos > 0)
	{
	  /* Must read more data to the buffer. */
	  memmove (inbuf, inbuf + bufpos, eol - bufpos);
	  data_in_buffer = eol - bufpos;
	  bufpos = 0;

	  to_read = INBUFSIZE - data_in_buffer;
	  got = fread (inbuf + data_in_buffer, 1, to_read, ifp);
	  if (got < to_read)
	    eof_seen = 1;

	  data_in_buffer += got;
	  continue;
	}
	
      /* Evaluate state expressions. */
      first_idx = eol;
      match_len = 0;
      first_rule = NULL;
      current_match = NULL;
      for (rule = state->head; rule; rule = rule->next)
	{
	  int err;

	  r = (Cons *) rule->data;
	  exp = (Node *) r->car;
	  if (exp == RULE_BEGIN || exp == RULE_END)
	    continue;

	  if (exp->type == nSYMBOL)
	    {
	      Node *n;

	      /* Lookup this variable by hand from global variables. */
	      if (!strhash_get (ns_vars, exp->u.sym, strlen (exp->u.sym),
				(void **) &n))
		{
		  fprintf (stderr, _("%s: error: undefined variable `%s'\n"),
			   program, exp->u.sym);
		  exit (1);
		}
	      if (n->type != nREGEXP)
		/* Skip this rule */
		continue;

	      exp = n;
	    }

	  err = re_search (&exp->u.re.compiled, inbuf, eol, bufpos,
			   eol - bufpos, &exp->u.re.matches);
	  if (err < 0)
	    /* No mach. */
	    continue;

	  idx = exp->u.re.matches.start[0];
	  if (idx >= 0 
	      && (idx < first_idx
		  || (idx == first_idx
		      && (exp->u.re.matches.end[0]
			  - exp->u.re.matches.start[0]
			  > match_len))))
	    {
	      first_idx = idx;
	      first_rule = rule;
	      match_len = (exp->u.re.matches.end[0]
			   - exp->u.re.matches.start[0]);
	      current_match = &exp->u.re.matches;
	      current_match_buf = inbuf;
	    }
	}

      /* Print all data before the first rule. */
      fwrite (inbuf + bufpos, 1, first_idx - bufpos, ofp);

      if (first_rule)
	{
	  /* Execute statements. */
	  bufpos = current_match->end[0];

	  node_free (result);
	  result = eval_statement_list ((List *)
					((Cons *) first_rule->data)->cdr,
					NULL, &return_seen);
	  if (return_seen)
	    goto out;
	}
      else
	bufpos = first_idx;
    }

out:

  /* End rule? */
  for (rule = state->head; rule; rule = rule->next)
    {
      r = (Cons *) rule->data;
      if (r->car == RULE_END)
	{
	  node_free (result);
	  result = eval_statement_list ((List *) r->cdr, NULL, &return_seen);
	}
    }

  return result;
}
