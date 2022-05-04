/*
 * ===========================================================================
 *
 * endless.c --
 *
 *
 * Wolfram Schenck <wolfram.schenck@fh-bielefeld.de>
 *
 *    Copyright (C) 2015
 *    Engineering Informatics
 *    Department of Engineering Sciences and Mathematics
 *    University of Applied Sciences Bielefeld
 *
 * 1.2 / 13. Oct 15 (ws)
 *
 *
 * ===========================================================================
 */


#include <stdio.h>
#include <unistd.h>


int
main( int argc, char **argv, char **envp )
{
  printf( "Starting endless loop...\n" );

  while( 1 )
    usleep( 100000 );

  _exit( 0 );
}
