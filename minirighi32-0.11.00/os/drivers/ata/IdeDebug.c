/*!	\file drivers/ata/IdeDebug.c
 *	\brief IDE driver::Debug.
 *	\author Luca Giovacchini
 *	\date Last update: 2003-11-07
 *	\note Copyright (&copy;) 2003 Luca Giovacchini
 *
 *	This driver is based on Atadrv by Hale Landis
 *	but it is completely rearranged for the minirighi32.
 *	\n
 *	<b>IMPORTANT!!!</b>\n
 *	Here you can find code for debugging and showing errors.
 */

#include <string.h>

#include <kernel/IdeLow.h>
#include <kernel/Ide.h>
#include <kernel/IdeTimer.h>
#include <kernel/video.h>

#include <kernel/IdeDebug.h>

static byte hex_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// Not Used
CmdNames_Struct CmdNames[] =
	{
		{HDC_CMD_CFA_ERASE_SECTORS			,   "CFA ERASE SECTORS"			} ,
		{HDC_CMD_CFA_REQUEST_EXT_ERR_CODE		,   "CFA REQUEST EXT ERR CODE"		} ,
		{HDC_CMD_CFA_TRANSLATE_SECTOR			,   "CFA TRANSLATE SECTOR"		} ,
		{HDC_CMD_CFA_WRITE_MULTIPLE_WO_ERASE		,   "CFA WRITE MULTIPLE WO ERASE"	} ,
		{HDC_CMD_CFA_WRITE_SECTORS_WO_ERASE		,   "CFA WRITE SECTORS WO ERASE"	} ,
		{HDC_CMD_CHECK_POWER_MODE1			,   "CHECK POWER MODE"			} ,
		{HDC_CMD_CHECK_POWER_MODE2			,   "CHECK POWER MODE"			} ,
		{HDC_CMD_DEVICE_RESET				,   "DEVICE RESET"			} ,
		{HDC_CMD_EXECUTE_DEVICE_DIAGNOSTIC		,   "EXECUTE DEVICE DIAGNOSTIC"		} ,
		{HDC_CMD_FLUSH_CACHE				,   "FLUSH CACHE"			} ,
		{HDC_CMD_FORMAT_TRACK				,   "FORMAT TRACK"			} ,
		{HDC_CMD_IDENTIFY_DEVICE			,   "IDENTIFY DEVICE"			} ,
		{HDC_CMD_IDENTIFY_PACKET_DEVICE			,   "IDENTIFY PACKET DEVICE"		} ,
		{HDC_CMD_IDLE1					,   "IDLE"				} ,
		{HDC_CMD_IDLE2					,   "IDLE"				} ,
		{HDC_CMD_IDLE_IMMEDIATE1			,   "IDLE IMMEDIATE"			} ,
		{HDC_CMD_IDLE_IMMEDIATE2			,   "IDLE IMMEDIATE"			} ,
		{HDC_CMD_INITIALIZE_DRIVE_PARAMETERS		,   "INITIALIZE DEVICE PARAMETERS"	} ,
		{HDC_CMD_NOP							,   "NOP"							} ,
		{HDC_CMD_PACKET							,   "PACKET"						} ,
		{HDC_CMD_READ_BUFFER					,   "READ BUFFER"					} ,
		{HDC_CMD_READ_DMA_QUEUED				,   "READ DMA QUEUED"				} ,
		{HDC_CMD_READ_DMA						,   "READ DMA"						} ,
		{HDC_CMD_READ_MULTIPLE					,   "READ MULTIPLE"					} ,
		{HDC_CMD_READ_SECTORS					,   "READ SECTORS"					} ,
		{HDC_CMD_READ_VERIFY_SECTORS			,   "READ VERIFY SECTORS"			} ,
		{HDC_CMD_RECALIBRATE					,   "RECALIBRATE"					} ,
		{HDC_CMD_SEEK							,   "SEEK"							} ,
		{HDC_CMD_SET_FEATURES					,   "SET FEATURES"					} ,
		{HDC_CMD_SET_MULTIPLE_MODE				,   "SET MULTIPLE MODE"				} ,
		{HDC_CMD_SLEEP1							,   "SLEEP"							} ,
		{HDC_CMD_SLEEP2							,   "SLEEP"							} ,
		{HDC_CMD_STANDBY1						,   "STANDBY"						} ,
		{HDC_CMD_STANDBY2						,   "STANDBY"						} ,
		{HDC_CMD_STANDBY_IMMEDIATE1				,   "STANDBY IMMEDIATE"				} ,
		{HDC_CMD_STANDBY_IMMEDIATE2				,   "STANDBY IMMEDIATE"				} ,
		{HDC_CMD_WRITE_BUFFER					,   "WRITE BUFFER"					} ,
		{HDC_CMD_WRITE_DMA						,   "WRITE DMA"						} ,
		{HDC_CMD_WRITE_DMA_QUEUED				,   "WRITE DMA QUEUED"				} ,
		{HDC_CMD_WRITE_MULTIPLE					,   "WRITE MULTIPLE"				} ,
		{HDC_CMD_WRITE_SECTORS					,   "WRITE SECTORS"					} ,
		{HDC_CMD_WRITE_VERIFY					,   "WRITE VERIFY"					} ,
		{0x00 ,   ""}                  // end of table
	} ;

static ErrNames_Struct ErrNames[] =
	{
		{1 ,  "Soft Reset timed out polling for device 0 to set BSY=0"	},
		{2 ,  "Soft Reset timed out polling device 1 to access register"},
		{3 ,  "Soft Reset timed out polling for device 1 to set BSY=0"  },
		{4 ,  "Soft Reset timed out polling for device 0 and 1"		},

		{11 ,  "Device selection timed out polling for BSY=0"           },
		{12 ,  "Device selection timed out polling for RDY=1"           },

		{21 ,  "Non-Data command ended with bad status"                 },
		{22 ,  "Non-Data command timed out waiting for an interrupt"    },
		{23 ,  "Non-Data command timed out polling for BSY=0"           },
		{24 ,  "Exec Dev Diag command timed out polling device 1"       },

		{31 ,  "PIO Data In command terminated by error status"         },
		{32 ,  "Device should be ready to transfer data but DRQ=0"      },
		{33 ,  "PIO Data In command ended with bad status"              },
		{34 ,  "PIO Data In command timed out waiting for an interrupt" },
		{35 ,  "PIO Data In command timed out polling for BSY=0"        },

		{41 ,  "PIO Data Out command terminated by error status"        },
		{42 ,  "Device should be ready to transfer data but DRQ=0"      },
		{43 ,  "PIO Data Out command ended with bad status"             },
		{44 ,  "PIO Data Out command timed out waiting for an interrupt"},
		{45 ,  "PIO Data Out command timed out polling for BSY=0"       },
		{46 ,  "Extra interrupt at start of a PIO Data Out command"     },
		{47 ,  "PIO Data Out command timed out polling for BSY=0"       },

		{51 ,  "Timeout waiting for BSY=0/DRQ=1 for cmd packet transfer"},
		{52 ,  "Bad status at command packet transfer time"             },
		{53 ,  "Timeout waiting for interrupt for data packet transfer" },
		{54 ,  "Timeout polling for BSY=0/DRQ=1 for a data packet"      },
		{55 ,  "Bad status at data packet transfer time"                },
		{56 ,  "Timout waiting for final interrupt at end of command"   },
		{57 ,  "Timeout polling for final BSY=0 at end of command"      },
		{58 ,  "Bad status at end of command"                           },
		{59 ,  "Buffer overrun (host buffer too small)"                 },
		{60 ,  "Byte count for data packet is zero"                     },

		{70 ,  "No DMA channel is setup"                                },
		{71 ,  "End of command without complete data transfer"          },
		{72 ,  "Timeout waiting for 1st transfer to complete"           },
		{73 ,  "Timeout waiting for command to complete"                },
		{74 ,  "Bad status at end of command"                           },
		{75 ,  "Timeout waiting for BSY=0/DRQ=1 for cmd packet transfer"},
		{76 ,  "Bad status at command packet transfer time"             },
		{77 ,  "ATA command code is not C8H or CAH"                     },

		{80 ,  "No tag available now"                                   },
		{81 ,  "Timeout polling for SERV=1"                             },
		{0x00  ,  ""}                      // end of table
   } ;


// ***************** GetIdeErrorMEssage *********************
// Return Ide error message associated to a Error
// This can be useful for higher level module error handling routine
// **********************************************************
char * GetIdeErrorMessage(int Err)
{
	int i=0;
	while (ErrNames[i].ErrCode != 0x00 && ErrNames[i].ErrCode != Err)
		i++;

	return ErrNames[i].ErrName;
}

// ******************** ShowIdeError ************************
// Display Ide error message
// With Dump = TRUE will be displayed also value for some
// ide register
// **********************************************************
void ShowIdeErrorMessage(int Err,int Dump)
{
	int i=0;
	int j=0;

	if (Err!=0x00)
	{
		while (ErrNames[i].ErrCode != 0x00 && ErrNames[i].ErrCode != Err)
			i++;

		kprintf("\n\rIde Driver Error: ");

		if (ErrNames[i].ErrCode != 0x00)
			kprintf("%s",ErrNames[j].ErrName);
		else
			// if we are here the error code is not 0 and is not found
			kprintf("Unknow Error");

		if (Dump)
			ShowIdeRegisterDump();
	}	
}

// ***************** ShowIdeRegisterDump ********************
// Print register value on screen for error debugging purpose
// **********************************************************
void ShowIdeRegisterDump()
{
	kprintf("\n\rIDE REGISTER DUMP");

	kprintf("\n\rSTATUS  =0x");
	PrintByteHex(InPortAta(CC_ASTAT));
	gotoxy(20,-1);
	kprintf("ERROR   =0x");
	PrintByteHex(InPortAta(CC_ERR));

	kprintf("\n\rSEC CNT =0x");
	PrintByteHex(InPortAta(CC_SECC));
	gotoxy(20,-1);
	kprintf("SEC NUM =0x");
	PrintByteHex(InPortAta(CC_SECN));
	
	kprintf("\n\rCYL LOW =0x");
	PrintByteHex(InPortAta(CC_CYLL));
	gotoxy(20,-1);
	kprintf("CYL HIGH=0x");
	PrintByteHex(InPortAta(CC_CYLH));
	gotoxy(40,-1);
	kprintf("DEV HEAD=0x");
	PrintByteHex(InPortAta(CC_DEVH));

	kprintf("\n\r");
}

void PrintByteHex(byte Data)
{
	kputchar(hex_table[(Data>>4) & 0xF]);
	kputchar(hex_table[(Data) & 0xF]);
}

void ShowMatrixData(char * DataBuffer,int Col,int BytePerCol,int RowToWait,int Len, int Mode)
{
	int i=0;
	int j=0;
	int z=0;
	int RowCount=0;
	int CrtWidth=k_get_crt_width();

	while(i<Len)		
	{
		RowCount++;		
		kprintf("\n\r");
		j=0;
		// Print a Row
		while (i<Len && j<Col)
		{
			gotoxy(j*(CrtWidth/Col),-1);
			kprintf("[");
			PrintByteHex( (byte)i );
			kprintf("]");
			z=0;

			// Print a Column
			while (i<Len && z<BytePerCol)
			{		
				switch (Mode)
				{
					case DBG_SHOWMAT_ASCII:
					kprintf("%c",DataBuffer[i]);
					break;
					case DBG_SHOWMAT_HEX:
					default:					
					kprintf(" ");
					PrintByteHex(DataBuffer[i]);					
					break;								
				}
				z++;
				i++;
			}			
			j++;
		}
		if ( (i<Len) && ((RowCount % RowToWait) == 0) )
			WaitKeyPress(MSG_PRESSKEY);
	}
}

