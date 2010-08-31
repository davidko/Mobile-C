#!/bin/ch
#include <chshell.h>  // for chinfo() 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

string_t pkgname="chmobilec"; // define package name
chinfo_t info;              // for Ch version number
string_t cwd = _cwd;        // the current working directory
string_t chhome;

//make sure pgkcreate.ch is run from the current working directory
if(access("pkgcreate.ch", R_OK)) {
    echo Run ./pkgcreate.ch in the current directory.
    exit(-1);
}
// run this script in proper Ch version
chinfo(&info);
if ((info.vermajor*100+ info.verminor*10 +  info.vermicro) < 501) {
   echo "To run this script, you need to install Ch version 5.0.1.12201 or higher"
   echo "You can download the latest version from http://www.softintegration.com/download"
   exit(-1);
}

//echo clean up existing directory and create new ones
if (!access(pkgname, F_OK))
  rm -rf $pkgname
mkdir $pkgname 
//mkdir $pkgname/dl $pkgname/lib $pkgname/include $pkgname/demos $pkgname/bin
mkdir $pkgname/include

//echo copying demo programs ...
//cp -rf demos/* $pkgname/demos

//echo copying and modify header files ...
cp src/include/fipa_acl.h  $pkgname/include
cp src/include/dynstring.h $pkgname/include

echo package $pkgname created successfully!

