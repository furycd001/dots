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
 *      Copyright (C) 1998, Caldera, Inc.
 */
#include "kcheckpass.h"
#ifdef HAVE_PAM

extern  char caller[20];

/*****************************************************************
 * This is the authentication code if you use PAM
 * Ugly, but proven to work.
 *****************************************************************/
#include <stdlib.h>
#include <string.h>
#include <security/pam_appl.h>

static const char *PAM_username;
static const char *PAM_password;

#ifdef PAM_MESSAGE_NONCONST
typedef struct pam_message pam_message_type;
#else
typedef const struct pam_message pam_message_type;
#endif

static int
PAM_conv (int num_msg, pam_message_type **msg,
	  struct pam_response **resp,
	  void *appdata_ptr)
{
  int             count;
  struct pam_response *repl;

  if (!(repl = calloc(num_msg, sizeof(struct pam_response))))
    return PAM_CONV_ERR;

  for (count = 0; count < num_msg; count++)
    switch (msg[count]->msg_style) {
    case PAM_PROMPT_ECHO_ON:
      if (PAM_username)
	if (!(repl[count].resp = strdup(PAM_username)))
	  goto conv_err;
      repl[count].resp_retcode = PAM_SUCCESS;
      /* PAM frees resp */
      break;
    case PAM_PROMPT_ECHO_OFF:
      if (PAM_password)
        if (!(repl[count].resp = strdup(PAM_password)))
	  goto conv_err;
      repl[count].resp_retcode = PAM_SUCCESS;
      /* PAM frees resp */
      break;
    case PAM_TEXT_INFO:
      message("unexpected message from PAM: %s\n", msg[count]->msg);
      break;
    case PAM_ERROR_MSG:
      message("unexpected error from PAM: %s\n", msg[count]->msg);
      break;
    default:
      /* Must be an error of some sort... */
      goto conv_err;
    }
  *resp = repl;
  return PAM_SUCCESS;

 conv_err:
  for (; count >= 0; count--)
    if (repl[count].resp) {
      switch (msg[count]->msg_style) {
      case PAM_PROMPT_ECHO_OFF:
	memset (repl[count].resp, 0, strlen(repl[count].resp));
	/* fall through */
      case PAM_ERROR_MSG:
      case PAM_TEXT_INFO:
      case PAM_PROMPT_ECHO_ON:
	free(repl[count].resp);
	break;
      }
      repl[count].resp = 0;
    }
  free(repl);
  return PAM_CONV_ERR;
}

static struct pam_conv PAM_conversation = {
  &PAM_conv,
  NULL
};


int Authenticate(const char *login, const char *passwd)
{
  pam_handle_t	*pamh;
  int		pam_error;

  const char *tty;
  const char *kde_pam = KCHECKPASS_PAM_SERVICE;

  PAM_username = login;
  PAM_password = passwd;

  if (caller[0])
    kde_pam = caller;
  pam_error = pam_start(kde_pam, login, &PAM_conversation, &pamh);
  if (pam_error != PAM_SUCCESS)
    return 0;

  tty = getenv ("DISPLAY");
  if (!tty)
    tty = ":0";
  pam_error = pam_set_item (pamh, PAM_TTY, tty);
  if (pam_error != PAM_SUCCESS) {
    pam_end(pamh, pam_error);
    return 0;
  }

  pam_error = pam_authenticate(pamh, 0);
  if (pam_error != PAM_SUCCESS) {
    pam_end(pamh, pam_error);
    return 0;
  }

  /* Refresh credentials (Needed e.g. for AFS (timing out Kerberos tokens)) */
  pam_error = pam_setcred(pamh, PAM_REFRESH_CRED);
  if (pam_error != PAM_SUCCESS)  {
    pam_end(pamh, pam_error);
    return 0;
  }

  pam_end(pamh, PAM_SUCCESS);
  return 1;
}

#endif
