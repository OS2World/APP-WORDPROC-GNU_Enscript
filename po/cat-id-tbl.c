/* Automatically generated by po2tbl.sed from enscript.pot.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "libgettext.h"

const struct _msg_ent _msg_tbl[] = {
  {"", 1},
  {"%s: option `%s' is ambiguous\n", 2},
  {"%s: option `--%s' doesn't allow an argument\n", 3},
  {"%s: option `%c%s' doesn't allow an argument\n", 4},
  {"%s: option `%s' requires an argument\n", 5},
  {"%s: unrecognized option `--%s'\n", 6},
  {"%s: unrecognized option `%c%s'\n", 7},
  {"%s: illegal option -- %c\n", 8},
  {"%s: invalid option -- %c\n", 9},
  {"%s: option requires an argument -- %c\n", 10},
  {"xmalloc(): couldn't allocate %d bytes\n", 11},
  {"xcalloc(): couldn't allocate %d bytes\n", 12},
  {"xrealloc(): couldn't reallocate %d bytes\n", 13},
  {"Success", 14},
  {"No match", 15},
  {"Invalid regular expression", 16},
  {"Invalid collation character", 17},
  {"Invalid character class name", 18},
  {"Trailing backslash", 19},
  {"Invalid back reference", 20},
  {"Unmatched [ or [^", 21},
  {"Unmatched ( or \\(", 22},
  {"Unmatched \\{", 23},
  {"Invalid content of \\{\\}", 24},
  {"Invalid range end", 25},
  {"Memory exhausted", 26},
  {"Invalid preceding regular expression", 27},
  {"Premature end of regular expression", 28},
  {"Regular expression too big", 29},
  {"Unmatched ) or \\)", 30},
  {"No previous regular expression", 31},
  {"$3v $-40N $3% pages $4L lines  $E $C", 32},
  {"couldn't get passwd entry for uid=%d: %s", 33},
  {"couldn't open config file \"%s/%s\": %s", 34},
  {"unknown encoding: %s", 35},
  {"couldn't open AFM library: %s", 36},
  {"\
known media:\n\
name             width\theight\tllx\tlly\turx\tury\n\
------------------------------------------------------------\n", 37},
  {"do not know anything about media \"%s\"", 38},
  {"illegal page label format \"%s\"", 39},
  {"illegal non-printable format \"%s\"", 40},
  {"illegal N-up argument: %d", 41},
  {"N-up argument must be power of 2: %d", 42},
  {"malformed underlay position: %s", 43},
  {"illegal underlay style: %s", 44},
  {"couldn't create toc file name: %s", 45},
  {"couldn't create toc file \"%s\": %s", 46},
  {"couldn't stat input file \"%s\": %s", 47},
  {"couldn't rewind toc file: %s", 48},
  {"Table of Contents", 49},
  {"no output generated\n", 50},
  {"[ %d pages * %d copy ]", 51},
  {" sent to %s\n", 52},
  {"printer", 53},
  {" left in %s\n", 54},
  {"%d lines were %s\n", 55},
  {"truncated", 56},
  {"wrapped", 57},
  {"%d characters were missing\n", 58},
  {"missing character codes (decimal):\n", 59},
  {"%d non-printable characters\n", 60},
  {"non-printable character codes (decimal):\n", 61},
  {"couldn't open printer `%s': %s", 62},
  {"couldn't create output file \"%s\": %s", 63},
  {"\
syntax error in option string %s=\"%s\":\n\
missing end of quotation: %c", 64},
  {"\
warning: didn't process following options from environment variable %s:\n", 65},
  {"  option %d = \"%s\"\n", 66},
  {"file alignment must be larger than zero", 67},
  {"malformed font spec: %s", 68},
  {"couldn't find header definition file \"%s.hdr\"", 69},
  {"must print at least one line per each page: %s", 70},
  {"%s: illegal newline character specifier: '%s': expected 'n' or 'r'\n", 71},
  {"slice must be greater than zero", 72},
  {"Try `%s --help' for more information.\n", 73},
  {"\
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
  -e, --escapes[=CHAR]       enable special escape interpretation\n", 74},
  {"  -E, --pretty-print[=LANG]  pretty-print source code\n", 75},
  {"\
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
                               --lines-per-page=66, --no-header, \
--portrait,\n\
                               --columns=1\n", 76},
  {"\
  -L, --lines-per-page=NUM   specify how many lines are printed on each \
page\n\
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
  -R, --portrait             print in portrait mode\n", 77},
  {"\
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
                             without any modifications\n", 78},
  {"\
Long-only options:\n\
  --download-font=NAME       download font NAME\n\
  --filter-stdin=NAME        specify how stdin is shown to the input filter\n\
  --help                     print this help and exit\n\
  --help-pretty-print        describe all supported --pretty-print \
languages\n\
                             and file formats\n\
  --highlight-bar-gray=NUM   print highlight bars with gray NUM (0 - 1)\n\
  --list-media               list names of all known media\n\
  --list-options             list all options and their values\n\
  --non-printable-format=FMT specify how non-printable chars are printed\n\
  --page-label-format=FMT    set page label format to FMT\n\
  --printer-options=OPTIONS  pass extra options to the printer command\n", 79},
  {"\
  --slice=NUM                print vertical slice NUM\n\
  --toc                      print table of contents\n\
  --ul-angle=ANGLE           set underlay text's angle to ANGLE\n\
  --ul-font=NAME             print underlays with font NAME\n\
  --ul-gray=NUM              print underlays with gray value NUM\n\
  --ul-position=POS          set underlay's starting position to POS\n\
  --ul-style=STYLE           print underlays with style STYLE\n", 80},
  {"couldn't find prolog \"%s\": %s\n", 81},
  {"couldn't find encoding file \"%s.enc\": %s\n", 82},
  {"couldn't find header definition file \"%s.hdr\": %s\n", 83},
  {"processing file \"%s\"...\n", 84},
  {"EPS file \"%s\" is too large for page\n", 85},
  {"unknown special escape: %s", 86},
  {"illegal option %c for ^@epsf escape", 87},
  {"malformed ^@epsf escape: no ']' after options", 88},
  {"\
too long file name for ^@epsf escape:\n\
%.*s", 89},
  {"unexpected EOF while scanning ^@epsf escape", 90},
  {"malformed ^@epsf escape: no '{' found", 91},
  {"malformed %s escape: no '{' found", 92},
  {"\
too long argument for %s escape:\n\
%.*s", 93},
  {"malformed font spec for ^@font escape: %s", 94},
  {"malformed color spec for ^@color escape: %s", 95},
  {"invalid value for ^@shade escape: %s", 96},
  {"epsf: couldn't open pipe to command \"%s\": %s\n", 97},
  {"couldn't open EPS file \"%s\": %s\n", 98},
  {"EPS file \"%s\" does not start with \"%%!\" magic\n", 99},
  {"\
EPS file \"%s\" contains malformed %%%%BoundingBox row:\n\
\"%.*s\"\n", 100},
  {"EPS file \"%s\" is not a valid EPS file\n", 101},
  {"passing through %s file \"%s\"\n", 102},
  {"couldn't create divert file name: %s", 103},
  {"couldn't create divert file \"%s\": %s", 104},
  {"couldn't rewind divert file: %s", 105},
  {"illegal value \"%s\" for option %s", 106},
  {"invalid value \"%s\" for option %s", 107},
  {"illegal option: %s", 108},
  {"%s:%d: %%Format: no name", 109},
  {"%s:%d: %%Format: too long name, maxlen=%d", 110},
  {"%s:%d: %%Format: name \"%s\" is already defined", 111},
  {"%s:%d: %%HeaderHeight: no argument", 112},
  {"%s:%d: %%FooterHeight: no argument", 113},
  {"%s: warning: font size is negative\n", 114},
  {"%s: warning: font width is negative\n", 115},
  {"%s: warning: font height is negative\n", 116},
  {"reading AFM info for font \"%s\"\n", 117},
  {"couldn't open AFM file for font \"%s\", using default\n", 118},
  {"couldn't open AFM file for the default font: %s", 119},
  {"downloading font \"%s\"\n", 120},
  {"couldn't open font description file \"%s\": %s\n", 121},
  {"%s: too long format for %%D{} escape", 122},
  {"%s: unknown `%%' escape `%c' (%d)", 123},
  {"%s: no closing ')' for $() escape", 124},
  {"%s: too long variable name for $() escape", 125},
  {"%s: too long format for $D{} escape", 126},
  {"%s: unknown `$' escape `%c' (%d)", 127},
  {"malformed float dimension: \"%s\"", 128},
  {"couldn't open input filter \"%s\" for file \"%s\": %s", 129},
  {"couldn't open input file \"%s\": %s", 130},
  {"file=%s\n", 131},
  {"stdout", 132},
  {"%s: couldn't open output file \"%s\"", 133},
  {"couldn't create AFM library", 134},
  {"\
Usage: %s [OPTION]... FILE...\n\
Mandatory arguments to long options are mandatory for short options too.\n\
  -h, --help              print this help and exit\n\
  -p, --output-file=NAME  print output to file NAME (default file is\n\
                          font.map).  If FILE is `-', leavy output to\n\
                          stdout.\n\
  -V, --version           print version number\n", 135},
  {"states for GNU %s %s", 136},
  {"%s: malformed variable definition \"%s\"\n", 137},
  {"%s: couldn't create output file \"%s\": %s\n", 138},
  {"%s: unknown warning level `%s'\n", 139},
  {"%s: couldn't open definition file `%s': %s\n", 140},
  {"%s: out of memory\n", 141},
  {"%s: couldn't open input file `%s': %s\n", 142},
  {"\
Usage: %s [OPTION]... [FILE]...\n\
Mandatory arguments to long options are mandatory for short options too.\n", 143},
  {"\
  -D, --define=VAR=VAL       define variable VAR to have value VAR\n\
  -f, --file=NAME            read state definitions from file NAME\n\
  -h, --help                 print this help and exit\n\
  -o, --output=NAME          save output file file NAME\n\
  -s, --state=NAME           start from state NAME\n\
  -V, --version              print version number\n\
  -W, --warning=LEVEL        set the warning level to LEVEL\n", 144},
  {"%s: primitive `%s': illegal argument type\n", 145},
  {"%s: panic: ", 146},
  {"%s: primitive `%s': malformed version string `%s'\n", 147},
  {"\
%s: FATAL ERROR: States version %s or higher is required for this script\n", 148},
  {"%s: primitive `%s': too few arguments for format\n", 149},
  {"%s: primitive `%s': argument %d doesn't match format\n", 150},
  {"%s: primitive `%s': no extra options can be specified for %%s\n", 151},
  {"%s: primitive `%s': illegal type specifier `%c'\n", 152},
  {"%s: primitive `%s': start offset is bigger than end offset\n", 153},
  {"%s: primitive `%s': too large offsets\n", 154},
  {"%s: undefined state `%s'\n", 155},
  {"%s: error: undefined variable `%s'\n", 156},
  {"%s: ouf of memory", 157},
  {"warning: redefining state `%s'", 158},
  {"%s:%d: warning: redefining subroutine `%s'\n", 159},
  {"%s:%d: error: undefined variable `%s'\n", 160},
  {"%s: too few arguments for subroutine\n", 161},
  {"%s: too many arguments for subroutine\n", 162},
  {"%s: undefined procedure `%s'\n", 163},
  {"%s:%d: error: illegal lvalue for assignment\n", 164},
  {"%s:%d: error: array reference index is not integer\n", 165},
  {"%s:%d: error: negative array reference index\n", 166},
  {"%s:%d: error: illegal rvalue for string assignment\n", 167},
  {"%s:%d: error: illegal type for array reference\n", 168},
  {"%s:%d: error: array reference index out of rance\n", 169},
  {"%s:%d: error: expression between illegal types\n", 170},
};

int _msg_tbl_length = 170;