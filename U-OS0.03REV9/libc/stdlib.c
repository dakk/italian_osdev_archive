  #include <mydef.h>
  #include <fismem.h>
  #include <stdlib.h>

  void *malloc(const size_t size)
  {
      return richiedi_pagine(size % 4096 ? size / 4096 + 1 : size / 4096);
  }

 void free(void *ptr)
 {
     rilascia_pagine(ptr);
 }
