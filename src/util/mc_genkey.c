/*
 *  This program generates RSA keys public anf private keys.
 *  It can generate the private key that is encrypted with 
 *  AES. For this you need to enter the passphrase that will  	 
 *  be used to encrypt the priavte key.
 */


#include "../security/interface.h" 

void print_usage(){
        printf("Usage:  ./mc_genkey -rsakeys -pt  (for plain text private key)\n\t./mc_genkey -rsakeys -en (for encrypted private key) \n");
        printf("Press any key to exit ... \t");
        getchar();
	exit(0);
}


int main(int argc, char* argv[])
{
  unsigned char passphrase[512];
  
  if(argc < 3){
    print_usage();
  }

  if( !strcmp (argv[1], "-rsakeys" )){
    if( !strcmp (argv[2],"-en") ){
        printf("Enter Passphrase (A-Z, a-z, 0-9)to encrypt privatekey file\n");
	printf("(remember your passphrase otherwise encrypted private key file is useless)\n");
	scanf("%s", &passphrase);
	generate_RSA_keys_ciphertext("rsa_pub", "rsa_priv", passphrase);
    }
    else{ 
	if (!strcmp (argv[2],"-pt") )
	   generate_RSA_keys_plaintext("rsa_pub", "rsa_priv");
	else
	   printf("specify correctly whether to encrypt[-en] private key or keep it in plaintext[-pt] \n");		
    }
  }
  else
    print_usage();
  
  return 1;
}
