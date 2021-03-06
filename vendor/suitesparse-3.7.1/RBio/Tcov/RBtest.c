/* ========================================================================== */
/* === RBio/Tcov/RBtest.c: C test =========================================== */
/* ========================================================================== */

/* Copyright 2009, Timothy A. Davis, All Rights Reserved.
   Refer to RBio/Doc/license.txt for the RBio license. */

#include "RBio.h"
#define Int UF_long
#define ID "%ld"

#define SLEN 4096

int main (int argc, char **argv)
{
    double xr, xz, xmin, xmax ;
    double *Ax, *Az ;
    Int nrow, ncol, mkind, skind, *Ap, *Ai, i, *Zp, *Zi, asize, mkind2, skind2,
        znz, j, p, status, njumbled, nzeros, build_upper, zero_handling, fem,
        xsize, nelnz, nnz, kk, anz ;
    int ok ;
    char title [73], key [9], mtype [4], mtype2 [4], *filename, s [100], *As ;

    UFconfig config ;
    config.malloc_memory = malloc ;
    config.free_memory = free ;

    /* test arg-handling for RB functions */

    status = RBread (NULL, 0, 0, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL) ;
    if (status != RBIO_ARG_ERROR)
    {
        printf ("RBtest failure (1)!\n") ;
        return (1) ;
    }

    status = RBreadraw (NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL) ;
    if (status != RBIO_ARG_ERROR)
    {
        printf ("RBtest failure (2)\n") ;
        return (1) ;
    }

    status = RBread ("no file", build_upper, zero_handling, title, key, mtype,
        &nrow, &ncol, &mkind, &skind, &asize, &znz,
        &Ap, &Ai, &Ax, &Az, &Zp, &Zi, &config) ;
    if (status != RBIO_FILE_IOERROR)
    {
        printf ("RBtest failure (3)\n") ;
        return (1) ;
    }

    status = RBreadraw ("no file", title, key, mtype, &nrow, &ncol, &nnz,
        &nelnz, &mkind, &skind, &fem, &xsize, &Ap, &Ai, &Ax, NULL) ;
    if (status != RBIO_FILE_IOERROR)
    {
        printf ("RBtest failure (4)\n") ;
        return (1) ;
    }

    status = RBwrite (NULL, NULL, NULL, 1, 1, NULL, NULL, NULL, NULL,
        NULL, NULL, 0, NULL, NULL) ;
    if (status != RBIO_ARG_ERROR)
    {
        printf ("RBtest failure (5)\n") ;
        return (1) ;
    }

    status = RBkind (1, 1, NULL, NULL, NULL, NULL, 0, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL) ;
    if (status != RBIO_ARG_ERROR)
    {
        printf ("RBtest failure (6)\n") ;
        return (1) ;
    }

    status = RBread ("matrices/m4.rb", 1, 2, title, key, mtype,
        &nrow, &ncol, &mkind, &skind, &asize, &znz,
        &Ap, &Ai, &Ax, NULL, &Zp, &Zi, &config) ;
    if (status != 0)
    {
        UFfree (Ap, &config) ;
        UFfree (Ai, &config) ;
        UFfree (Ax, &config) ;
        UFfree (Zp, &config) ;
        UFfree (Zi, &config) ;
        printf ("RBread test failure (7) "ID"\n", status) ;
        return (1) ;
    }

    /* mangle the matrix */
    Ap [0] = 1 ;
    status = RBwrite ("temp.rb", title, key, nrow, ncol, Ap, Ai, Ax,
        NULL, Zp, Zi, mkind, mtype2, NULL) ;
    if (status == RBIO_OK)
    {
        printf ("RBtest failure (8)\n") ;
        return (1) ;
    }

    Ap [0] = 0 ;
    Zp [0] = 1 ;
    status = RBwrite ("temp.rb", title, key, nrow, ncol, Ap, Ai, Ax,
        NULL, Zp, Zi, mkind, mtype2, NULL) ;
    if (status == RBIO_OK)
    {
        printf ("RBtest failure (9)\n") ;
        return (1) ;
    }
    Zp [0] = 0 ;

    /* valid matrix */
    status = RBwrite ("temp.rb", title, key, nrow, ncol, Ap, Ai, Ax,
        NULL, Zp, Zi, mkind, NULL, NULL) ;
    if (status != RBIO_OK)
    {
        printf ("RBtest failure (10) "ID"\n", status) ;
        return (1) ;
    }

    /* valid matrix, different integer formats */
    Ax [0] = 1e5 ;
    for (kk = 0 ; kk < 10 ; kk++)
    {
        sprintf (s, "temp_" ID ".rb", kk) ;    
        status = RBwrite (s, title, key, nrow, ncol, Ap, Ai, Ax,
            NULL, Zp, Zi, mkind, NULL, NULL) ;
        if (status != RBIO_OK)
        {
            printf ("RBtest failure (11) "ID" "ID"\n", status, kk) ;
            return (1) ;
        }
        Ax [0] *= 10 ;
    }

    /* valid matrix, but with a very large integer */
    Ax [0] = 1e9 + 1 ;
    status = RBwrite ("temp1.rb", title, key, nrow, ncol, Ap, Ai, Ax,
        NULL, Zp, Zi, mkind, NULL, NULL) ;
    if (status != RBIO_OK)
    {
        printf ("RBtest failure (12) "ID"\n", status) ;
        return (1) ;
    }

    /* valid matrix, but with a very large real */
    Ax [0] = 1e100 ;
    status = RBwrite ("temp2.rb", title, key, nrow, ncol, Ap, Ai, Ax,
        NULL, Zp, Zi, mkind, NULL, NULL) ;
    if (status != RBIO_OK)
    {
        printf ("RBtest failure (13) "ID"\n", status) ;
        return (1) ;
    }

    /* valid matrix, but with a Z that overlaps A */
    Ax [0] = 1 ;
    Zi [0] = 1 ;
    status = RBwrite ("temp3.rb", title, key, nrow, ncol, Ap, Ai, Ax,
        NULL, Zp, Zi, mkind, NULL, NULL) ;
    if (status != RBIO_OK)
    {
        printf ("RBtest failure (14) "ID"\n", status) ;
        return (1) ;
    }

    /* invalid file name */
    status = RBwrite ("gunk/gunk.rb", title, key, nrow, ncol, Ap, Ai, Ax,
        NULL, Zp, Zi, mkind, NULL, NULL) ;
    if (status != RBIO_FILE_IOERROR)
    {
        printf ("RBtest failure (12) "ID"\n", status) ;
        return (1) ;
    }

    /* cannot write a matrix with no entries */
    for (j = 0 ; j <= ncol ; j++)
    {
        Ap [j] = 0 ; 
        Zp [j] = 0 ; 
    }
    status = RBwrite ("temp.rb", title, key, nrow, ncol, Ap, Ai, Ax,
        NULL, Zp, Zi, mkind, NULL, NULL) ;
    if (status != RBIO_DIM_INVALID)
    {
        printf ("RBtest failure (13) "ID"\n", status) ;
        return (1) ;
    }

    UFfree (Ap, &config) ;
    UFfree (Ai, &config) ;
    UFfree (Ax, &config) ;
    UFfree (Zp, &config) ;
    UFfree (Zi, &config) ;

    /* re-read a valid matrix */
    status = RBread ("matrices/m4.rb", 1, 0, title, key, mtype,
        &nrow, &ncol, &mkind, &skind, &asize, &znz,
        &Ap, &Ai, &Ax, NULL, NULL, NULL, &config) ;
    if (status != 0)
    {
        UFfree (Ap, &config) ;
        UFfree (Ai, &config) ;
        UFfree (Ax, &config) ;
        UFfree (Zp, &config) ;
        UFfree (Zi, &config) ;
        printf ("RBread test failure (14) "ID"\n", status) ;
        return (1) ;
    }

    status = RBok (nrow, ncol, asize, Ap, Ai, Ax, Az, NULL, mkind,
        &njumbled, &nzeros) ;
    if (status != RBIO_OK)
    {
        printf ("RBread test failure (15) "ID"\n", status) ;
    }

    status = RBok (nrow, ncol, asize, Ap, Ai, Ax, Az, NULL, -1,
        &njumbled, &nzeros) ;
    if (status != RBIO_MKIND_INVALID)
    {
        printf ("RBread test failure (16) "ID"\n", status) ;
    }

    status = RBok (-1, ncol, asize, Ap, Ai, Ax, Az, NULL, mkind,
        &njumbled, &nzeros) ;
    if (status != RBIO_DIM_INVALID)
    {
        printf ("RBread test failure (17) "ID"\n", status) ;
    }

    i = Ap [1] ;
    Ap [1] = 999999 ;
    status = RBok (nrow, ncol, asize, Ap, Ai, Ax, Az, NULL, mkind,
        &njumbled, &nzeros) ;
    if (status != RBIO_CP_INVALID)
    {
        printf ("RBread test failure (18) "ID"\n", status) ;
    }
    Ap [1] = i ;

    status = RBok (nrow, ncol, asize, Ap, NULL, Ax, Az, NULL, mkind,
        &njumbled, &nzeros) ;
    if (status != RBIO_ROW_INVALID)
    {
        printf ("RBread test failure (19) "ID"\n", status) ;
    }

    i = Ai [0] ;
    Ai [0] = 999999 ;
    status = RBok (nrow, ncol, asize, Ap, Ai, Ax, Az, NULL, mkind,
        &njumbled, &nzeros) ;
    if (status != RBIO_ROW_INVALID)
    {
        printf ("RBread test failure (20) "ID"\n", status) ;
    }
    Ai [0] = i ;

    i = Ai [1] ;
    Ai [1] = Ai [0] ;
    status = RBok (nrow, ncol, asize, Ap, Ai, Ax, Az, NULL, mkind,
        &njumbled, &nzeros) ;
    if (status != RBIO_JUMBLED)
    {
        printf ("RBread test failure (21) "ID"\n", status) ;
    }
    Ai [1] = i ;

    ok = 1 ;
    As = (char *) UFmalloc (asize, sizeof (char), &ok, &config) ;
    for (i = 0 ; i < asize ; i++) As [i] = 1 ;
    status = RBok (nrow, ncol, asize, Ap, Ai, Ax, Az, As, 1,
        &njumbled, &nzeros) ;
    if (status != RBIO_OK)
    {
        printf ("RBread test failure (22) "ID"\n", status) ;
    }
    UFfree (As, &config) ;

    printf ("RBtest OK\n") ;
    return (0) ;
}
