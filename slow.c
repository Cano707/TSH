/*
 * ===========================================================================
 *
 * slow.c --
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
#include <stdlib.h>
#include <unistd.h>


int
main( int argc, char **argv, char **envp )
{
  printf( "Waiting...\n" );

  if( argc > 1 ) {
    printf( "Arg: %s\n", argv[1] );
    usleep( atoi( argv[1] )*1000000 );
    _exit( atoi( argv[1] ) );
  } else {
    printf( "No argument given!\n" );
    usleep( 3000000 );
    _exit( 0 );
  }
}
