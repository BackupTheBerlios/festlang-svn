#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lts.h"

int
main (int argc, char* argv[])
{
    FILE *f = NULL;    
    int i;

    char buf[256];
    char *result[LTS_NBEST];
    
    for (i = 1; i < argc; i++)
       {
    	    if (strncmp("--help", argv[i], 6) == 0)
    	      {
	    	    printf ("Usage: lts [filename]");
	    	    return 1;
    	      }

       	    f = fopen (argv[i], "r");
	    if (!f) 
    	       {
		    printf ("File %s not found\n", argv[i]);
		    return 1;
	       }
      }	

    while (fgets (buf, 256, f ? f : stdin))
      {	
    	    for (i = 0; i < LTS_NBEST; i++)
    		result[i] = malloc (256);
    	    lts (buf, result);
    	    for (i = 0; i < LTS_NBEST; i++)
    		lts_dump_string (result[i]);
      }

}	/*main */
