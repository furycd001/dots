/*
 * vm.c
 *
 * Copyright (c) 2000 Artur Rataj
 * Distributed under the terms of the GNU General Public License
 *
 */

#include "vm.h"

#define	ERROR_VALUE	INT_MAX

int vm_init_pool( struct tvm_pool**	pool,
                  int			area_size,
                  int			max_stack_size,
                  int			max_threads_num ) {
 int position;
 
 ( *pool ) = (struct tvm_pool*)malloc( sizeof(struct tvm_pool) );
 ( *pool )->area_size = area_size;
 ( *pool )->area = (int*)malloc( (*pool)->area_size*sizeof(int) );
 ( *pool )->processes = NULL;
 ( *pool )->max_stack_size = max_stack_size;
 ( *pool )->max_threads_num = max_threads_num;
 vm_enable_reverse( *pool, 0 ); 
 for( position = 0; position < (*pool)->area_size; ++position )
  ( *pool )->area[position] = VM_OP_STOP;
 return 1;
}

void vm_done_pool( struct tvm_pool*	pool ) {
 struct tvm_process*	curr_process;
 free( pool->area );
 curr_process = pool->processes;
 while( curr_process ) {
  struct tvm_process*	tmp_process;
  
  tmp_process = curr_process;
  curr_process = curr_process->next;
  free( tmp_process->stack );
  free( tmp_process );
 }
 free( pool );
}

static int push( struct tvm_pool*	pool,
                 struct tvm_process*	process,
                 int			value ) {
 if( process->stack_top == pool->max_stack_size )
  return ERROR_VALUE;
 else
  process->stack[process->stack_top++] = value;
 return 1;
}

static int pop( struct tvm_pool*	pool,
                struct tvm_process*	process ) {
 if( process->stack_top == 0 )
  return ERROR_VALUE;
 else
{
  return process->stack[--process->stack_top];
}
}

void vm_modify( struct tvm_pool*	pool,
                int			position,
                int			op ) {
 pool->area[position] = op;
}

void vm_exec( struct tvm_pool*	pool,
              int		position,
              int		age,
              int		reverse ) {
 struct tvm_process*	new_process;
              
 new_process = (struct tvm_process*)malloc( sizeof(struct tvm_process) );
 new_process->position = position;
 new_process->stack = (int*)malloc( pool->max_stack_size*sizeof(int) );
 new_process->stack_top = 0;
 new_process->age = age;
 new_process->reverse = reverse;
 new_process->next = pool->processes;
 pool->processes = new_process;
}

void vm_enable_reverse( struct tvm_pool*	pool,
                        const int		enabled ) {
 pool->reverse_enabled = enabled;
}

int vm_get_reverse( struct tvm_pool*       pool ) {
 if( pool->reverse_enabled )
  return (int)( vm_random(&(pool->vm_random_data))*2.0/
                ( VM_RAND_MAX + 1.0 ) ); 
 else
  return 0;
}

void vm_iterate( struct tvm_pool*	pool,
                 char*			modified ) {
 struct tvm_process*	prev_process;
 struct tvm_process*	curr_process;
 struct tvm_process*	next_process;
 int			processes_num;
 
 processes_num = 0;
 prev_process = NULL;
 curr_process = pool->processes;
 while( curr_process ) {
  int			op;
  int			arg;
  int			arg_2;
  int			arg_3;
  
  ++curr_process->age;
  next_process = curr_process->next;
  op = pool->area[curr_process->position];
  if( curr_process->reverse )
   --curr_process->position;
  else
   ++curr_process->position;
  curr_process->position = ( curr_process->position + pool->area_size )%
                           pool->area_size;
  switch( op ) {
   case VM_OP_WAIT:
    break;
    
   case VM_OP_STOP:
    if( !prev_process )
     pool->processes = curr_process->next;
    else
     prev_process->next = curr_process->next;
    free( curr_process->stack );
    free( curr_process );
    curr_process = prev_process;
    --processes_num; 
    break;
    
   case VM_OP_EXEC:
    if( (arg = pop( pool, curr_process )) == ERROR_VALUE ) {
     if( !prev_process )
      pool->processes = curr_process->next;
     else
      prev_process->next = curr_process->next;
     free( curr_process->stack );
     free( curr_process );
     curr_process = prev_process;
     --processes_num; 
    } else {
     arg = curr_process->position + arg;
     if( arg < 0 )
      arg += pool->area_size;
     if( arg >= pool->area_size )
      arg -= pool->area_size;
     vm_exec( pool, arg, curr_process->age, vm_get_reverse(pool) );
    }
    break;
    
   case VM_OP_COPY:
    if( (arg = pop( pool, curr_process )) == ERROR_VALUE ) {
     if( !prev_process )
      pool->processes = curr_process->next;
     else
      prev_process->next = curr_process->next;
     free( curr_process->stack );
     free( curr_process );
     curr_process = prev_process;
     --processes_num; 
    } else if( (arg_2 = pop( pool, curr_process )) == ERROR_VALUE ) {
     if( !prev_process )
      pool->processes = curr_process->next;
     else
      prev_process->next = curr_process->next;
     free( curr_process->stack );
     free( curr_process );
     curr_process = prev_process;
     --processes_num; 
    } else if( 1 && (arg_3 = pop( pool, curr_process )) == ERROR_VALUE ) {
     if( !prev_process )
      pool->processes = curr_process->next;
     else
      prev_process->next = curr_process->next;
     free( curr_process->stack );
     free( curr_process );
     curr_process = prev_process;
     --processes_num; 
    } else {
     int	count;
     int direction;
     
     arg = curr_process->position + arg;
     if( arg < 0 )
      arg += pool->area_size;
     if( arg >= pool->area_size )
      arg -= pool->area_size;
     arg_2 = curr_process->position + arg_2;
     if( arg_2 < 0 )
      arg_2 += pool->area_size;
     if( arg_2 >= pool->area_size )
      arg_2 -= pool->area_size;
     if( curr_process->reverse )
      direction = -1;
     else
      direction = 1;
     for( count = 0; count < arg_3; ++count ) {
      int	i, j;
      int	offset;
      
      offset = count*direction + pool->area_size;
      i = pool->area[( arg_2 + offset )%pool->area_size];
      j = pool->area[( arg_2 + offset )%pool->area_size] = pool->area[( arg + offset )%pool->area_size];
      if( modified && i != j )
       modified[( arg_2 + offset )%pool->area_size] = 1;
     }
    }
    break;
    
   default: /* >= VM_OP_PUSH */
    arg = op - VM_OP_PUSH;
    if( push(pool, curr_process, arg) == ERROR_VALUE ) {
     if( !prev_process )
      pool->processes = curr_process->next;
     else
      prev_process->next = curr_process->next;
     free( curr_process->stack );
     free( curr_process );
     curr_process = prev_process;
     --processes_num; 
    }
    break;
  }
  prev_process = curr_process;
  curr_process = next_process;
  ++processes_num;
 }
 while( processes_num > pool->max_threads_num ) {
  int	process_num;
  int	curr_process_num;
  
  process_num = (int)( vm_random(&(pool->vm_random_data))*1.0*processes_num/
                       ( VM_RAND_MAX + 1.0 ) );
/*
  process_num = (int)( rand()*1.0*processes_num/
                       ( RAND_MAX + 1.0 ) );
 */
  curr_process_num = 0;
  curr_process = pool->processes;
  prev_process = NULL;
  while( curr_process_num != process_num ) {
   prev_process = curr_process;
   curr_process = curr_process->next;
   ++curr_process_num;
  }
  if( prev_process )
   prev_process->next = curr_process->next;
  else
   pool->processes = curr_process->next;
  free( curr_process->stack );
  free( curr_process );
  --processes_num;
 }
}
