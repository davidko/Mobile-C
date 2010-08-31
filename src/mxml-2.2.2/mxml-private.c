/* SVN FILE INFO
 * $Revision: 531 $ : Last Committed Revision
 * $Date: 2010-06-17 10:39:34 -0700 (Thu, 17 Jun 2010) $ : Last Committed Date */
/*
 * "$Id: mxml-private.c,v 1.1 2007/05/23 20:43:27 david_ko Exp $"
 *
 * Private functions for Mini-XML, a small XML-like file parsing library.
 *
 * Copyright 2003-2005 by Michael Sweet.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Contents:
 *
 *   mxml_error()      - Display an error message.
 *   mxml_integer_cb() - Default callback for integer values.
 *   mxml_opaque_cb()  - Default callback for opaque values.
 *   mxml_real_cb()    - Default callback for real number values.
 */

/*
 * Include necessary headers...
 */

#ifdef _WIN32
#include "winconfig.h"
#else
#include "config.h"
#endif
#include "mxml.h"


/*
 * Error callback function...
 */

void	(*mxml_error_cb)(const char *) = NULL;


/*
 * 'mxml_error()' - Display an error message.
 */

void
mxml_error(const char *format,		/* I - Printf-style format string */
           ...)				/* I - Additional arguments as needed */
{
  va_list	ap;			/* Pointer to arguments */
  char		*s;			/* Message string */


 /*
  * Range check input...
  */

  if (!format)
    return;

 /*
  * Format the error message string...
  */

  va_start(ap, format);

  s = mxml_strdupf(format, ap);

  va_end(ap);

 /*
  * And then display the error message...
  */

  if (mxml_error_cb)
    (*mxml_error_cb)(s);
  else
    fprintf(stderr, "mxml: %s\n", s);

 /*
  * Free the string...
  */

  free(s);
}


/*
 * 'mxml_integer_cb()' - Default callback for integer values.
 */

mxml_type_t				/* O - Node type */
mxml_integer_cb(mxml_node_t *node)	/* I - Current node */
{
  (void)node;

  return (MXML_INTEGER);
}


/*
 * 'mxml_opaque_cb()' - Default callback for opaque values.
 */

mxml_type_t				/* O - Node type */
mxml_opaque_cb(mxml_node_t *node)	/* I - Current node */
{
  (void)node;

  return (MXML_OPAQUE);
}


/*
 * 'mxml_real_cb()' - Default callback for real number values.
 */

mxml_type_t				/* O - Node type */
mxml_real_cb(mxml_node_t *node)		/* I - Current node */
{
  (void)node;

  return (MXML_REAL);
}


/*
 * End of "$Id: mxml-private.c,v 1.1 2007/05/23 20:43:27 david_ko Exp $".
 */
