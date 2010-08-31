/* SVN FILE INFO
 * $Revision: 199 $ : Last Committed Revision
 * $Date: 2008-07-11 10:33:31 -0700 (Fri, 11 Jul 2008) $ : Last Committed Date */
#ifndef _MC_DH_H_
#define _MC_DH_H_
#include "xyssl-0.9/include/xyssl/rsa.h"
#include "asm_node.h"
#ifndef _WIN32
#include "config.h"
#else
#include "../winconfig.h"
#endif

#ifdef MC_SECURITY

int 
dh_ReadPrivKey(rsa_context* rsa);
 
dh_data_p 
dh_data_Initialize(void);

dh_data_p 
dh_data_InitializeFromString(char* P, char* G, char* GY);

int 
dh_GenPrime(dhm_context* dhm);

int 
dh_GetPrime(dhm_context* dhm);

int 
dh_GetKeyPair(rsa_context* rsa);

int rsa_GenKeyPair(rsa_context* rsa);
#endif /*MC_SECURITY*/
#endif
