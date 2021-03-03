/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kcheckpass.h"

#ifdef _AIX
#include <stdlib.h>

/* 
 * The AIX builtin authenticate() uses whichever method the system 
 * has been configured for.  (/etc/passwd, DCE, etc.)
 */
int authenticate(wchar_t *, wchar_t *, int *, wchar_t **);

int Authenticate(const char *login, const char *passwd) {
  int result;
  int reenter;  /* Tells if authenticate is done processing or not. */
  wchar_t *msg; /* Contains a prompt message or failure reason.     */

  result = authenticate((wchar_t *)login, (wchar_t *)passwd, &reenter, &msg);
  
  if (result == 0 && reenter == 0) {
    return 1;
  }
  else {
    if (msg) {
      message((char *)msg);
      free(msg);
    }
    return 0; 
  }
}

#endif
