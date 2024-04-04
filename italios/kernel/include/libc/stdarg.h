#ifndef _STDARG_H
#define _STDARG_H

typedef void *__gnuc_va_list;


#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_start(AP, LASTARG) 						\
 (AP = ((__gnuc_va_list) __builtin_next_arg (LASTARG)))

#undef va_end
void va_end (__gnuc_va_list);
#define va_end(AP)	((void)0)

#define va_arg(AP, TYPE)						\
 (AP = (__gnuc_va_list) ((char *) (AP) + __va_rounded_size (TYPE)),	\
  *((TYPE *) (void *) ((char *) (AP) - __va_rounded_size (TYPE))))


#define __va_copy(dest, src) (dest) = (src)


typedef __gnuc_va_list va_list;


#endif

