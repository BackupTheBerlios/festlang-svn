#ifndef __G2P_H_
#define __G2P_H_

#define MAX_G2P_PHON	256 //Max number of phonemes in each G2P decomposition
#define PHON_STR_SIZE	10  //Max number of charchters of each phoneme.

void	G2P		(char* buf , char*** Phon_Decom , int* num_phon);
int	Get_NBest	();

#endif
