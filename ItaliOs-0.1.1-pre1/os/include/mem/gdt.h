/*
 * gdt.h:   symbols and macros for building descriptors and descriptor tables
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __GDT_H
#define __GDT_H

#define NO_GDT_ENTRIES 8192


/*
 * desc_table, stnd_desc and gate_desc sono macro per la creazione di tabelle di descrittori
 *
 * Esempio di uso:
 *
 *  desc_table(GDT, 2) {
 *     stnd_desc(0, 0, 0),
 *     stnd_desc(0, 0xfffff, (D_CODE + D_READ + D_BIG + D_BIG_LIM))
 *  };
 *
 * gate_desc(offset, selector, control)   ;For gate descriptors
 * stnd_desc(base, limit, control)	  ;For all other descriptors
 *
 *  base    è tutto l'indiizzo a 32bit del byte zero di inizio del segmento
 *  limit   inidica dove finisce il segmento è
 *  control tutti gli altri bit, è ottenuto con la somma delle costanti D-xxx
 *
 * desc_table(symbol, size)               ;inizia il DT
 *
 *  symbol  è il nome della variabile con cui ci si riferirà durante il  programma
 *  size    è il numero di descrttori definiti nella tabella
 *
 * La DT può essere accessa come una variabile normale, infatti essa è definita come 
  * un vettore di union con il descrtittore di segmento e il gate descriptor
 *
 * Un valore a piacere può essere inserito così:
 *
 *  desc_table(GDT, 2) {
 *     {dummy: 0x1234567890123456},
 *     ...
 *  };
 */


/*
 *  Ogni descrittore ha esattamente uno di questi 8 codic
 */

#define D_LDT   0x200   /* LDT segment        */
#define D_TASK  0x500   /* Task gate          */
#define D_TSS   0x900   /* TSS                */
#define D_CALL  0x0C00  /* 386 call gate      */
#define D_INT   0x0E00  /* 386 interrupt gate */
#define D_TRAP  0x0F00  /* 386 trap gate      */
#define D_DATA  0x1000  /* Data segment       */
#define D_CODE  0x1800  /* Code segment       */


/*
 *  privilegi e presenze
 */

#define D_DPL3         0x6000   /* DPL3 or mask for DPL                    */
#define D_DPL2         0x4000   /* DPL2 or mask for DPL                    */
#define D_DPL1         0x2000   /* DPL1 or mask for DPL                    */
#define D_PRESENT      0x8000   /* Present                                 */
#define D_NOT_PRESENT  0x8000   /* Not Present                             */


/*
 *  descrittori di segmento
 */

#define D_ACC      0x100  /* Accessed (Data or Code)          */

#define D_WRITE    0x200  /* Writable (Data segments only)    */
#define D_READ     0x200  /* Readable (Code segments only)    */
#define D_BUSY     0x200  /* Busy (TSS only)                  */

#define D_EXDOWN   0x400  /* Expand down (Data segments only) */
#define D_CONFORM  0x400  /* Conforming (Code segments only)  */

#define D_BIG      0x40	  /* Default to 32 bit mode           */
#define D_BIG_LIM  0x80	  /* Limit is in 4K units             */


/*
 *  Definiamo le strtture per i gate e i descrittori
 */

struct x86_desc {
   unsigned short limit_low;     /* limit 0..15    */
   unsigned short base_low;      /* base  0..15    */
   unsigned char base_med;       /* base  16..23   */
   unsigned char access;         /* access byte    */
   unsigned int limit_high:4;    /* limit 16..19   */
   unsigned int granularity:4;   /* granularity    */
   unsigned char base_high;      /* base 24..31    */
} __attribute__ ((packed));


struct x86_gate {
   unsigned short offset_low;   /* offset 0..15    */
   unsigned short selector;     /* selector        */
   unsigned short access;       /* access flags    */
   unsigned short offset_high;  /* offset 16..31   */
} __attribute__ ((packed));


/*
 *  LA TAbella dei descrtittori è un vettore di standard o gate descriptors
 *  Fa eccezione il primo elemento che se accesso fa crashare la macchina
 *
 *
 *  Ora definiamo union DT_entry come un tipogenerale di tabella dei descrittori
 *  Essa puòcontenere sia un normale descrittore che dei dati propri a
 *  64-bit
 */

union DT_entry {
   struct x86_desc desc;     /* Normal descriptor */
   struct x86_gate gate;     /* Gate descriptor   */
   unsigned long long dummy; /* Any other info    */
};


/*
 * Macro magiche perla dichiarazione dei descrittori :-)
 */

#define desc_table(name,length) union DT_entry name[length] =
#define stnd_desc(base, limit, control) {desc: {(limit & 0xffff), (base & 0xffff), ((base >> 16) & 0xff), \
                                               ((control+D_PRESENT) >> 8), (limit >> 16), \
                                               ((control & 0xff) >> 4), (base >> 24)}}
#define gate_desc(offset, selector, control) {gate: {(offset & 0xffff), selector, \
                                                     (control+D_PRESENT), (offset >> 16) }}

/* These are bound to look a bit cryptic, so I'll just show what the result
 * would look like:
 *
 * desc_table(GDT, 3) {
 *    stnd_desc(base, limit, control),
 *    stnd_desc(base, limit, control),
 *    gate_desc(offset, selector, control)
 * };
 *
 * ... would produce something looking like:
 *
 * union DT_entry GDT[3] = {
 *    {desc: {limit_low, base_low, base_med, access, limit_high, granularity, base_high}},
 *    {desc: {limit_low, base_low, base_med, access, limit_high, granularity, base_high}},
 *    {gate: {offset_low, selector, word_count, access, offset_high}}
 * };
 *
 * The values are conveted using a bit of cryptic, but straightforward magic.
 */

void loadGDTR (const unsigned int address, const unsigned int size);
unsigned int add_desc_to_gdt(unsigned int base, unsigned int limit, unsigned int opt) ;
void init_gdt(void);

#endif
