
    if (!(reteDEBUG&2))
    if (adesso!=tbarra) { 
     tbarra=adesso;
     NetX=0;NetY=0;Netcolore=0x17;
     NetPrintf(" Net:Ip=");NetPrintf(NUMinIP(IPlocale)); NetX++;
     if ((stato&0xf)==0xf) NetPrintf("P"); else NetPrintf("p");
     if ((stato&0x30)==0x30) NetPrintf("P"); else NetPrintf("p");
     if ((stato&0x3c0)==0x3c0) NetPrintf("P"); else NetPrintf("p");
     NetPrintf("(");
     tRete=(int)inb(porta1+4);
     if (tRete&1) NetPrintf("8"); else NetPrintf("."); /*DTR */
     if (tRete&2) NetPrintf("5"); else NetPrintf(".");/* RTS */
     tRete=(int)inb(porta1+6);
     if (tRete&0x10) NetPrintf("6"); else NetPrintf(".");/* CTS */
     if (tRete&0x20) NetPrintf("7"); else NetPrintf(".");/* DSR */
     if (tRete&0x80) NetPrintf("9"); else NetPrintf(".");/* DCD */
     
     NetPrintf(")_");
     if (leds&2) Netcolore=0x1E; else Netcolore=0x17; NetPrintf("Tx");
     if (leds&1) Netcolore=0x1E; else Netcolore=0x17; NetPrintf("Rx");
     Netcolore=0x17;
     NetPrintf(" IP_");
     if (leds&16) Netcolore=0x1E; else Netcolore=0x17; NetPrintf("Tx");
     if (leds&8) Netcolore=0x1C; else 
     if (leds&4) Netcolore=0x1E; else
             Netcolore=0x17; NetPrintf("Rx");
     Netcolore=0x17;        
     NetPrintf(" Sok=");
     for (tRete=0;tRete<10;tRete++)
        if (Sok[tRete]!=NULL) {
          if (Sok[tRete]->buflenRX>2500)
             Netcolore=0x47; else Netcolore=0x17;
          NetPutc(Sok[tRete]->stato+48); 
/*          if ((Sok[tRete]->stato!=3)&&(Sok[tRete]->stato!=6)
            &&((adesso-Sok[tRete]->timeout)>15)) {
              Debug("timeoutReteRX\n");
              deregistraIP(tRete);
            }  */
        } else NetPutc('.');
     Netcolore=0x17;        
     NetX++;
     for (tRete=0;tRete<10;tRete++) {
      NetPutInt(Errori[tRete]);NetPrintf(",");
     } 
     NetPrintf(" ");
/*     NetPutInt(adesso); */
     NetPutc(*(barra+nbarra));
     NetPutc(*(barra+nbarra1));nbarra1=nbarra1+1-4*(nbarra1>3);
     ledsold=0;leds=0;
    } 


