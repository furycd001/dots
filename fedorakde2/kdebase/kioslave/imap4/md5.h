#ifndef MD5_H
#define MD5_H

#define MD5BLKLEN 64            /* MD5 block length */
#define MD5DIGLEN 16            /* MD5 digest length */

typedef struct
{
  unsigned long chigh;          /* high 32bits of byte count */
  unsigned long clow;           /* low 32bits of byte count */
  unsigned long state[4];       /* state (ABCD) */
  unsigned char buf[MD5BLKLEN]; /* input buffer */
  unsigned char *ptr;           /* buffer position */
}
MD5CONTEXT;


/* Prototypes */

void md5_init (MD5CONTEXT * ctx);
void md5_update (MD5CONTEXT * ctx, unsigned char *data, unsigned long len);
void md5_final (unsigned char *digest, MD5CONTEXT * ctx);
#endif
