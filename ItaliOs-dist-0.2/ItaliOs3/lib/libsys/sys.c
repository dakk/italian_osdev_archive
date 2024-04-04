#define size_t unsigned int

void puts(char *str)
{
      asm("int $0x55": :"a"(6), "b"(str));
}

void putchar(char car){
	  asm("int $0x55": :"a"(5), "b"(car));
}

int getchar()
{
	int car;
      asm("int $0x55": "=a"(car):"a"(12));
	return car;
}

char *gets(char *string){
      asm("int $0x55": :"a"(13), "b"(string));
	return string;
}


void *malloc(size_t dim){
	int pos;
      asm("int $0x55": "=a"(pos):"a"(1), "b"(dim));
	return (void*)pos;
}

void free(void *addr){
	asm("int $0x55"::"a"(2), "b"(addr));
}

//Feature oscura realloc(NULL, size) == malloc(size)
//                                 realloc(pointer, 0) == free(pointer)
void *realloc(void *pointer, size_t size){
	void *punt;
	asm("int $0x55":"=a"(punt):"a"(3), "b"(pointer), "c"(size));
	return punt;
}

void *calloc(size_t nmemb, size_t size){
	
	void *punt;
	asm("int $0x55":"=a"(punt):"a"(4), "b"(nmemb), "c"(size));
	return punt;
}
