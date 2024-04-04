/*
     Pallo

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/* motore 
opz:
  84218421 84218421  
  xxxxxxxx xxxxxxxx
  | |||||| ||||||||  
  | |||||| ||||++++-->000X Mossa tipo: Rimbalzo
  | |||||| |||+------>0010 non considera collisione e sovrascrivi livello
  | |||||| ||+------->0020 Nemico
  | |||||| |+-------->0040 Pallo
  | |||||| +--------->0080 diamante
  | |||||+----------->0100 Sparo
  | ||||+------------>0200 Uscita
  | |||+------------->0400 Mossa tipo: Serpente
  | ||+-------------->0800 Mossa tipo: Insegue
  | |+--------------->1000 Griglia
  | +---------------->2000 Mossa tipo: AI
  |+----------------->4000 Tarma
  +------------------>8000 DiamanteMorto

 griglia  0x1000
 fantasma 0x0020
 diamante 0x0090
 pallo    0x0040
 uscita   0x0200
 masso    0x0021
 serpente 0x0420

*/
#define STAM 1
#define CANC 0
#define obggg obg[xcoll[x][y]-1]
u_short video_1(struct Tobg *obgg) {
 u_short coll;
 u_int coll1 = 0;
 char *dis,*nick;
 int x,y;
 coll=0;
 if (obgg->opz&0xff7fff) { /* se non e' diamantemorto */
  dis=obgg->img;nick=obgg->nick;x=obgg->x;y=obgg->y;
  for (;*dis;dis++) { /* prova a scrivere l'oggetto in xcoll per vedere */
    switch (*dis) {   /* se ci sono collisioni */
      case '\1' : y-=(y>1);  break; /* SU         */
      case '\2' : y+=(y<25); break; /* GIU'       */
      case '\3' : x+=(x<80); break; /* DESTRA     */
      case '\4' : x-=(x>1);  break; /* SINISTRA   */
      case '\5' : dis+=4; break; /* COLORE   */
      case '\r' : x=1;           break; /* RETURN   */
      case '\7' : x+=(x<80);break;
      default : 
/*     if ((xcoll[x][y])&&(xcoll[x][y]!=t)&&(xcoll[x][y]!=DIAMANTE) ))  */
           if (xcoll[x][y]) {
             if (obg[t].opz&0x40000) {
                if (obggg.opz&0x80) coll= 0x100 | (t+1); else
                if (obggg.opz&0x10) xcoll[x][y] = 0;
             } else
              if ((xcoll[x][y]!=(t+1))&&(!(obggg.opz&0x10)))
                 coll=(xcoll[x][y]<<8)|(t+1);
           }    
           if ((x <= 1) || (x >= 80) ||
              (y >= 25) || (y <= 1)) 
              if (obgg->opz&0x40000) coll=(xcoll[x][y]<<8)|(t+1);   
           x+=(x<80);break;
    }
  }
  coll1 = obg[((coll&0xFF00)>>8)-1].opz|obg[(coll&0xFF)-1].opz;
#ifdef DEBUG
  if (coll) fprintf(stderr,"%04x coll = %08x %08x ->",coll,coll1,obgg->opz);
#endif  
/*
  if (((coll1 & 0xC0) != 0xC0 )&&(obgg->opz&0x0f)) obgg->opz+=1-12*((obgg->opz&0x0f)>11); else
*/
    
 if ((obgg->x != obgg->xold) || (obgg->y != obgg->yold)) {
  if (coll&&((coll1&0xC0) != 0xC0)&&(obgg->opz&0x0f)) obgg->opz+=1-12*((obgg->opz&0x0f)>11); else
  if ((!coll)&&(obgg->opz&0x0f)) {
   switch (obgg->opz&0x0f) {
    case 3 : case 5 :obgg->opz=(obgg->opz & 0xfffffff0) | 1;break;
    case 6: case 8 :obgg->opz=(obgg->opz &  0xfffffff0) | 4;break;
    case 9: case 11 :obgg->opz=(obgg->opz & 0xfffffff0) | 7;break;
    case 2: case 12 :obgg->opz=(obgg->opz & 0xfffffff0) | 10;break;
   }
#ifdef DEBUG
  fprintf(stderr,"scalato -> %08x %u,%u  %u,%u  coll=%08x %04x\n",obgg->opz,obgg->x,obgg->y,obgg->xold,obgg->yold,coll,coll1);
#endif  
  
  } 
 } 
#ifdef DEBUG
  if (coll) fprintf(stderr," , %04x coll = %08x %08x\n",coll,coll1,obgg->opz);
#endif  
  if ( (!coll)&&( (obgg->x!=obgg->xold)||(obgg->y!=obgg->yold )||(obgg->vis)) ) {
   if (obgg->xold) {
   dis=obgg->img;nick=obgg->nick;x=obgg->xold;y=obgg->yold;
   ccprint "\033[%i;%iH",obgg->yold,obgg->xold);
   for (;*dis;dis++)
    switch (*dis) {
      case '\1' : y-=(y>1);  ccprint "\033[A"); break; /* SU         */
      case '\2' : y+=(y<25); ccprint "\033[B"); break; /* GIU'       */
      case '\3' : x+=(x<80); ccprint "\033[C"); break; /* DESTRA     */
      case '\4' : x-=(x>1);  ccprint "\033[D"); break; /* SINISTRA   */
      case '\5' : dis+=4;                      break; /* COLORE   */
      case '\r' : ccprint "\r"); x=1;    break; /* RETURN   */
      default : ccprint " "); xcoll[x][y]=0;
                x+=(x<80);break;
    }
   }
   dis=obgg->img;nick=obgg->nick;
   x=obgg->x;y=obgg->y;
   ccprint "\033[%i;%iH",obgg->y,obgg->x);
   if (obgg->opz & 0x00020) /*nemico  */ ccprint "\033[1;31m"); else
   if (obgg->opz & 0x04000) /*tarma*/    ccprint "\033[1;36m"); else
   if (obgg->opz & 0x00080) /*diamante*/ ccprint "\033[1;33m"); else
   if (obgg->opz & 0x01000) /*ostacolo*/ ccprint "\033[1;32m");
   for (;*dis;dis++)
    switch (*dis) {
      case '\1' : y-=(y>1);  ccprint "\033[A"); break; /* SU         */
      case '\2' : y+=(y<25); ccprint "\033[B"); break; /* GIU'       */
      case '\3' : x+=(x<80); ccprint "\033[C"); break; /* DESTRA     */
      case '\4' : x-=(x>1);ccprint "\033[D"); break; /* SINISTRA   */
      case '\5' : ccprint "\033[%i;3%i;4%im",dis[2]-48,dis[3]-48,dis[4]-48); dis+=4; break; /* COLORE   */
      case '\r' : x=1;ccprint "\r");     break; /* RETURN   */
      case '\7' : x+=(x<80);break;
      default : if (*dis=='\6') ccprint "%c",*(nick++)); else
                       ccprint "%c",*dis);
                xcoll[x][y]=t+1;
                x+=(x<80);break;
    }
 }} else 
  if (obgg->opz&0x8000) {
   dis=obgg->img;nick=obgg->nick;x=obgg->x;y=obgg->y;
   ccprint "\033[%i;%iH",obgg->y,obgg->x);
   for (;*dis;dis++)
    switch (*dis) {
      case '\1' : y-=(y>1);  ccprint "\033[A"); break; /* SU         */
      case '\2' : y+=(y<25); ccprint "\033[B"); break; /* GIU'       */
      case '\3' : x+=(x<80); ccprint "\033[C"); break; /* DESTRA     */
      case '\4' : x-=(x>1);  ccprint "\033[D"); break; /* SINISTRA   */
      case '\5' : dis+=4;                      break; /* COLORE   */
      case '\r' : ccprint "\r"); x=1;    break; /* RETURN   */
      default : ccprint " "); xcoll[x][y]=0;x+=(x<80); break;
    }
  obgg->opz=0;obg->vis=0;
 }

#ifdef DEBUG
  if (coll) fprintf(stderr," , %04x coll = %08x %08x ",coll,coll1,obgg->opz);
#endif  
#ifdef DEBUG3
  ccprint "\033[u(%02x%02x,%01x-%04x)\033[s",obgg->x,obgg->y,obgg->opz,coll);
#endif
  colorebase;
  return coll;
}
/*
#define CH1 cN(ch1)
u_short cN(u_short Ch1) {
  return obg[((Ch1&0xFF00)>>8)-1].opz|obg[(Ch1&0xFF)-1].opz;
}
   */
void video() {
#ifdef DEBUG2
  int x,y;
#endif  
  u_int ch1,ch2;
  ccprint "\033[18;1H\033[s"); 
  for (t=0;t<nobg;t++) {
    ch2=video_1(&obg[t]);
    ch1=obg[((ch2&0xFF00)>>8)-1].opz|obg[(ch2&0xFF)-1].opz;
#ifdef DEBUG
  fprintf(stderr,"{%x %x-%x }",t,ch1,ch2);
#endif
    if (ch1&0x80000) { /* sposta */
#ifdef DEBUG
  fprintf(stderr,"sposta -> %x %x %u\n",ch1,ch2,tsposta);   
#endif
       tsposta = (++tsposta) % 4;
       if (obg[csposta = ((ch2&0xFF00)>>8)-1].opz & 0x80000) ; else
               csposta=(ch2&0xFF)-1;
          switch (tsposta) {
            case 0 : obg[csposta].x++;break;
            case 1 : obg[csposta].y++;break;
            case 2 : obg[csposta].x--;break;
            case 3 : obg[csposta].y--;break;
          }
    }    
/*    if ((ch1&0x140) == 0x140) {
       obg[sparo1].xold = obg[sparo1].x;
       obg[sparo1].yold = obg[sparo1].y;
    }*/
    if (ch1) { 
/*    if (CH1&&(CH1!=MANGIA)) { */
      obg[t].x=obg[t].xold; 
      obg[t].y=obg[t].yold; 
    }
    if ((ch1&0x140) == 0x100) {
       if (obg[csposta = ((ch2&0xFF00)>>8)-1].opz & 0x100) 
          {
             obg[csposta].opz = 0x8000;
             obg[csposta].vis = 1;
          } else
       if (obg[csposta=(ch2&0xFF)-1].opz & 0x100)
          {
             obg[csposta].opz = 0x8000;
             obg[csposta].vis = 1;
          }
       if (obg[csposta = ((ch2&0xFF00)>>8)-1].opz & 0x20) 
          {
             if (!(--obg[csposta].nvite)) {
               obg[csposta].opz = 0x8000;
               pnt += 200;
             }  
             obg[csposta].vis = 1;
          } else
       if (obg[csposta=(ch2&0xFF)-1].opz & 0x20)
          {
             if (!(--obg[csposta].nvite)) {
               obg[csposta].opz = 0x8000;
               pnt += 200;
             } 
             obg[csposta].vis = 1;
          };
       
    }
    if ((ch1&0x4040)==0x4040) pnt-=pntTARMA; 
    if (pnt<-20) { Msg=PartPersa;pnt+=100; }
    if ((ch1&0x0060)==0x0060) { 
       Msg=PartPersa; 
       pnt-=500;
/*       if (ch1&0x2000) saldaF(); */
    }   
    if (((ch1&0x0240)==0x240)&&(ndia<=0)) Msg=PartVinta; else
    if ((ch1&0x00c0)==0x00c0) {
      if (obg[csposta = ((ch2&0xFF00)>>8)-1].opz & 0x80) ; else
              csposta=(ch2&0xFF)-1;
      obg[csposta].opz = 0x8000;
#ifdef DEBUG
  fprintf(stderr,"{*** %x-%x ***}",csposta,obg[csposta].opz);
#endif
      video_1(&obg[t]);
      pnt+=100;
      --ndia;
    } 
    obg[t].xold=obg[t].x; 
    obg[t].yold=obg[t].y; 
   
  } 
  ccprint "\033[1;5HPunti:%i  Pallo:%i x:%i y:%i  Livello=%i Colpi=%i "
,pnt,nvite,obg[pl].x,obg[pl].y,livell,nsparo1);fflush(stdout);
  if (fls) fls=0;
#ifdef DEBUG2  
  for (x=1;x<80;x++)
   for (y=1;y<25;y++)
    printf("\033[%i;%iH%01x",y,x,xcoll[x][y]);
#endif    
}

void motore() {
  int x11,y11;
  initF();
  Abilita; 
  svuota_buff(0);
  video();
/*  ccprint "\033[2J\033[1;1H"); */
  for (pl=0;pl<nobg;pl++)
       if (obg[pl].opz & 0x40) break;
  if (pl == nobg) { printf("non c'e' un pallo valido\n"); Disabilita; exit(0); }
  ch = '<';	
/*  while ((ch!='q')&&(Msg==NULL)&&(ch!='x')) { */
  while ((ch!='q')&&(Msg==NULL)) { 
    usleep(10000);
    switch(ch=leggichar()) {
      case 200: obg[pl].y-=(obg[pl].y>1);obg[sparo1].nick[0]=4;break;
      case 208: obg[pl].y+=(obg[pl].y<25);obg[sparo1].nick[0]=3;break;
      case 203: obg[pl].x-=(obg[pl].x>1);obg[sparo1].nick[0]=2;break;
      case 205: obg[pl].x+=(obg[pl].x<80);obg[sparo1].nick[0]=1;break;
      case ' ': pnt-=1000; Msg=PartPersa; break;
      case 'c': congela = 1 - congela; break; 
      case 'a': if (!(sparo1) || (obg[sparo1].opz == 0x100)) break;
                if (nsparo1 == 0) break;
                nsparo1--;
                obg[sparo1].nick[1]=obg[sparo1].nick[0];
                obg[sparo1].img=sparo[0];
                obg[sparo1].opz=0x100; 
                obg[sparo1].xold=obg[sparo1].x=obg[pl].x;
                obg[sparo1].yold=obg[sparo1].y=obg[pl].y;
                switch (obg[sparo1].nick[1]) {
                  case 1 : obg[sparo1].x += 4; break;
                  case 3 : obg[sparo1].y += 2; break;
                }
                break;
/*      case 'h': Help;break; */
    }
#ifdef DEBUG
  if (ch == 'o') {
     for (ch=0;ch <nobg;ch++ ) fprintf(stderr,"%% %u %x\n",ch,obg[ch].opz);
     ch = '<';
  }
#endif
    if (congela) { video(); continue;}
    if (obg[sparo1].opz &0x100)
      if (ogni(5,50)) { /* sparo */
        if (obg[sparo1].img == sparo[0]) obg[sparo1].img = sparo[1]; else
        if (obg[sparo1].img == sparo[1]) obg[sparo1].img = sparo[0];
        switch (obg[sparo1].nick[1]) {
         case 1 :obg[sparo1].x +=(obg[sparo1].x<80);break;
         case 2 :obg[sparo1].x -=(obg[sparo1].x>1);break;
         case 3 :obg[sparo1].y +=(obg[sparo1].y<25);break;
         case 4 :obg[sparo1].y -=(obg[sparo1].y>1);break;
        } 
        video();
      }
    if (ogni(4,tgra)) {
      if (++ngra > 3) ngra = 0;
      for (t=0;t<nobg;t++)
        if (obg[t].opz&0x100000)
          switch (ngra) {
            case 0: case 2:     obg[t].y++;    break;
            case 1: obg[t].x++; obg[t].y++;     break;
            case 3: obg[t].x--; obg[t].y++;     break;
          }
    }      
    if (ogni(0,tfantasm)) {
      fsm=1-fsm;
      if (++ngra > 3) ngra = 0;
      for (t=0;t<nobg;t++) {
       if (obg[t].img==fanta[1-fsm]) obg[t].img=fanta[fsm]; else
       if (obg[t].img==fanta[fsm]) obg[t].img=fanta[1-fsm]; 
       if (obg[t].img==mostro[1-fsm]) obg[t].img=mostro[fsm]; else
       if (obg[t].img==mostro[fsm]) obg[t].img=mostro[1-fsm]; 
       if (obg[t].opz&0x2000) { 
         leggiF(t); calcolaF(t); applicaF(t);
         if ((obg[t].img == fantaAI[0]) || 
             (obg[t].img == fantaAI[1]) || 
             (obg[t].img == fantaAI[2]) || 
             (obg[t].img == fantaAI[3]))
                  obg[t].img=fantaAI[(u_char)varF[t][0]-1];
       }
       if (obg[t].img==tarma[0]) obg[t].img=tarma[1]; else
       if (obg[t].img==tarma[1]) obg[t].img=tarma[0];
       if ((obg[t].opz&0x0200)&&(ndia==0)) {
         obg[t].img=uscita[1];ndia--;obg[t].opz|=0x0010;;
         obg[t].vis=1;
       }  
     
       if (obg[t].opz&0x0800) {
        if (fsm) {
          obg[t].x-=(obg[pl].x<obg[t].x);
          obg[t].x+=(obg[pl].x>obg[t].x);
        } else {
          obg[t].y-=(obg[pl].y<obg[t].y);
          obg[t].y+=(obg[pl].y>obg[t].y);
        }
       } 
/*
  ++ -+ ++ +- ++ +- -- +- -- -+ -- -+
*/
#ifdef DEBUG
        fprintf(stderr,"[%u-%u]",t,obg[t].opz&0x0f);
#endif        
       if (obg[t].opz&0x0f) {
        obg[t].nick[0]=(obg[t].opz&0x0f)+64;
#ifdef DEBUG
        fprintf(stderr,"(%u)",obg[t].opz&0x0f);
#endif        
        if (obg[t].opz&0x400000) {
          switch (obg[t].opz&0x0f) { /* labirinto */
           case 1: case 3: case 5:obg[t].y+=(obg[t].y<25);break;
           case 2: case 10:case 12:obg[t].x-=(obg[t].x>0);break;
           case 4: case 6:case 8:obg[t].x+=(obg[t].x<79);break;
           case 7: case 9:case 11:obg[t].y-=(obg[t].y>0);break;
          } 
        if (!(obg[t].opz&0x2000)) /* se non e' un AI */
          if ((obg[t].img == fantaAI[0]) || (obg[t].img == fantaAI[1]) || 
             (obg[t].img == fantaAI[2]) || (obg[t].img == fantaAI[3]))
             switch (obg[t].opz&0x0f) { /* rimbalzo */
              case 1: case 3: case 5:obg[t].img=fantaAI[2];break;
              case 2: case 10:case 12:obg[t].img=fantaAI[3];break;
              case 4: case 6:case 8:obg[t].img=fantaAI[1];break;
              case 7: case 9:case 11:obg[t].img=fantaAI[0];break;
             } 
/*         if (ogni(6,tfantasm<<1)) */
          switch (obg[t].opz&0xf) {
           case 1: case 4:case 7: case 10: 
                 if (obg[t].cnt < 10) {
                      obg[t].opz+=+1-12*((obg[t].opz&0x0f)>11);
                      obg[t].cnt++;
                 }     
                 break;
           default : obg[t].cnt=0;
          }
#ifdef DEBUG
        fprintf(stderr,"<%08x>\n",obg[t].opz);
#endif        
        } else
          switch (obg[t].opz&0x0f) { /* rimbalzo */
           case 1: case 3: case 5:obg[t].y+=(obg[t].y<25);obg[t].x+=(obg[t].x<79);break;
           case 2: case 10:case 12:obg[t].y-=(obg[t].y>0);obg[t].x+=(obg[t].x<79);break;
           case 4: case 6:case 8:obg[t].y+=(obg[t].y<25);obg[t].x-=(obg[t].x>0);break;
           case 7: case 9:case 11:obg[t].y-=(obg[t].y>0);obg[t].x-=(obg[t].x>0);break;
          } 
       }
       if (obg[t].opz&0x30000) {
        obg[t].vis=1;
        if ((obg[t].opz&0x30000) == 0x30000) {
          x11 = (obg[t+1].x + obg[t-1].x)/2;
          y11 = (obg[t+1].y + obg[t-1].y)/2;
        } else
        if (obg[t].opz&0x20000) {
          x11 = obg[t+1].x;
          y11 = obg[t+1].y;
        } else {
          x11 = obg[t-1].x;
          y11 = obg[t-1].y;
        }  
        if (fsm&&(abs(obg[t].x-x11))) obg[t].x -= (obg[t].x>x11)*2-1;
        else if (abs(obg[t].y-y11)) obg[t].y -= (obg[t].y>y11)*2-1;
       }
       if (obg[t].opz&0x0400) {
          obg[t].vis=1;
          if (obg[t+1].opz&0x0400) {

           if (abs(obg[t].x-obg[t+1].x)>1) obg[t].x=obg[t].xold;
           if (abs(obg[t].y-obg[t+1].y)>1) obg[t].y=obg[t].yold;
          } 
          if (abs(obg[t].x-obg[t-1].x)>1) obg[t].x=obg[t].xold;
          if (abs(obg[t].y-obg[t-1].y)>1) obg[t].y=obg[t].yold;
/*
           if (abs(obg[t].x-obg[t+1].x)>abs(obg[t].xold-obg[t+1].x)) obg[t].x=obg[t].xold;
           if (abs(obg[t].y-obg[t+1].y)>abs(obg[t].yold-obg[t+1].y)) obg[t].y=obg[t].yold;
          } 
          if (abs(obg[t].x-obg[t-1].x)>abs(obg[t].xold-obg[t-1].x)) obg[t].x=obg[t].xold;
          if (abs(obg[t].y-obg[t-1].y)>abs(obg[t].yold-obg[t-1].y)) obg[t].y=obg[t].yold;
*/
       }
       if (obg[t].opz&0x200000) { /*relativo*/
          if (obg[(u_char)obg[t].nick[1]].opz) {
           x11 = obg[(u_char)obg[t].nick[1]].x + obg[t].nick[2];
           y11 = obg[(u_char)obg[t].nick[1]].y + obg[t].nick[3];
           if (fsm&&(abs(obg[t].x-x11))) obg[t].x -= (obg[t].x>x11)*2-1;
           else if (abs(obg[t].y-y11)) obg[t].y -= (obg[t].y>y11)*2-1;
           obg[t].vis = 1;
          }
       }
      }
      video();
    } else
    if (ogni(1,1000)) {
      ndiam=1-ndiam;
      randomF();
      for (t=0;t<nobg;t++) {
       obg[t].vis=1;
       if (obg[t].img == diamante[0]) obg[t].img = diamante[1]; else
       if (obg[t].img == diamante[1]) obg[t].img = diamante[0]; else
       if (obg[t].opz&0x0200) obg[t].vis = 1; else
       
                      obg[t].vis = 0;
      }
      video();
    }  
/*    if (ogni(2,3000)) fls=1; */
    if (ch!='#') video();   
  }
  svuota_buff(0);
  Disabilita;
}

