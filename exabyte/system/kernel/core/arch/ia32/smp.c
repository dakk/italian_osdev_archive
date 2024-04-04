/* smp.c -- Symmetric Multiprocessing support */
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

#include <config.h>		// questo va SEMPRE per primo
#include <arch-ia32/smp.h>
#include <arch-ia32/apic.h>
#include <arch-ia32/descriptor.h>
#include <arch-ia32/segment.h>
#include <arch-ia32/string.h>
#include <arch-ia32/cpuid.h>
#include <arch-ia32/ioapic.h>
#include <arch-ia32/pic8259.h>
#include <arch-ia32/irq.h>
#include <kernel.h>
#include <panic.h>

//#define SMP_FORCE_PIC8259
#define SMP_USE_PIC8259_WHEN_UNIPROCESSOR

cpu_info_t cpus[CONFIG_ARCH_MAX_PROCESSORS];
cpu_info_t *bsp_cpu;
ioapic_info_t ioapics[CONFIG_ARCH_MAX_IOAPICS];
system_info_t system;

unsigned char irq_to_int[IRQ_MAXNUM]={0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F};
int irq_to_cpu[IRQ_MAXNUM]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void ia32_smp_build_pm_tables();
void ia32_smp_check_system();
void ia32_smp_init_system();
int ia32_smp_check_apic();
int ia32_smp_check_hardware_threading();
void ia32_smp_print_system();

void ia32_smp_uniprocessor_system();
void ia32_smp_hardware_threading_system();
void ia32_smp_setup_hardware_threading();

void ia32_smp_use_default_config(int config);

extern void asm_smp_timer_handler();
extern void asm_smp_lint0_handler();
extern void asm_smp_lint1_handler();
extern void asm_smp_error_handler();
extern void asm_smp_perf_handler();
extern void asm_smp_thermal_handler();

void ia32_smp_init() {
	ia32_smp_check_system();
	ia32_smp_print_system();
	ia32_smp_init_system();
}

void ia32_smp_check_system() {
	unsigned long mp_pos;
	kprint(SMP_CHECK);
	if (ia32_smp_check_apic()<0) {
		system.use_apic=0;
		ia32_smp_uniprocessor_system(); // credo sia cosi...
		return;
	} else
		system.use_apic=1;
	if (ia32_smp_check_hardware_threading()<0)
		system.use_ht=0;
	else
		system.use_ht=1;
	kprint(SMP_SEARCHING);
	mp_pos=ia32_smp_search_table(0x400,0x100);
	if (mp_pos==0) {
		mp_pos=ia32_smp_search_table(0x40E0,0x400);
		if (mp_pos==0) {
			mp_pos=ia32_smp_search_table(0x9FC00,0x400);
			if (mp_pos==0) {
				mp_pos=ia32_smp_search_table(0xF0000,0x10000);
				if (mp_pos==0) {
					kprint(SMP_NOTFOUND);
					if (system.use_ht)
						ia32_smp_hardware_threading_system();
					else
						ia32_smp_uniprocessor_system();
					return;
				}
			}
		}
	}
	kprint(SMP_FOUND,mp_pos);
	mp_table_t *mp=(mp_table_t *)mp_pos;
	switch (mp->version) {
	case 1:
		kprint("1.1\n");
		break;
	case 4:
		kprint("1.4\n");
		break;
	default:
		kprint(SMP_UNKNOWN,mp->version);
	}
	if (mp->features2&(1<<7)) {
		system.use_imcr=1;
		kprint(SMP_IMCR);
	} else
		system.use_imcr=0;
	if (mp->config_addr==0) {
		kprint(SMP_NOTPRESENT);
		switch (mp->features1) {
		case DEFAULT_CONFIG_ISA_82489DX:
			kprint("smp: using default configuration, isa, 82489DX apic\n");
			ia32_smp_use_default_config(DEFAULT_CONFIG_ISA_82489DX);
			break;
		case DEFAULT_CONFIG_EISA_82489DX:
			kprint("smp: using default configuration, eisa, 82489DX apic\n");
			ia32_smp_use_default_config(DEFAULT_CONFIG_EISA_82489DX);
			break;
		case DEFAULT_CONFIG_EISA2_82489DX:
			kprint("smp: using default configuration, eisa, 82489DX apic (2)\n");
			ia32_smp_use_default_config(DEFAULT_CONFIG_EISA2_82489DX);
			break;
		case DEFAULT_CONFIG_MCA_82489DX:
			kprint("smp: using default configuration, mca, 82489DX apic\n");
			ia32_smp_use_default_config(DEFAULT_CONFIG_MCA_82489DX);
			break;
		case DEFAULT_CONFIG_ISAPCI_INTEGRATED:
			kprint("smp: using default configuration, isa/pci, integrated apic\n");
			ia32_smp_use_default_config(DEFAULT_CONFIG_ISAPCI_INTEGRATED);
			break;
		case DEFAULT_CONFIG_EISAPCI_INTEGRATED:
			kprint("smp: using default configuration, eisa/pci, integrated apic\n");
			ia32_smp_use_default_config(DEFAULT_CONFIG_EISAPCI_INTEGRATED);
			break;
		case DEFAULT_CONFIG_MCAPCI_INTEGRATED:
			kprint("smp: using default configuration, mca/pci, integrated apic\n");
			ia32_smp_use_default_config(DEFAULT_CONFIG_MCAPCI_INTEGRATED);
			break;
		default:
			if (system.use_ht)
				ia32_smp_hardware_threading_system();
			else
				ia32_smp_uniprocessor_system();
		}
		return;
	} else {
		mp_config_table_t *config=(mp_config_table_t *)mp->config_addr;
		if (config->signature[0]!='P' || config->signature[1]!='C' || config->signature[2]!='M' || config->signature[3]!='P') {
			kprint(SMP_INVSIGN);
			if (system.use_ht)
				ia32_smp_hardware_threading_system();
			else
				ia32_smp_uniprocessor_system();
			return;
		}
	#if 0 // non va porca troia
		unsigned long i;
		unsigned char checksum=0;
		for (i=0; i<sizeof(mp_config_table_t); i++)
			checksum+=*(unsigned char *)((unsigned)config+i);
		if (checksum!=0) {
			kprint(SMP_INVCHECKSUM);
			if (system.use_ht)
				ia32_smp_hardware_threading_system();
			else
				ia32_smp_uniprocessor_system();
			return;
		}
	#endif
		kprint(SMP_CONFENTRIES,config->entry_count);
		unsigned int num;
		unsigned long addr=(unsigned)config+sizeof(mp_config_table_t);
		mp_processor_entry_t *proc_entry;
		mp_ioapic_entry_t *ioapic_entry;
		unsigned int curcpu=0;
		system.num_cpu=0;
		system.num_ioapic=0;
		system.bsp_cpu_num=0;
		for (num=0; num<config->entry_count; num++) {
			proc_entry=(mp_processor_entry_t *)addr;
			switch (proc_entry->type) {
				case MP_ENTRY_TYPE_PROCESSOR: {
					if (curcpu>=CONFIG_ARCH_MAX_PROCESSORS)
						continue;
					cpus[curcpu].apic_id=proc_entry->apic_id;
					cpus[curcpu].apic_base=config->apic_address;
					cpus[curcpu].flags=0;
					if (proc_entry->flags&MP_PROCESSOR_FLAG_CPU_ENABLED)
						cpus[curcpu].flags|=CPU_FLAG_ENABLED;
					if (proc_entry->flags&MP_PROCESSOR_FLAG_CPU_BSP) {
						cpus[curcpu].flags|=CPU_FLAG_BSP;
						system.bsp_cpu_num=curcpu;
					}
					curcpu++;
					addr+=sizeof(mp_processor_entry_t);
					break;
				}
				case MP_ENTRY_TYPE_BUS: {
					// me ne frego per ora...
					addr+=8;
					break;
				}
				case MP_ENTRY_TYPE_IOAPIC: {
					ioapic_entry=(mp_ioapic_entry_t *)addr;
					ioapics[system.num_ioapic].id=ioapic_entry->ioapic_id;
					ioapics[system.num_ioapic].version=ioapic_entry->ioapic_version;
					ioapics[system.num_ioapic].address=ioapic_entry->ioapic_address;
					system.num_ioapic++;
					addr+=sizeof(mp_ioapic_entry_t);
					break;
				}
				case MP_ENTRY_TYPE_IOINT: {
					// me ne frego per ora...
					addr+=8;
					break;
				}
				case MP_ENTRY_TYPE_LOCALINT: {
					// me ne frego per ora...
					addr+=8;
					break;
				}
			};
		}
		system.num_cpu=curcpu;
	}
	if (system.use_ht)
		ia32_smp_setup_hardware_threading();
	else {
		if (system.num_cpu>1)
			system.type=SYSTEM_TYPE_MULTIPROCESSOR;
		else
			system.type=SYSTEM_TYPE_UNIPROCESSOR;
	}
}

void ia32_smp_uniprocessor_system() {
	system.num_cpu=1;
	system.num_ioapic=0;
	system.bsp_cpu_num=0;
	system.use_imcr=0;
	cpus[0].apic_base=0xFEE00000;
	cpus[0].apic_id=0;
	cpus[0].flags=CPU_FLAG_ENABLED|CPU_FLAG_BSP;
	system.type=SYSTEM_TYPE_UNIPROCESSOR;
}

void ia32_smp_hardware_threading_system() {
	unsigned long eax,ebx,ecx,edx;
	ia32_cpuid(1,&eax,&ebx,&ecx,&edx);
	system.logicals_per_package=(ebx>>16)&0xFF;
	unsigned long eax2,ebx2,ecx2,edx2;
	ia32_cpuid(0,&eax2,&ebx2,&ecx2,&edx2);
	if (eax2<4)
		system.cores_per_package=0;
	else
		system.cores_per_package=(eax>>26)&0x3F;
	system.num_cpu=system.logicals_per_package; // credo sia cosi
	system.bsp_cpu_num=0;
	unsigned int i;
	for (i=0; i<system.num_cpu; i++) {
		cpus[i].apic_base=0xFEE00000;
		cpus[i].apic_id=i;
		cpus[i].flags=CPU_FLAG_ENABLED;
	}
	cpus[0].flags|=CPU_FLAG_BSP;
	system.num_ioapic=0; // credo cazzo, non c'è scritto da nessuna parte :|
	if (system.num_cpu>1)
		system.type=SYSTEM_TYPE_HARDWARE_THREADING;
	else
		system.type=SYSTEM_TYPE_UNIPROCESSOR;
}

void ia32_smp_setup_hardware_threading() {
	unsigned long eax,ebx,ecx,edx;
	ia32_cpuid(1,&eax,&ebx,&ecx,&edx);
	system.logicals_per_package=(ebx>>16)&0xFF;
	unsigned long eax2,ebx2,ecx2,edx2;
	ia32_cpuid(0,&eax2,&ebx2,&ecx2,&edx2);
	if (eax2<4)
		system.cores_per_package=0;
	else
		system.cores_per_package=(eax>>26)&0x3F;
	if (system.num_cpu<system.logicals_per_package) { // li abilito tutti
		if (system.logicals_per_package>CONFIG_ARCH_MAX_PROCESSORS)
			system.num_cpu=CONFIG_ARCH_MAX_PROCESSORS;
		else
			system.num_cpu=system.logicals_per_package; // credo sia cosi
		unsigned int i;
		system.bsp_cpu_num=0;
		for (i=0; i<system.num_cpu; i++) {
			cpus[i].apic_base=0xFEE00000;
			cpus[i].apic_id=i;
			cpus[i].flags=CPU_FLAG_ENABLED;
		}
		cpus[0].flags|=CPU_FLAG_BSP;
	}
	if (system.num_cpu>1)
		system.type=SYSTEM_TYPE_HARDWARE_THREADING;
	else
		system.type=SYSTEM_TYPE_UNIPROCESSOR;
}

void ia32_smp_use_default_config(int config) {
	// uso sempre e solo due processori, per qualsiasi config...
	system.num_cpu=2;
	system.num_ioapic=0;
	system.bsp_cpu_num=0;
	cpus[0].apic_base=0xFEE00000;
	cpus[0].apic_id=0;
	cpus[0].flags=CPU_FLAG_ENABLED|CPU_FLAG_BSP;
	cpus[1].apic_base=0xFEE00000;
	cpus[1].apic_id=1;
	cpus[1].flags=CPU_FLAG_ENABLED;
	system.num_ioapic=0; // credo cazzo, non c'è scritto da nessuna parte :|
	system.type=SYSTEM_TYPE_MULTIPROCESSOR;
}

void ia32_smp_print_system() {
	unsigned int i;
	switch (system.type) {
	case SYSTEM_TYPE_UNIPROCESSOR:
		kprint("smp: uniprocessor system, ");
		if (system.use_apic)
			kprint("local apic found, ");
		if (system.num_ioapic>0) {
			if (system.num_ioapic==1)
				kprint("ioapic found\n");
			else
				kprint("%u ioapics found\n");
		} else
			kprint("using pic8259\n");
		break;
	case SYSTEM_TYPE_MULTIPROCESSOR:
		kprint("smp: multiprocessor system, %u cpus, ",system.num_cpu);
		if (system.num_ioapic>0) {
			if (system.num_ioapic==1)
				kprint("ioapic found\n");
			else
				kprint("%u ioapics found\n");
		} else
			kprint("using pic8259\n");
		for (i=0; i<system.num_cpu; i++) {
			kprint("cpu%u: id = 0x%02x, ",i,cpus[i].apic_id);
			if (cpus[i].flags&CPU_FLAG_BSP)
				kprint("bootstrap, ");
			else
				kprint("application processor, ");
			if (cpus[i].flags&CPU_FLAG_ENABLED)
				kprint("enabled\n");
			else
				kprint("disabled\n");
		}
		break;
	case SYSTEM_TYPE_HARDWARE_THREADING:
		kprint("smp: multiprocessor system, %u cpus, ",system.num_cpu);
		if (system.num_ioapic>0) {
			if (system.num_ioapic==1)
				kprint("ioapic found\n");
			else
				kprint("%u ioapics found\n");
		} else
			kprint("using pic8259\n");
		if (system.cores_per_package>0)
			kprint("smp: dual-core technology found, %u cores/package, %u logicals/package\n",system.cores_per_package,system.logicals_per_package);
		else
			kprint("smp: hyper-threading technology found, %u logicals/package\n",system.logicals_per_package);
		for (i=0; i<system.num_cpu; i++) {
			kprint("cpu%u: id = 0x%02x, ",i,cpus[i].apic_id);
			if (cpus[i].flags&CPU_FLAG_BSP)
				kprint("bootstrap, ");
			else
				kprint("application processor, ");
			if (cpus[i].flags&CPU_FLAG_ENABLED)
				kprint("enabled\n");
			else
				kprint("disabled\n");
		}
	}
}

void ia32_smp_init_system() {
	unsigned int i;
	unsigned int ioapic;
	kprint(SMP_INIT2);
	// setuppo l'array
	for (i=0; i<IRQ_MAXNUM; i++)
		irq_to_cpu[i]=system.bsp_cpu_num; // per ora...
	// inizializzo gli interrupt controller
#ifdef SMP_USE_PIC8259_WHEN_UNIPROCESSOR
	if (system.num_cpu==1)
		system.num_ioapic=0;
#endif
#ifdef SMP_FORCE_PIC8259
	system.num_ioapic=0;
#endif
	if (system.num_ioapic==0) {
		kprint(SMP_IRQM);
		for (i=0; i<IRQ_MAXNUM; i++)
			irq_to_int[i]=0x20+i; // per forza è così, quindi forzo l'array...
		ia32_pic8259_init();
		kprint(SMP_OK);
	} else {
		ia32_pic8259_disable_all_irqs();
		for (ioapic=0; ioapic<system.num_ioapic; ioapic++)
			ia32_ioapic_init(ioapic);
	}
	// inizializzo gli irq
	ia32_irq_init(irq_to_int);
	// abilito tutti gli irq
	if (system.num_ioapic==0) {
		ia32_pic8259_enable_all_irqs();
	} else {
		for (ioapic=0; ioapic<system.num_ioapic; ioapic++)
			ia32_ioapic_enable_all_irqs(ioapic,irq_to_int,irq_to_cpu);
	}
	if (system.use_apic) {
		ia32_idt_set_desc(SMP_TIMER_INTERRUPT,KERNEL_CS_SEL,(unsigned)asm_smp_timer_handler,GATE_TYPE_32BIT_INTGATE,0);
		ia32_idt_set_desc(SMP_LINT0_INTERRUPT,KERNEL_CS_SEL,(unsigned)asm_smp_lint0_handler,GATE_TYPE_32BIT_INTGATE,0);
		ia32_idt_set_desc(SMP_LINT1_INTERRUPT,KERNEL_CS_SEL,(unsigned)asm_smp_lint1_handler,GATE_TYPE_32BIT_INTGATE,0);
		ia32_idt_set_desc(SMP_ERROR_INTERRUPT,KERNEL_CS_SEL,(unsigned)asm_smp_error_handler,GATE_TYPE_32BIT_INTGATE,0);
		ia32_idt_set_desc(SMP_PERF_INTERRUPT,KERNEL_CS_SEL,(unsigned)asm_smp_perf_handler,GATE_TYPE_32BIT_INTGATE,0);
		ia32_idt_set_desc(SMP_THERMAL_INTERRUPT,KERNEL_CS_SEL,(unsigned)asm_smp_thermal_handler,GATE_TYPE_32BIT_INTGATE,0);
		ia32_apic_init(system.bsp_cpu_num);
	}
	if (system.type!=SYSTEM_TYPE_UNIPROCESSOR) {
		ia32_smp_build_pm_tables();
		unsigned int curcpu;
		for (curcpu=0; curcpu<system.num_cpu; curcpu++) {
			if (cpus[curcpu].flags&CPU_FLAG_BSP || !(cpus[curcpu].flags&CPU_FLAG_ENABLED))
				continue;
			ia32_smp_setup_ap_boot(curcpu);
			ia32_apic_boot_ap(curcpu,SMP_AP_BOOT_PAGE+curcpu);
			ia32_smp_wait_for_cpu_to_boot(curcpu);
			kprint(APIC_BOOTED,curcpu);
		}
	}
	kprint(SMP_INITED);
}

void ia32_smp_timer_handler() {
	kprint("#");
	ia32_apic_eoi(system.bsp_cpu_num);
}

void ia32_smp_lint0_handler() {
	panic("SMP: LINT0\n");
}

void ia32_smp_lint1_handler() {
	panic("SMP: LINT1\n");
}

void ia32_smp_error_handler() {
	panic("SMP: ERROR\n");
}

void ia32_smp_perf_handler() {
	panic("SMP: PERF\n");
}

void ia32_smp_thermal_handler() {
	panic("SMP: THERMAL\n");
}

unsigned long ia32_smp_search_table(unsigned long start,unsigned long size) {
	unsigned long addr;
	for (addr=start; addr<(start+size); addr++) {
		mp_table_t *mp=(mp_table_t *)addr;
		if (mp->signature[0]=='_' && mp->signature[1]=='M' && mp->signature[2]=='P' && mp->signature[3]=='_') {
			unsigned char checksum=0;
			unsigned long i;
			for (i=0; i<sizeof(mp_table_t); i++)
				checksum+=*(unsigned char *)((unsigned long)mp+i);
			if (checksum==0)
				return (unsigned long)mp;
		}
	}
	return 0;
}

void ia32_smp_build_pm_tables() {
	ia32_gdt_raw_set_desc((ia32_desc_t *)(SMP_AP_GDT_ADDR+sizeof(ia32_desc_t)),0,0xFFFFFFFF,SEGMENT_TYPE_CODE|SEGMENT_TYPE_READABLE|DESCRIPTOR_32BIT|DESCRIPTOR_GRANULAR,0);
	ia32_gdt_raw_set_desc((ia32_desc_t *)(SMP_AP_GDT_ADDR+sizeof(ia32_desc_t)*2),0,0xFFFFFFFF,SEGMENT_TYPE_DATA|SEGMENT_TYPE_WRITABLE|DESCRIPTOR_32BIT|DESCRIPTOR_GRANULAR,0);
	ia32_gdt_raw_set_desc((ia32_desc_t *)(SMP_AP_GDT_ADDR+sizeof(ia32_desc_t)*3),0,0xFFFFFFFF,SEGMENT_TYPE_CODE|SEGMENT_TYPE_READABLE|SEGMENT_TYPE_CONFORMING|DESCRIPTOR_32BIT|DESCRIPTOR_GRANULAR,3);
	ia32_gdt_raw_set_desc((ia32_desc_t *)(SMP_AP_GDT_ADDR+sizeof(ia32_desc_t)*4),0,0xFFFFFFFF,SEGMENT_TYPE_DATA|SEGMENT_TYPE_WRITABLE|DESCRIPTOR_32BIT|DESCRIPTOR_GRANULAR,3);
	// poi devo settare l'idt...
}

extern const char g_ap_boot[];
extern const char g_ap_boot_end[];

void ia32_smp_setup_ap_boot(unsigned int cpu) {
	unsigned char boot_page=SMP_AP_BOOT_PAGE+cpu;
	// devo riservare le pagine di mem fisica per il codice dell'ap boot <-- TODO
	memcpy((void *)((boot_page&0xFF)*SMP_PAGE_SIZE),g_ap_boot,g_ap_boot_end-g_ap_boot);
	ap_boot_data_t *data=(ap_boot_data_t *)((boot_page&0xFF)*SMP_PAGE_SIZE+SMP_AP_BOOT_CODE_SIZE);
	data->booted=0;
	data->id=cpus[cpu].apic_id;
	data->cpu_num=cpu;
}

void ia32_smp_wait_for_cpu_to_boot(unsigned int cpu) {
	ap_boot_data_t *data=(ap_boot_data_t *)((SMP_AP_BOOT_PAGE+cpu)*SMP_PAGE_SIZE+SMP_AP_BOOT_CODE_SIZE);
	while (data->booted==0)
		asm("pause");
}	

void ia32_smp_ap_entry_point(unsigned int cpu,unsigned long id) {
	kprint("--> CPU%u is HERE!\n",cpu);
	while (1);
}

int ia32_smp_check_apic() {
	unsigned long eax,ebx,ecx,edx;
	ia32_cpuid(1,&eax,&ebx,&ecx,&edx);
	if (edx&(1<<9))
		return 0;
	else
		return -1;
}

int ia32_smp_check_hardware_threading() {
	unsigned long eax,ebx,ecx,edx;
	ia32_cpuid(1,&eax,&ebx,&ecx,&edx);
	if (edx&(1<<28))
		return 0;
	else
		return -1;
}

unsigned char ia32_smp_get_int_by_irq(unsigned int irqn) {
	if (irqn>=IRQ_MAXNUM)
		return 0;
	return irq_to_int[irqn];
}
