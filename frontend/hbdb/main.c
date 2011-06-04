/* Copyright (C) 2001-2010, Parrot Foundation. */

/*

=head1 NAME

hbdb - The Honey Bee Debugger

=head1 SYNOPSIS

hbdb program

=head1 DESCRIPTION

The Honey Bee Debugger (hbdb) is the standard debugger for the Parrot virtual
machine.

=head1 STATIC FUNCTIONS

=over 4

=cut

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parrot/parrot.h"
#include "parrot/hbdb.h"
#include "parrot/api.h"
#include "imcc/api.h"

/* TODO Define usage() function          */

static void fail       (Parrot_PMC interp);
static void load_bytecode(Parrot_PMC interp, Parrot_String file, Parrot_PMC *pbc);

/*

=item C<int main(int argc, char *argv[])>

Entry point of C<hbdb>. Reads source code from file in C<argv[1]>.

=cut

*/

int
main(int argc, char *argv[])
{
    Parrot_PMC        interp   = NULL,
                      pbc      = NULL,
                      main_sub = NULL;
    Parrot_String     file     = NULL;
    Parrot_Init_Args *initargs = NULL;

    /* Setup default initialization parameters */
    GET_INIT_STRUCT(initargs);

    /* Create new interpreter */
    if (!Parrot_api_make_interpreter(NULL, 0, initargs, &interp)) {
        fail(NULL);
    }

    /* Register hbdb runcore */
    if (!Parrot_api_set_runcore(interp, "hbdb", 0)) {
        Parrot_api_destroy_interpreter(interp);
        fail(interp);
    }

    /* TODO Check for file w/o relying on it being in argv[1] */
    /* Get filename */
    if (!Parrot_api_string_import_ascii(interp, argv[1], &file)) {
        fail(interp);
    }

    /* Load bytecode */
    if (file) {
        load_bytecode(interp, file, &pbc);
    }
    else { /* No file specified */
        /* TODO Use parrot_debugger's technique for faking bytecode */
    }

    /* Ready bytecode */
    if (!Parrot_api_ready_bytecode(interp, pbc, &main_sub)) {
        Parrot_api_destroy_interpreter(interp);
        fail(interp);
    }

    /* Run bytecode */
    if (!Parrot_api_run_bytecode(interp, pbc, NULL)) {
        Parrot_api_destroy_interpreter(interp);
        fail(interp);
    }

    /* DEBUG */
    printf("Reached line %d\n", __LINE__);
    /* DEBUG */

    Parrot_api_destroy_interpreter(interp);
    return (0);
}

/*

=item C<static void fail(Parrot_PMC interp)>

Called when an API function fails. Prints an error message and exits with
the status code C<EXIT_FAILURE>.

=cut

*/

static void
fail(Parrot_PMC interp)
{
    char          *msg;

    Parrot_Int    is_error,
                  exit_code;
    Parrot_String errmsg;
    Parrot_PMC    exception;

    if (!Parrot_api_get_result(interp, &is_error, &exception, &exit_code, &errmsg))
        exit(EXIT_FAILURE);

    if (is_error) {
        Parrot_api_string_export_ascii(interp, errmsg, &msg);
        fprintf(stderr, "ERROR: %s\n", msg);
        Parrot_api_string_free_exported_ascii(interp, msg);
    }

    exit(exit_code);
}

/*

=item C<static void load_bytecode(Parrot_PMC interp, Parrot_String file, Parrot_PMC *pbc)>

If C<file> is a C<.pbc> file, the bytecode is loaded and stored in C<pbc>. Otherwise, it must
be compiled first and then loaded into C<pbc>.

=cut

*/

static void
load_bytecode(Parrot_PMC interp, Parrot_String file, Parrot_PMC *pbc)
{
    /* Compile file if it's not already bytecode */
    if (!strcmp(strrchr(file, '.'), "pbc")) {
        Parrot_PMC pir_compreg = NULL;

        /* Create and register a PIR IMCCompiler PMC */
        if (!imcc_get_pir_compreg_api(interp, 1, &pir_compreg))
            fail(interp);

        /* Compile file */
        if (!imcc_compile_file_api(interp, pir_compreg, file, pbc))
            fail(interp);
    }

    /* Load bytecode */
    if (!Parrot_api_load_bytecode_file(interp, file, pbc)) {
        Parrot_api_destroy_interpreter(interp);
        fail(interp);
    }

/*

=back

=head1 SEE ALSO

F<src/hbdb.c>, F<include/parrot/hbdb.h>

=head1 HISTORY

The initial version of C<hbdb> was written by Kevin Polulak (soh_cah_toa) as
part of Google Summer of Code 2011.

=cut

*/

/*
 * Local variables:
 *   c-file-style: "parrot"
 * End:
 * vim: expandtab shiftwidth=4 cinoptions='\:2=2' :
 */

