#line 5 "finduses.nw"
static char rcsid[] = "$Id: finduses.nw,v 1.22 2008/10/06 01:03:05 nr Exp nr $";
static char rcsname[] = "$Name: v2_12 $";
static struct keepalive { char *s; struct keepalive *p; } keepalive[] =
  { {rcsid, keepalive}, {rcsname, keepalive} }; /* avoid warnings */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "errors.h"
#include "match.h"
#include "getline.h"
#include "recognize.h"
#line 42 "finduses.nw"
static Recognizer nwindex;
#define ALPHANUM "_'@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789#"
#define SYMBOLS "!%^&*-+:=|~<>./?`"
#define LISP_ALPHANUM "~<>./?=!%^&*-+_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define LISP_SYMBOLS "()#'@:|`"
#define FINDUSES_LISP "FINDUSES_LISP"
/* note $ and \ both delimiters */
#line 53 "finduses.nw"
static int showquotes = 1;
#line 126 "finduses.nw"
typedef struct line_and_outfile {
    char *line;
    FILE *out;
} LineOut;
#line 95 "finduses.nw"
static void read_ids(FILE *in);
#line 131 "finduses.nw"
static void add_use_markers(FILE *in, FILE *out);
#line 183 "finduses.nw"
static void write_index_use(void *closure, char *id, char *instance);
static char *emit_up_to(FILE *f, char *s, char *limit);
#line 35 "finduses.nw"
int is_lisp() {
    char *lisp = getenv(FINDUSES_LISP);
    return (lisp && atoi(lisp) ? 1 : 0);
}
#line 58 "finduses.nw"
int main(int argc, char **argv) {
    FILE *fp;
    int i;

    progname = argv[0];
    for (i = 1; i < argc && argv[i][0] == '-' && argv[i][1] != 0; i++)
        if (!strcmp(argv[i], "-noquote"))
            showquotes = 0;
        else
            errormsg(Error, "%s: unknown option %s\n", progname, argv[i]);
    /* Start GF modification */
    /* nwindex = new_recognizer(ALPHANUM, SYMBOLS); */
    if (is_lisp())
        nwindex = new_recognizer(LISP_ALPHANUM, LISP_SYMBOLS);
    else
        nwindex = new_recognizer(ALPHANUM, SYMBOLS);
    /* end GF modification */
    if (i == argc) {
       
#line 106 "finduses.nw"
{   FILE *tmp = tmpfile();
    char *line;
    if (tmp == NULL) 
#line 186 "finduses.nw"
errormsg(Fatal, "%s: couldn't open temporary file\n", progname);
#line 109 "finduses.nw"
    while ((line = getline_nw(stdin)) != NULL) {
        if (fputs(line, tmp) == EOF) 
#line 188 "finduses.nw"
errormsg(Fatal, "%s: error writing temporary file\n", progname);
#line 111 "finduses.nw"
        if (is_index(line, "defn")) {
            if (line[strlen(line)-1] == '\n') line[strlen(line)-1] = 0;
            add_ident(nwindex, line+1+5+1+4+1);
        } else if (is_index(line, "localdefn")) {
            if (line[strlen(line)-1] == '\n') line[strlen(line)-1] = 0;
            add_ident(nwindex, line+1+5+1+9+1);
        } else if (is_keyword(line, "fatal")) {
	    exit(1);
	}
    }
    rewind(tmp);
    stop_adding(nwindex);
    add_use_markers(tmp, stdout);
}
#line 77 "finduses.nw"
    } else {
       
#line 87 "finduses.nw"
for (; i < argc; i++)
    if ((fp=fopen(argv[i],"r"))==NULL)
        errormsg(Error, "%s: couldn't open file %s\n", progname, argv[i]);
    else {
        read_ids(fp);
        fclose(fp);
    }
#line 79 "finduses.nw"
       stop_adding(nwindex);
       add_use_markers(stdin, stdout);
    }
    nowebexit(NULL);
    return errorlevel;          /* slay warning */
}
#line 97 "finduses.nw"
static void read_ids(FILE *in) {
    char *line;
    while ((line = getline_nw(in)) != NULL) {
        if (line[strlen(line)-1] == '\n') line[strlen(line)-1] = 0;
        add_ident(nwindex, line);
    }
}
#line 133 "finduses.nw"
static void add_use_markers(FILE *in, FILE *out) {
    char *line;
    int incode = 0;
    LineOut info; info.line = (char *)0; info.out = out;

    while ((line = getline_nw(in)) != NULL) {
        if (is_begin(line, "code") || (showquotes && is_keyword(line, "quote")))
            incode = 1;
        else if (is_end(line, "code") || is_keyword(line, "endquote"))
            incode = 0;
        if (is_keyword(line, "text") && incode) {
            info.line = line + 6; /* skip "@text " */
            search_for_ident(nwindex, line, write_index_use, &info);
            if (*info.line && *info.line != '\n')
                fprintf(out, "@text %s", info.line);    /* has newline */
        } else
            fprintf(out, "%s", line);
    }
}
#line 164 "finduses.nw"
static void write_index_use(void *closure, char *id, char *instance) {
  LineOut *info = (LineOut *) closure;
  info->line = emit_up_to(info->out, info->line, instance);
  fprintf(info->out, "@index use %s\n", id);
  info->line = emit_up_to(info->out, info->line, instance + strlen(id));
}
#line 171 "finduses.nw"
static char *emit_up_to(FILE *f, char *s, char *limit) {
  if (s < limit) {
    char saved = *limit;
    *limit = 0;
    fprintf(f, "@text %s\n", s);
    *limit = saved;
    return limit;
  } else {
    return s;
  }
}
