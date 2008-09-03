#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lts.h"
#include "G2P.h"

/*------------------------------------------------------------------------------------------
	Accessor function to retrive NBEst
  ------------------------------------------------------------------------------------------*/
int Get_NBest()
{
	int		i;
	i = LTS_NBEST;
	return i;
}/*Get_NBest*/

/*------------------------------------------------------------------------------------------
	DOCUMENTATION IF NEEDED
  ------------------------------------------------------------------------------------------*/
void G2P(char* buf , char*** Phon_Decom , int* num_phon)
{
	int i;
        char *result[LTS_NBEST];

	lts_init();    

	for (i = 0; i < LTS_NBEST; i++)
	{
    	result[i] = malloc(sizeof(char)*MAX_G2P_PHON);
	}
	lts (buf, result);
	for (i = 0; i < LTS_NBEST; i++)
	{
    	num_phon[i] = lts_copy_string(Phon_Decom[i]  , result[i]);
	}

	for (i=0 ; i<LTS_NBEST ; i++)
	{
		free(result[i]);
	}
}/*G2P*/
