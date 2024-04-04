/*!	\file arch/i386/kernel/mem.c
 *	\brief Low level (i386) memory manager.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-01-20
 *	\note Copyright (&copy;) 2003 Andrea Righi
 *
 *	\n
 *	For a complete documentation about GDT and descriptors
 *	you can see the "IA-32 Intel® Architecture Software Developer's
 *	Manual - Volume 3: System Programming Guide"
 *	(http://www.intel.com/design/pentium4/manuals).
 *	\n
 *	For an intalian documentation you can download the
 *	"Minirighi32 - Documentazione Ufficiale"
 *	(http://sourceforge.net/project/showfiles.php?group_id=80923&release_id=180542).
 *
 */

#include <const.h>
#include <string.h>

#include <arch/i386.h>
#include <arch/paging.h>
#include <arch/panic.h>

#include <kernel/console.h>
#include <kernel/task.h>

#include <arch/mem.h>

//! Global Descriptor Table (GDT).
volatile gdt_entry_t gdt[GDT_DIM];
//! GDT pointer.
static gdt_reg gdt_ptr;

//! \brief Initialize a GDT entry.
//! \param limit The size of the memory segment.
//! \param base The starting linear address of the memory segment.
//! \param attribs0_7
//!		The first 8-bit of the segment descriptor attributes.
//! \param attribs8_15
//!		The last 8-bit of the segment descriptor attributes.
//! \return
//!	The selector of the new GDT entry.
word setup_GDT_entry(word limit, dword base, byte attribs0_7, byte attribs8_15)
{
	// Skip the dummy entry (#0).
	register unsigned short i = 1;
	uint32_t flags;

	local_irq_save( flags );

	for( ; i < GDT_DIM; i++ )
	{
		if( !(gdt[i].attribs0_7) )
		{
			gdt[i].limit = limit;
			gdt[i].base0_15 = base & 0xFFFF;
			gdt[i].base16_23 = (base >> 16) & 0xFF;
			gdt[i].base24_31 = (base >> 24) & 0xFF;
			gdt[i].attribs0_7 = attribs0_7;
			gdt[i].attribs8_15 = attribs8_15;

			local_irq_restore( flags );

			// Return the selector.
			return(i * GDT_ENTRY_DIM);
		}
	}
	return( NULL );
}

//! \brief Remove a GDT entry.
//! \param sel The selector to remove from the GDT.
void remove_GDT_entry(word sel)
{
	uint32_t flags;

	local_irq_save( flags );

	gdt[sel/GDT_ENTRY_DIM].limit = 0;
	gdt[sel/GDT_ENTRY_DIM].base0_15 = 0;
	gdt[sel/GDT_ENTRY_DIM].base16_23 = 0;
	gdt[sel/GDT_ENTRY_DIM].base24_31 = 0;
	gdt[sel/GDT_ENTRY_DIM].attribs0_7 = 0;
	gdt[sel/GDT_ENTRY_DIM].attribs8_15 = 0;

	local_irq_restore( flags );
}

extern tss_t double_fault_tss;

//! \brief
//!	Initialize the Global Descriptor Table with the default
//!	selectors and update the segment registers.
void __INIT__ install_GDT()
{
	// First of all reset all entries in the GDT //
	memset(&gdt, 0, sizeof(gdt));

	// ------------- Reserved entries in the GDT ------------------ //

	// Dummy descriptor.
	setup_GDT_entry(0, 0, 0, 0);
	// KERNEL_DATA descriptor 0x08.
	setup_GDT_entry(0xFFFF, 0, DATA_SEG, (ATTR_GRANUL | ATTR_32BIT | 0xF));
	// KERNEL_CODE descriptor 0x10.
	setup_GDT_entry(0xFFFF, 0, CODE_SEG, (ATTR_GRANUL | ATTR_32BIT | 0xF));
	// USER_DATA descriptor 0x18.
	setup_GDT_entry(0xFFFF, 0, (DATA_SEG | DPL_3), (ATTR_GRANUL | ATTR_32BIT | 0xF));
	// USER_CODE descriptor 0x20.
	setup_GDT_entry(0xFFFF, 0, (CODE_SEG | DPL_3), (ATTR_GRANUL | ATTR_32BIT | 0xF));

	// GDT_DOUBLE_FAULT_ENTRY descriptor 0x28.
	setup_GDT_entry( sizeof(double_fault_tss)-1,
			(uint32_t)&double_fault_tss,
			TSS_SEG, ATTR_32BIT );

	// --------- End of reserved entries in the GDT --------------- //

	// Set up the GDT pointer //
	gdt_ptr.limit = (uint16_t)(GDT_DIM * GDT_ENTRY_DIM - 1);
	gdt_ptr.base = (size_t)gdt;

	// Load info into GDTR //
	__asm__ __volatile__ ("lgdtl (%0)" : : "r"(&gdt_ptr));

	// Update segment registers //
	k_update_segment_regs();
}
