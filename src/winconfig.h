/* SVN FILE INFO
 * $Revision: 543 $ : Last Committed Revision
 * $Date: 2010-06-23 11:20:13 -0700 (Wed, 23 Jun 2010) $ : Last Committed Date */
#define PACKAGE_STRING "MobileC V2.0.0"
#define PACKAGE_VERSION "2.0.0"
#define strtok_r( buf, delim, save_ptr ) \
	strtok( buf, delim )
#define strdup( str ) \
  _strdup( str )
//#define HAVE_LIBBLUETOOTH 1
#undef NEW_SECURITY 
#undef MC_SECURITY
