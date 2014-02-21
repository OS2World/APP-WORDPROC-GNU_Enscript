/* 
 * The main for states.
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
#include "getopt.h"

/*
 * Global variables.
 */

char *program;

/* Namespaces. */
StringHashPtr ns_prims = NULL;
StringHashPtr ns_vars = NULL;
StringHashPtr ns_subs = NULL;
StringHashPtr ns_states = NULL;

/*
 * Global expressions which are evaluated after config file has been
 * parsed.
 */
List *global_stmts = NULL;

/* Statements from the start{} block. */
List *start_stmts = NULL;

/* Start and name rules. */
List *startrules = NULL;
List *namerules = NULL;

Node *nvoid = NULL;

FILE *ifp = NULL;
char *inbuf = NULL;
unsigned int data_in_buffer;
unsigned int bufpos;
int eof_seen;
char *current_fname;

struct re_registers *current_match = NULL;
char *current_match_buf = NULL;

/* Options. */

/*
 * -D VAR=VAL, --define=VAR=VAL
 *
 * Define variable VAR to have value VAL.
 */

VariableDef *vardefs = NULL;
VariableDef *vardefs_tail = NULL;

/*
 * -f NAME, --file=NAME
 *
 * Read state definitions from file NAME.  The default is "states.st" in 
 * the current working directory.
 */

char *defs_file = "states.st";
unsigned int linenum = 1;

/*
 * -o FILE, --output=FILE
 *
 * Save output to file FILE, the default is stdout.
 */

FILE *ofp = stdout;

/*
 * -s NAME, --state=NAME
 *
 * Start from state NAME.  As a default, start date is resolved during
 * the program startup by using start, namerules and startrules rules.
 */
char *start_state_arg = NULL;
char *start_state;

/*
 * -W LEVEL, --warning=LEVEL
 *
 * Set the warning level to LEVEL.
 */
WarningLevel warning_level = WARN_LIGHT;


/*
 * Static variables.
 */

static struct option long_options[] =
{
  {"define",		required_argument,	0, 'D'},
  {"file",		required_argument,	0, 'f'},
  {"help",		no_argument,		0, 'h'},
  {"output",		required_argument,	0, 'o'},
  {"state",		required_argument,	0, 's'},
  {"version",		no_argument,		0, 'V'},
  {"warning",		required_argument,	0, 'W'},

  {NULL, 0, 0, 0},
};

/* Version string. */
char version[256];


/*
 * Prototypes for static functions.
 */

static void usage ___P ((void));


/*
 * Global functions.
 */

int
main (argc, argv)
     int argc;
     char *argv[];
{
  int c;
  VariableDef *vardef;

  /* Get program's name. */
  program = strrchr (argv[0], '/');
  if (program == NULL)
    program = argv[0];
  else
    program++;

  /* Make getopt_long() to use our modified program name. */
  argv[0] = program;

  /* Format version string. */
  sprintf (version, _("states for GNU %s %s"), PACKAGE, VERSION);

  /* Internationalization. */
#if HAVE_SETLOCALE 
#if HAVE_LC_MESSAGES
  setlocale (LC_MESSAGES, "");
#endif
#endif
#if ENABLE_NLS
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
#endif

  /* Init namespaces. */
  ns_prims = strhash_init ();
  ns_vars = strhash_init ();
  ns_subs = strhash_init ();
  ns_states = strhash_init ();

  global_stmts = list ();
  start_stmts = list ();
  startrules = list ();
  namerules = list ();

  nvoid = node_alloc (nVOID);
  inbuf = (char *) xmalloc (INBUFSIZE);

  init_primitives ();

  re_set_syntax (RE_SYNTAX_AWK);

  /* Enter some system variables. */
  enter_system_variable ("program", program);
  enter_system_variable ("version", version);

  /* Parse arguments. */
  while (1)
    {
      int option_index = 0;

      c = getopt_long (argc, argv, "D:f:ho:s:VW:", long_options,
		       &option_index);
      if (c == EOF)
	break;

      switch (c)
	{
	case 'D':		/* define variable */
	  vardef = (VariableDef *) xcalloc (1, sizeof (*vardef));
	  vardef->sym = (char *) xmalloc (strlen (optarg) + 1);
	  strcpy (vardef->sym, optarg);

	  vardef->val = strchr (vardef->sym, '=');
	  if (vardef->val == NULL)
	    {
	      fprintf (stderr, _("%s: malformed variable definition \"%s\"\n"),
		       program, vardef->sym);
	      exit (1);
	    }
	  *vardef->val = '\0';
	  vardef->val++;

	  if (vardefs)
	    vardefs_tail->next = vardef;
	  else
	    vardefs = vardef;
	  vardefs_tail = vardef;
	  break;

	case 'f':		/* definitions file */
	  defs_file = optarg;
	  break;

	case 'h':		/* help */
	  usage ();
	  exit (0);
	  break;

	case 'o':		/* output file */
	  ofp = fopen (optarg, "w");
	  if (ofp == NULL)
	    {
	      fprintf (stderr,
		       _("%s: couldn't create output file \"%s\": %s\n"),
		       program, optarg, strerror (errno));
	      exit (1);
	    }
	  break;

	case 's':		/* start state */
	  start_state_arg = optarg;
	  break;

	case 'V':		/* version */
	  printf ("%s\n", version);
	  exit (0);
	  break;

	case 'W':		/* warning level */
	  if (strcmp (optarg, "light") == 0)
	    warning_level = WARN_LIGHT;
	  else if (strcmp (optarg, "all") == 0)
	    warning_level = WARN_ALL;
	  else
	    {
	      fprintf (stderr,
		       _("%s: unknown warning level `%s'\n"),
		       program, optarg);
	      exit (1);
	    }
	  break;

	case '?':		/* Errors found during getopt_long(). */
	  fprintf (stderr, _("Try `%s --help' for more information.\n"),
		   program);
	  exit (1);
	  break;

	default:
	  printf ("Hey! main() didn't handle option \"%c\" (%d)", c, c);
	  if (optarg)
	    printf (" with arg %s", optarg);
	  printf ("\n");
	  abort ();
	  break;
	}
    }

  /* Parse config file. */

  yyin = fopen (defs_file, "r");
  if (yyin == NULL)
    {
      fprintf (stderr, _("%s: couldn't open definition file `%s': %s\n"),
	       program, defs_file, strerror (errno));
      exit (1);
    }

  yyparse ();
  fclose (yyin);

  /* Evaluate all top-level statements. */
  {
    Node *n;
    int return_seen = 0;

    n = eval_statement_list (global_stmts, NULL, &return_seen);
    node_free (n);
  }

  /* Define variables given at the command line. */
  for (vardef = vardefs; vardef; vardef = vardef->next)
    {
      Node *val;
      Node *old_val;

      val = node_alloc (nSTRING);
      val->u.str.len = strlen (vardef->val);
      val->u.str.data = (char *) xmalloc (val->u.str.len);
      memcpy (val->u.str.data, vardef->val, val->u.str.len);

      if (!strhash_put (ns_vars, vardef->sym, strlen (vardef->sym),
			val, (void **) &old_val))
	{
	  fprintf (stderr, _("%s: out of memory\n"), program);
	  exit (1);
	}
      node_free (old_val);
    }

  /* Process files. */
  if (optind == argc)
    {
      ifp = stdin;
      process_file ("(stdin)");
    }
  else
    for (; optind < argc; optind++)
      {
	ifp = fopen (argv[optind], "r");
	if (ifp == NULL)
	  {
	    fprintf (stderr, _("%s: couldn't open input file `%s': %s\n"),
		     program, argv[optind], strerror (errno));
	    exit (1);
	  }
	process_file (argv[optind]);
	fclose (ifp);
      }

  /* Close output file. */
  if (ofp != stdout)
    fclose (ofp);

  return 0;
}


/* 
 * Static functions.
 */

static void
usage ()
{
  printf (_("\
Usage: %s [OPTION]... [FILE]...\n\
Mandatory arguments to long options are mandatory for short options too.\n"),
          program);
  printf (_("\
  -D, --define=VAR=VAL       define variable VAR to have value VAR\n\
  -f, --file=NAME            read state definitions from file NAME\n\
  -h, --help                 print this help and exit\n\
  -o, --output=NAME          save output file file NAME\n\
  -s, --state=NAME           start from state NAME\n\
  -V, --version              print version number\n\
  -W, --warning=LEVEL        set the warning level to LEVEL\n"));
}
