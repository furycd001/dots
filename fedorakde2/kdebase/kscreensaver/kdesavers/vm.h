#ifndef __VM_H__
#define __VM_H__

#include <stdlib.h>
#include <stdio.h> /* !!! */
#include <limits.h>
#include "vm_random.h"

#define	VM_OP_STOP		1
#define	VM_OP_COPY		2
#define	VM_OP_EXEC		3
#define	VM_OP_WAIT		4
#define	VM_OP_PUSH		(INT_MAX/2)

struct tvm_process {
 int			position;
 int*			stack;
 int			stack_top;
 int			age;
 int			reverse;
 struct tvm_process*	next;
};

struct tvm_pool {
 int*			area;
 int			area_size;
 struct tvm_process*	processes;
 int			max_stack_size;
 int			max_threads_num;
 int			reverse_enabled;
 struct vm_random_data	vm_random_data;
};

int vm_init_pool( struct tvm_pool**	pool,
                  int			area_size,
                  int			max_stack_size,
                  int			max_threads_num );
void vm_modify( struct tvm_pool*	pool,
                int			position,
                int			op );
void vm_exec( struct tvm_pool*	pool,
              int		position,
              int		age,
              int		reverse );
void vm_enable_reverse( struct tvm_pool*	pool,
                        const int		enabled );
int vm_get_reverse( struct tvm_pool*       pool );
void vm_iterate( struct tvm_pool*	pool,
                 char*			modified );
void vm_done_pool( struct tvm_pool*	pool );

#endif /* !defined( __VM_H__ ) */
