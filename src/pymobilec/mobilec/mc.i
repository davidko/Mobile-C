%module mc
%feature("autodoc", "1");
%{
#include "../include/libmc.h"
#include "../include/agent.h"
%}
%include "../include/libmc.h"
%include "../include/agent.h"
typedef struct ChOptions_ {
   int shelltype;        /* shell type: CH_REGULARCH or CH_SAFECH */
   /* int chrc; */       /* deprecated, use default value */
   /* char *chrcname; */ /* deprecated, use default value  */
   char *chhome;         /* Embedded Ch home directory. if NULL, use Ch home directory for 
                            standard/professional edition as Embedded Ch home directory */
   char *licensestr;     /* license string from SoftIntegration to a customer for 
                            distributing applications built with Embedded Ch */
   char *chmtdir;        /* directory for dynamically loaded libs chmt1.dl, chmt2.dl etc. 
                            By defualt, the chmtdir is chhome/extern/lib */

} ChOptions_t;
