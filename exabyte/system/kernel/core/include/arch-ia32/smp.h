/* smp.h -- Symmetric Multiprocessing definitions */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#ifndef IA32_SMP_H
#define IA32_SMP_H

#include <arch-ia32/config.h>

#define SMP_AP_BOOT_PAGE			0x03
#define SMP_AP_GDT_ADDR				0x1000
#define SMP_AP_IDT_ADDR				0x2000
#define SMP_AP_BOOT_CODE_SIZE			0x0500
#define SMP_PAGE_SIZE				0x1000

#define SMP_OK			"ok\n"

#ifdef LANG_US
#define SMP_CHECK		"smp: checking system...\n"
#define SMP_SEARCHING		"smp: looking for MP table... "
#define SMP_NOTFOUND		"not found\n"
#define SMP_FOUND		"found at 0x%08x, version "
#define SMP_UNKNOWN		"unknown (%u)\n"
#define SMP_NOTPRESENT		"smp: MP configuration table not present\n"
#define SMP_INVSIGN		"smp: invalid signature in the MP configuration table\n"
#define SMP_INVCHECKSUM		"smp: invalid checksum of the MP configuration table\n"
#define SMP_CONFENTRIES		"smp: found %u entries in MP configuration table\n"
#define SMP_DISABLED		"disabled, "
#define SMP_IOAPIC		"I/O APIC: ID 0x%02x, version 0x%02x, address 0x%08x\n"
#define SMP_IMCR		"smp: IMCR present, PIC Mode implemented\n"
#define SMP_FINAL		"smp: multiprocessor system, %u processors found, %u I/O APICs found\n"
#define SMP_UNI			"smp: initializing as uniprocessor system\n"
#define SMP_INIT2		"smp: initializing system...\n"
#define SMP_IRQM		"smp: mapping IRQs... "
#define SMP_INITED		"smp: system initialized\n"
#define SMP_INFOHT		"smp: Intel (R) Hyper-Threading (TM) technology, %u logical processors\n"
#define SMP_INFOMC		"smp: Intel (R) Multi-Core (TM) technology, %u logical processors, %u cores\n"
#elif defined LANG_IT
#define SMP_CHECK		"smp: controllo del sistema...\n"
#define SMP_SEARCHING		"smp: ricerca della tabella MP... "
#define SMP_NOTFOUND		"non trovata\n"
#define SMP_FOUND		"trovata all'indirizzo 0x%08x, versione "
#define SMP_UNKNOWN		"sconosciuta (%u)\n"
#define SMP_NOTPRESENT		"smp: tabella di configurazione MP non presente\n"
#define SMP_INVSIGN		"smp: signature non valida nella tabella di configurazione MP\n"
#define SMP_INVCHECKSUM 	"smp: checksum non valido nella tabella di configurazione MP\n"
#define SMP_CONFENTRIES		"smp: trovate %u entry nella tabella di configurazione MP\n"
#define SMP_DISABLED		"disabilitato, "
#define SMP_IOAPIC		"I/O APIC: ID 0x%02x, versione 0x%02x, indirizzo 0x%08x\n"
#define SMP_IMCR		"smp: IMCR presente, implementata modalita' PIC\n"
#define SMP_FINAL		"smp: sistema multiprocessore, %u processori, %u I/O APIC\n"
#define SMP_UNI			"smp: inizializzazione come sistema uniprocessore\n"
#define SMP_INIT2		"smp: inizializzazione del sistema...\n"
#define SMP_IRQM		"smp: mappatura IRQ... "
#define SMP_INITED		"smp: sistema inizializzato\n"
#define SMP_INFOHT		"smp: tecnologia Intel (R) Hyper-Threading (TM), %u processori logici\n"
#define SMP_INFOMC		"smp: tecnologia Intel (R) Multi-Core (TM), %u processori logici , %u core\n"
#endif

typedef struct mp_table_t {
	char signature[4];
	unsigned long config_addr;
	unsigned char length;
	unsigned char version;
	unsigned char checksum;
	unsigned char features1;
	unsigned char features2;
	unsigned char features[3];
} mp_table_t;

#define DEFAULT_CONFIG_ISA_82489DX			1
#define DEFAULT_CONFIG_EISA_82489DX			2
#define DEFAULT_CONFIG_EISA2_82489DX		3
#define DEFAULT_CONFIG_MCA_82489DX			4
#define DEFAULT_CONFIG_ISAPCI_INTEGRATED	5
#define DEFAULT_CONFIG_EISAPCI_INTEGRATED	6
#define DEFAULT_CONFIG_MCAPCI_INTEGRATED	7

typedef struct mp_config_table_t {
	char signature[4];
	unsigned short length;
	unsigned char revision;
	unsigned char checksum;
	char oem_id[8];
	char product_id[12];
	unsigned long oem_table;
	unsigned short oem_size;
	unsigned short entry_count;
	unsigned long apic_address;
	unsigned short table_length;
	unsigned char table_checksum;
	unsigned char reserved;
} mp_config_table_t;

#define MP_ENTRY_TYPE_PROCESSOR			0
#define MP_ENTRY_TYPE_BUS				1
#define MP_ENTRY_TYPE_IOAPIC			2
#define MP_ENTRY_TYPE_IOINT				3
#define MP_ENTRY_TYPE_LOCALINT			4

typedef struct mp_processor_entry_t {
	unsigned char type;
	unsigned char apic_id;
	unsigned char apic_version;
	unsigned char flags;
	unsigned char cpu_signature[4];
	unsigned long cpu_feature_flags;
	unsigned long reserved1;
	unsigned long reserved2;
} mp_processor_entry_t;

#define MP_PROCESSOR_FLAG_CPU_ENABLED	1
#define MP_PROCESSOR_FLAG_CPU_BSP		2

typedef struct mp_ioapic_entry_t {
	unsigned char type;
	unsigned char ioapic_id;
	unsigned char ioapic_version;
	unsigned char flags;
	unsigned long ioapic_address;
} mp_ioapic_entry_t;

#define MP_IOAPIC_FLAG_ENABLED			1

typedef struct cpu_info_t {
	unsigned long apic_base;
	unsigned char apic_id;
	unsigned char apic_version;
	unsigned char apic_lvts;
	unsigned int flags;
	unsigned long speed;
} cpu_info_t;

#define CPU_FLAG_ENABLED				1
#define CPU_FLAG_BSP					2

typedef struct ioapic_info_t {
	unsigned char id;
	unsigned char version;
	unsigned long address;
} ioapic_info_t;

typedef struct system_info_t {
	unsigned int num_cpu;
	unsigned int num_ioapic;
	unsigned int bsp_cpu_num;
	int use_imcr;
	int type;
	unsigned int logicals_per_package;
	unsigned int cores_per_package;
	int use_tsc;
	int use_apic;
	int use_ht;
	unsigned long bus_speed;
	unsigned long bus_ticks;
} system_info_t;

#define SYSTEM_TYPE_UNIPROCESSOR		0
#define SYSTEM_TYPE_MULTIPROCESSOR		1
#define SYSTEM_TYPE_HARDWARE_THREADING	2

typedef struct ap_boot_data_t {
	unsigned char booted;
	unsigned char id;
	unsigned int cpu_num;
} ap_boot_data_t;

extern cpu_info_t cpus[CONFIG_ARCH_MAX_PROCESSORS];
extern ioapic_info_t ioapics[CONFIG_ARCH_MAX_IOAPICS];
extern system_info_t system;

#define SMP_TIMER_INTERRUPT				0x30
#define SMP_LINT0_INTERRUPT				0x31
#define SMP_LINT1_INTERRUPT				0x32
#define SMP_ERROR_INTERRUPT				0x33
#define SMP_PERF_INTERRUPT				0x34
#define SMP_THERMAL_INTERRUPT			0x35

void ia32_smp_timer_handler();
void ia32_smp_lint0_handler();
void ia32_smp_lint1_handler();
void ia32_smp_error_handler();
void ia32_smp_perf_handler();
void ia32_smp_thermal_handler();

void ia32_smp_init();
unsigned long ia32_smp_search_table(unsigned long start,unsigned long size);
void ia32_smp_setup_ap_boot(unsigned int cpu);
void ia32_smp_wait_for_cpu_to_boot(unsigned int cpu);
unsigned char ia32_smp_get_int_by_irq(unsigned int irqn);

#endif	/* IA32_SMP_H */
