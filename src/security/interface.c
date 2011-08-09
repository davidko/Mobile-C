// interface.c 

#include "interface.h"
#ifdef _WIN32
#include "winsock2.h"
#include <memory.h>
#else
#include <sys/socket.h>
#endif


//#define COM 1	// comments

#ifdef OBS
static void mystrncpy_binary(char *dest, char *src, int start_index, int length) /*{{{*/
{

        int i, j, len; 
        j=start_index; 
        len = strlen(src);
        memset(dest, '\0', strlen(dest) );
        for(i=0; j < (start_index+length) /*&& j<=len*/ ; i++,j++ ){
                dest[i] = src[j];
        }       
}/*}}}*/
#endif // OBS

static void separate_key_parts(char *key, char *N, char *E, char *D, char *P, char *Q, char *DP, char *DQ, char*QP) /*{{{*/
{
        int i = 0, j = 0;
#ifdef COM
	printf("key : '%s'\n",key);	
	printf(" . key length %d\n", strlen(key) );
#endif
	while( key[i] != '\n') {
		N[j] = key[i];
		i++;
		j++;
        }
        N[j++]='\n';
        N[j] = '\0';
	i++;
        j=0;
        while(key[i] != '\n'){
          E[j++]=key[i++];
        }
        E[j++]='\n';
	E[j]='\0';

	if( strlen(key) > 500 ) // this means that key is private key
	{
		j=0; i++;
		while(key[i] != '\n')
			D[j++] = key[i++];
		D[j++]='\n';
		D[j] = '\0';

		j=0; i++;
		while(key[i] != '\n')
                        P[j++] = key[i++];
                P[j++]='\n';
		P[j]='\0';	

		j=0; i++;
                while(key[i] != '\n')
                        Q[j++] = key[i++];
                Q[j++]='\n';
		Q[j] = '\0';
		
		j=0; i++;
                while(key[i] != '\n')
                        DP[j++] = key[i++];
                DP[j++]='\n';
		DP[j] = '\0';

		j=0; i++;
                while(key[i] != '\n')
                        DQ[j++] = key[i++];
                DQ[j++]='\n';
		DQ[j] = '\0';

		j=0; i++;
                while(key[i] != '\n')
                        QP[j++] = key[i++];
                QP[j++]='\n';
		QP[j]='\0';
		
	}
} /*}}}*/
 
int rsa_encryption(char *publickey, char *plaintext, char *ciphertext) /*{{{*/
{
        int ret, ilen, status, mode;
        rsa_context rsa;
	char N[265], E[13];
        
        memset(N, '\0', 265); 
        memset(E, '\0', 13); 	

	separate_key_parts(publickey, N, E, NULL, NULL, NULL, NULL, NULL, NULL);
        
        rsa_init( &rsa, RSA_PKCS_V15, 0, NULL, NULL );
        if( ( ret = mpi_read_mystring( &rsa.N, 16, N ) ) != 0 ||
            ( ret = mpi_read_mystring( &rsa.E, 16, E ) ) != 0 )
        {   
                printf( " failed\n  ! mpi_read_file returned %d\n\n", ret );
                return -1;
        }       
        rsa.len = ( mpi_msb( &rsa.N ) + 7 ) >> 3;

    if (rsa_check_pubkey(&rsa) != 0){
       printf("Public key is invalid \n");
       printf("please ensure: Remove space at the end of each public key from known_host file \n");
                return -1;
    }	

        // Encrypting plaintext
        ilen = strlen(plaintext);
        mode = RSA_EN;
        status = rsa_pkcs1_encrypt(
            &rsa, 
            mode, 
            ilen, 
            (unsigned char*)plaintext, 
            (unsigned char*)ciphertext );
#ifdef COM
        printf( " . Encryption status = %d \n",status );
#endif
        return status; // 0 if successfull
        
} /*}}}*/

int rsa_decryption(char *ciphertext, char *plaintext, char *privatekey) /*{{{*/
{
     rsa_context rsa;
     int ret, olen, status, mode;

     char N[265], E[13], D[265], P[140], Q[140], DP[140], DQ[140], QP[140];

     memset(N,'\0',265); memset(E,'\0',13); memset(D,'\0',265); memset(P,'\0',140);
     memset(Q,'\0',140); memset(DP,'\0',140); memset(DQ,'\0',140); memset(QP,'\0',140);

	separate_key_parts(privatekey, N, E, D, P, Q, DP, DQ, QP);

	rsa_init( &rsa, RSA_PKCS_V15, 0, NULL, NULL );
        if( ( ret = mpi_read_mystring( &rsa.N , 16, N  ) ) != 0 ||
            ( ret = mpi_read_mystring( &rsa.E , 16, E  ) ) != 0 ||
	    ( ret = mpi_read_mystring( &rsa.D , 16, D  ) ) != 0 ||
            ( ret = mpi_read_mystring( &rsa.P , 16, P  ) ) != 0 ||
            ( ret = mpi_read_mystring( &rsa.Q , 16, Q  ) ) != 0 ||
            ( ret = mpi_read_mystring( &rsa.DP, 16, DP ) ) != 0 ||
            ( ret = mpi_read_mystring( &rsa.DQ, 16, DQ ) ) != 0 ||
            ( ret = mpi_read_mystring( &rsa.QP, 16, QP ) ) != 0     )
        {  
#ifdef COM 
                printf( " failed\n  ! mpi_read_file returned %d\n\n", ret );
#endif
                return -1;
        }
	if (rsa_check_privkey(&rsa) != 0){
#ifdef COM
		printf("Private key is invalid \n");
#endif
		return -1;
	}else
#ifdef COM
		printf("Private Key is valid \n");
#endif

        rsa.len = ( mpi_msb( &rsa.N ) + 7 ) >> 3;

        mode = RSA_DE;
		
        status = rsa_pkcs1_decrypt(
            &rsa, 
            mode, 
            &olen, 
            (unsigned char*)ciphertext, 
            (unsigned char*)plaintext );
	plaintext[strlen(plaintext)] = '\0';
#ifdef COM
        printf( " . Decryption status = %d \n",status );        
#endif	
        return status;  // 0 if successfull
} /*}}}*/

static int append_nonce_to_MA(int *my_nonce, char *MA_file ) /*{{{*/
{
	char nonce_str[10];
	FILE *fd;
#ifdef COM	
	printf("Append nonce to MA file \n");
#endif
	// open file in append mode
	if ( ( fd = fopen(MA_file, "a+") ) == NULL ){
#ifdef COM
		printf("fopen() error. \n");
#endif
		return -1;
	}
   
	(*my_nonce)+=2;	
	memset(nonce_str, '\0', 10);
	sprintf(nonce_str, "%d", *my_nonce );
	fputs("\n", fd);
	if (fputs (nonce_str, fd) == EOF  ){
#ifdef COM
		printf("fputs() error. \n");
#endif		
		fclose(fd);
		return -1;
	}
#ifdef COM
	printf(". nonce append \n");
#endif
	fclose(fd);
	return 1;
}/*}}}*/

static int remove_nonce_from_MA(char *MA_file){ /*{{{*/
	
	FILE *fd;
	struct stat stbuf;	
	int len; 
	char *MA_buffer, *p;
	int numbytes;
	
	if ( ( fd = fopen(MA_file, "r+") ) == NULL ){
        printf(" . Unable to open MA file \n");
        return -1;
    }     
        
	stat(MA_file ,&stbuf );

	// reading the whole file
	MA_buffer = (char*) malloc (sizeof(char)*((int)stbuf.st_size + 2) );
    if (MA_buffer == NULL){
  		printf("malloc() error. \n");
		fclose(fd);
        return -1;
     }
    memset(MA_buffer, '\0', (int)stbuf.st_size);
    // copy whole file into buffer:
    len = (int)stbuf.st_size;
    fread ( MA_buffer, len, 1, fd);
    fclose(fd);
    p = strstr (MA_buffer, "</MOBILEC_MESSAGE>"); 
    p[18] = '\r';
    p[19] = '\0';
 
    len = strlen(MA_buffer);
	
	if( (fd = fopen(MA_file, "w")) == NULL ){
		printf("fopen() error. ");
		free(MA_buffer);
        return -1;
	}
	numbytes = fwrite (MA_buffer , strlen(MA_buffer), 1, fd );	
	fclose(fd);
	free(MA_buffer);
	return 1;
}/*}}}*/

static int extract_nonce_from_MA(int sockfd, int *my_nonce, char *MA_file) /*{{{*/
{
	char nonce_str[15];
	FILE *fd;
	struct stat stbuf;	
	int rece_nonce;
	char ack[4];
	int numbytes;
#ifdef COM	
	printf("Extract and verify Nonce from MA file \n");
#endif
	if ( ( fd = fopen(MA_file, "r+") ) == NULL ){
		printf("Error while opening a file \n");
        return -1;
    }
        
	// obtain size of MA file
	stat(MA_file ,&stbuf );

	// set file descriptor to read nonce
	fseek ( fd , ((int)stbuf.st_size)-11 , SEEK_SET );

	// read nonce
	memset(nonce_str, '\0', 15);
	fread ( nonce_str, 11, 1, fd);

	if( strlen(nonce_str) < 10 ){
		printf(" . Error while reading nonce \n");
		fclose(fd);
		return -1;
	}
	rece_nonce = atoi(nonce_str);
	fclose(fd);

	// remove nonce from MA file
	remove_nonce_from_MA(MA_file);

	// nonce verification	
	(*my_nonce)++;
	if( rece_nonce != (*my_nonce) ){
#ifdef COM
		printf(" . nonce doesn't matches \n");
		printf(" . remove received MA file\n");
#endif
		remove(MA_file);
		strcpy(ack,"-ve");
	}
	else{
		strcpy(ack,"+ve");
	}
	// sending encrypted AES key to server
#ifdef COM
	printf(" . sending acknowledge \n");
#endif
    if ( (numbytes=send(sockfd , ack, 20, 0)) == -1){
         perror("send \n");
         return -1;
    }
#ifdef COM
	printf(" . send \n");
#endif
	return 1;
}/*}}}*/


int read_known_host_file(char *pubkey, char *hname, char *filename)/*{{{*/
{
  FILE *fd;
  char c;
  char hostname[40];
  int first,second,i,j;
  int found=-1; 
  char temp_E[15];
  memset(pubkey,'\0', 300); 
  memset(temp_E, '\0', 15);
  second = i = j= 0;
  first = 1;
#ifdef COM
  printf("Known host file lookup for peer %s's public key ... \n", hname);
#endif

  if ( (fd = fopen (filename,"r"))== NULL ) {
    printf("fopen() error. %s:%d\n", __FILE__, __LINE__);
    return -1;
  }
  while(  (c=fgetc(fd)) != EOF ){
    if(c =='#' ){
      pubkey[j]='\0';
      first = 1; second = 0;
      i = j = 0;
      c = fgetc(fd);
      if(c == ' ')
        while(fgetc(fd)==' '); // to avoid any space
      if (found == 1)
        break;
    }
    else if( second ){
      //        pubkey[j++] = c;
    }
    else if(c == ' ' && first){
      first = 0; second = 1;
      hostname[i]='\0';
      if ( !strcmp(hostname, hname) ){
        found=1;
        for (i=0; i<260; i++) // reading N
          pubkey[j++]= fgetc(fd);
        pubkey[j]='\n';
        while(fgetc(fd)!='\n'); // to avoid any space 
        j++;	
        for (i=0; i<10; i++)    // reading E	    
          pubkey[j++]=fgetc(fd);
        pubkey[j]='\n';					
        //printf("%s's Pub key = '%s' size = %d \n",hostname,pubkey, strlen(pubkey));	
        break;
      }else{
        memset(hostname, '\0', 40);
        memset(pubkey, '\0', 300);
      }
    }
    else if(first && c!=' ' && c !='\r' && c!='\n'){
      hostname[i++] = c;
    }
  } // end while

  fclose(fd);
#ifdef COM
  printf("read_known_host_file(): Public key of connected peer : %s \n", pubkey);
#endif

  if(found == -1)
    pubkey=NULL;
  return found;
}/*}}}*/

int read_encrypted_file(char *enfile, char *string, unsigned char *passphase) /*{{{*/
{	
	char keyfile[] = "keyfile";
	int num=0;
	FILE *f;
	int result;
	struct stat stbuf;
#ifdef COM
	printf("Reading private key file \n");
#endif

	if( aes_en_de(1, enfile, keyfile, passphase, &num, 0) == -1){
		printf("aes_en_de() error. \n");		
		return -1;
	}
	// reading outfile
	if( ( f = fopen( keyfile, "rb+" ) ) == NULL ){
#ifdef COM
	   //fprintf(stderr, "fopen() error. %s:%d\n",__FILE__, __LINE__ );
	   printf( " failed ! Could not open to read %s \n", enfile);
#endif
       return -1;
    }
    stat(keyfile ,&stbuf );
    result = fread ( string, 1, (int)stbuf.st_size, f);
	fclose(f);
	if( remove(keyfile) )  printf("read_encrypted_file(): remove error");

#ifdef COM
	printf("Successfully read the key file \n");
#endif
	return 1;
}/*}}}*/

int initiate_migration_process(int new_fd, int *my_nonce, char *pubkey, char *privkey, unsigned char *aes_key) /*{{{*/
{

	char my_challengetext[80], rece_challengetext[80];
	char my_MD5_digest_hex[35], rece_MD5_digest_hex[35], solve_MD5_digest_hex[35];
	char rece_plaintext[135];
	char my_nonce_str[11], rece_nonce_str[11];
	char auth_status[5];  // +ve in case of successful otherwise -ve
	char rece_auth_status[5];
	char ciphertext[135];
	int  rece_nonce;

	int len_n, len_c;
	int numbytes, status = -1, i, j;
	char number[10];

	unsigned char digest[16];
        char ch,temp[5];

	char publickey[1024];
	char privatekey[1210];
	
	// initializing data structures
	memset(privatekey, '\0', 1210);		
	memset(publickey, '\0', 1024);	
	memset(my_challengetext, '\0', 80);   
	memset(rece_challengetext, '\0', 80);
	memset(my_MD5_digest_hex, '\0', 35);  
	memset(rece_MD5_digest_hex, '\0', 35);
	memset(solve_MD5_digest_hex, '\0', 35);
	memset(my_nonce_str, '\0', 11);		
	memset(rece_nonce_str, '\0', 11);
	memset(ciphertext, '\0', 135);	    
	memset(rece_plaintext, '\0', 135);
	memset(temp, '\0', 5);			
	memset(number, '\0', 10);
	memset(auth_status, '\0', 5);
	memset(rece_auth_status, '\0', 5);
	strcpy(publickey , pubkey);
	strcpy(privatekey, privkey);
	strcpy(auth_status, "-ve");

	///////////////////////////////////////////////////
	/////                 Phase 1		      /////
	/////	create challenge, generate nonce      /////
	/////	encrypt them and send to other peer   /////	
	///////////////////////////////////////////////////  
#ifdef COM		
	printf("\n ################################### \n");
	printf("Create challenge and nonce, encrypt them and sending other peer for authentication. \n");
	printf("Send at socket %d \n", new_fd);
#endif
	generate_AES_key(my_challengetext);			
	strcpy((char*)aes_key, my_challengetext);
	
#ifdef COM	
	printf("AES key = %s size %d \n",aes_key, strlen(aes_key) );
	printf(" . challenge text created \n");
#endif
	// get random number as nonce
	while ( (*my_nonce) < 1000000020 ){  //make sure that nonce is 10 digit  
		havege_state hs;
        	havege_init( &hs );
        	(*my_nonce) = havege_rand(&hs);
		// since nonce is incremented every time
		// a message is send make sure that it
		// does not exceed 10 digits 
		(*my_nonce) = (*my_nonce)-10;
	}

	sprintf(my_nonce_str, "%d", *my_nonce);
#ifdef COM
	printf(" . nonce generated \n");
#endif

	// appending nonce at the end of challengetext
	len_c =strlen(my_challengetext); 
	len_n = strlen(my_nonce_str);
	for(i=len_c, j=0 ; i < (len_c+len_n); i++, j++ )
		my_challengetext[i] = my_nonce_str[j];
		
	// Encrypting and sending challengetext
	if( rsa_encryption (publickey, my_challengetext, ciphertext) != 0){
		//fprintf(stderr, "rsa_encryption() warning. %s:%d\n",__FILE__, __LINE__ );
		printf(" . fail to encrypt challengetext \n");
		return -1;
	}
#ifdef COM
	printf(" . encrypted \n");
#endif
	if ((numbytes = send(new_fd , ciphertext, 135, 0)) == -1){
		// fprintf(stderr, "send() error. %s:%d\n",__FILE__, __LINE__ );
		perror("send \n");
		return -1;
	}
#ifdef COM
	 printf(" . send \n");
#endif

	// computing MD5 of challenge text to compare 
	// when received from other peer
	md5( (unsigned char *) my_challengetext, strlen(my_challengetext), digest );
	

	// converting digest from binary to hex
	for( i = 0,j=0; i < 16; i++,j=j+2 ){
		sprintf(temp, "%02x", digest[i]);
		my_MD5_digest_hex[j]   = temp[0];
		my_MD5_digest_hex[j+1] = temp[1];
	}	
	
	///////////////////////////////////////////////////
	/////		     Phase 2		      /////
	/////  receive solution of challenge that it  /////
	/////  send previously, challenge from other  /////
	/////	    peer and incremented nonce	      /////	
	///////////////////////////////////////////////////  
		
	memset(ciphertext, '\0', 135);
#ifdef COM
	printf(" Receive solution of challenge that it send previously challenge from otehr peer and incremented nonce \n");
#endif

#ifndef _WIN32
  numbytes = recvfrom(new_fd,
      (void *) ciphertext,
      (size_t) 135, /*sizeof(char)*SOCKET_INPUT_SIZE,*/
      0,
      (struct sockaddr *) 0,
       0);
       //printf("recev message %s\n", buffer);	
#else
  numbytes = recvfrom(new_fd,
      ciphertext,
      (size_t) 135, /*sizeof(char)*SOCKET_INPUT_SIZE,*/
      0,
      (struct sockaddr *) 0,
      0);
#endif
    if (numbytes < 0) {
	  printf("An Error occur while receiving data \n");
	  perror("recvfrom \n");
      return -1;
    } 
    else if (numbytes == 0) {
      printf("No message is available or peer close the connection \n");
	  return -1;
    }

	memset(rece_plaintext, '\0', 75);
	
	// Decrypting received ciphertext
	if( rsa_decryption( ciphertext, rece_plaintext, privatekey) != 0){
		printf(" . Error while decryption \n");
		return -2;
	}
#ifdef COM
	printf(" . received %s size = %d \n", rece_plaintext, strlen(rece_plaintext));
#endif
	// rece_plaintext is composed of (challenge, MD5 digest, nonce)	
	// separating rece_challenge from rece_plaintext 
	for(i=0; i<32; i++){
		rece_challengetext[i]=rece_plaintext[i];
	}

	// separating rece_MD5_digest_hex from rece_plaintext( MD5: 32-63)
	i=32; j=0;
	for(i=32; i<(32+32); i++,j++)
		rece_MD5_digest_hex[j] = rece_plaintext[i];	

	//separating rece_nonce_str from rece_plaintext( nonce: 64-'\0')
	i=64; j=0;
	ch=rece_plaintext[i];
	while(ch!='\0'){
		rece_nonce_str[j] = ch;
		i++; j++;
		ch=rece_plaintext[i];
	}
	rece_nonce = atoi(rece_nonce_str);
	
#ifdef COM
	// Verifying the solution of challenge(that I send to other peer)
	printf(" . rece_nonce = %d, my_nonce = %d \n", rece_nonce, (*my_nonce) );
#endif
	if( ( ++(*my_nonce) ) == rece_nonce)
	{
		if( !strcmp(rece_MD5_digest_hex, my_MD5_digest_hex) )
		{
			status = 1;
			memset(auth_status, '\0', 3);      
			strcpy(auth_status, "+ve");
#ifdef COM
			printf(" . received solution is verified \n");
			printf(" . MD5 digest matches \n");
#endif
		}
#ifdef COM
		else
			printf(" . MD5 digest doesn't matches \n");
		printf(" . nonce matches \n");
#endif
	}
#ifdef COM
	else
		printf(" . nonce doesn't match \n");
#endif
	
	// Solving the rece_challenge 
	// combine rece_challenge with nonce and calculate MD5 digest
	len_c = strlen(rece_challengetext);
        len_n = strlen(rece_nonce_str);
	for( i=len_c, j=0 ; i < (len_c+len_n); i++,j++ )
                rece_challengetext[i] = rece_nonce_str[j];
	
	// calculating MD5 digest of plaintext
	memset(digest, '\0', 16);
	md5( (unsigned char *) rece_challengetext, strlen(rece_challengetext) , digest);

	// converting digest from binary to hex
        for( i = 0,j=0; i < 16; i++,j=j+2 ){
                sprintf(temp, "%02x", digest[i]);
                solve_MD5_digest_hex[j]   = temp[0];
                solve_MD5_digest_hex[j+1] = temp[1];
        }
#ifdef COM	
	printf(" . challenged solved \n");
#endif
	// Combine Auth_status, solve_MD5_digest_hex, rece_nonce++
	// increment rece_nonce
	(*my_nonce)++;
	memset(my_nonce_str, '\0', 11);
	sprintf(my_nonce_str, "%d", *my_nonce);
	// concatinating ....
	memset(rece_challengetext, '\0', 50);
	strcat(rece_challengetext, auth_status);
	strcat(rece_challengetext, solve_MD5_digest_hex);
	strcat(rece_challengetext, my_nonce_str);

	// encrypt and send to other peer
	memset(ciphertext, '\0', 135);
	if( rsa_encryption (publickey, rece_challengetext, ciphertext) != 0){
		printf(" . fail to encrypt challengetext \n");
		return -1;
	}
#ifdef COM
	printf(" . encrypted \n");
#endif
	if (send(new_fd , ciphertext, 135, 0) == -1){
          perror("send \n");
          return -1;
        }
	if(status != 1 ){ 
		return -1; // this means that nonce or MD 5 digest doesnot matches
	}
#ifdef COM
	printf(" . send \n");
#endif

	///////////////////////////////////////////////////
	/////                 Phase 3		      /////
	/////	      receive auth_status and         ///// 
	/////		  incremented nonce	      /////         
	///////////////////////////////////////////////////	
	
	// receive auth_status for the challenge it solved 

	memset(ciphertext, '\0', 135);
#ifdef COM
        printf("Receiving auth_status and incremented nonce from peer \n");
#endif

#ifndef _WIN32
  numbytes = recvfrom(new_fd,
      (void *) ciphertext,
      (size_t) 135, /*sizeof(char)*SOCKET_INPUT_SIZE,*/
      0,
      (struct sockaddr *) 0,
       0);
      //printf("recev message %s\n", buffer);	
#else
  numbytes = recvfrom(new_fd,
      ciphertext,
      (size_t) 135, /*sizeof(char)*SOCKET_INPUT_SIZE,*/
      0,
      (struct sockaddr *) 0,
      0);
#endif
     if (numbytes < 0) {
	  printf("An Error occur while receiving data \n");
	  perror("recvfrom \n");
           return -1;
     } 
     else if (numbytes == 0) {
          printf("No message is available or peer close the connection \n");
	  return -1;
    }


#ifdef COM
	printf(" . received \n");
#endif
	memset(rece_plaintext, '\0', 130);
	// Decrypting received ciphertext
	if(rsa_decryption( ciphertext, rece_plaintext, privatekey)!= 0){
            printf(" . Error while decryption \n");
            return -2;
        }	
#ifdef COM
	printf(" . decrypted \n");
#endif

	for(i=0; i<3; i++)
		rece_auth_status[i]=rece_plaintext[i];
	rece_auth_status[i]='\0';

	//separating rece_nonce_str from rece_plaintext( nonce: 3 -'\0')
	memset(rece_nonce_str, '\0', 11);
        i=3; j=0;
        ch=rece_plaintext[i];
        while(ch != '\0'){
                rece_nonce_str[j] = ch;
                i++; j++;
                ch=rece_plaintext[i];
        }
        rece_nonce = atoi(rece_nonce_str);
	if( (++(*my_nonce) ) != rece_nonce){
		printf(" warning: Nonce doesn't matches %s:%d\n", __FILE__, __LINE__);
		return -1;			
	}
#ifdef COM
	printf("rece_auth_status = %s \n", rece_auth_status);
#endif

	if( !strcmp(rece_auth_status, "+ve") ){
#ifdef COM
		printf("Client: Authentication Process is successfull. \n"); 
    	printf("AES key is exchanged. \n");
#endif
		return 1;
	}
	else{
		return -2;
	}
}/*}}}*/ 

int reply_migration_process(int sockfd, int *my_nonce, char *pubkey, char *privkey, unsigned char *aes_key) /*{{{*/
{
	char my_challengetext[80], rece_challengetext[50];
        char my_MD5_digest_hex[35], rece_MD5_digest_hex[35], solve_MD5_digest_hex[35];
        char rece_plaintext[135];
        char my_nonce_str[15], rece_nonce_str[15];
	char temp[75], ch;
        char auth_status[]="-ve";  // +ve in case of successful otherwise -ve
	char rece_auth_status[5];
        char ciphertext[135];
        int rece_nonce;

        int len_n, len_c;
        int numbytes, status = -1, i, j; //num;
        char number[15];

        unsigned char digest[16];

	char publickey[1024];	
	char privatekey[1210];
	

	// initializing data structures
	memset(privatekey, '\0', 1210);        memset(publickey, '\0', 1024);  
        memset(temp, '\0', 75 );
        memset(my_challengetext, '\0', 80);    memset(rece_challengetext, '\0', 50);
        memset(my_MD5_digest_hex, '\0', 35);   memset(rece_MD5_digest_hex, '\0', 35);
        memset(solve_MD5_digest_hex, '\0', 35);
        memset(my_nonce_str, '\0', 15);        memset(rece_nonce_str, '\0', 15);
        memset(ciphertext, '\0', 135);
        memset(number, '\0', 15);
	memset(ciphertext, '\0', 135);
	memset(rece_plaintext, '\0', 135);

	strcpy(publickey , pubkey );
	strcpy(privatekey, privkey);

	///////////////////////////////////////////////////
	/////                 Phase 1                 /////
	/////  Wait to receive challenge and nonce    /////
	/////             from other peer	      /////
	///////////////////////////////////////////////////  
	
        // Receiving challgenge from server
#ifdef COM
	printf("\n ################################### \n");
	printf("Authenticating connected peer ");
	printf("Wait to receiving challenge from peer \n");
#endif

#ifndef _WIN32
  numbytes = recvfrom(sockfd,
      (void *) ciphertext,
      (size_t) 135, /*sizeof(char)*SOCKET_INPUT_SIZE,*/
      0,
      (struct sockaddr *) 0,
      0);
      //printf("recev message %s\n", buffer);	
#else
  numbytes = recvfrom(sockfd,
      ciphertext,
      (size_t) 135, /*sizeof(char)*SOCKET_INPUT_SIZE,*/
      0,
      (struct sockaddr *) 0,
      0);
#endif
    if (numbytes < 0) {
	  printf("An Error occur while receiving data \n");
	  perror("recvfrom \n");
      return -1;
    } 
    else if (numbytes == 0) {
      printf("No message is available or peer close the connection \n");
	  return -1;
    }


#ifdef COM
        printf(" . challenge received %d bytes \n", numbytes);
        printf("Solving the challenge and send back to peer \n");
#endif

    if ( rsa_decryption( ciphertext, rece_plaintext, privatekey) != 0){
		printf(" . fail to decrypt\n");
		return -1;
	}
#ifdef COM
	printf(" . decrypted \n");
	printf("rece_challenge = %s \n",rece_plaintext);		 
#endif
        // rece_plaintext is composed of (challenge , nonce)
        // separating rece_challenge from rece_plaintext( 0 - 31 )
	for(i=0; i<32; i++)
		rece_challengetext[i] = rece_plaintext[i];
	strcpy((char*)aes_key, rece_challengetext);
#ifdef COM
	printf("AES key = %s size %d \n",aes_key, strlen(aes_key) );
#endif
	// separate nonce (32 - '\0')
	i=32; j=0;	
        ch=rece_plaintext[i];
        while(ch!='\0'){
                rece_nonce_str[j] = ch;
                i++; j++;
                ch = rece_plaintext[i];
        }
	
	// convert nonce into integer
        rece_nonce = atoi(rece_nonce_str);  

	(*my_nonce) = rece_nonce;
	(*my_nonce)++; 
#ifdef COM
	printf(" . rece_nonce = %d my_nonce = %d", rece_nonce, (*my_nonce));
#endif
        // convert nonce into string 
	sprintf(my_nonce_str, "%d", *my_nonce);
	//printf(" . my_nonce_str = %s", my_nonce_str);
      
	// calculating MD5 digest of plaintext
        md5( (unsigned char *) rece_plaintext, strlen(rece_plaintext), digest );
        //printf(" . MD5 digest calculated \n");
  
       // converting digest from binary to hex
        for( i = 0,j=0; i < 16; i++,j=j+2 ){
                sprintf(temp, "%02x", digest[i]);
                solve_MD5_digest_hex[j]   = temp[0];
                solve_MD5_digest_hex[j+1] = temp[1];
        }
#ifdef COM
	printf(" . challenge solved \n" );
	printf(" . Creating challenge text for other peer \n");
#endif

	generate_AES_key(my_challengetext);
#ifdef COM
	printf(" . challenge text created \n");
#endif
	
	// Computing MD5 of challenge text + nonce to compare 
	// when received from other peer 
	strcpy(temp,my_challengetext);
	len_c = strlen(temp);
	len_n = strlen(my_nonce_str);
	for(i=len_c, j=0; i < (len_c+len_n); i++, j++)
		temp[i] = my_nonce_str[j]; 
	
	md5( (unsigned char *) temp, strlen(temp), digest );

        // converting digest from binary to hex
        for( i = 0,j=0; i < 16; i++,j=j+2 ){
                sprintf(temp, "%02x", digest[i]);
                my_MD5_digest_hex[j]   = temp[0];
                my_MD5_digest_hex[j+1] = temp[1];
        }
	
	// Concatinating my_challengetext, solve_MD5_digest_hex, my_nonce
	strcat(my_challengetext, solve_MD5_digest_hex);
	strcat(my_challengetext, my_nonce_str);
	
#ifdef COM
	printf(" . send = %s size = %d \n", my_challengetext,strlen(my_challengetext));
#endif
	// encrypt and send
	memset(ciphertext, '\0', 135);
	if( rsa_encryption(publickey, my_challengetext, ciphertext ) != 0){
            printf(" . fail to encrypt \n");
            return -1;
        }
#ifdef COM
	printf(" . encrypted \n");
#endif
      // sending to peer 
        if ( (numbytes = send(sockfd , ciphertext, 135, 0)) == -1){
                perror("send \n");
                return -1;
        }
#ifdef COM
	printf(" . send \n");
#endif

	///////////////////////////////////////////////////
	/////                 Phase 2                 /////
	/////    Receive Auth_status, solution of     /////
	/////    challenge it send previously and     /////
        /////             incremented nonce           ///// 
	///////////////////////////////////////////////////  


	memset(ciphertext, '\0', 135);
#ifdef COM
        printf("Receiving Auth_status, soultion to challenge it send previously and incremented nonce from peer \n");
#endif

#ifndef _WIN32
  numbytes = recvfrom(sockfd,
      (void *) ciphertext,
      (size_t) 135, /*sizeof(char)*SOCKET_INPUT_SIZE,*/
      0,
      (struct sockaddr *) 0,
      0);
#else
  numbytes = recvfrom(sockfd,
      ciphertext,
      (size_t) 135, /*sizeof(char)*SOCKET_INPUT_SIZE,*/
      0,
      (struct sockaddr *) 0,
      0);
#endif
    if (numbytes < 0) {
	  printf("An Error occur while receiving data \n");
	  perror("recvfrom \n");
      return -1;
    } 
    else if (numbytes == 0) {
      printf("No message is available or peer close the connection \n");
	  return -1;
    }

#ifdef COM
        printf(" . received %d bytes\n", numbytes);
#endif
	memset(rece_plaintext, '\0', 75);
        
        // Decrypting received ciphertext
        if(rsa_decryption( ciphertext, rece_plaintext, privatekey)!= 0){
	    printf("rsa_decryption() warning. \n");
            return -2;
        }
	
	// rece_plaintext is composed of (auth_status, MD5 digest, nonce)
	// separating auth_status from rece_plaintext (0-2)
        for(i=0; i<3; i++)
                rece_auth_status[i]=rece_plaintext[i];
	if( !strcmp(rece_auth_status,"-ve") )
		return -2;
	
	//printf("rece_auth_status = %s \n",rece_auth_status);
	// separate MD5 digest from rece_plaintext (3 - 30)
	memset(rece_MD5_digest_hex, '\0', 35);
	j=0;
	for(i=3; i<35; i++, j++)
                rece_MD5_digest_hex[j] = rece_plaintext[i];
#ifdef COM
        printf(" rece_MD5_digest_hex = %s \n", rece_MD5_digest_hex);
	printf(" my_MD5_digest_hex   = %s \n", my_MD5_digest_hex);
#endif
	// separate nonce from rece_plaintext (35 - \0)
	ch = rece_plaintext[i];
	j=0;
        memset(rece_nonce_str, '\0', 15);
	while(ch != '\0'){
		rece_nonce_str[j] = ch;
		i++; j++;
		ch = rece_plaintext[i];
	}
	
	rece_nonce = atoi(rece_nonce_str);

	// verifying the challenge
        if( (++(*my_nonce)) == rece_nonce)
        {
                if( !strcmp(rece_MD5_digest_hex, my_MD5_digest_hex) )
                {
                        status = 1;
                        memset(auth_status, '\0', 3);
                        strcpy(auth_status, "+ve");
#ifdef COM
			printf(" . received solution is verified \n");
                        printf(" . MD5 digest matches \n");
#endif
                }
#ifdef COM
		else
                        printf(" . MD5 digest doesn't matches \n");
                printf(" . nonce matches \n");
#endif
        }
#ifdef COM
        else
                printf(" . nonce doesn't match \n");
#endif
	
	// concatinating my_nonce, auth_status
	(*my_nonce)++;
        memset(my_nonce_str, '\0', 15); 
	sprintf(my_nonce_str, "%d", (*my_nonce) );
	memset(rece_challengetext, '\0', 50);
	strcat(rece_challengetext, auth_status);
	strcat(rece_challengetext, my_nonce_str);
#ifdef COM
	printf("auth_status = %s \n", auth_status);
	printf(" . Sending Auth_status \n");
#endif
	// Encrypting challengetext
	memset(ciphertext, '\0', 135);
        if( rsa_encryption (publickey, rece_challengetext, ciphertext) != 0){
                printf(" . fail to encrypt challengetext \n");
                return -1;
        }
#ifdef COM
	printf(" . encrypted \n");
#endif
        if ((numbytes = send(sockfd , ciphertext, 135, 0)) == -1){
            perror("send \n");
            return -1;
        }
#ifdef COM
        printf(" . send \n");
#endif
	
	if ( !strcmp(auth_status, "+ve") ){
#ifdef COM
		printf("Server: Authentication Process is successfull \n");
		printf(" AES key is received successfully \n");
#endif
        return 1;	
	}
    else
       return -1;    
} /*}}}*/

void generate_AES_key(char *key) /*{{{*/
{
        havege_state hs;
//        time_t t;
        int num,i;
        char number[10];
        memset(number, '\0', 10);

        havege_init( &hs );
        for (i=0; i<4; i++){
                num = havege_rand(&hs);
                if(num < 0)
                        num=num*(-1);
                sprintf(number, "%X",num);
                if(strlen(number) == 8)
                        strcat(key, number);
                else
                        i--;
                memset(number, '\0', 10);
        }
} /*}}}*/


int aes_en_de(int mode, char *infile, char *outfile, unsigned char *AES_key, int *nonce, int new_fd)/*{{{*/
{
    int ret = -1, i, n;
    int keylen, lastn;
	FILE *fin, *fout;
    int flag= -1;  	
    char *p;
    unsigned char IV[16];
    unsigned char key[512];
    unsigned char digest[35];
    unsigned char buffer[1024];
	struct stat stbuf;

    aes_context aes_ctx;
    sha2_context sha_ctx;
    
    off_t filesize, offset;

    if( mode == MODE_ENCRYPT  && (*nonce) != 0 ) {
       // append nonce to MA file
       if( append_nonce_to_MA( nonce, infile ) == -1){
          printf("Unable to write nonce to MA file\n");
          //goto exit;
		  return -1;
        }else
		flag=1;
    }

	if( mode != MODE_ENCRYPT && mode != MODE_DECRYPT )
    {
        printf("invalid operation mode \n" );
        //goto exit;
		return -1;
    }

    if( strcmp( infile, outfile ) == 0 )
    {
        printf("input and output filenames must differ\n" );
        //goto exit;
		return -1;
    }

   if( ( fin = fopen( infile, "rb" ) ) == NULL )
    {
        printf("fopen(%s,rb) failed\n", infile );
        return -1;
    }

    if( ( fout = fopen( outfile, "wb+" ) ) == NULL )
    {
        printf("fopen(%s,wb+) failed\n", outfile );
		fclose(fin);
        return -1;
    }

            keylen = strlen( (const char*)AES_key );
            
            if( keylen > (int) sizeof( key ) )
                keylen = (int) sizeof( key );
                
            memcpy( key, AES_key, keylen );
   
    //printf("key = %s \n",key);

    stat(infile ,&stbuf );
    filesize = (int)stbuf.st_size;


    if( mode == MODE_ENCRYPT )
    {
        // Generate the initialization vector as:
        // IV = SHA-256( filesize || filename )[0..15]
     
        for( i = 0; i < 8; i++ )
            buffer[i] = (unsigned char)( filesize >> ( i << 3 ) );

        p = infile;

        sha2_starts( &sha_ctx, 0 );
        sha2_update( &sha_ctx, buffer, 8 );
        sha2_update( &sha_ctx, (unsigned char *) p, strlen( p ) );
        sha2_finish( &sha_ctx, digest );

        memcpy( IV, digest, 16 );

        
	// The last four bits in the IV are actually used
	// to store the file size modulo the AES block size.

        lastn = (int)( filesize & 0x0F );

        IV[15] = (unsigned char)
            ( ( IV[15] & 0xF0 ) | lastn );

        
	// Append the IV at the beginning of the output.

        if( fwrite( IV, 1, 16, fout ) != 16 )
        {
            printf("fwrite() failed\n" );
	    fclose(fin); fclose(fout);
	    return -1;
        }

        
	// Hash the IV and the secret key together 8192 times
	// using the result to setup the AES context and HMAC.

        memset( digest, 0,  32 );
	memcpy( digest, IV, 16 );

        for( i = 0; i < 8192; i++ )
        {
            sha2_starts( &sha_ctx, 0 );
            sha2_update( &sha_ctx, digest, 32 );
            sha2_update( &sha_ctx, key, keylen );
            sha2_finish( &sha_ctx, digest );
        }

        memset( key, 0, sizeof( key ) );
        //printf("\n AES: digest = %s, size = %d \n",digest,strlen(digest));
        aes_setkey_enc( &aes_ctx, digest, 256 );
        sha2_hmac_starts( &sha_ctx, digest, 32, 0 );

        
	// Encrypt and write the ciphertext.
	
        for( offset = 0; offset < filesize; offset += 16 )
        {
            n = ( filesize - offset > 16 ) ? 16 : (int)
                ( filesize - offset );

            if( fread( buffer, 1, n, fin ) != (size_t) n )
            {
                printf("fread() failed\n");
                fclose(fin); fclose(fout);
				return -1;
            }

            for( i = 0; i < 16; i++ )
                buffer[i] = (unsigned char)( buffer[i] ^ IV[i] );

            aes_crypt_ecb( &aes_ctx, AES_ENCRYPT, buffer, buffer );
            sha2_hmac_update( &sha_ctx, buffer, 16 );

            if( fwrite( buffer, 1, 16, fout ) != 16 )
            {
                printf("fwrite() failed\n");
                fclose(fin); fclose(fout);
				return -1;
            }

            memcpy( IV, buffer, 16 );
        }

        
        // Finally write the HMAC.

        sha2_hmac_finish( &sha_ctx, digest );

        if( fwrite( digest, 1, 32, fout ) != 32 )
        {
            printf("fwrite() failed\n");
            fclose(fin); fclose(fout);
			return -1;
        }
    }

	// MODE_DECRYPT
    if( mode == MODE_DECRYPT )
    {
        unsigned char tmp[16];

        //  The encrypted file must be structured as follows:
	//  00 .. 15              Initialization Vector
	//  16 .. 31              AES Encrypted Block #1
	//	 ..
	// N*16 .. (N+1)*16 - 1    AES Encrypted Block #N
	// (N+1)*16 .. (N+1)*16 + 32   HMAC-SHA-256(ciphertext)
                                                                
        if( filesize < 48 )
        {
	    printf( "File too short to be encrypted.\n" );
            fclose(fin); fclose(fout);
	    return -1;
        }

        if( ( filesize & 0x0F ) != 0 )
        {
	    printf("File size not a multiple of 16.\n ");
            fclose(fin); fclose(fout);
	    return -1;
        }

        
	// Substract the IV + HMAC length.
 
        filesize -= ( 16 + 32 );
	
	// Read the IV and original filesize modulo 16.
        if( fread( buffer, 1, 16, fin ) != 16 )
        {
	    printf( "fread() failed\n");
            fclose(fin); fclose(fout);
	    return -1;
        }

        memcpy( IV, buffer, 16 );
        lastn = IV[15] & 0x0F;

        
	// Hash the IV and the secret key together 8192 times
        // using the result to setup the AES context and HMAC.

        memset( digest, 0,  32 );
        memcpy( digest, IV, 16 );

        for( i = 0; i < 8192; i++ )
        {
            sha2_starts( &sha_ctx, 0 );
            sha2_update( &sha_ctx, digest, 32 );
            sha2_update( &sha_ctx, key, keylen );
            sha2_finish( &sha_ctx, digest );
        }

        memset( key, 0, sizeof( key ) );
        aes_setkey_dec( &aes_ctx, digest, 256 );
        sha2_hmac_starts( &sha_ctx, digest, 32, 0 );

        
        // Decrypt and write the plaintext.

        for( offset = 0; offset < filesize; offset += 16 )
        {
            if( fread( buffer, 1, 16, fin ) != 16 )
            {
		printf(" fread() failed \n ");
                fclose(fin); fclose(fout);
		return -1;
            }

            memcpy( tmp, buffer, 16 );

            sha2_hmac_update( &sha_ctx, buffer, 16 );
            aes_crypt_ecb( &aes_ctx, AES_DECRYPT, buffer, buffer );

            for( i = 0; i < 16; i++ )
                buffer[i] = (unsigned char)( buffer[i] ^ IV[i] );

            memcpy( IV, tmp, 16 );

            n = ( lastn > 0 && offset == filesize - 16 )
                ? lastn : 16;
	 if( fwrite( buffer, 1, n, fout ) != (size_t) n )
            {
		printf(" fwrite() failed \n ");
                fclose(fin); fclose(fout);
		return -1;
            }
        }
       
        // Verify the message authentication code.
 
        sha2_hmac_finish( &sha_ctx, digest );

        if( fread( buffer, 1, 32, fin ) != 32 )
        {
	    printf(" fread() failed\n");
            fclose(fin); fclose(fout);
	    return -1;
        }
#ifdef COM
	printf("verify the message authentication code MAC \n");
#endif
        if( memcmp( digest, buffer, 32 ) != 0 )
        {
	    printf(" HMAC check failed: wrong key, or file corrupted.\n ");
            fclose(fin); fclose(fout);
	    return -1;
        }
#ifdef COM
		printf("MAC done ... \n");
#endif
    }
	fclose(fin);	
	fclose(fout);
	ret = 1;
	
	if( (*nonce)!=0 &&  mode == MODE_DECRYPT){
	   if ( extract_nonce_from_MA(new_fd, nonce, outfile) != 1){
                printf("Unable to extract nonce from MA file");
                return -1;
           }
	}
    return 1;
} /*}}}*/

int send_AES_en_MA(int sockfd, int *my_nonce ,char *outfile, char *pubkey) /*{{{*/
{
    FILE *fd;
    struct stat stbuf;
    char *MA_buffer; // *buffer;
    int numbytes;
    char data[20];
    char size_str[5];
    int size;
    char ciphertext[135];
    char publickey[1024];	

#ifdef COM
	printf("Send AES encrypted MA \n");
#endif

	memset(publickey, '\0', 1024);
	memset(data, '\0', 20);
	memset(ciphertext, '\0', 135);

	strcpy(publickey, pubkey);
	// open encrypted MA file to read
    if ( (fd = fopen(outfile, "rb")) == NULL ){
       printf("Error while opening MA file");
       return -1;
    }
    // obtain file size:
    stat(outfile ,&stbuf );

        // allocate memory to copy the whole file
	MA_buffer = (char*) malloc (sizeof(char)*(int)stbuf.st_size);
	if (MA_buffer == NULL){
                printf("Memory error: Cann't allocate memory for MA \n");
                return -1;
        }

     // copy whole file into buffer:
     numbytes = fread ( MA_buffer, 1, (int)stbuf.st_size, fd);
     if (numbytes != (int)stbuf.st_size){  //fsize) {
        printf("Error: Reading MA file");
		free(MA_buffer);
        return -1;
     }
     fclose(fd);

	// sends a complete encrypted mobile agent
	// prepare to send size of encrypted message
#ifdef COM
	    printf("Prepare to send size of encrypted message \n");
#endif

	size = (int)stbuf.st_size;

    (*my_nonce)--;
	sprintf(data, "%d", *my_nonce );
	sprintf(size_str, "%d", size);
	strcat(data, size_str);
#ifdef COM
    printf("Size = %d \n", size);
    printf("my_nonce = %d \n", *my_nonce);
	printf("data to send = %s \n", data);
#endif
	if( rsa_encryption (publickey, data, ciphertext) != 0){
		printf(" . fail to encrypt challengetext \n");
		return -1;
	}
	
    if ((numbytes = send(sockfd , ciphertext, 135, 0)) == -1){	
		printf("Error while sending size of encryptee message to receiver \n");
		return -1;
	}

#ifdef COM
		printf("Send. \n");
	    printf("Size of encrypted msg is send. \n");
		printf("Now Sending Encrypted msg. \n");
#endif	

	// sending encrypted message
	if ( (numbytes = send(sockfd , MA_buffer, (int)stbuf.st_size, 0)) == -1 ) {	
	//if (numbytes = send(sockfd , "University of California, Davis", 35, 0) == -1){
        perror("send");
#ifdef COM
	    printf("Cannot send encrypted MA \n");
#endif
		return -1;
    }
	free(MA_buffer);
#ifdef COM
    printf(" . encrypted message is send \n");
	printf(" . waiting for acknowlegement \n");
#endif

     // wait to receive acknowledgement
     memset(data, '\0', 20);
	
#ifndef _WIN32
  numbytes = recvfrom(sockfd,
      (void *) data,
      (size_t) 20,
      0,
      (struct sockaddr *) 0,
      0);

#else
  numbytes = recvfrom(sockfd,
      data,
      (size_t) 20,
      0,
      (struct sockaddr *) 0,
      0);
#endif

    if (numbytes < 0) {
	  printf("An Error occur while receiving data \n");
	  perror("recvfrom \n");
      return -1;
    } 
    else if (numbytes == 0) {
      printf("No message is available or peer close the connection \n");
	  return -1;
    }
#ifdef COM
    printf(" . acknowledgment received = %s \n", data);
#endif
    if(!strcmp(data, "-ve" ) ){
       printf("Error: receiver cannot get MA successfuly \n");
	   //free(MA_buffer);				
       return -2;
    }	
#ifdef COM	
	printf("------------------------------ \n");
#endif
	return 1;
} /*}}}*/

int receive_AES_en_MA(int new_fd, int *nonce, char *infile, char *privkey) /*{{{*/
{
	FILE *fd;
	int numbytes;
	//char buffer[135];
	char *buffer;
	char plaintext [135];	
    int i,j;
	int rece_nonce;
	char nonce_str[15];
	int size;
	char size_str[5], ch;
	char privatekey[1210];

	memset(plaintext, '\0', 135 );
        memset(nonce_str, '\0', 15);
	strcpy(privatekey, privkey );

 #ifdef COM
	printf("Receiving Size of encrypted Message and then message \n ");	
	printf("Receiving size ... \n");
 #endif
 
 buffer = (char*) malloc(sizeof(char)*135);
 if (buffer == NULL){
    printf("Memory error: Cann't allocate memory for MA \n");
    return -1;
 }
 memset(buffer, '\0', 135);	
  
 #ifndef _WIN32
  numbytes = recvfrom(new_fd,
      (void *) buffer,
      (size_t) 135, 
      0,
      (struct sockaddr *) 0,
       0);
 #else
  numbytes = recvfrom(new_fd,
      buffer,
      (size_t) 135, 
      0,
      (struct sockaddr *) 0,
      0);      
 #endif

  if (numbytes < 0) {
	  printf("An Error occur while receiving data \n");
	  perror("recvfrom \n");
      return -1;
    } 
  else if (numbytes == 0) {
      printf("No message is available or peer close the connection \n");
	  return -1;
  }
#ifdef COM
  printf(" decrypting receive msg (Size of En. Msg.) \n"); 
#endif

  if ( rsa_decryption( buffer, plaintext, privatekey) != 0){		
		printf(" . fail to decrypt\n");
		return -1;
  }  

  free(buffer);
  for(i=0; i<10; i++)
	  nonce_str[i]=plaintext[i];
  j=0;
  ch = plaintext[i];
  while(ch != '\0'){
	size_str[j++] = ch;
	ch = plaintext[++i];
  }
  size = atoi(size_str);
  rece_nonce = atoi(nonce_str);

  (*nonce)++;
  if((*nonce) != rece_nonce ){
	  printf("Nonce Doesnot matches \n");
	  return -1;	
  }
  // Receiving Encrypted Maessage
  buffer = (char*) malloc (sizeof(char)*size);
  if (buffer == NULL){
       printf("Memory error: Cann't allocate memory for MA \n");
       return -1;
  }  

  #ifdef COM
  printf("size = %d \n", size);
  printf("nonce_rece = %d \n", rece_nonce);
  printf("Receiving encrypted message ... \n");
  #endif  

#ifndef _WIN32
  numbytes = recvfrom(new_fd,
      (void *) buffer,
      (size_t) size, 
      0,
      (struct sockaddr *) 0,
      0);
#else
  numbytes = recvfrom(new_fd,
      buffer,
      (size_t) size,
      0,
      (struct sockaddr *) 0,
      0);         
#endif

  if (numbytes < 0) {
	  printf("An Error occur while receiving data \n");
	  perror("recvfrom \n");
      return -1;
    } 
  else if (numbytes == 0) {
      printf("No message is available or peer close the connection \n");
	  return -1;
  }
#ifdef COM
  printf(" . received %d bytes \n", numbytes);
#endif

  if( (fd = fopen(infile,"wb") ) == NULL ){
     printf("Error: while writing MA content in a file");
     return -1;
  }
  fwrite (buffer , 1 , numbytes , fd );
  free(buffer);  
  fclose(fd);  
  return 1;
} /*}}}*/


/////////////////////////////////////////////////////////////////////
/////                   Utility functions                       /////
///////////////////////////////////////////////////////////////////// 

int generate_RSA_keys_plaintext(char *pubkeyfile, char *privkeyfile) /*{{{*/
{
	
	FILE *fpub, *fpriv;
	rsa_context rsa;
	havege_state hs;
	int ret;
	
	printf( "\nSeeding the random number generator \n" );
	fflush( stdout );
	havege_init( &hs );

	printf( "Generating the RSA key [ %d-bit ] \n", KEY_SIZE );
	rsa_init( &rsa, RSA_PKCS_V15, 0, havege_rand, &hs );
	if( ( ret = rsa_gen_key( &rsa, KEY_SIZE, EXPONENT ) ) != 0 )
	{
		printf( " failed, rsa_gen_key returned %d\n", ret );
		return -1;
	}

	printf( "Exporting public key in %s \n", pubkeyfile );
	fflush( stdout );
	if( ( fpub = fopen( pubkeyfile, "wb+" ) ) == NULL )
	{
		printf( " failed, could not open %s for writing\n",pubkeyfile);
		return -1;
	}

	if( ( ret = mpi_write_file( "N = ", &rsa.N, 16, fpub ) ) != 0 ||
            ( ret = mpi_write_file( "E = ", &rsa.E, 16, fpub ) ) != 0 )
	{
		printf( " failed, mpi_write_file returned %d\n", ret );
		return -1;
	}

	printf( "Exporting the private key in %s \n", privkeyfile );
	fflush( stdout );

	if( ( fpriv = fopen( privkeyfile, "wb+" ) ) == NULL )
	{
		printf( " failed, could not open %s for writing\n", privkeyfile);
	}

	if( ( ret = mpi_write_file( "N = " , &rsa.N , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "E = " , &rsa.E , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "D = " , &rsa.D , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "P = " , &rsa.P , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "Q = " , &rsa.Q , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "DP = ", &rsa.DP, 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "DQ = ", &rsa.DQ, 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "QP = ", &rsa.QP, 16, fpriv ) ) != 0 )
	    { 
		printf( " failed, mpi_write_file returned %d\n", ret );
		return -1;
	    }	
	printf(" done. \n");	
	return 1;
} /*}}}*/

int generate_RSA_keys_ciphertext(char *pubkeyfile, char *privkeyfile, unsigned char *passphrase) /*{{{*/
{
	
	FILE *fpub, *fpriv;
	rsa_context rsa;
        havege_state hs;
	int ret;
	int num=0;	

        printf( "\nSeeding the random number generator \n" );
        fflush( stdout );
        havege_init( &hs );

        printf( "Generating the RSA key [ %d-bit ] \n", KEY_SIZE );
        rsa_init( &rsa, RSA_PKCS_V15, 0, havege_rand, &hs );
        if( ( ret = rsa_gen_key( &rsa, KEY_SIZE, EXPONENT ) ) != 0 )
        {
                printf( " failed, rsa_gen_key returned %d\n", ret );
                return -1;
        }

        printf( "Exporting public key in %s \n", pubkeyfile );
        fflush( stdout );
        if( ( fpub = fopen( pubkeyfile, "wb+" ) ) == NULL )
        {
                printf( " failed, could not open %s for writing\n",pubkeyfile);
                return -1;
        }

        if( ( ret = mpi_write_file( "N = ", &rsa.N, 16, fpub ) ) != 0 ||
            ( ret = mpi_write_file( "E = ", &rsa.E, 16, fpub ) ) != 0 )
        {
                printf( " failed, mpi_write_file returned %d\n", ret );
                return -1;
        }
	fclose(fpub);
        printf( "Exporting the private key in %s \n", privkeyfile );
        fflush( stdout );

        if( ( fpriv = fopen( "temp_priv", "wb+" ) ) == NULL )
        {
                printf( "failed, could not open temp_priv for writing\n");
		return -1;
        }

        if( ( ret = mpi_write_file( "N = " , &rsa.N , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "E = " , &rsa.E , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "D = " , &rsa.D , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "P = " , &rsa.P , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "Q = " , &rsa.Q , 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "DP = ", &rsa.DP, 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "DQ = ", &rsa.DQ, 16, fpriv ) ) != 0 ||
            ( ret = mpi_write_file( "QP = ", &rsa.QP, 16, fpriv ) ) != 0 )
        {
                printf( " failed, mpi_write_file returned %d\n", ret );
                return -1;
        }
	fclose(fpriv);
	
	if( aes_en_de(0, "temp_priv" ,privkeyfile, passphrase, &num, 0) == -1){
		printf("failed to encrypt private key file \n");
		if( remove("temp_priv") )  printf("interface.c : remove error 1");	 	
		return -1;
	}
	else 
		printf("encrypted. \ndone. \n");
	if( remove("temp_priv") )  printf("interface.c : remove error 2");	
    return 1;
} /*}}}*/
