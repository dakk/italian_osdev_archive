/*!	\file apps/src/hello/hello.c
 *	\brief Simple routine to test the console I/O.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-04\n
 */

#include <stdio.h>
#include <stdlib.h>


// ASCII art.
static char message[] =
"\n\r"
"                   ___====-_  _-====___\n"
"             _--~~~#####//      \\\\#####~~~--_\n"
"          _-~##########// (    ) \\\\##########~-_\n"
"         -############//  :\\^^/:  \\\\############-\n"
"       _~############//   (@::@)   \\\\############~_\n"
"      ~#############((     \\\\//     ))#############~\n"
"     -###############\\\\    (^^)    //###############-\n"
"    -#################\\\\  / \"\" \\  //#################-\n"
"   -###################\\\\/      \\//###################-\n"
"  _#/:##########/\\######(   /\\   )######/\\##########:\\#_\n"
"  :/ :#/\\#/\\#/\\/  \\#/\\##\\  :  :  /##/\\#/  \\/\\#/\\#/\\#: \\:\n"
"  '  :/  V  V  '   V  \\#\\: :  : :/#/  V   '  V  V  \\:  '\n"
"     '   '  '      '   \\ : :  : : /   '      '  '   '\n"
"\n\t\"Welcome in the Minirighi dungeon! :)\"\n\n\r";
;

// Print the ASCII art.
int main( int argc, char **argv )
{
	printf( message );

	return( EXIT_SUCCESS );
}
