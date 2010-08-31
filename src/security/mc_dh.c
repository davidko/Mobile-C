/* SVN FILE INFO
 * $Revision: 199 $ : Last Committed Revision
 * $Date: 2008-07-11 10:33:31 -0700 (Fri, 11 Jul 2008) $ : Last Committed Date */
#include <stdio.h>
#include "../include/mc_error.h"
#include "xyssl-0.9/include/xyssl/dhm.h"
#include "xyssl-0.9/include/xyssl/havege.h"
#include "mc_dh.h"
#include "asm_node.h"
#ifndef _WIN32
#include "config.h"
#else
#include "../winconfig.h"
#endif

#ifdef MC_SECURITY

int dh_data_Destroy(dh_data_p dh_data)
{
  /* This is buggy for some reason */
  /*  dhm_free(&(dh_data->dhm)); */
  return 0;
}

dh_data_p dh_data_Initialize(void)
{
  int len;
  havege_state hs;
  char *buf;
  dh_data_p dh_data;

  buf = (char*)malloc(sizeof(char) * 1024);
  CHECK_NULL(buf, exit(0););
  dh_data = (dh_data_p)malloc(sizeof(dh_data_t));
  CHECK_NULL(dh_data, exit(0););

  memset(dh_data, 0, sizeof(dh_data_t));

  /* Seed random num generator */
  havege_init(&hs);

/*  dh_GetKeyPair( &(dh_data.rsa) ); */ /* FIXME: Implement this later:
                                           it is too complicated for now */
  dh_GetPrime( &(dh_data->dhm) );

  /* Set up dh parameters */
  if (
      dhm_make_params
      (
       &(dh_data->dhm),
       havege_rand,
       &hs,
       buf,
       &len
      )
     )
  {
    fprintf(stderr, "Potentially fatal error. %s:%d\n",__FILE__,__LINE__);
  }

  free(buf);
  return dh_data;
}

dh_data_p dh_data_InitializeFromString(char* P, char* G, char* GY)
{ 
  dh_data_p dh_data;

  dh_data = (dh_data_p)malloc(sizeof(dh_data_t));

  memset(dh_data, 0, sizeof(dh_data_t));

  if ( mpi_read_string( &(dh_data->dhm.P), 16, P) ) {
    fprintf
      (
       stderr,
       "Error initializing prime number P. %s:%d\n",
       __FILE__, __LINE__
      );
    goto cleanup;
  }
  if ( mpi_read_string( &(dh_data->dhm.G), 16, G) ) {
    fprintf
      (
       stderr,
       "Error initializing. %s:%d\n",
       __FILE__, __LINE__
      );
    goto cleanup;
  }
  if ( mpi_read_string( &(dh_data->dhm.GY), 16, GY) ) {
    fprintf
      (
       stderr,
       "Error initializing. %s:%d\n",
       __FILE__, __LINE__
      );
    goto cleanup;
  }

  return dh_data;
cleanup:
  free(dh_data);
  return NULL;
}

#define DH_P_SIZE 1024
#define GENERATOR "4"
int dh_GenPrime(dhm_context* dhm)
{
  mpi Q;
  havege_state hs;
  int ret;

  mpi_init( &(dhm->G), &(dhm->P), &Q, NULL);
  mpi_read_string( &(dhm->G), 10, GENERATOR );

  /* Seed random number generator */
  havege_init(&hs);
  /*
   * This can take a long time...
   */
  if( ( ret = mpi_gen_prime( &(dhm->P), DH_P_SIZE, 1,
          havege_rand, &hs ) ) != 0 )
  {
    printf( " failed\n  ! mpi_gen_prime returned %08x\n\n", ret );
    goto exit;
  } else
  {
    printf("Prime number successfully generated...\n");
  }

  /* Verify our prime number is prime */
  if( ( ret = mpi_sub_int( &Q,       &(dhm->P), 1 ) ) != 0 ||
      ( ret = mpi_div_int( &Q, NULL, &Q, 2 ) ) != 0 ||
      ( ret = mpi_is_prime( &Q ) ) != 0 )
  {
    printf( " failed\n  ! mpi_xx returned %08x\n\n", ret );
    goto exit;
  }
  return MC_SUCCESS;

exit:
  mpi_free(&Q, NULL);
  return ret;
}

int dh_GetPrime(dhm_context* dhm)
{
  FILE* f;

  char filename[200] = "dh_prime.txt";
  char buf[80];
  char *tmp;

  int gen_prime = 0;
  int ret;

  while
    ( 
     (( f = fopen( "dh_prime.txt", "rb" ) ) == NULL) &&
     gen_prime == 0
    )
    {
      printf("Could not open %s for reading.\n", filename);
      printf("Would you like to:\n");
      printf("\t1. Specify another file containing the prime number, or\n");
      printf("\t2. Generate a new prime number? (May take some time)\n");
#ifndef _WIN32
      fgets(buf, sizeof(buf), stdin); /* FIXME: Why the HELL doesn't this work in windows???*/
#else
	  gets(buf);
#endif
      switch(buf[0]) 
      {
        case '1':
          printf("Please enter filename: ");
          fgets(filename, sizeof(filename), stdin);
          break;
        case '2':
          ret = dh_GenPrime(dhm);
          gen_prime = 1;
          if(!ret) 
          {
            printf("Would you like to save the generated prime number to a file?\n");
            printf("y/n: ");
#ifndef _WIN32
            fgets(buf, sizeof(buf), stdin);
#else
			gets(buf);
#endif
            switch(buf[0]) {
              case 'y':
                printf("Please enter filename [dh_prime.txt]: ");
#ifndef _WIN32
                fgets(filename, sizeof(filename), stdin);
#else
				gets(filename);
#endif
                tmp = strrchr(filename, '\n');
                if (tmp != NULL) { *tmp = '\0'; }
                if(filename[0] == '\0') { strcpy(filename, "dh_prime.txt"); }
                if ( (f=fopen(filename, "wb+")) == NULL) {
                  break;
                }
                if( mpi_write_file( "P = ", &(dhm->P), 16, f ) ||
                    mpi_write_file( "G = ", &(dhm->G), 16, f ) ) {
                  fprintf(stderr, "Error! %s:%d\n", __FILE__, __LINE__);
                }
                break;
              case 'n':
                printf("Not saving file...\n");
                break;
              default:
                printf("Unknown option. Not saving file...\n");
                break;
            }
            goto exit;
          }
        default:
          printf("Invalid option.\n");
          break;
      }
    }

  if( mpi_read_file( &(dhm->P), 16, f ) != 0 ||
      mpi_read_file( &(dhm->G), 16, f ) != 0 )
  {
    printf( " failed\n  ! Invalid DH parameter file\n\n" );
    goto exit;
  }

exit:
  if(f) fclose(f);
  return ret;
}

/* Get rsa key pair */
int rsa_GetKeyPair(rsa_context* rsa)
{
  FILE* f;

  char filename[200];
  char buf[80];
  char* tmp;

  int ret=0;

  strcpy(filename, "rsa_priv.txt");
  while( ( f = fopen( filename, "rb" ) ) == NULL )
  {
    printf("Private key file: %s not found.\n", filename);
    printf("Would you like to:\n");
    printf("\t1. Specify another filename? or\n");
    printf("\t2. Generate a new private key file?\n");
    fgets(buf, sizeof(buf), stdin);
    switch(buf[0]) {
      case '1':
        printf("Please enter filename: ");
        fgets(filename, sizeof(filename), stdin);
        while( (tmp = strchr(filename, '\n') ) != NULL ) {
          *tmp = '\0';
        }
        break;
      case '2':
        rsa_GenKeyPair(rsa);
        break;
      default:
        printf("Incorrect Option.\n");
        break;
    }
  }

  if ( ( ret = rsa_read_private( rsa, f ) ) != 0 ){
    printf( " failed\n  ! rsa_read_private returned %08x\n\n", ret );
    goto exit;
  }

exit:
  fclose( f );
  return ret;
}

int rsa_GenKeyPair(rsa_context* rsa)
{
  /* FIXME: Does nothing for now. This function is not currently used */
  return 0;
}

#endif /*MC_SECURITY*/
