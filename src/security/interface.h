// interface.h
// prototype of functions that interfaces with library 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PATH xyssl-0.9/include/xyssl

#include "xyssl-0.9/include/xyssl/havege.h"
#include "xyssl-0.9/include/xyssl/bignum.h"
#include "xyssl-0.9/include/xyssl/rsa.h"
#include "xyssl-0.9/include/xyssl/sha2.h"
#include "xyssl-0.9/include/xyssl/sha1.h" 
#include "xyssl-0.9/include/xyssl/aes.h"
#include "xyssl-0.9/include/xyssl/md5.h"
//#include "PATH/mc_error.h"

#define MODE_ENCRYPT    0
#define MODE_DECRYPT    1
#define MAXDATASIZE	4096

#define KEY_SIZE 1024
#define EXPONENT 65537

#define RSA_EN 0
#define RSA_DE 1

/* 
 * Copying a part of string 'src' starting from 
 * 'start_index' upto 'length' and stroe in 'dest' 
 */
//static void mystrncpy(char *dest, char *src, int start_index, int length);

/* set 'pubkey' with the public key of host name 'hname' from known-host file lookup
 * pubkey	:	public key of hname 
 * hname	:	host name for which public key is requested		
 *
 * Return value :	1 if public key of 'hname' is found in known host file 
 *			otherwise -1
 */
int read_known_host_file(char *pubkey, char *hname, char *filename);

/* 
 * separating the two part of pubkey into N and E 	
 */
//static void separate_key_parts(char *pubkey, char *N, char *E, char *D, char *P, char *Q, char *DP, char *DQ, char *QP);

/* Read encrypted file, decrypt it and put the content in 'string' 
 * and return private key  (plaintext) in privatekey
 * enfile	:	AES encrypted private key file 
 * string	:	output plaintext private key
 * passphase	:	passphase to decrypt the encrypte private key file,
 *	 		must be same as when used to encrypt that file
 *
 * Return value	:	1 if decryption is successfull otherwise -1
 */
int read_encrypted_file(char *enfile, char *string, unsigned char *passphase);


/* Encrypting plaintext into ciphertext with public key (N, E) using RSA  
 * publickey	:	public key
 * plaintext	:	input text to be encrypted
 * ciphertext	:	output containing encrypted text
 *
 * Return value	:	1 if encryption is successful otherwise -1	
 */
int rsa_encryption(char *publickey, char *plaintext, char *ciphertext);

/* Decrypting ciphertext into plaintext with private key using RSA
 * ciphertext		:	input encrypted text
 * plaintext		:	output plain text
 * privatekeyfile	:	file contain private key
 *
 * Return value		:	1 if decryption is successful otherwise -1
 */
int rsa_decryption(char *ciphertext, char *plaintext, char *privatekeyfile);
 
/* Called by agency who want to initiate migration process.
 * handle all piggyback authentication process
 * sockfd	:	socket descriptor
 * publickey	:	public key of other peer
 * privatekey	:	private key
 * 
 * Return value	:	1 upon success, -1 if connected agency is not authenticated
 *			-2 if I am unable to get authentication from otehr agency	
 */

int initiate_migration_process(int sockfd, int *nonce, char *publickey, char *privatekey, unsigned char *aes_key);

/* Called by agency who is a receiver agency in migration process.
 * handle all piggyback authentication process
 * sockfd       :       socket descriptor
 * publickey    :       public key of other peer
 * privatekey   :       private key
 * 
 * Return value :       1 upon success, -1 if connected agency is not authenticated
 *                      -2 if I am unable to get authentication from otehr agency       
 */

int reply_migration_process(int new_fd, int *nonce, char *publickey, char *privatekey, unsigned char *aes_key);

/* Prove own authentication to other peer:
 * This function waits to receive challenge from peer, decrypt it calculate 
 * MD 5 digest, increment received nonce and send back to peer and waits for 
 * authentication status response.
 * sockfd	:	socket descriptor
 * publickey	:	public key of otehr peer
 * privatekey	:	private key
 *
 * Return value	:	1 upon success otherwise -1
 */
//int proving_own_authentication(int sockfd, char *publickey, char *privatekey);

/* Authenticating the peer:  
 * This funcation prepare challenge, send to peer and waits for solution. 
 * If peer solve successfully it is authenticated other wise not.
 * sockfd	:	socket descriptor
 * publickey	:	public key
 * privatekey	:	private key 
 *
 * Return value	:	1 upon success otherwise -1
 */
//int verifying_peer_authentication(int new_fd, char *publickey, char *pirvatekey);

/* Generate AES Keys
 * 
 *
 */

void generate_AES_key(char *key);

/* generate AES key, encrypt it with peer public key and send to it.
 * sockfd 	:     socket connect with peer
 * key    	:     AES key
 * publickey	:     Public key of peer
 *
 * Return value	:     1 upon success othrewise -1, -2 if receiver is unable to receive it successfully	
 */
int generate_encrypt_send_AES_key(int sockfd, int *nonce, unsigned char *key, char *publickey, char *privkey);

/* Receive encrypted AES key, decrypt it and save in 'key'
 * new_fd	:	socket descriptor
 * key		:	AES key
 * privkeyfile	:  	private key		
 *
 * Return value :	1 upon success othrewise -1, -2 if receiver is unable to receive it successfully
 */
int receive_decrypt_AES_key(int new_fd, int *nonce, unsigned char *key, char *privkey, char *publickey);

/* AES encryption decryption
 * mode    	:	MODE_ENCRYPT 0 , MODE_DECRYPT 1
 * infile  	:	input file  (plain text / cipher text )
 * outfile 	:	output file (cipher text / plain text )
 * AES_key 	:	256 bits key  (32 Bytes)
 * 
 * Return value :	1 upon success
 */
int aes_en_de(int mode, char *infile, char *outfile, unsigned char *AES_key, int *nonce, int sockfd);

/* Append session identifier (nonce) at the end of MA file	
 * my_nonce	:	session identifier 
 * MA_file	:	MA file
 * 
 * Return value	:	1 upon success otherwise -1
 */
//static int append_nonce_to_MA(int *my_nonce, char *MA_file );

/* Remove session identifier (nonce) at the end of MA file and verify it
 * sockfd	:	socket descriptor	
 * my_nonce     :       session identifier 
 * MA_file      :       MA file
 * 
 * Return value :       1 upon success otherwise -1
 */
//static int extract_nonce_from_MA(int sockfd, int *my_nonce, char *MA_file);



/* Read encrypted MA file 'outfile' and send to peer
 * sockfd 	:	socket connected with peer
 * outfile	:	file to send (En. MA file)
 *
 * Return value :	1 upon success othrewise -1
 */
int send_AES_en_MA(int sockfd, int *nonce, char *outfile, char *peer_pubkey);

/* Receive encrypted MA from client and put write 'infile'
 * new_fd	:	socket descriptor
 * infile	:	MA file	
 *
 * Return value :	1 upon success othrewise -1
 */
int receive_AES_en_MA(int new_fd, int *nonce, char *infile, char *privatekey);

// used for integrity when RSA is used to en-decrypt MA
int receiving_verifying_MA_RSA(int sockfd, char *privkeyfile);

int receiving_decrypting_MA_RSA(int new_fd, char *privkeyfile);


/////////////////////////////////////////////////////////////////////
/////			Utility functions			/////
/////////////////////////////////////////////////////////////////////

/* Generate RSA public private key pair and store in specified file 
 * in plain text
 *
 */
int generate_RSA_keys_plaintext(char *pubkeyfile, char *privkeyfile);

/* Generate RSA public private key pair and store in specified file 
 * but private key in plain text
 *
 */
int generate_RSA_keys_ciphertext(char *pubkeyfile, char *privkeyfile, unsigned char *passphrase);


