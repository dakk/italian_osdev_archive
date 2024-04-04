/*!	\file drivers/pci/pci.c
 *	\brief PCI bus driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-09
 *	\note Copyright (&copy;) 2003 Andrea Righi
 *
 *	\n
 *	To make this driver I have gotten some information from
 *	"Linux" and "The Mobius" drivers. Thanks!\n
 *	-Andrea Righi.
 */

#include <const.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <arch/clock.h>
#include <arch/i386.h>

#include <kernel/console.h>
#include <kernel/keyboard.h>
#include <kernel/kmalloc.h>

#include <kernel/pci.h>

//! The amount of supported PCI classes.
#define PCI_CLASS_ENTRIES	( sizeof(classes)/sizeof(classes_t) )

//! PCI supported classes. It is used to recognize to what
//! class a device belongs.
classes_t classes[] =
{
    { 0x00, 0x00, 0x00, "Undefined" },
    { 0x00, 0x01, 0x00, "VGA" },

    { 0x01, 0x00, 0x00, "SCSI" },
    { 0x01, 0x01, 0x00, "IDE" },
    { 0x01, 0x01, 0x8A, "Controller IDE via Bus Master" },
    { 0x01, 0x02, 0x00, "Floppy" },
    { 0x01, 0x03, 0x00, "IPI" },
    { 0x01, 0x04, 0x00, "RAID" },
    { 0x01, 0x05, 0x20, "ATA (Single DMA)" },
    { 0x01, 0x05, 0x30, "ATA (Chained DMA)" },
    { 0x01, 0x80, 0x00, "Other" },

    { 0x02, 0x00, 0x00, "Ethernet" },
    { 0x02, 0x01, 0x00, "Token Ring" },
    { 0x02, 0x02, 0x00, "FDDI" },
    { 0x02, 0x03, 0x00, "ATM" },
    { 0x02, 0x04, 0x00, "ISDN" },
    { 0x02, 0x05, 0x00, "WordFip" },
    { 0x02, 0x06, 0x00, "PICMG 2.14" },
    { 0x02, 0x80, 0x00, "Other" },

    { 0x03, 0x00, 0x00, "VGA" },
    { 0x03, 0x00, 0x01, "VGA+8514" },
    { 0x03, 0x01, 0x00, "XGA" },
    { 0x03, 0x02, 0x00, "3D" },
    { 0x03, 0x80, 0x00, "Other" },

    { 0x04, 0x00, 0x00, "Video" },
    { 0x04, 0x01, 0x00, "Audio" },
    { 0x04, 0x02, 0x00, "Telephony" },
    { 0x04, 0x80, 0x00, "Other" },

    { 0x05, 0x00, 0x00, "RAM" },
    { 0x05, 0x01, 0x00, "Flash" },
    { 0x05, 0x80, 0x00, "Other" },

    { 0x06, 0x00, 0x00, "PCI to HOST" },
    { 0x06, 0x01, 0x00, "PCI to ISA" },
    { 0x06, 0x02, 0x00, "PCI to EISA" },
    { 0x06, 0x03, 0x00, "PCI to MCA" },
    { 0x06, 0x04, 0x00, "PCI to PCI" },
    { 0x06, 0x04, 0x01, "PCI to PCI (Subtractive Decode)" },
    { 0x06, 0x05, 0x00, "PCI to PCMCIA" },
    { 0x06, 0x06, 0x00, "PCI to NuBUS" },
    { 0x06, 0x07, 0x00, "PCI to Cardbus" },
    { 0x06, 0x08, 0x00, "PCI to RACEway (Transparent)" },
    { 0x06, 0x08, 0x01, "PCI to RACEway (End-point)" },
    { 0x06, 0x09, 0x00, "PCI to PCI" },
    { 0x06, 0x09, 0x40, "PCI to PCI (Primary bus to host)" },
    { 0x06, 0x09, 0x80, "PCI to PCI (Secondary bus to host)" },
    { 0x06, 0x0A, 0x00, "PCI to InfiBand" },
    { 0x06, 0x80, 0x00, "PCI to Other" },

    { 0x07, 0x00, 0x00, "Serial" },
    { 0x07, 0x00, 0x01, "Serial - 16450" },
    { 0x07, 0x00, 0x02, "Serial - 16550" },
    { 0x07, 0x00, 0x03, "Serial - 16650" },
    { 0x07, 0x00, 0x04, "Serial - 16750" },
    { 0x07, 0x00, 0x05, "Serial - 16850" },
    { 0x07, 0x00, 0x06, "Serial - 16950" },
    { 0x07, 0x01, 0x00, "Parallel" },
    { 0x07, 0x01, 0x01, "Parallel - BiDir" },
    { 0x07, 0x01, 0x02, "Parallel - ECP" },
    { 0x07, 0x01, 0x03, "Parallel - IEEE1284" },
    { 0x07, 0x01, 0xFE, "Parallel - IEEE1284 Target" },
    { 0x07, 0x02, 0x00, "Multiport Serial" },
    { 0x07, 0x03, 0x00, "Hayes Compatible Modem" },
    { 0x07, 0x03, 0x01, "Hayes Compatible Modem, 16450" },
    { 0x07, 0x03, 0x02, "Hayes Compatible Modem, 16550" },
    { 0x07, 0x03, 0x03, "Hayes Compatible Modem, 16650" },
    { 0x07, 0x03, 0x04, "Hayes Compatible Modem, 16750" },
    { 0x07, 0x80, 0x00, "Other" },

    { 0x08, 0x00, 0x00, "PIC" },
    { 0x08, 0x00, 0x01, "ISA PIC" },
    { 0x08, 0x00, 0x02, "EISA PIC" },
    { 0x08, 0x00, 0x10, "I/O APIC" },
    { 0x08, 0x00, 0x20, "I/O(x) APIC" },
    { 0x08, 0x01, 0x00, "DMA" },
    { 0x08, 0x01, 0x01, "ISA DMA" },
    { 0x08, 0x01, 0x02, "EISA DMA" },
    { 0x08, 0x02, 0x00, "Timer" },
    { 0x08, 0x02, 0x01, "ISA Timer" },
    { 0x08, 0x02, 0x02, "EISA Timer" },
    { 0x08, 0x03, 0x00, "RTC" },
    { 0x08, 0x03, 0x01, "ISA RTC" },
    { 0x08, 0x04, 0x00, "Hot-Plug" },
    { 0x08, 0x80, 0x00, "Other" },

    { 0x09, 0x00, 0x00, "Keyboard" },
    { 0x09, 0x01, 0x00, "Pen" },
    { 0x09, 0x02, 0x00, "Mouse" },
    { 0x09, 0x03, 0x00, "Scanner" },
    { 0x09, 0x04, 0x00, "Game Port" },
    { 0x09, 0x04, 0x10, "Game Port (Legacy)" },
    { 0x09, 0x80, 0x00, "Other" },

    { 0x0a, 0x00, 0x00, "Generic" },
    { 0x0a, 0x80, 0x00, "Other" },

    { 0x0b, 0x00, 0x00, "386" },
    { 0x0b, 0x01, 0x00, "486" },
    { 0x0b, 0x02, 0x00, "Pentium" },
    { 0x0b, 0x03, 0x00, "PentiumPro" },
    { 0x0b, 0x10, 0x00, "DEC Alpha" },
    { 0x0b, 0x20, 0x00, "PowerPC" },
    { 0x0b, 0x30, 0x00, "MIPS" },
    { 0x0b, 0x40, 0x00, "Coprocessor" },
    { 0x0b, 0x80, 0x00, "Other" },

    { 0x0c, 0x00, 0x00, "FireWire" },
    { 0x0c, 0x00, 0x10, "OHCI FireWire" },
    { 0x0c, 0x01, 0x00, "Access.bus" },
    { 0x0c, 0x02, 0x00, "SSA" },
    { 0x0c, 0x03, 0x00, "USB (UHCI)" },
    { 0x0c, 0x03, 0x10, "USB (OHCI)" },
    { 0x0c, 0x03, 0x20, "USB (EHCI)" },
    { 0x0c, 0x03, 0x80, "USB" },
    { 0x0c, 0x03, 0xFE, "USB Device" },
    { 0x0c, 0x04, 0x00, "Fiber" },
    { 0x0c, 0x05, 0x00, "SMBus Controller" },
    { 0x0c, 0x06, 0x00, "InfiniBand" },
    { 0x0c, 0x80, 0x00, "Other" },

    { 0x0d, 0x00, 0x00, "iRDA" },
    { 0x0d, 0x01, 0x00, "Consumer IR" },
    { 0x0d, 0x10, 0x00, "RF" },
    { 0x0d, 0x80, 0x00, "Other" },

    { 0x0e, 0x00, 0x00, "I2O" },
    { 0x0e, 0x80, 0x00, "Other" },

    { 0x0f, 0x01, 0x00, "TV" },
    { 0x0f, 0x02, 0x00, "Audio" },
    { 0x0f, 0x03, 0x00, "Voice" },
    { 0x0f, 0x04, 0x00, "Data" },
    { 0x0f, 0x80, 0x00, "Other" },

    { 0x10, 0x00, 0x00, "Network" },
    { 0x10, 0x10, 0x00, "Entertainment" },
    { 0x10, 0x80, 0x00, "Other" },

    { 0x11, 0x00, 0x00, "DPIO Modules" },
    { 0x11, 0x01, 0x00, "Performance Counters" },
    { 0x11, 0x10, 0x00, "Comm Sync, Time+Frequency Measurement" },
    { 0x11, 0x20, 0x00, "Management Card" },
    { 0x11, 0x80, 0x00, "Other" },
};

//! \brief
//!	Read a configuration byte/word/dword from the PCI
//!	controller.
//! \param bus The bus number.
//! \param dev The device number.
//! \param func The function number.
//! \param reg The PCI register.
//! \param uint8_ts
//!	The size of the read operation:
//!	\li 1 = 8-bit;
//!	\li 2 = 16-bit;
//!	\li 4 = 32-bit.
//! \return The byte/word/dword read from the controller.
//! \note
//!	Thanks to "The Mobius" operating system.
uint32_t pciRead(int bus, int dev, int func, int reg, int uint8_ts)
{
	uint16_t base;

	union {
		confadd_t c;
		uint32_t n;
	} u;

	u.n = 0;
	u.c.enable = 1;
	u.c.rsvd = 0;
	u.c.bus = bus;
	u.c.dev = dev;
	u.c.func = func;
	u.c.reg = reg & 0xFC;

	out32(0xCF8, u.n);
	base = 0xCFC + (reg & 0x03);

	switch(uint8_ts)
	{
		case 1: return in(base);
		case 2: return in16(base);
		case 4: return in32(base);
		default: return 0;
	}
}

//! \brief
//!	Write a configuration byte/word/dword to the PCI
//!	controller.
//! \param bus The bus number.
//! \param dev The device number.
//! \param func The function number.
//! \param reg The PCI register.
//! \param v The value to write.
//! \param uint8_ts
//!	The size of the write operation:
//!	\li 1 = 8-bit;
//!	\li 2 = 16-bit;
//!	\li 4 = 32-bit.
//! \note
//!	Thanks to "The Mobius" operating system.
void pciWrite(int bus, int dev, int func, int reg, uint32_t v, int uint8_ts)
{
	uint16_t base;

	union {
		confadd_t c;
		uint32_t n;
	} u;

	u.n = 0;
	u.c.enable = 1;
	u.c.rsvd = 0;
	u.c.bus = bus;
	u.c.dev = dev;
	u.c.func = func;
	u.c.reg = reg & 0xFC;

	base = 0xCFC + (reg & 0x03);
	out32(0xCF8, u.n);

	switch(uint8_ts)
	{
		case 1: out(base, (uint8_t) v); break;
		case 2: out16(base, (uint16_t) v); break;
		case 4: out32(base, v); break;
	}
}

// --- PCI read functions --------------------------------------------- //

//! \brief
//!	Read a configuration byte from the PCI controller.
//! \param bus The bus number.
//! \param dev The device number.
//! \param func The function number.
//! \param reg The PCI register.
//! \return The byte read from the controller.
uint8_t pci_read_config_byte(int bus, int dev, int func, int reg)
{
	return ( pciRead(bus, dev, func, reg, sizeof(byte)) );
}

//! \brief
//!	Read a configuration word from the PCI controller.
//! \param bus The bus number.
//! \param dev The device number.
//! \param func The function number.
//! \param reg The PCI register.
//! \return The word read from the controller.
uint16_t pci_read_config_word(int bus, int dev, int func, int reg)
{
	return ( pciRead(bus, dev, func, reg, sizeof(word)) );
}

//! \brief
//!	Read a configuration double word from the PCI controller.
//! \param bus The bus number.
//! \param dev The device number.
//! \param func The function number.
//! \param reg The PCI register.
//! \return The dword read from the controller.
uint32_t pci_read_config_dword(int bus, int dev, int func, int reg)
{
	return ( pciRead(bus, dev, func, reg, sizeof(dword)) );
}

// --- PCI write functions -------------------------------------------- //

//! \brief
//!	Write a configuration byte to the PCI controller.
//! \param bus The bus number.
//! \param dev The device number.
//! \param func The function number.
//! \param reg The PCI register.
//! \param val The byte to write.
void pci_write_config_byte(int bus, int dev, int func, int reg, uint8_t val)
{
	pciWrite(bus, dev, func, reg, val, sizeof(byte));
}

//! \brief
//!	Write a configuration word to the PCI controller.
//! \param bus The bus number.
//! \param dev The device number.
//! \param func The function number.
//! \param reg The PCI register.
//! \param val The word to write.
void pci_write_config_word(int bus, int dev, int func, int reg, uint16_t val)
{
	pciWrite(bus, dev, func, reg, val, sizeof(word));
}

//! \brief
//!	Write a configuration double word to the PCI controller.
//! \param bus The bus number.
//! \param dev The device number.
//! \param func The function number.
//! \param reg The PCI register.
//! \param val The dword to write.
void pci_write_config_dword(int bus, int dev, int func, int reg, uint32_t val)
{
	pciWrite(bus, dev, func, reg, val, sizeof(dword));
}

// --- PCI utility functions ------------------------------------------ //

//! \brief Read the IRQ line of the device if it is present.
//! \param cfg The PCI device structure.
//! \note This routine sets up the pci_cfg_t::irq field.
void pci_read_irq(pci_cfg_t *cfg)
{
	unsigned char irq;

	irq = pci_read_config_byte(cfg->bus, cfg->dev, cfg->func, PCI_INTERRUPT_PIN);
	if (irq)
		irq = pci_read_config_byte(cfg->bus, cfg->dev, cfg->func, PCI_INTERRUPT_LINE);

	cfg->irq = irq;
}

//! \brief Calculate the size of an I/O space.
//! \param base The size read from the controller.
//! \param mask The address mask of the I/O space.
//! \return
//!	The size of the given I/O space aligned by the mask.
//! \note
//!	For memory-based devices the size of an I/O space is the
//!	size of the memory-mapped buffer; for I/O based devices it
//!	is the maximum offset of the ports.
uint32_t pci_size(uint32_t base, unsigned long mask)
{
	// Find the significant bits					//
	uint32_t size = mask & base;
	// Get the lowest of them to find the decode size		//
	size = size & ~(size-1);
	// extent = size - 1						//
	return(size-1);
}

//! \brief Read the base addresses of the selected deivice.
//! \param cfg The PCI device structure.
//! \param tot_bases
//!	The amount of bases to read.
//!	Every PCI device has up to 6 base addresses (6 for
//!	normal devices, 2 for PCI to PCI bridges and only 1 for
//!	cardbuses).
//! \param rom
//!	The ROM address register (from this register we can read the
//!	ROM base address and the ROM space size).
//! \note
//!	This routine sets up the pci_cfg_t::base field and the
//!	pci_cfg_t::rom_base field.
void pci_read_bases(pci_cfg_t *cfg, int tot_bases, int rom)
{
	uint32_t l, sz, reg;
	int i;

	// Clear all previous bases and sizes informations		//
	memset(cfg->base, 0, sizeof(cfg->base));
	memset(cfg->size, 0, sizeof(cfg->size));
	memset(cfg->type, 0, sizeof(cfg->type));

	// Read informations about bases and sizes			//
	for(i=0; i<tot_bases; i++)
	{
		// Read bases and size					//
		reg = PCI_BASE_ADDRESS_0 + (i << 2);
		l = pci_read_config_dword(cfg->bus, cfg->dev, cfg->func, reg);
		pci_write_config_dword(cfg->bus, cfg->dev, cfg->func, reg, ~0);
		sz = pci_read_config_dword(cfg->bus, cfg->dev, cfg->func, reg);
		pci_write_config_dword(cfg->bus, cfg->dev, cfg->func, reg, l);

		// Check if informations are valid			//
		if (!sz || sz == 0xFFFFFFFF)
			continue;
		if (l == 0xFFFFFFFF)
			l = 0;

		// Store informations					//
		if ( (l & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY )
		{
			cfg->base[i] = l & PCI_BASE_ADDRESS_MEM_MASK;
			cfg->size[i] = pci_size(sz, PCI_BASE_ADDRESS_MEM_MASK);
			cfg->type[i] = PCI_IO_RESOURCE_MEM;
		}
		else
		{
			cfg->base[i] = l & PCI_BASE_ADDRESS_IO_MASK;
			cfg->size[i] = pci_size(sz, PCI_BASE_ADDRESS_IO_MASK);
			cfg->type[i] = PCI_IO_RESOURCE_IO;
		}
	}

	// --- ROM ---							//
	if (rom)
	{
		// Initialize values					//
		cfg->rom_base = 0;
		cfg->rom_size = 0;

		l = pci_read_config_dword(cfg->bus, cfg->dev, cfg->func, rom);
		pci_write_config_dword(cfg->bus, cfg->dev, cfg->func, rom, ~PCI_ROM_ADDRESS_ENABLE);
		sz = pci_read_config_dword(cfg->bus, cfg->dev, cfg->func, rom);
		pci_write_config_dword(cfg->bus, cfg->dev, cfg->func, rom, l);
		if (l == 0xFFFFFFFF)
			l = 0;
		if (sz && sz != 0xFFFFFFFF)
		{
			cfg->rom_base = l & PCI_ROM_ADDRESS_MASK;
			sz = pci_size(sz, PCI_ROM_ADDRESS_MASK);
			cfg->rom_size = cfg->rom_size + (unsigned long)sz;
		}
	}
}

//! \brief Probe for a PCI device.
//! \param bus The bus number.
//! \param dev The device number.
//! \param func The function number.
//! \param cfg The PCI device structure.
//! \note This routine sets up the pci_cfg_t structure.
//! \return
//!	\li #TRUE if a device is present at the configuration
//!	(\p bus, \p dev, \p func);
//!	\li #FALSE the device is not present.
bool pci_probe(int bus, int dev, int func, pci_cfg_t *cfg)
{
	uint32_t *temp = (uint32_t *) cfg;
	int i;

	for(i=0; i<4; i++)
		temp[i] = pci_read_config_dword(bus, dev, func, (i << 2));

	if(cfg->vendor_id == 0xFFFF) return FALSE;

	// Setup the bus, device and function number			//
	cfg->bus = bus;
	cfg->dev = dev;
	cfg->func = func;

	// Set the power state to unknown				//
	cfg->current_state = 4;

	// Identify the type of the device				//
	switch(cfg->header_type & 0x7F)
	{
		case PCI_HEADER_TYPE_NORMAL:
		// --- NORMAL DEVICE ---				//
		// Read the IRQ line					//
		pci_read_irq(cfg);
		// Read the base memory and I/O addresses		//
		pci_read_bases(cfg, 6, PCI_ROM_ADDRESS);
		// Read subsysem vendor and subsystem device id		//
		cfg->subsys_vendor = pci_read_config_word(bus, dev, func, PCI_SUBSYSTEM_VENDOR_ID);
		cfg->subsys_device = pci_read_config_word(bus, dev, func, PCI_SUBSYSTEM_ID);
		break;

		case PCI_HEADER_TYPE_BRIDGE:
		// --- PCI <-> PCI BRIDGE ---				//
		pci_read_bases(cfg, 2, PCI_ROM_ADDRESS_1);
		break;

		case PCI_HEADER_TYPE_CARDBUS:
		// --- PCI CARDBUS ---					//
		// Read the IRQ line					//
		pci_read_irq(cfg);
		// Read the base memory and I/O addresses		//
		pci_read_bases(cfg, 1, 0);
		// Read subsysem vendor and subsystem device id		//
		cfg->subsys_vendor = pci_read_config_word(bus, dev, func, PCI_CB_SUBSYSTEM_VENDOR_ID);
		cfg->subsys_device = pci_read_config_word(bus, dev, func, PCI_CB_SUBSYSTEM_ID);
		break;

		default:
		// --- UNKNOW HEADER TYPE ---				//
		break;
	}

	return(TRUE);
}

//! SiS 5597 and 5598 require latency timer set to at most 32 to avoid
//! lockups; otherwise default value is 255.
unsigned int pcibios_max_latency=255;

//! \brief Enable bus-mastering (aka 32-bit DMA) for a PCI device.
//! \param cfg The PCI device structure.
void pci_set_master(pci_cfg_t *cfg)
{
	uint16_t cmd;
	uint8_t lat;

	cmd = pci_read_config_word(cfg->bus, cfg->dev, cfg->func, PCI_COMMAND);
	if ( !(cmd & PCI_COMMAND_MASTER) )
	{
		kprintf("PCI: Enabling bus mastering for device in slot %d:%d:%d\n", cfg->bus, cfg->dev, cfg->func);
		cmd |= PCI_COMMAND_MASTER;
		pci_write_config_word(cfg->bus, cfg->dev, cfg->func, PCI_COMMAND, cmd);
	}
	// Check the latency time, because certain BIOSes forget to set	//
	// it properly...						//
	lat = pci_read_config_byte(cfg->bus, cfg->dev, cfg->func, PCI_LATENCY_TIMER);
	if ( lat < 16 )
		lat = (64 <= pcibios_max_latency) ? 64 : pcibios_max_latency;
	else if ( lat > pcibios_max_latency )
		lat = pcibios_max_latency;
	else
		return;
	kprintf("PCI: Setting latency timer of device %d:%d:%d to %u\n", cfg->bus, cfg->dev, cfg->func, lat);
	pci_write_config_byte(cfg->bus, cfg->dev, cfg->func, PCI_LATENCY_TIMER, lat);
}

//! \brief Tell if a device supports a given PCI capability.
//! \param cfg The PCI device structure.
//! \param cap
//!	The given capability (see pci.h PCI_CAP_* for a
//!	complete list of supported capabilities).
//! \return
//!	\li 0 if the capability is not supported;
//!	\li otherwise it returns the position of the supported
//!	capability in the capability list of this device.
int pci_find_capability(pci_cfg_t *cfg, int cap)
{
	uint16_t status;
	uint8_t pos, id;
	int ttl = 48;

	status = pci_read_config_word(cfg->bus, cfg->dev, cfg->func, PCI_STATUS);
	if ( !(status & PCI_STATUS_CAP_LIST) )
		return(0);

	switch (cfg->header_type & 0x7F)
	{
		case PCI_HEADER_TYPE_NORMAL:
		case PCI_HEADER_TYPE_BRIDGE:
		pos = pci_read_config_byte(cfg->bus, cfg->dev, cfg->func, PCI_CAPABILITY_LIST);
		break;

		case PCI_HEADER_TYPE_CARDBUS:
		pos = pci_read_config_byte(cfg->bus, cfg->dev, cfg->func, PCI_CB_CAPABILITY_LIST);
		break;

		default:
		return(0);
		break;
	}

	while (ttl-- && pos>=0x40)
	{
		pos &= ~3;
		id = pci_read_config_byte(cfg->bus, cfg->dev, cfg->func, pos+PCI_CAP_LIST_ID);
		if (id == 0xff)
			break;
		if (id == cap)
			return(pos);
		pos = pci_read_config_byte(cfg->bus, cfg->dev, cfg->func, pos+PCI_CAP_LIST_NEXT);
	}
	return(0);
}

//! \brief
//!	Set a new power state for the device using the Power
//!	Management Capabilities.
//! \param cfg
//!	The PCI device structure.
//! \param state The power state (from D0 to D3).
//! \return
//!	\li 0 if we can successfully change the power state;
//!	\li -#EIO if the device doesn't support PCI Power Management;
//!	\li -#EINVAL if trying to enter to a state we're already in.
int pci_set_power_state(pci_cfg_t *cfg, int state)
{
	int pm;
	uint16_t pmcsr;
	uint16_t pmc;

	// Bound the state to a valid range				//
	if (state > 3) state = 3;

	// Validate current state.					//
	// Can enter D0 from any state, but we can't go deeper if we're //
	// in a low power state.					//
	if (state > 0 && cfg->current_state > state)
		return(-EINVAL);
	else if (cfg->current_state == state)
		// we're already there 					//
		return(0);

	// find PCI PM capability in list 				//
	pm = pci_find_capability(cfg, PCI_CAP_ID_PM);

	// Abort if the device doesn't support PM capabilities		//
	if (!pm) return(-EIO);

	// Check if this device supports the desired state		//
	if (state == 1 || state == 2)
	{
		pmc = pci_read_config_word(cfg->bus, cfg->dev, cfg->func, pm+PCI_PM_PMC);
		if ( (state == 1 && !(pmc & PCI_PM_CAP_D1)) )
			return(-EIO);
		else if ( (state == 2 && !(pmc & PCI_PM_CAP_D2)) )
			return(-EIO);
	}

	// If we're in D3, force entire word to 0.			//
	// This doesn't affect PME_Status, disables PME_En, and		//
	// sets PowerState to 0.					//
	if ( cfg->current_state>=3 )
		pmcsr = 0;
	else
	{
		pmcsr = pci_read_config_word(cfg->bus, cfg->dev, cfg->func, pm+PCI_PM_CTRL);
		pmcsr &= ~PCI_PM_CTRL_STATE_MASK;
		pmcsr |= state;
	}

	// Enter specified state //
	pci_write_config_word(cfg->bus, cfg->dev, cfg->func, pm+PCI_PM_CTRL, pmcsr);

	// Mandatory power management transition delays			//
	// see PCI PM 1.1 5.6.1 table 18				//
	if( (state == 3) || (cfg->current_state == 3) )
	{
		// Set task state to interruptible			//
		// LINUX do it so:					//
		// 	set_current_state(TASK_UNINTERRUPTIBLE);	//
		// 	schedule_timeout(HZ/100);			//
		delay(HZ/100);
	}
	else if( (state == 2) || (cfg->current_state == 2) )
		// udelay(200);
		delay(200);
	cfg->current_state = state;

	return(0);
}

//! \brief
//!	Low level function to initialize a PCI device before it's
//!	used by a driver.
//! \param cfg The PCI device structure.
//! \note
//!	This is a low-level routine, so it is strongly recommended
//!	to do not use it externally.
//!	In this case you can use pci_enable_device() instead.
int pcibios_enable_device_io(pci_cfg_t *cfg)
{
	uint16_t cmd, old_cmd;
	int i;

	kprintf("\n\rLow level enabling PCI device %d:%d:%d... ", cfg->bus, cfg->dev, cfg->func);

	old_cmd = cmd = pci_read_config_word(cfg->bus, cfg->dev, cfg->func, PCI_COMMAND);
	for (i=0; i<sizeof(cfg->type); i++)
		if (cfg->type[i] == PCI_IO_RESOURCE_IO)
			// Command IO based				//
			cmd |= PCI_COMMAND_IO;

	if ( !(cmd & PCI_COMMAND_IO) )
	{
		// Device is not IO-based				//
		kprintf("\n\rDevice is not IO-based!!!");
		return(-EINVAL);
	}

	if ( (cfg->header_type & 0x7F) == PCI_HEADER_TYPE_BRIDGE )
	{
		// Any PCI-to-PCI bridge must be enabled by setting	//
		// both I/O space and memory space access bits in the	//
		// command register.					//
		cmd |= PCI_COMMAND_MEMORY;
	}

	// Always enable bus master!!!					//
	cmd |= PCI_COMMAND_MASTER;

	if ( cmd!=old_cmd )
	{
		// Set the cache line and default latency (32)			//
		pci_write_config_word(cfg->bus, cfg->dev, cfg->func,
				PCI_CACHE_LINE_SIZE, (32 << 8) | (L1_CACHE_BYTES / sizeof(uint32_t)));
		// Enable the appropriate bits in the PCI command register	//
		pci_write_config_word(cfg->bus, cfg->dev, cfg->func, PCI_COMMAND, cmd);
		kprintf("OK!");
	}
	else
		kprintf("Already enabled.");
	return(0);
}

//! \brief Initialize a PCI device before it's used by a driver.
//! \param cfg The PCI device structure.
int pci_enable_device(pci_cfg_t *cfg)
{
	int err, pm;

	kprintf("\n\rPowering on PCI device %d:%d:%d... ", cfg->bus, cfg->dev, cfg->func);
	pm = pci_set_power_state(cfg, 0);
	switch( pm )
	{
		case 0:
		kprintf("OK!");
		break;

		case (-EIO):
		kprintf("\n\rDevice doesn't support Power Management Capabilities!!!");
		break;

		case (-EINVAL):
		kprintf("\n\rDevice is already in this power state.");
		break;
	}

	if ((err = pcibios_enable_device_io(cfg)) < 0)
		return(err);
	return(0);
}

//! \brief
//!	Look for a device in the PCI bus and eventually enable it.
//! \param cfg The PCI device configuration structure.
//! \param enable
//!	\li #TRUE enable the device if present;
//!	\li #FALSE simply look for a device without enabling it.
//! \return
//!	\li #TRUE a device has been found;
//!	\li #FALSE device not found.
bool pci_find_cfg(pci_cfg_t *cfg, bool enable)
{
	uint16_t bus, dev, func;
	pci_cfg_t *pcfg;

	pcfg = kmalloc( sizeof(pci_cfg_t), GFP_KERNEL );

	for (bus=0; bus<4; bus++)
	for (dev=0; dev<32; dev++)
	for (func=0; func<8; func++)
	{
		if ( pci_probe(bus, dev, func, pcfg) )
		{
			if (
				cfg->base_class == pcfg->base_class &&
				cfg->sub_class == pcfg->sub_class &&
				cfg->interface == pcfg->interface
			)
			{
				// Device found				//
				memcpy(cfg, pcfg, sizeof(pci_cfg_t));
				// Enable the device if required	//
				if (enable) pci_enable_device(pcfg);
				// Free the temporary structure		//
				kfree(pcfg);
				return(TRUE);
			}
		}
	}
	// Device not found						//
	kfree(pcfg);
	return(FALSE);
}

//! \brief
//!	Scan all the PCI buses, looking for devices.
//!	If a device is found it will be enabled.
void pci_scan()
{
	uint16_t bus, dev, func;
	pci_cfg_t pcfg;
	int i, key;

	for (bus=0; bus<4; bus++)
	for (dev = 0; dev < 32; dev++)
	for (func = 0; func < 8; func++)
	{
		if ( pci_probe(bus, dev, func, &pcfg) )
		{
			set_color(WHITE);
			kprintf("\n\rPCI:%u:%u:%u", bus, dev, func);
			set_color(DEFAULT_COLOR);

			kprintf(	"\n\rVendor       :%04X Device       :%04X"
				"\n\rSubSys_Vendor:%04X SubSys_Device:%04X",
				pcfg.vendor_id, pcfg.device_id, pcfg.subsys_vendor, pcfg.subsys_device);
			kprintf(	"\n\rBase_Class   :%02X   Sub_Class    :%02X   Interface    :%02X",
				pcfg.base_class, pcfg.sub_class, pcfg.interface);

			for (i=0;; i++)
			{
				if ( i>=PCI_CLASS_ENTRIES )
				{
					kprintf("\n\r* Description : Unknown device!");
					break;
				}
				if
				(
					(classes[i].base_class == pcfg.base_class) &&
					(classes[i].sub_class == pcfg.sub_class) &&
					(classes[i].interface == pcfg.interface)
				)
				{
					kprintf("\n\r* Description : %s", classes[i].name);
					break;
				}
			}

			for (i=0; i<6; i++)
				if (pcfg.base[i])
				{
					if (pcfg.type[i] == PCI_IO_RESOURCE_IO)
						kprintf("\n\r* Base Register %d IO: %#06x (%#06x)",
							i, pcfg.base[i], pcfg.size[i]);
					else
	    					kprintf("\n\r* Base Register %d MM: %#010x (%#010x)",
							i, pcfg.base[i] & 0xfffffff0, pcfg.size[i]);
				}
				if (pcfg.rom_base)
					kprintf("\n\r* Base Register ROM : %#010x (%#010x)",
						pcfg.rom_base, pcfg.rom_size);

			if (pcfg.irq)
				kprintf("\n\r* Interrupt line: %u", pcfg.irq);


			switch(pcfg.header_type & 0x7F)
			{
				case PCI_HEADER_TYPE_NORMAL:
				kprintf("\n\r* Normal device");
				break;

				case PCI_HEADER_TYPE_BRIDGE:
				kprintf("\n\r* PCI <-> PCI bridge");
				break;

				default:
				kprintf("\n\r* Unknown header type");
				break;
			}

			// kprintf("\n\rDo you want to enable this device (Y/N)? ");
			pci_enable_device(&pcfg);
			key = kgetchar();
			if( key == CTRL_C ) return;
			/*
			key &= 0xFF;
			if ( key=='Y' || key=='y' )
			{
				putchar(key);
				pci_enable_device(&pcfg);
			}
			else
				putchar('N');
			*/
			kprintf("\n\r");
		}
	}
	kprintf("\n\rPCI: finished\n\r");
}
