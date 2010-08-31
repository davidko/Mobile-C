
CC=cl.exe
LN=link.exe

CFLAGSDEBUG=/Od /D "WIN32" /D "_DEBUG" /D "_VC80_UPGRADE=0x0710" /D "_MBCS" /FD /EHsc /MDd /Fd"vc90.pdb" /W3 /nologo /c /Zi /TP /errorReport:prompt

CFLAGSRELEASE=/D "WIN32" /D "NDEBUG" /D "_VC80_UPGRADE=0x0710" /D "_MBCS" /FD /EHsc /MD /Fo"Release\\" /Fd"Release\vc90.pdb" /W3 /nologo /c /Zi /TP /errorReport:prompt

LFLAGSDEBUG=/NOLOGO
LFLAGSRELEASE=/NOLOGO


