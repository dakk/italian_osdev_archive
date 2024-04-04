#ifndef _PAGING_H_
#define _PAGING_H_

#define make_pgf_address(address,avail,us,rw,p) ((unsigned long)(((address)) | ((avail) << 8) | ((us) << 2) | ((rw) << 1) | (p)))

/*Some definition relevant to paging*/
#define PRESENT 1
#define NOT_PRESENT 0
#define SUPERVISOR 0
#define USER 1
#define READ_ONLY 0
#define READ_WRITE 1


#endif
