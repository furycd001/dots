#ifndef __VM_RANDOM_H__
#define __VM_RANDOM_H__

#define VM_RAND_MAX        2147483647

#define int32_t int

struct vm_random_data
{
 int32_t *fptr;              /* Front pointer.  */
 int32_t *rptr;              /* Rear pointer.  */
 int32_t *state;             /* Array of state values.  */
 int vm_rand_type;              /* Type of random number generator.  */
 int vm_rand_deg;               /* Degree of random number generator.  */
 int vm_rand_sep;               /* Distance between front and rear.  */
 int32_t *end_ptr;           /* Pointer behind state table.  */
};

int vm_initstate (unsigned int seed, 
                  void* arg_state, 
                  size_t n, 
                  struct vm_random_data* buf);
int vm_setstate (void* arg_state, 
                 struct vm_random_data* buf);
void vm_default_initstate( int seed,
                           struct vm_random_data* buf );
int vm_srandom (unsigned int seed, 
                struct vm_random_data* buf);
int32_t vm_random (struct vm_random_data* buf);

#endif /* !defined( __VM_RANDOM_H__ ) */
