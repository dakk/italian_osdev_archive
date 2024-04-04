#include <fismem.h>
#include <io.h>
#include <8259.h>
#include <video.h>


mem_area_pointer mem_list = (mem_area_pointer)0x100000;
mem_area_pointer mem_list_root  = (mem_area_pointer)0x100000;

size_t total_mem;
mem_info free_pages;

unsigned int pos_mem;
int max_pages;
int max_vet;

//--------------------------------------------------------------------------------------------------------

void mem_size()
{

    unsigned int *mem_pointer;

    mem_pointer = (unsigned int *) BASE_MEM;

    outportb(0xFF, MASTER2_PIC);
    outportb(0xFF, SLAVE2_PIC);

    *mem_pointer = VALUE_MEM;
    while(1)
    {

        if(*mem_pointer == VALUE_MEM)
        {
            *mem_pointer=~VALUE_MEM;
            asm("": : :"memory");
            if((*mem_pointer==ANTI_VALUE_MEM))
            {
                mem_pointer = mem_pointer + OFFSET_MEM;
                *mem_pointer = VALUE_MEM;
                asm("": : :"memory");
            }

            else break;
        }
        else break;

        asm("": : :"memory");
    }

total_mem = (unsigned long) mem_pointer;
}

//----------------------------------------------------------------------------------------------

void init_pages()
{
    int i=0;

    while (i<SIZE_VETT)
    {
        if(i<8)
            free_pages.mem_map[i] = 0xFFFFFFFF;
        else
            free_pages.mem_map[i] = 0x00000000;
        i++;
    }


    mem_list->pages_info[0].inizio = 0x00000000;
    mem_list->pages_info[0].num_pages = 256;
    mem_list->next_free = 1;
    mem_list->next = NULL;

    max_pages = total_mem /4096;
    max_vet = max_pages /32;

}

//---------------------------------------------------------------------------------------------
void* richiedi_pagine(int num)
{
    int i, j, count;
    size_t address;
    i=0;
    j=0;

   count=0;
    while(count < num)
    {
       if(free_pages.mem_map[i] & (1<<j))
       {
                j++;
                count=0;
       }
       else
       {
            count++;
            j++;
       }
       if(j==32 && i<max_vet && count<num)
       {
           i++;
           j=0;
       }
       if(i==max_vet && j==31)
           return NULL;
    }
   address= ((i*32+j)-count)*4096;

   i= (address/4096)/32;
   j= (address/4096)%32;

  count=0;
  while(count<num)
  {
      free_pages.mem_map[i] = (free_pages.mem_map[i] | (1<<j));
      j++;
      count++;
      if(j==32 && count<num)
      {
          i++;
          j=0;
      }
  }
  add_mem_list(address, num);
  return (void *)address;
}

//---------------------------------------------------------------------------------------------------
void add_mem_list(size_t address, int num)
{
    int pos;    
    pos = mem_list->next_free;
    if(pos < 510)
    {
        mem_list->pages_info[pos].inizio = (void*) address;
        mem_list->pages_info[pos].num_pages = num;
        mem_list->next_free++;
    }
    else
    {
        mem_list->next = (mem_area_pointer) richiedi_pagine(1);
        if(mem_list != NULL)
        {
             mem_list->pages_info[0].inizio = (void*) address;
             mem_list->pages_info[0].num_pages = num;
             mem_list->next_free=1;
        }
    }
}

//-----------------------------------------------------------------------------------------------
void rilascia_pagine(void *indirizzo)
{
    int i;
    void *min, *max;
    int last;
    char vet_char[20] = {};
    i=0;
    mem_area_pointer mem_tmp;
    mem_tmp = mem_list_root;
    while(i<510)
    {

        max = mem_tmp->pages_info[i].inizio + mem_tmp->pages_info[i].num_pages;
        min = mem_tmp->pages_info[i].inizio;
        if(min<=indirizzo && max>=indirizzo)
            {
                last=mem_list->next_free;
                last--;
                mem_tmp->pages_info[i].inizio = mem_list->pages_info[last].inizio;
                mem_tmp->pages_info[i].num_pages = mem_list->pages_info[last].num_pages;
                mem_list->next_free--;
                break;
            }
           i++;
           if(i== 509 && mem_tmp->next != NULL)
               mem_tmp = mem_tmp->next;
     }
    _kntos(vet_char, (int) mem_tmp->pages_info[i].inizio, 10);
    _kputs("Dopo il rilascio ho nella posizione della mia strutturina: "); _kputs(vet_char); _kputs(" - ");
    _kntos(vet_char, mem_tmp->pages_info[i].num_pages, 10);
    _kputs("Num Pagine: "); _kputs(vet_char); _kputs(" - ");
}
//---------------------------------------------------------------------------
void print_mem_list()
{
    int i;
    i=0;
    char vet_char[20];
    while(i<6)
    {
        _kntos(vet_char, (int) mem_list->pages_info[i].inizio, 10);
        _kputs(" - "); _kputs(vet_char); _kputs(" --");
        _kntos(vet_char, mem_list->pages_info[i].num_pages, 10);
        _kputs(vet_char); _kputs("\n");
        i++;
    }
    _kputs("\n");
}
//-------------------------------------------------------------------------

