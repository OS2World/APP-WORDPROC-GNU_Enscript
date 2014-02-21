/* 
 * Argument handling and main.
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
#include "getopt.h"

#ifdef OS2
  char * WildcardExpandFilename ( char * );

#define INCL_DOSFILEMGR   /* File Manager values */
#define INCL_DOSERRORS    /* DOS error values */
#include <os2.h>

  void save_path_location( char *, char * );

#endif 


/*
 * Prototypes for static functions.
 */

/*
 * Open output file according to user options.  Void if output file
 * has already been opened.
 */
static void open_output_file ();

/* Close output file. */
static void close_output_file ();

/* Handle options from environment variable <var> */
static void handle_env_options ___P ((char *var));

/* Handle options from <argv> array. */
static void handle_options ___P ((int argc, char *argv[]));

/* List options and their values. */
static void do_list_options ();

/* Print usage info. */
static void usage ();

/* Print version info. */
static void version ();


/*
 * Global variables.
 */

char *program;			/* Program's name, used for messages. */
FILE *ofp = NULL;		/* Output file. */
void *printer_context;		/* Context for the printer. */
char version_string[256];	/* Enscript's version string. */
char ps_version_string[20];	/* Version string for PS procsets. */
char date_string[256];		/* Preformatted time string. */
struct tm run_tm;		/* Time when program is run. */
struct tm mod_tm;		/* Last modification time for current file. */
struct passwd *passwd;		/* Passwd entry for the user running this 
				   program. */

/* Path to our library. */
char *enscript_library = LIBRARY;

/* Library lookup path. */
char libpath[1024];

/* AFM library lookup path. */
char *afm_path = NULL;
char afm_path_buffer[1024];

MediaEntry *media_names = NULL;	/* List of known media. */
MediaEntry *media = NULL;	/* Entry for used media. */
int bs = 8;			/* The backspace character. */

/* Statistics. */
int total_pages = 0;		/* Total number of pages printed. */
int num_truncated_lines = 0;	/* Number of lines truncated. */
int num_missing_chars = 0;	/* Number of unknown characters. */
int missing_chars[256] = {0};	/* Table of unknown characters. */
int num_non_printable_chars = 0; /* Number of non-printable characters. */
int non_printable_chars[256] = {0}; /* Table of non-printable characters. */

/* Output media dimensions that are used during PostScript emission. */
int d_page_w = 0;		/* page's width */
int d_page_h = 0;		/* page's height */
int d_header_w = 0;		/* fancy header's width */
int d_header_h = 0;		/* fancy header's height */
int d_footer_h = 0;		/* fancy footer's height */
int d_output_w = 0;		/* output area's width */
int d_output_h = 0;		/* output area's height  */
int d_output_x_margin = 5;	/* output area's x marginal */
int d_output_y_margin = 2;	/* output area's y marginal */

/* Document needed resources. */
StringHashPtr res_fonts;	/* fonts */

/* Fonts to download. */
StringHashPtr download_fonts;

/* Additional key-value pairs, passed to the generated PostScript code. */
StringHashPtr pagedevice;	/* for setpagedevice */
StringHashPtr statusdict;	/* for statusdict */

/* User defined strings. */
StringHashPtr user_strings;

/* Cache for AFM files. */
StringHashPtr afm_cache = NULL;
StringHashPtr afm_info_cache = NULL;

/* AFM library handle. */
AFMHandle afm = NULL;


/* Options. */

/*
 * Free single-letter options are: J, Q, w, W, x, y, Y
 */

/* 
 * -1, -2, --columns
 *
 * Number of columns per page.  Default is 1 column.
 */
int num_columns = 1;

/*
 * -a PAGES, --pages=PAGES
 *
 * Specify which pages are printed.
 */
PageRange *page_ranges = NULL;

/*
 * -A ALIGN, --file-align=ALIGN
 *
 * Align input files to start from ALIGN page count.  This is handy
 * for two-side printings.
 */
unsigned int file_align = 1;

/* 
 * -b, --header
 * 
 * Set string to be used as a page header.  As a default page header is 
 * constructed from filename, date and page number.
 */
char *page_header = NULL;

/* 
 * -B, --no-header
 *
 * Do not print page headers.
 */

/*
 * -c, --truncate-lines
 *
 * Truncate lines that are longer than the page width.  Default is wrap.
 */
int truncate_lines = 0;

/*
 * -C, --line-numbers
 *
 * Precede each line with its line number.  As a default, do not mark
 * line numbers.
 */
int line_numbers = 0;

/*
 * -d, -P, --printer
 *
 * Name of the printer to which output is send.  Defaults to system's
 * default printer.
 */
char *printer = NULL;
char printer_buf[256];

/* 
 * -e [CHAR], --escapes[=CHAR]
 *
 * Enable special escape ('\000') interpretation.  If option CHAR is given
 * it is assumed to specify the escape character.
 */
int special_escapes = 0;
int escape_char = '\0';

/*
 * -E [LANG], --pretty-print[=LANG]
 * 
 * Prettyprint program source code.  Prettyprinting is handled by
 * creating an input filter with states-preprocessor.  States makes an
 * educated guess about the start state but sometimes it fails, so the
 * start state can also be specified to be LANG.  This option
 * overwrites input filter and enables special escapes.
 */

int pretty_print = 0;
char *pp_start_state = NULL;
char pp_filter[4096];

/*
 * -f, --font
 *
 * Select body font.
 */
char *Fname = "Courier";
FontPoint Fpt = {10.0, 10.0};
FontPoint default_Fpt;		/* Point size of the original font. */
char *default_Fname;		/* Name of the original font. */
int user_body_font_defined = 0;	/* Has user defined new body font? */

double font_widths[256];	/* Width array for body font. */
char font_ctype[256];		/* Font character types. */
int font_is_fixed;		/* Is body font a fixed pitch font? */
double font_bbox_lly;		/* Font's bounding box's lly-coordinate. */

/*
 * -F, --header-font
 *
 * Select font to be used to print the standard simple header.
 */
char *HFname = "Courier-Bold";
FontPoint HFpt = {10.0, 10.0};

/*
 * -g, --print-anyway
 *
 * Print document even it contains binary data.  This does nothing
 * since enscript prints files anyway.
 */

/* 
 * -G, --fancy-header
 *
 * Add a fancy header to top of every page.  There are several header styles
 * but the default is 'no fancy header'.
 */
HeaderType header = HDR_SIMPLE;
char *fancy_header_name = NULL;
char fancy_header_default[256];

/*
 * -h, --no-job-header
 *
 * Supress the job header page.
 */
static int no_job_header = 0;

/*
 * -H num, --highlight-bars=num
 *
 * Print highlight bars under text.  Bars will be <num> lines high.
 * As a default, do not print bars.
 */
unsigned int highlight_bars = 0;

/* 
 * -i, --indent
 *
 * Indent every line this many characters.
 */
double line_indent = 0.0;
char *line_indent_spec = "0";

/*
 * -I CMD, --filter=CMD
 *
 * Read input files through input filter CMD.
 */
char *input_filter = NULL;

/*
 * -j, --borders
 *
 * Print borders around columns.
 */
int borders = 0;

/*
 * -k, --page-prefeed
 * -K, --no-page-prefeed
 *
 * Control page prefeed.
 */
int page_prefeed = 0;

/* 
 * -L, --lines-per-page
 *
 * Specify how many lines should be printed on a single page.  Normally
 * enscript counts it from font point sizes.
 */
unsigned int lines_per_page = (unsigned int) -1;

/*
 * -m, --mail
 *
 * Send mail notification to user after print job has been completed.
 */
int mail = 0;

/* 
 * -M, --media
 *
 * Name of the output media.  Default is A4.
 */
char *media_name = "A4";
char media_name_buffer[256];

/*
 * -n, --copies
 *
 * Number of copies to print.
 */
int num_copies = 1;

/*
 * -N, --newline
 *
 * Set the newline character: '\n' or '\r'.  As a default, the newline
 * character is specified by the input encoding.
 */
int nl = -1;

/* 
 * -o, -p, --output
 *
 * Leave output to the specified file.  As a default result is spooled to 
 * printer.
 */
char *output_file = OUTPUT_FILE_NONE;

/*
 * -O, --missing-characters
 * 
 * List all missing characters.  Default is no listing.
 */
int list_missing_characters = 0;

/* 
 * -q, --quiet
 *
 * Do not tell what we are doing.  Default is to tell something but
 * not --verbose.
 */
int quiet = 0;

/*
 * -r, --landscape
 * -R, --portrait
 *
 * Print with page rotated 90 degrees (landscape mode).  Default is
 * portrait.
 */
int landscape = 0;

/* 
 * -s, --baselineskip
 *
 * Specify baselineskip value that is used when enscript moves to
 * a new line.  Current point movement is font_point_size + baselineskip.
 */
double baselineskip = 1.0;

/* 
 * -t, --title
 * 
 * Title which is printed to the banner page.  If this option is given
 * from the command line, this sets also the name of the stdin which
 * is by the default "".
 */
char *title = "enscript output";
int title_given = 0;

/*
 * -T, --tabsize
 *
 * Specify tabulator size.
 */
int tabsize = 8;

/* 
 * -u, --underlay
 *
 * Place text under every page.  Default is no underlay.
 */
double ul_gray = .8;
FontPoint ul_ptsize = {200.0, 200.0};
char *ul_font = "Times-Roman";
char *underlay = NULL;
char ul_position_buf[256];
char *ul_position = "+0-0";	/* Position info as a string. */
double ul_x;			/* Position x-coordinate. */
double ul_y;			/* Position y-coordinate. */
double ul_angle;
unsigned int ul_style = UL_STYLE_OUTLINE;
char *ul_style_str = "outline";
char ul_style_str_buf[256];
int ul_position_p = 0;		/* Is ul-position given? */
int ul_angle_p = 0;		/* Is ul-angle given? */

/*
 * -U NUM, --nup=NUM
 *
 * Print NUM PostScript pages on each output page (n-up printing).
 */
unsigned int nup = 1;
unsigned int nup_exp = 0;
unsigned int nup_rows = 1;
unsigned int nup_columns = 1;
int nup_landscape = 0;
unsigned int nup_width;
unsigned int nup_height;
double nup_scale;

/*
 * -v, --verbose
 *
 * Tell what we are doing.  Default is no verbose outputs.
 */
int verbose = 0;

/*
 * -X, --encoding
 *
 * Specifies input encoding.  Default is ISO-8859.1.
 */
InputEncoding encoding = ENC_LATIN1;
char *encoding_name = "latin1";
char encoding_name_buffer[256];

/*
 * -z, --no-formfeed
 *
 * Do not interpret form feed characters.  As a default, form feed
 * characters are interpreted.
 */
int interpret_formfeed = 1;

/* 
 * -Z, --pass-through
 * 
 * Pass through all PostScript and PCL files without any modifications.  
 * As a default, don't.
 */
int pass_through = 0;

/*
 * --filter-stdin=STR
 *
 * How stdin is shown to the filter command.  The default is "" but
 * some utilities might want it as "-".
 */
char *input_filter_stdin = "";

/*
 * --help-pretty-print
 *
 * Descript all supported -E, --pretty-print languages and file formats.
 */
int help_pretty_print = 0;

/*
 * --highlight-bar-gray=val
 *
 * Specify the gray level for highlight bars.
 */
double highlight_bar_gray = .97;

/* 
 * --list-media
 *
 * List all known media.  As a default do not list media names.
 */
int list_media = 0;

/*
 * --list-options
 *
 * Show all options and their values.  Exit successfully.
 */
int list_options = 0;

/*
 * --non-printable-format
 *
 * Format in which non-printable characters are printed.
 */
char *npf_name = "octal";
char npf_name_buf[256];
NonPrintableFormat non_printable_format = NPF_OCTAL;

/* 
 * --page-label-format
 *
 * Format in which page labels are printed, default is "short".
 */
char *page_label_format = "short";
char page_label_format_buf[256];
PageLabelFormat page_label;

/*
 * --printer-options=OPTIONS
 *
 * Pass extra options OPTIONS to the printer spooler.
 */
char *printer_options = NULL;

/*
 * --slice=NUM
 *
 * Horizontal input slicing.  Print only NUMth wrapped input pages.
 */
int slicing = 0;
unsigned int slice = 1;

/*
 * --toc
 *
 * Print Table of Contents page.
 */
int toc = 0;
char toc_fname[512];
FILE *toc_fp;
char *toc_fmt_string;

/*
 * AcceptCompositeCharacters: bool
 *
 * Specify whatever we accept composite characters or should them be 
 * considered as non-existent.  As a default, do not accept them.
 */
int accept_composites = 0;

/*
 * AppendCtrlD: bool
 *
 * Append ^D character to the end of the output.  Some printers require this
 * but the default is false.
 */
int append_ctrl_D = 0;

/*
 * Clean7Bit: bool
 *
 * Specify how characters greater than 127 are printed.
 */
int clean_7bit = 1;

/*
 * FormFeedType: type
 * 
 * Specify what to do when a formfeed character is encountered from the 
 * input stream.  The default action is to jump to the beginning of the
 * next column.
 */
FormFeedType formfeed_type = FORMFEED_COLUMN;

/*
 * NoJobHeaderSwitch: switch
 *
 * Spooler switch to suppress the job header (-h).
 */
char no_job_header_switch[16];

/*
 * OutputFirstLine: line
 *
 * Set the PostScript output's first line to something your system can handle.
 * The default is "%!PS-Adobe-3.0"
 */
char output_first_line[256];

/*
 * QueueParam: param
 *
 * The spooler command switch to select the printer queue (-P).
 */
char queue_param[16];

/*
 * Spooler: command
 *
 * The spooler command name (lpr).
 */
char spooler_command[256];

/*
 * StatesColorModel: model
 *
 * Color model for states.
 */
char states_color_model[50];

/*
 * StatesConfigFile: file
 *
 * Read states' configuration from file <file>.
 */
char states_config_file[256];

/*
 * StatesHighlightLevel: level
 *
 * HighlightLevel for states.
 */
char states_highlight_level[50];

/* ^@shade{GRAY}, set the line highlight gray. */
double line_highlight_gray = 1.0;

/*
 * Static variables.
 */

static struct option long_options[] =
{
  {"columns",		required_argument,	0, 0},
  {"pages",		required_argument,	0, 'a'},
  {"file-align",	required_argument,	0, 'A'},
  {"header",		required_argument,	0, 'b'},
  {"no-header",		no_argument,		0, 'B'},
  {"truncate-lines",	no_argument,		0, 'c'},
  {"line-numbers",	no_argument,		0, 'C'},
  {"printer",		required_argument,	0, 'd'}, 
  {"setpagedevice",	required_argument,	0, 'D'},
  {"escapes",		optional_argument,	0, 'e'},
  {"pretty-print",	optional_argument,	0, 'E'},
  {"font",		required_argument,	0, 'f'},
  {"header-font",	required_argument,	0, 'F'},
  {"print-anyway",	no_argument,		0, 'g'},
  {"fancy-header",	optional_argument,	0, 'G'},
  {"no-job-header",	no_argument, 		0, 'h'},
  {"highlight-bars",	optional_argument,	0, 'H'},
  {"indent",		required_argument,	0, 'i'},
  {"filter",		required_argument,	0, 'I'},
  {"borders",		no_argument,		0, 'j'},
  {"page-prefeed",	no_argument,		0, 'k'},
  {"no-page-prefeed",	no_argument,		0, 'K'},
  {"lineprinter",	no_argument,		0, 'l'},
  {"lines-per-page",	required_argument,	0, 'L'},
  {"mail",		no_argument,		0, 'm'},
  {"media",		required_argument,	0, 'M'},
  {"copies",		required_argument,	0, 'n'},
  {"newline",		required_argument,	0, 'N'},
  {"output",		required_argument,	0, 'p'},
  {"missing-characters",	no_argument,	0, 'O'},
  {"quiet",		no_argument,		0, 'q'},
  {"silent",		no_argument,		0, 'q'},
  {"landscape",		no_argument,		0, 'r'},
  {"portrait",		no_argument,		0, 'R'},
  {"baselineskip",	required_argument,	0, 's'},
  {"statusdict",	required_argument,	0, 'S'},
  {"title",		required_argument,	0, 't'},
  {"tabsize",		required_argument,	0, 'T'},
  {"underlay",		optional_argument,	0, 'u'},
  {"nup",		required_argument,	0, 'U'},
  {"verbose",		optional_argument,	0, 'v'},
  {"version",		no_argument,		0, 'V'},
  {"encoding",		required_argument,	0, 'X'},
  {"no-formfeed",	no_argument,		0, 'z'},
  {"pass-through",	no_argument,		0, 'Z'},

  /* Long options without short counterparts. */
  {"download-font",	required_argument,	0, 131},
  {"filter-stdin",	required_argument,	0, 138},
  {"help", 		no_argument, 		0, 135},
  {"help-pretty-print", no_argument, 		0, 141},
  {"highlight-bar-gray",	required_argument, 	0, 136},
  {"list-media",	no_argument,		&list_media, 1},
  {"list-options",	no_argument,		&list_options, 1},
  {"non-printable-format",	required_argument,	0, 134},
  {"page-label-format",	required_argument,	0, 130},
  {"printer-options",	required_argument,	0, 139},
  {"slice",		required_argument,	0, 140},
  {"toc",		no_argument,		&toc, 1},
  {"ul-angle",		required_argument,	0, 132},
  {"ul-font",		required_argument,	0, 128},
  {"ul-gray",		required_argument,	0, 129},
  {"ul-position",	required_argument,	0, 133},
  {"ul-style",		required_argument,	0, 137},

  {NULL, 0, 0, 0},
};


static struct
{
  char *names[3];
  InputEncoding encoding;
  int nl;
  int bs;
} encodings[] =
  {
    {{"latin1", "iso8859", "iso"},	ENC_LATIN1, 		'\n', 8},
    {{"latin2", "iso2", NULL},		ENC_LATIN2, 		'\n', 8},
    {{"latin3", "iso3", NULL},		ENC_LATIN3, 		'\n', 8},
    {{"latin5", "iso5", NULL},		ENC_LATIN5, 		'\n', 8},
    {{"ascii", NULL, NULL},		ENC_ASCII, 		'\n', 8},
    {{"asciifise", "asciifi", "asciise"},	ENC_ASCII_FISE,	'\n', 8},
    {{"asciidkno", "asciidk", "asciino"},	ENC_ASCII_DKNO,	'\n', 8},
    {{"ibmpc", "pc", "dos"},		ENC_IBMPC, 		'\n', 8},
    {{"mac", NULL, NULL},		ENC_MAC, 		'\r', 8},
    {{"vms", NULL, NULL},		ENC_VMS, 		'\n', 8},
    {{"hp8", NULL, NULL},		ENC_HP8,		'\n', 8},
    {{"koi8", NULL, NULL},		ENC_KOI8,		'\n', 8},
    {{"ps", "PS", NULL},		ENC_PS, 		'\n', 8},
    {{"pslatin1", "ISOLatin1Encoding", NULL},	ENC_LATIN1,	'\n', 8},

    {{NULL, NULL, NULL}, 0, 0, 0},
  };


/* 
 * Global functions.
 */

int 
main (int argc, char *argv[])
{
  InputStream is;
  time_t tim;
  struct tm *tm;
  int i, j, found;
  unsigned int ui;
  MediaEntry *mentry;
  AFMError afm_error;
  char *cp, *cp2;

  /* Get program's name. */
  program = strrchr (argv[0], '/');
  if (program == NULL)
    program = argv[0];
  else
    program++;

  /* Make getopt_long() to use our modified programname. */
  argv[0] = program;

  /* Create version strings. */
  sprintf (version_string, "GNU %s %s", PACKAGE, VERSION);
  strcpy (ps_version_string, VERSION);
  cp = strrchr (ps_version_string, '.');
  *cp = ' ';

  /* Create the default TOC format string.  Wow, this is cool! */
  toc_fmt_string = _("$3v $-40N $3% pages $4L lines  $E $C");

  /* Internationalization. */
#if HAVE_SETLOCALE 
  /*
   * We want to change only messages (gs do not like decimals in 0,1
   * format ;)
   */
#if HAVE_LC_MESSAGES
  setlocale (LC_MESSAGES, "");
#endif
#endif
#if ENABLE_NLS
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
#endif

  /* Create date string. */

  tim = time (NULL);
  tm = localtime (&tim);
  memcpy (&run_tm, tm, sizeof (*tm));

  sprintf (date_string, "%s", asctime (&run_tm));
  i = strlen (date_string);
  date_string[i - 1] = '\0';

  /* Get user's passwd entry. */
  passwd = getpwuid (getuid ());
  if (passwd == NULL)
    fatal (_("couldn't get passwd entry for uid=%d: %s"), getuid (), 
	   strerror (errno));

  /* Defaults for some options. */
  strcpy (spooler_command, "lpr");
  strcpy (queue_param, "-P");
  strcpy (no_job_header_switch, "-h");
  strcpy (fancy_header_default, "enscript");
  strcpy (output_first_line, "%!PS-Adobe-3.0");

  strcpy (states_color_model, "blackwhite");
  sprintf (states_config_file, "%s/enscript.st", LIBRARY);
  strcpy (states_highlight_level, "heavy");

  /* Check ENSCRIPT_LIBRARY for custom library location. */
  cp = getenv ("ENSCRIPT_LIBRARY");
  if (cp)
    enscript_library = cp;
    
  /* Fill up build-in libpath. */
  sprintf (libpath, "%s%c%s/.enscript", enscript_library, PATH_SEPARATOR,
	   passwd->pw_dir);

  /* Initialize resource sets. */
  res_fonts = strhash_init ();
  download_fonts = strhash_init ();
  pagedevice = strhash_init ();
  statusdict = strhash_init ();
  user_strings = strhash_init ();


  /* 
   * Read configuration files. 
   */

  /* Global config. */
#define CFG_FILE_NAME "enscript.cfg"
  if (!read_config (SYSCONFDIR, CFG_FILE_NAME))
    {
      int saved_errno = errno;

      /*
       * Try to read it from our library directory.  This is mostly the
       * case for the micro ports.
       */
      if (!read_config (enscript_library, CFG_FILE_NAME))
	{
	  /* Maybe we are not installed yet, let's try "../lib". */
	  if (!read_config ("../lib", CFG_FILE_NAME))
	    {
	      /* No luck, report error from the original config file. */
	      fatal (_("couldn't open config file \"%s/%s\": %s"),
		     enscript_library, CFG_FILE_NAME, strerror (saved_errno));
	    }

	  /*
	   * Ok, we are not installed yet.  Here is a small kludge to
	   * conform the GNU coding standards: we must be able to run
	   * without being installed, so we must append the "../lib"
	   * directory to the libpath.
	   */
	  strcat (libpath, ":../lib");
	}
    }

  /* Site config. */
  (void) read_config (SYSCONFDIR, "enscriptsite.cfg");

  /* Personal config. */
  (void) read_config (passwd->pw_dir, ".enscriptrc");

  /*
   * Options.
   */

  /* Environment variables. */
  handle_env_options ("ENSCRIPT");
  handle_env_options ("GENSCRIPT");

  /* Command line arguments. */
  handle_options (argc, argv);

  /* 
   * Check options which have some validity conditions.
   */

  /* Input encoding. */

  found = 0;
  for (i = 0; !found && encodings[i].names[0]; i++)
    for (j = 0; j < 3; j++)
      if (encodings[i].names[j] != NULL && MATCH (encodings[i].names[j],
						  encoding_name))
	{
	  /* Found a match for this encoding. */
	  encoding = encodings[i].encoding;
	  encoding_name = encodings[i].names[0];
	  if (nl < 0)
	    nl = encodings[i].nl;
	  bs = encodings[i].bs;
	  found = 1;
	  break;
	}
  if (!found)
    fatal (_("unknown encoding: %s"), encoding_name);

  /* Fonts. */

  /* Default font for landscape, 2 column printing is Courier 7. */
  if (!user_body_font_defined && landscape && num_columns > 1)
    Fpt.w = Fpt.h = 7.0;

  /* Cache for font AFM information. */
  afm_cache = strhash_init ();
  afm_info_cache = strhash_init ();

  /* Open AFM library. */
  afm_error = afm_create (afm_path, verbose, &afm);
  if (afm_error != AFM_SUCCESS)
    {
      char buf[256];

      afm_error_to_string (afm_error, buf);
      fatal (_("couldn't open AFM library: %s"), buf);
    }

  /*
   * Save default Fpt and Fname since special escape 'font' can change 
   * it and later we might want to switch back to the "default" font.
   */
  default_Fpt.w = Fpt.w;
  default_Fpt.h = Fpt.h;
  default_Fname = Fname;

  /* Register that document uses at least these fonts. */
  strhash_put (res_fonts, Fname, strlen (Fname) + 1, NULL, NULL);
  strhash_put (res_fonts, HFname, strlen (HFname) + 1, NULL, NULL);

  /* As a default, download both named fonts. */
  strhash_put (download_fonts, Fname, strlen (Fname) + 1, NULL, NULL);
  strhash_put (download_fonts, HFname, strlen (HFname) + 1, NULL, NULL);

  /* Read font's character widths and character types. */
  read_font_info ();

  /* Count the line indentation. */
  line_indent = parse_float (line_indent_spec, 1, 1);

  /* Escape page header string. */
  if (page_header != NULL)
    page_header = escape_string (page_header);

  /* List media names. */
  if (list_media)
    {
      printf (_("known media:\n\
name             width\theight\tllx\tlly\turx\tury\n\
------------------------------------------------------------\n"));
      for (mentry = media_names; mentry; mentry = mentry->next)
	printf ("%-16s %d\t%d\t%d\t%d\t%d\t%d\n",
		mentry->name, mentry->w, mentry->h,
		mentry->llx, mentry->lly, mentry->urx, mentry->ury);
      /* Exit after listing. */
      exit (0);
    }

  /* Output media. */
  for (mentry = media_names; mentry; mentry = mentry->next)
    if (strcmp (media_name, mentry->name) == 0)
      {
	media = mentry;
	break;
      }
  if (media == NULL)
    fatal (_("do not know anything about media \"%s\""), media_name);

  /* Page label format. */
  if (MATCH (page_label_format, "short"))
    page_label = LABEL_SHORT;
  else if (MATCH (page_label_format, "long"))
    page_label = LABEL_LONG;
  else
    fatal (_("illegal page label format \"%s\""), page_label_format);

  /* Non-printable format. */
  if (MATCH (npf_name, "space"))
    non_printable_format = NPF_SPACE;
  else if (MATCH (npf_name, "questionmark"))
    non_printable_format = NPF_QUESTIONMARK;
  else if (MATCH (npf_name, "caret"))
    non_printable_format = NPF_CARET;
  else if (MATCH (npf_name, "octal"))
    non_printable_format = NPF_OCTAL;
  else
    fatal (_("illegal non-printable format \"%s\""), npf_name);

  /* N-up. */
  for (i = 0; ; i++)
    {
      ui = nup >> i;

      if (ui == 0)
	fatal (_("illegal N-up argument: %d"), nup);

      if (ui & 0x1)
	{
	  if (ui != 1)
	    fatal (_("N-up argument must be power of 2: %d"), nup);
	  
	  nup_exp = i;
	  break;
	}
    }

  nup_rows = nup_exp / 2 * 2;
  if (nup_rows == 0)
    nup_rows = 1;
  nup_columns = (nup_exp + 1) / 2 * 2;
  if (nup_columns == 0)
    nup_columns = 1;

  nup_landscape = nup_exp & 0x1;

  if (nup_landscape)
    {
      nup_width = media->h;
      nup_height = media->w;
    }
  else
    {
      nup_width = media->w;
      nup_height = media->h;
    }
  nup_scale = (double) nup_width / nup_columns / media->w;
  nup_width /= nup_columns;
  nup_height /= nup_rows;

  /*
   * We print the data inside media's bouding box but the data has also
   * its own marginals => they get printed 1.n times so we must adjust
   * marginals a bit to get them right.
   */
  if (nup > 1)
    {
      double adjscale = 1.0 / (1 + nup_scale);

      media->llx *= adjscale;
      media->lly *= adjscale;
      media->urx = media->w - ((media->w - media->urx) * adjscale);
      media->ury = media->h - ((media->h - media->ury) * adjscale);
    }

  /* 
   * Count output media dimensions.
   */

  if (landscape)
    {
      d_page_w = media->ury - media->lly;
      d_page_h = media->urx - media->llx;
    }
  else
    {
      d_page_w = media->urx - media->llx;
      d_page_h = media->ury - media->lly;
    }

  /*
   * Underlay (this must come after output media dimensions, because
   * `underlay position' needs them).
   */
  if (underlay != NULL)
    {
      strhash_put (res_fonts, ul_font, strlen (ul_font) + 1, NULL, NULL);
      underlay = escape_string (underlay);
    }

  /* Underlay X-coordinate. */
  ul_x = strtod (ul_position, &cp);
  if (cp == ul_position)
    {
    malformed_position:
      fatal (_("malformed underlay position: %s"), ul_position);
    }
  if (ul_position[0] == '-')
    ul_x += d_page_w;

  /* Underlay Y-coordinate. */
  ul_y = strtod (cp, &cp2);
  if (cp2 == cp)
    goto malformed_position;
  if (cp[0] == '-')
    ul_y += d_page_h;

  /* Underlay Angle. */
  if (!ul_angle_p)
    /* No angle given, count the default. */
    ul_angle = (atan2 (-d_page_h, d_page_w) / 3.14159265 * 180);

  /* Underlay style. */
  if (strcmp (ul_style_str, "outline") == 0)
    ul_style = UL_STYLE_OUTLINE;
  else if (strcmp (ul_style_str, "filled") == 0)
    ul_style = UL_STYLE_FILLED;
  else
    fatal (_("illegal underlay style: %s"), ul_style_str);

  /*
   * Header.  Note! The header attributes can be changed from
   * the `.hdr' files, these are only the defaults.
   */

  d_header_w = d_page_w;
  switch (header)
    {
    case HDR_NONE:
      d_header_h = 0;
      break;

    case HDR_SIMPLE:
      d_header_h = HFpt.h * 1.5;
      break;

    case HDR_FANCY:
      d_header_h = 36;
      break;
    }

  /* Help pretty-print. */
  if (help_pretty_print)
    {
      /* Create description with states. */
      sprintf (pp_filter,
	       "states -f %s -s describe_languages %s",
	       states_config_file, states_config_file);
      system (pp_filter);
      exit (0);
    }

  /* Pretty-printing */
  if (pretty_print)
    {
      /* Create pretty-print input filter. */
      sprintf (pp_filter,
	       "states -f %s %s%s -Dcolormodel=%s -Dhl_level=%s -Dfont_spec=%s@%g/%g %%s",
	       states_config_file,
	       pp_start_state ? "-s " : "",
	       pp_start_state ? pp_start_state : "",
	       states_color_model,
	       states_highlight_level,
	       Fname, Fpt.w, Fpt.h);
      input_filter = pp_filter;
    }

  /* List options. */
  if (list_options)
    {
      do_list_options ();
      exit (1);
    }

  /* Table of Contents. */
  if (toc)
    {
      cp = tmpnam (toc_fname);
      if (cp == NULL)
	fatal (_("couldn't create toc file name: %s"), strerror (errno));
      
      toc_fp = fopen (toc_fname, "w+b");
      if (toc_fp == NULL)
	fatal (_("couldn't create toc file \"%s\": %s"), toc_fname,
	       strerror (errno));
      
      if (remove (toc_fname) == 0)
	/* Remove successfull, no need to remove file at exit. */
	toc_fname[0] = '\0';
    }


  /* 
   * Process files.
   */

  if (optind == argc)
    {
      /* stdin's modification time is the current time. */
      memcpy (&mod_tm, &run_tm, sizeof (run_tm));

      if (is_open (&is, stdin, NULL, input_filter))
	{
	  /* Open output file. */
	  open_output_file ();
	  process_file (title_given ? title : "", &is);
	  is_close (&is);
	}
    }
  else
    {
      for (; optind < argc; optind++)
	{
          char * pszUseFile;
          
#ifdef OS2
          /* do file globing here ... This is a sad side effect of UNIX Shells ... */
          char * pszInitGlob;
          pszInitGlob = argv[optind];

          /* ..................... os2ScanFiles(pszInitGlob) ........... */
          while ( (pszUseFile = WildcardExpandFilename(pszInitGlob)) != NULL )
            {
#else
              pszUseFile = argv[optind];
#endif
	      if (is_open (&is, NULL, pszUseFile, input_filter))
 	        {
	          struct stat stat_st;

	          /* Get modification time. */
	          if (stat (pszUseFile, &stat_st) == 0)
		    {
 		      tim = stat_st.st_mtime;
		      tm = localtime (&tim);
		      memcpy (&mod_tm, tm, sizeof (*tm));

		      /*
		       * Open output file.  Output file opening is delayed to
		       * this point so we can optimize the case when a 
		       * non-existing input file is printed => we do nothing.
		       */
		      open_output_file ();

		      process_file (pszUseFile, &is);
		    }
	          else
		    error (_("couldn't stat input file \"%s\": %s"), pszUseFile,
		           strerror (errno));

	          is_close (&is);
	        }
#ifdef OS2
              pszInitGlob = NULL;
            } /* End While Glob */
          if (pszInitGlob && !pszUseFile)
            {
              fprintf( stderr, "No file found/no such file: %s\n", pszInitGlob );
            }
#endif
	}
    }

  /* Table of Contents. */
  if (toc)
    {
      /* This is really cool... */

      /* Set the printing options for toc. */
      toc = 0;
      special_escapes = 1;

      if (fseek (toc_fp, 0, SEEK_SET) != 0)
	fatal (_("couldn't rewind toc file: %s"), strerror (errno));

      memcpy (&mod_tm, &run_tm, sizeof (run_tm));
      if (is_open (&is, toc_fp, NULL, NULL))
	{
	  process_file (_("Table of Contents"), &is);
	  is_close (&is);
	}

      /* Clean up toc file. */
      fclose (toc_fp);
      
      /* Do we have to remove the toc file? */
      if (toc_fname[0])
	(void) remove (toc_fname);
    }

  /* Give trailer a chance to dump itself. */
  dump_ps_trailer ();

  /*
   * Append ^D to the end of the output?  Note! It must be ^D followed
   * by a newline.
   */
  if (ofp != NULL && append_ctrl_D)
    fprintf (ofp, "\004\n");

  /* Close output file. */
  close_output_file ();

  /* Tell how things went. */
  if (ofp == NULL)
    {
      /*
       * The value of <ofp> is not reset in close_output_file(),
       * this is ugly but it saves one flag.
       */
      message (0, _("no output generated\n"));
    }
  else
    {
      unsigned int real_total_pages;

      if (nup > 1)
	{
	  if (total_pages > 0)
	    real_total_pages = (total_pages - 1) / nup + 1;
	  else
	    real_total_pages = 0;
	}
      else
	real_total_pages = total_pages;

      /* We did something, tell what.  */
      message (0, _("[ %d pages * %d copy ]"), real_total_pages, num_copies);
      if (output_file == OUTPUT_FILE_NONE)
	message (0, _(" sent to %s\n"), printer ? printer : _("printer"));
      else
	message (0, _(" left in %s\n"),
		 output_file == OUTPUT_FILE_STDOUT ? "-" : output_file);
      if (num_truncated_lines)
	message (0, _("%d lines were %s\n"), num_truncated_lines,
		 truncate_lines ? _("truncated") : _("wrapped"));

      if (num_missing_chars)
	{
	  message (0, _("%d characters were missing\n"), num_missing_chars);
	  if (list_missing_characters)
	    {
	      message (0, _("missing character codes (decimal):\n"));
	      do_list_missing_characters (missing_chars);
	    }
	}

      if (num_non_printable_chars)
	{
	  message (0, _("%d non-printable characters\n"),
		   num_non_printable_chars);
	  if (list_missing_characters)
	    {
	      message (0, _("non-printable character codes (decimal):\n"));
	      do_list_missing_characters (non_printable_chars);
	    }
	}
    }

  /* This is the end. */
  return 0;
}


/*
 * Static functions.
 */

static void
open_output_file ()
{
  if (ofp)
    /* Output file has already been opened, do nothing. */
    return;

  if (output_file == OUTPUT_FILE_NONE)
    {
      char spooler_options[512];

      /* Format spooler options. */
      spooler_options[0] = '\0';
      if (mail)
	strcat (spooler_options, "-m ");
      if (no_job_header)
	{
	  strcat (spooler_options, no_job_header_switch);
	  strcat (spooler_options, " ");
	}
      if (printer_options)
	strcat (spooler_options, printer_options);

      /* Open printer. */
      ofp = printer_open (spooler_command, spooler_options, queue_param,
			  printer, &printer_context);
      if (ofp == NULL)
	fatal (_("couldn't open printer `%s': %s"), printer, strerror (errno));
    }
  else if (output_file == OUTPUT_FILE_STDOUT)
    ofp = stdout;
  else
    {
      ofp = fopen (output_file, "w");
      if (ofp == NULL)
	fatal (_("couldn't create output file \"%s\": %s"), output_file,
	       strerror (errno));
    }
}


static void
close_output_file ()
{
  if (ofp == NULL)
    /* Output file hasn't been opened, we are done. */
    return;

  if (output_file == OUTPUT_FILE_NONE)
    printer_close (printer_context);
  else if (output_file != OUTPUT_FILE_STDOUT)
    fclose (ofp);

  /* We do not reset <ofp> since its value is needed in diagnostigs. */
}


static void
handle_env_options (char *var)
{
  int argc;
  char **argv;
  char *string;
  char *str;
  int i;

  string = getenv (var);
  if (string == NULL)
    return;

  message (2, "handle_env_options(): %s=\"%s\"\n", var, string);

  /* Copy string so we can modify it in place. */
  str = xstrdup (string);

  /*
   * We can count this, each option takes at least 1 character and one 
   * space.  We also need one for program's name and one for the 
   * trailing NULL. 
   */
  argc = (strlen (str) + 1) / 2 + 2;
  argv = xcalloc (argc, sizeof (char *));
  
  /* Set program name. */
  argc = 0;
  argv[argc++] = program;

  /* Split string and set arguments to argv array. */
  i = 0;
  while (str[i])
    {
      /* Skip leading whitespace. */
      for (; str[i] && isspace (str[i]); i++)
	;
      if (!str[i])
	break;

      /* Check for quoted arguments. */
      if (str[i] == '"' || str[i] == '\'')
	{
	  int endch = str[i++];

	  argv[argc++] = str + i;

	  /* Skip until we found the end of the quotation. */
	  for (; str[i] && str[i] != endch; i++)
	    ;
	  if (!str[i])
	    fatal (_("syntax error in option string %s=\"%s\":\n\
missing end of quotation: %c"), var, string, endch);

	  str[i++] = '\0';
	}
      else
	{
	  argv[argc++] = str + i;

	  /* Skip until whitespace if found. */
	  for (; str[i] && !isspace (str[i]); i++)
	    ;
	  if (str[i])
	    str[i++] = '\0';
	}
    }
  
  /* argv[argc] must be NULL. */
  argv[argc] = NULL;

  message (2, "found following options (argc=%d):\n", argc);
  for (i = 0; i < argc; i++)
    message (2, "%3d = \"%s\"\n", i, argv[i]);

  /* Process options. */
  handle_options (argc, argv);

  /* Check that all got processed. */
  if (optind != argc)
    {
      message (0,
	       _("warning: didn't process following options from \
environment variable %s:\n"),
	       var);
      for (; optind < argc; optind++)
	message (0, _("  option %d = \"%s\"\n"), optind, argv[optind]);
    }

  /* Cleanup. */
  xfree (argv);

  /*
   * <str> must not be freed, since some global variables can point to
   * its elements
   */
}


static void
handle_options (int argc, char *argv[])
{
  int c;
  PageRange *prange;

  /* Reset optind. */
  optind = 0;

  while (1)
    {
      int option_index = 0;
      const char *cp;

      c = getopt_long (argc, argv,
		       "12a:A:b:BcCd:D:e::E::f:F:gGhH::i:I:jkKlL:mM:n:N:o:Op:P:qrRs:S:t:T:u::U:vVX:zZ",
		       long_options, &option_index);
      
      if (c == EOF)
	break;

      switch (c)
	{
	case 0:			/* Long option found. */
	  cp = long_options[option_index].name;

	  if (strcmp (cp, "columns") == 0)
	    num_columns = atoi (optarg);
	  break;

	  /* Short options. */

	case '1':		/* one column */
	case '2':		/* two columns */
	  num_columns = c - '0';
	  break;

	case 'a':		/* pages */
	  prange = (PageRange *) xcalloc (1, sizeof (PageRange));

	  if (strcmp (optarg, "odd") == 0)
	    prange->odd = 1;
	  else if (strcmp (optarg, "even") == 0)
	    prange->even = 1;
	  else
	    {
	      cp = strchr (optarg, '-');
	      if (cp)
		{
		  if (optarg[0] == '-')
		    /* -end */
		    prange->end = atoi (optarg + 1);
		  else if (cp[1] == '\0')
		    {
		      /* start- */
		      prange->start = atoi (optarg);
		      prange->end = (unsigned int) -1;
		    }
		  else
		    {
		      /* start-end */
		      prange->start = atoi (optarg);
		      prange->end = atoi (cp + 1);
		    }
		}
	      else
		/* pagenumber */
		prange->start = prange->end = atoi (optarg);
	    }

	  prange->next = page_ranges;
	  page_ranges = prange;
	  break;

	case 'A':		/* file alignment */
	  file_align = atoi (optarg);
	  if (file_align == 0)
	    fatal (_("file alignment must be larger than zero"));
	  break;

	case 'b':		/* page header */
	  page_header = optarg;
	  break;

	case 'B':		/* no page headers */
	  header = HDR_NONE;
	  break;

	case 'c':		/* truncate (cut) long lines */
	  truncate_lines = 1;
	  break;

	case 'C':		/* line numbers */
	  line_numbers = 1;
	  break;

	case 'd':		/* specify printer */
	case 'P':
	  printer = optarg;
	  output_file = OUTPUT_FILE_NONE;
	  break;

	case 'D':		/* setpagedevice */
	  parse_key_value_pair (pagedevice, optarg);
	  break;

	case 'e':		/* special escapes */
	  special_escapes = 1;
	  if (optarg)
	    /* Specify the escape character. */
	    escape_char = atoi (optarg);
	  break;

	case 'E':		/* pretty-print */
	  pretty_print = 1;
	  special_escapes = 1;
	  escape_char = '\0';
	  pp_start_state = optarg;
	  break;

	case 'f':		/* font */
	  if (!parse_font_spec (optarg, &Fname, &Fpt))
	    fatal (_("malformed font spec: %s"), optarg);
	  user_body_font_defined = 1;
	  break;

	case 'F':		/* header font */
	  if (!parse_font_spec (optarg, &HFname, &HFpt))
	    fatal (_("malformed font spec: %s"), optarg);
	  break;

	case 'g':		/* print anyway */
	  /* nothing. */
	  break;

	case 'G':		/* fancy header */
	  header = HDR_FANCY;
	  if (optarg)
	    fancy_header_name = optarg;
	  else
	    fancy_header_name = fancy_header_default;

	  if (!file_existsp (fancy_header_name, ".hdr"))
	    fatal (_("couldn't find header definition file \"%s.hdr\""),
		   fancy_header_name);
	  break;

	case 'h':		/* no job header */
	  no_job_header = 1;
	  break;

	case 'H':		/* highlight bars */
	  if (optarg)
	    highlight_bars = atoi (optarg);
	  else
	    highlight_bars = 2;
	  break;
	  
	case 'i':		/* line indent */
	  line_indent_spec = optarg;
	  break;

	case 'I':		/* input filter */
	  input_filter = optarg;
	  break;

	case 'j':		/* borders */
	  borders = 1;
	  break;

	case 'k':		/* enable page prefeed */
	  page_prefeed = 1;
	  break;

	case 'K':		/* disable page prefeed */
	  page_prefeed = 0;
	  break;

	case 'l':		/* simulate lineprinter */
	  lines_per_page = 66;
	  header = HDR_NONE;
	  landscape = 0;
	  num_columns = 1;
	  break;

	case 'L':		/* lines per page */
	  lines_per_page = atoi (optarg);
	  if (lines_per_page <= 0)
	    fatal (_("must print at least one line per each page: %s"),
		   argv[optind]);
	  break;

	case 'm':		/* send mail upon completion */
	  mail = 1;
	  break;

	case 'M':		/* select output media */
	  media_name = optarg;
	  break;

	case 'n':		/* num copies */
	  num_copies = atoi (optarg);
	  break;

	case 'N':		/* newline character */
	  if (!(optarg[0] == 'n' || optarg[0] == 'r') || optarg[1] != '\0')
	    {
	      fprintf (stderr, _("%s: illegal newline character specifier: \
'%s': expected 'n' or 'r'\n"),
		       program, optarg);
	      goto option_error;
	    }
	  if (optarg[0] == 'n')
	    nl = '\n';
	  else
	    nl = '\r';
	  break;

	case 'o':
	case 'p':		/* output file */
	  /* Check output file "-". */
	  if (strcmp (optarg, "-") == 0)
	    output_file = OUTPUT_FILE_STDOUT;
	  else
	    output_file = optarg;
	  break;

	case 'O':		/* list missing characters */
	  list_missing_characters = 1;
	  break;

	case 'q':		/* quiet */
	  quiet = 1;
	  verbose = 0;
	  break;

	case 'r':		/* landscape */
	  landscape = 1;
	  break;

	case 'R':		/* portrait */
	  landscape = 0;
	  break;

	case 's':		/* baselineskip */
	  baselineskip = atof (optarg);
	  break;

	case 'S':		/* statusdict */
	  parse_key_value_pair (statusdict, optarg);
	  break;

	case 't':
	  title = optarg;
	  title_given = 1;
	  break;

	case 'T':		/* tabulator size */
	  tabsize = atoi (optarg);
	  if (tabsize <= 0)
	    tabsize = 1;
	  break;

	case 'u':		/* underlay */
	  underlay = optarg;
	  break;

	case 'U':		/* nup */
	  nup = atoi (optarg);
	  break;

	case 'v':		/* verbose */
	  if (optarg)
	    verbose = atoi (optarg);
	  else
	    verbose++;
	  quiet = 0;
	  break;

	case 'V':		/* version */
	  version ();
	  exit (0);
	  break;

	case 'X':		/* input encoding */
	  encoding_name = optarg;
	  break;

	case 'z':		/* no form feeds */
	  interpret_formfeed = 0;
	  break;

	case 'Z':		/* pass through */
	  pass_through = 1;
	  break;

	case 128:		/* underlay font */
	  if (!parse_font_spec (optarg, &ul_font, &ul_ptsize))
	    fatal (_("malformed font spec: %s"), optarg);
	  break;

	case 129:		/* underlay gray */
	  ul_gray = atof (optarg);
	  break;

	case 130:		/* page label format */
	  page_label_format = optarg;
	  break;

	case 131:		/* download font */
	  strhash_put (download_fonts, optarg, strlen (optarg) + 1, NULL,
		       NULL);
	  break;

	case 132:		/* underlay angle */
	  ul_angle = atof (optarg);
	  ul_angle_p = 1;
	  break;

	case 133:		/* underlay position */
	  ul_position = optarg;
	  ul_position_p = 1;
	  break;

	case 134:		/* non-printable format */
	  npf_name = optarg;
	  break;

	case 135:		/* help */
	  usage ();
	  exit (0);
	  break;

	case 136:		/* highlight bar gray */
	  highlight_bar_gray = atof (optarg);
	  break;

	case 137:		/* underlay style */
	  ul_style_str = optarg;
	  break;

	case 138:		/* filter stdin */
	  input_filter_stdin = optarg;
	  break;

	case 139:		/* extra options for the printer spooler */
	  printer_options = optarg;
	  break;

	case 140:		/* slicing */
	  slicing = 1;
	  slice = atoi (optarg);
	  if (slice <= 0)
	    fatal (_("slice must be greater than zero"));
	  break;

	case 141:		/* help-pretty-print */
	  help_pretty_print = 1;
	  break;

	case '?':		/* Errors found during getopt_long(). */
	option_error:
	  fprintf (stderr, _("Try `%s --help' for more information.\n"),
		   program);
	  exit (1);
	  break;
	  
	default:
	  printf ("Hey!  main() didn't handle option \"%c\" (%d)", c, c);
	  if (optarg)
	    printf (" with arg %s", optarg);
	  printf ("\n");
	  fatal ("This is a bug!");
	  break;
	}
    }
}


#define TF(val) ((val) ? 't' : 'f')

static void
do_list_options ()
{
  int i, j;
  char *cp, *cp2;

  printf ("libpath=\"%s\"\n", 	libpath);
  printf ("printer=\"%s\"\n", 	printer ? printer : "");
  printf ("queue_param=\"%s\"\n", 	queue_param);
  printf ("verbose=%d\n", 		verbose);
  printf ("num_copies=%d\n", 	num_copies);
  printf ("title=\"%s\"\n", 	title ? title : "");
  printf ("columns=%d\n", 		num_columns);
  printf ("truncate=#%c\n", 	TF (truncate_lines));
  printf ("line_numbers=#%c\n", 	TF (line_numbers));
  printf ("mail=#%c\n", 		TF (mail));
  printf ("quiet=#%c\n", 		TF (quiet));
  printf ("landscape=#%c\n", 	TF (landscape));
  
  printf ("header=");
  switch (header)
    {
    case HDR_NONE:
      printf ("none");
      break;

    case HDR_SIMPLE:
      printf ("simple");
      break;

    case HDR_FANCY:
      printf ("fancy (%s)", fancy_header_name);
      break;
    }
  printf ("\n");
  
  printf ("page_header=\"%s\"\n", page_header ? page_header : "");
  printf ("font: name=%s size=%g/%gpt\n", Fname, Fpt.w, Fpt.h);
  printf ("header font: name=%s size=%g/%gpt\n", HFname, HFpt.w, HFpt.h);
  printf ("output_file=%s\n",
	  (output_file == OUTPUT_FILE_NONE
	   ? "none"
	   : (output_file == OUTPUT_FILE_STDOUT
	      ? "stdout"
	      : output_file)));
  printf ("media=%s (w=%d, h=%d, llx=%d, lly=%d, urx=%d, ury=%d)\n",
	  media->name, media->w, media->h, media->llx, media->lly,
	  media->urx,media->ury);
  
  printf ("encoding=%s\n", 		encoding_name);
  printf ("interpret_formfeed=#%c\n",	TF (interpret_formfeed));
  printf ("pass_through=#%c\n",		TF (pass_through));
  printf ("spooler_command=\"%s\"\n", 	spooler_command);
  printf ("special_escapes=#%c\n", 	TF (special_escapes));
  printf ("tabsize=%d\n", 		tabsize);
  printf ("baselineskip=%g\n", 	baselineskip);
  
  /* statusdict key-value pairs */
  printf ("statusdict: ");
  for (i = strhash_get_first (statusdict, &cp, &j, (void **) &cp2); i;
       i = strhash_get_next (statusdict, &cp, &j, (void **) &cp2))
    printf ("%s %s ", cp2, cp);
  printf ("\n");
  
  /* setpagedevice key-value pairs */
  printf ("setpagedevice: << ");
  for (i = strhash_get_first (pagedevice, &cp, &j, (void **) &cp2); i;
       i = strhash_get_next (pagedevice, &cp, &j, (void **) &cp2))
    printf ("/%s %s ", cp, cp2);
  printf (">>\n");
  
  printf ("nl=%c\n", 		nl == '\n' ? 'n' : 'r');
  printf ("AFM path=%s\n", 		afm_path ? afm_path : "(default)");
  
  /* Underlay. */
  printf ("underlay=(%s)\n", 	underlay ? underlay : "");
  printf ("ul_gray=%g\n", 		ul_gray);
  printf ("ul_font=%s %g/%gpt\n", 	ul_font, ul_ptsize.w, ul_ptsize.h);
  printf ("ul_position=%s (%g, %g)\n",	ul_position, ul_x, ul_y);
  printf ("ul_angle=%g\n",		ul_angle);
  
  /* Download fonts. */
  printf ("download-fonts:");
  for (i = strhash_get_first (download_fonts, &cp, &j, (void **) &cp2); i;
       i = strhash_get_next (download_fonts, &cp, &j, (void **) &cp2))
    printf (" %s", cp);
  printf ("\n");
}


static void
usage ()
{
  printf (_("\n\
Usage: %s [OPTION]... [FILE]...\n\
Mandatory arguments to long options are mandatory for short options too.\n\
  -1                         same as --columns=1\n\
  -2                         same as --columns=2\n\
      --columns=NUM          specify the number of columns per page\n\
  -a, --pages=PAGES          specify which pages are printed\n\
  -A, --file-align=ALIGN     align separate input files to ALIGN\n\
  -b, --header=HEADER        set page header\n\
  -B, --no-header            no page headers\n\
  -c, --truncate-lines       cut long lines (default is to wrap)\n\
  -C, --line-numbers         precede each line with its line number\n\
  -d                         an alias for option --printer\n\
  -D, --setpagedevice=KEY[:VALUE]\n\
                             pass a page device definition to output\n\
  -e, --escapes[=CHAR]       enable special escape interpretation\n"),
          program);

  printf (_("\
  -E, --pretty-print[=LANG]  pretty-print source code\n"));

  printf (_("\
  -f, --font=NAME            use font NAME for body text\n\
  -F, --header-font=NAME     use font NAME for header texts\n\
  -g, --print-anyway         nothing (compatibility option)\n\
  -G                         same as --fancy-header\n\
      --fancy-header[=NAME]  select fancy page header\n\
  -h, --no-job-header        suppress the job header page\n\
  -H, --highlight-bars=NUM   specify how high highlight bars are\n\
  -i, --indent=NUM           set line indent to NUM characters\n\
  -I, --filter=CMD           read input files through input filter CMD\n\
  -j, --borders              print borders around columns\n\
  -k, --page-prefeed         enable page prefeed\n\
  -K, --no-page-prefeed      disable page prefeed\n\
  -l, --lineprinter          simulate lineprinter, this is an alias for:\n\
                               --lines-per-page=66, --no-header, --portrait,\n\
                               --columns=1\n"));

  printf (_("\
  -L, --lines-per-page=NUM   specify how many lines are printed on each page\n\
  -m, --mail                 send mail upon completion\n\
  -M, --media=NAME           use output media NAME\n\
  -n, --copies=NUM           print NUM copies of each page\n\
  -N, --newline=NL           select the newline character.  Possible\n\
                             values for NL are: n (`\\n') and r (`\\r').\n\
  -o                         an alias for option --output\n\
  -O, --missing-characters   list missing characters\n\
  -p, --output=FILE          leave output to file FILE.  If FILE is `-',\n\
                             leave output to stdout.\n\
  -P, --printer=NAME         print output to printer NAME\n\
  -q, --quiet, --silent      be really quiet\n\
  -r, --landscape            print in landscape mode\n\
  -R, --portrait             print in portrait mode\n"));

  printf (_("\
  -s, --baselineskip=NUM     set baselineskip to NUM\n\
  -S, --statusdict=KEY[:VALUE]\n\
                             pass a statusdict definition to the output\n\
  -t, --title=TITLE          set banner page's job title to TITLE.  Option\n\
                             sets also the name of the input file stdin.\n\
  -T, --tabsize=NUM          set tabulator size to NUM\n\
  -u, --underlay[=TEXT]      print TEXT under every page\n\
  -U, --nup=NUM              print NUM logical pages on each output page\n\
  -v, --verbose              tell what we are doing\n\
  -V, --version              print version number\n\
  -X, --encoding=NAME        use input encoding NAME\n\
  -z, --no-formfeed          do not interpret form feed characters\n\
  -Z, --pass-through         pass through PostScript and PCL files\n\
                             without any modifications\n"));

  printf (_("Long-only options:\n\
  --download-font=NAME       download font NAME\n\
  --filter-stdin=NAME        specify how stdin is shown to the input filter\n\
  --help                     print this help and exit\n\
  --help-pretty-print        describe all supported --pretty-print languages\n\
                             and file formats\n\
  --highlight-bar-gray=NUM   print highlight bars with gray NUM (0 - 1)\n\
  --list-media               list names of all known media\n\
  --list-options             list all options and their values\n\
  --non-printable-format=FMT specify how non-printable chars are printed\n\
  --page-label-format=FMT    set page label format to FMT\n\
  --printer-options=OPTIONS  pass extra options to the printer command\n"));
  
  printf (_("\
  --slice=NUM                print vertical slice NUM\n\
  --toc                      print table of contents\n\
  --ul-angle=ANGLE           set underlay text's angle to ANGLE\n\
  --ul-font=NAME             print underlays with font NAME\n\
  --ul-gray=NUM              print underlays with gray value NUM\n\
  --ul-position=POS          set underlay's starting position to POS\n\
  --ul-style=STYLE           print underlays with style STYLE\n\
"));
}


static void
version ()
{
  printf ("\n%s\n", version_string);
}



#ifdef OS2
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/* Boiler Plate for a 'main' routine.
*/
char * WildcardExpandFilename ( char * lpszWildcard_in )
{
  static char   lpszWildcard[CCHMAXPATHCOMP];
  static char   lpszPath[CCHMAXPATHCOMP];
  static char   lpszFileBuffer[ 1024 ];
  static HDIR   hdirFindHandle = HDIR_SYSTEM;

  FILEFINDBUF3  FindBuffer     = {0};      /* Returned from FindFirst/Next */
  ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
  ULONG         ulFindCount    = 1;        /* Look for 1 file at a time    */
  APIRET        ApiReturn      = NO_ERROR; /* Return code                  */

  char * pReturn = NULL;

  if ( lpszWildcard_in )
    {
      strcpy ( lpszWildcard, lpszWildcard_in );
      save_path_location ( lpszWildcard, lpszPath );

      ApiReturn = DosFindFirst( lpszWildcard,
                                 &hdirFindHandle,
                                  FILE_NORMAL,
                                   &FindBuffer,
                                    ulResultBufLen,
                                     &ulFindCount,
                                      FIL_STANDARD);
      if (ApiReturn == NO_ERROR)
        {
          pReturn = lpszFileBuffer;
          sprintf( pReturn, "%s%s", lpszPath, FindBuffer.achName );
        }
      else
        {
          *lpszWildcard = '\0';
          hdirFindHandle = HDIR_SYSTEM;
        }

    }
  else if ( *lpszWildcard )
    {
      ulFindCount = 1;                      /* Reset find count.            */

      ApiReturn = DosFindNext(hdirFindHandle,      /* Directory handle             */
                               &FindBuffer,         /* Result buffer                */
                                ulResultBufLen,      /* Result buffer length         */
                                 &ulFindCount);       /* Number of entries to find    */

      if ( ApiReturn == NO_ERROR )
        {
          pReturn = lpszFileBuffer;
          sprintf( pReturn, "%s%s", lpszPath, FindBuffer.achName );
        }
      else
        {
          *lpszWildcard = '\0';

          if ( ApiReturn != ERROR_NO_MORE_FILES )
            {
              printf("DosFindNext error: return code = %u\n",ApiReturn);
            }

          /* Close our directory handle
          */
          ApiReturn = DosFindClose(hdirFindHandle);
          if (ApiReturn != NO_ERROR)
            {
              fprintf( stderr, "   *** DosFindClose error: return code = %u\n", ApiReturn);
            }

          hdirFindHandle = HDIR_SYSTEM;
        }
    }
  else
    {
      fprintf( stderr, "OS2 File Searching error/No filse found [Improper call.].\n" );
    }
  return pReturn;
}

void save_path_location( char * lpszArg, char *lpszDest )
{
  char drive        [ _MAX_DRIVE ];
  char path         [  _MAX_DIR  ];
  char fname        [ _MAX_FNAME ];
  char ext          [  _MAX_EXT  ];


  _splitpath( lpszArg, drive, path, fname, ext);

  if ( *drive )
    {
      if ( *path )
        {
          sprintf( lpszDest, "%s%s", drive, path );
        }
      else
        {
          sprintf( lpszDest, "%s", drive );
        }
    }
  else if ( *path )
    {
      sprintf( lpszDest, "%s", path );
    }
  else
    {
      strcpy( lpszDest, "" );
    }

/*fprintf( stderr, "'%s': -> [%s %s %s %s] (%s)\n", lpszArg, drive, path, fname, ext, lpszDest );
*/

}

#endif