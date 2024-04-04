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

int LIVELLMAX;
char immg1 = 0;
void messaggio(char *messagg,int nmesg) {

  ccprint "\033[s"); colorebase;
  ccprint "\033[5;5H **********************************************");
  if (nmesg==-1) ccprint "\033[6;5H * %s",messagg);
            else ccprint "\033[6;5H * %s %i",messagg,nmesg);
  ccprint "\033[7;5H **********************************************");
  fflush(stdout);
  usleep(1000000);
}

void insobg(char *nick,u_int liv,u_char x,u_char y,u_char *dis) {
  if (riff == -2) riff = nobg;
  if (liv&0x0080) ndia++;
  obg[nobg].nvite=numvite;
/*  if (liv&0x2000) FantAI|=(1<<nobg); */
/*  obg[nobg].liv=liv; */
  obg[nobg].cnt=0;
  obg[nobg].xold=0;
  obg[nobg].yold=0;
  obg[nobg].img=dis;obg[nobg].vis=0;
  obg[nobg].opz=liv;
  memcpy(obg[nobg].nick,nick,4);
  if (liv&0x200000) /* relativo */
    { if (riff < 0) { printf("Errore manca riferimento\n"); exit(0);}
      obg[nobg].nick[2]=x; obg[nobg].nick[3]=y;
      obg[nobg].nick[1]=riff;
/*      riff = -1; */
      x = obg[(u_char)riff].x + x;
      y = obg[(u_char)riff].y + y;
      }
  obg[nobg].x=x;obg[nobg].y=y;
  if (immg1) {
      obg[nobg].img = obg[nobg].nick+4;
      obg[nobg].nick[4] = immg1;
    } 
  obg[nobg].nick[5] = 0;
#ifdef DEBUG
  fprintf(stderr,"%x %u,%u %s N=%s\n",liv,x,y,dis,nick);
#endif  
  nobg++;
  immg1=0;
}                  
/*         !!    _-
   | , _ , LL  _-
*/


void grigl(u_char *gri) {
  for (;*gri;gri++) 
   switch (*gri) {
    case '_' : ngri+=snprintf(gri1+ngri,10000-ngri,"-----"); break;
    case '/' : ngri+=snprintf(gri1+ngri,10000-ngri,"_-\1_-\1/\2\2");break;
    case '\\' : ngri+=snprintf(gri1+ngri,10000-ngri,"\1\1-_\2-_\2\\");break;
    case '|' : ngri+=snprintf(gri1+ngri,10000-ngri,"|\1\4|\1\4|\2\2\3\3\3\3");break;
    case ' ' : ngri+=snprintf(gri1+ngri,10000-ngri,"\3\3\3\3\3");break;
    case '"' : ngri+=snprintf(gri1+ngri,10000-ngri,"|\3|\1\4\4\4|\3|\1\4\4\4|\3|\3\3\2\2");break;
    case '^' : ngri+=snprintf(gri1+ngri,10000-ngri,"|\3|\1\4\4\4|\3|\1\4\4\4/^\\\3\3\2\2");break;
    case 'U' : ngri+=snprintf(gri1+ngri,10000-ngri,"\\_/\1\4\4\4|\3|\1\4\4\4|\3|\3\3\2\2");break;
    case 'L' : ngri+=snprintf(gri1+ngri,10000-ngri,"+\1\4|\1\4|\2\2----");break;
    case '\r': ngri+=snprintf(gri1+ngri,10000-ngri,"\r\2\2\2");break;
   } 
}


 char ss1[100];
 char *ss;

int leggi(int ntt) {
 char *ss1;
 char ng = 0;
 ss1=ss;
 while ((*(++ss1))&&(ntt))
  if ((*ss1=='=')||(*ss1==',')) ntt--;
 ntt=0;
 if (*ss1 == '-') { ng = 1; ss1++; } else ng = 0;
 while ((*ss1)&&(*ss1>47)&&(*ss1<58)) ntt=ntt*10+(int)*(ss1++)-48;
 if (ng) ntt=-ntt;
 return ntt;
}

#define HEx(a,b) ((a-48-7*(a>64)-32*(a>96))<<b)

unsigned int leggihex() {
   return HEx(ss[2],16)+HEx(ss[3],12)+HEx(ss[4],8)+HEx(ss[5],4);+HEx(ss[6],0);
}


void livello(int lvl) {
 FILE *f1;
 int t1,OK;  
#define se(x) if (strstr(ss,x)==ss)
 timmag=0;
 for (t=0;t<NOMIMAX;t++)
  memset(nomi[t],0,20);
 nobg=0;Msg=0;ngri=0;ndia=0;nnomi=0;
 
 memset(xcoll,0,81*25);
 memset(gri1,0,10000);
 memset(obg,0,sizeof(struct Tobg)*NOBG);
 ngri=snprintf(gri1,10000,"\5,170");
 OK=0;
 riff = -1;
 f1=fopen("pallo.liv","r");
 while (feof(f1)!=1) {
  ss=ss1;
  fgets(ss,100,f1);
  if (ss[0] == '#') continue;
  se("gri=") ; else { /* togli gli spazi */
    for (t=0;ss[t];t++)
      if (ss[t] == ' ') {
        for (tz=t+1;ss[tz];tz++)
          ss[tz-1] = ss[tz];
        ss[tz-1] = 0;
      }    
  }
  if (!lvl) { /* impostazioni iniziali */
    se("tsparo=") nsparo1 = leggi(1);
    se("LivIni=") lvl=livell=leggi(1);
  }  
  se("LivMax=") LIVELLMAX=leggi(1);
  se("img+") {
    timmag--;
    for (t=4;(t<4096) && (timmag < 4096) && (ss[t] != ';') && ss[t]; t++)
      if ( ss[t] == '\\') {
         if ((ss[t+1] <= '9') && (ss[t+1]>=0))  immag[timmag++] = ss[++t]-48; 
         else immag[timmag++] = ss[++t];
       } else immag[timmag++] = ss[t];
    immag[timmag++] = 0;
  } else
  se("img") {
    if ((ss[3] < '0') || (ss[3] > '9' )) { printf("img: valore non valido\n"); exit(0);}
    nimmag[(u_char)ss[3]-48] = timmag;
    for (t=4;(t<4096) && (timmag < 4096) && (ss[t] != ';') && ss[t]; t++)
      if ( ss[t] == '\\') {
         if ((ss[t+1] <= '9') && (ss[t+1]>=0))  immag[timmag++] = ss[++t]-48; 
         else immag[timmag++] = ss[++t];
       } else immag[timmag++] = ss[t];
    immag[timmag++] = 0;
  }
  se("def") {
    for (t=3;ss[t] && (t<20) && (ss[t] != '='); t++)
        nomi[nnomi][0][t-3] = ss[t];
    nomi[nnomi][0][t-3] =',';   
    nomi[nnomi][0][t-2] =0;   
    if (ss[t] == '=') { t++;
        for (tz = 0;ss[t] && (tz<20) && (ss[t] != ';');) 
           nomi[nnomi][1][tz++] = ss[t++];
        nomi[nnomi][1][tz] =0;   
        nnomi++;
      } else nomi[nnomi][0][0] =0;
  }      
  se("nvitepallo=") vitemax=leggi(1);
  se("Liv=") {
   OK=0;
   for (t=1;t<10;t++)
    if ((t1=leggi(t))==lvl) { OK=1; t=10; }
    else if (!t1) t=10;
  }
  if (OK) {
#ifdef DEBUG
    if (ss[0] != '#')
      fprintf(stderr,"%s\n",ss);
#endif      
  for (tz=0;tz<nnomi;tz++)
   se(nomi[tz][0]) {
     ss = ss1+50;
     for (t=0;nomi[tz][1][t];t++) ss[t]=nomi[tz][1][t];
     for (tz=0;ss1[tz] && (ss1[tz] != ',');tz++) ;
     while (ss1[tz] && (ss1[tz] != ';')) ss[t++] = ss1[tz++];
     ss[t++]=';';
     ss[t++]=0;
#ifdef DEBUG
     fprintf(stderr,"->>  %s\n",ss);
#endif      
     break;     
   }
    se("#") ; else
    se("pntTARMA=")   pntTARMA=leggi(1); else
    se("tfantasm=")   tfantasm=leggi(1); else
    se("tgra=")       tgra=leggi(1); else
    se("rif=1")        riff=-2; else
    se("rif=0")        riff=-1; else
    se("nvite=")      numvite=leggi(1); else
    se("gri=")        { grigl(ss+4);grigl("\r");}  else
    se("tsparo=")     nsparo1 += leggi(1); else
    se("GRIGLIA,")    insobg("",GRIGLIA,leggi(1),leggi(2),gri1); else
    se("PALLO,")      insobg("Pllo",PALLO,leggi(1),leggi(2),pac); else
    se("FANTASMA,")   insobg("",FANTASMA,leggi(1),leggi(2),fanta[0]); else
    se("DIAMANTE,")   insobg("",DIAMANTE,leggi(1),leggi(2),diamante[0]); else
    se("USCITA,")     insobg("",USCITA,leggi(1),leggi(2),uscita[0]); else
    se("FANTASMAAI,") insobg("",FANTASMAAI,leggi(1),leggi(2),fantaAI[0]); else
    se("MASSO,")      insobg("@a",MASSO,leggi(1),leggi(2),masso); else
    se("SERPENTE,")   insobg("",SERPENTE,leggi(1),leggi(2),serpente); else
    se("SERPENTENO,") insobg("",SERPENTENO,leggi(1),leggi(2),ostacolo); else
    se("TARMA,")      insobg("",TARMA,leggi(1),leggi(2),tarma[0]); else
    se("TARMAAI,")    insobg("",TARMAAI,leggi(1),leggi(2),tarma[1]); else
    se("OSTACOLOAI,") insobg("",0x3000,leggi(1),leggi(2),ostacolo); else
    se("OSTACOLO,")   insobg("",0x1800,leggi(1),leggi(2),ostacolo); else
/*    se("0x")          insobg("",leggihex(),leggi(1),leggi(2),ostacolo); else */
    se("Mx")          {
        u_int liv11 = 0;
        u_char *immg = 0;
        for (t=2;(ss[t]!=',')&& ss[t] && (ss[t]!=':');t++)
          switch(ss[t]) {
          case 'r' : liv11 |= 0x00001; break; /* rimbalzo */
          case 't' : liv11 |= 0x00010; break; /* trasparente */
          case 'N' : liv11 |= 0x00020; break; /* nemico */
          case 'P' : liv11 |= 0x00040; break; /* pallo */
          case 'D' : liv11 |= 0x00080; break; /* diamante */
          case 'U' : liv11 |= 0x00200; break; /* uscita */
          case 's' : liv11 |= 0x00400; break; /* serpente */
          case 'i' : liv11 |= 0x00800; break; /* insegue */
          case 'O' : liv11 |= 0x01000; break; /* ostacolo */
          case 'a' : liv11 |= 0x02000; break; /* mossa in AI */
          case 'T' : liv11 |= 0x04000; break; /* tarma */
          case 'z' : liv11 |= 0x10000; break; /* serpente CODA */
          case 'Z' : liv11 |= 0x20000; break; /* serpente TESTA */
          case 'm' : liv11 |= 0x40000; break; /* mostro */
          case 'S' : liv11 |= 0x80000; break; /* sposta */
          case 'g' : liv11 |= 0x100000; break; /* gravita' */
          case 'R' : liv11 |= 0x200000; break; /* relativo */
          case 'l' : liv11 |= 0x400001; break; /* labirinto */
          case '_' : break;
          case 'I' : /* scelta immagine */
                     switch(ss[t+1]) {
                        case '?' : immg=immag+nimmag[ss[t+2]-48];t++; break;
                        case '>' : immg1=ss[t+2];t++;break;
                        case 'P' : immg=pac;         break;
                        case 'f' : immg=fanta[1];    break;
                        case 'F' : immg=fantaAI[1];  break;
                        case 'D' : immg=diamante[0]; break;
                        case 'S' : immg=serpente;    break;
                        case 'O' : immg=ostacolo;    break;
                        case 'T' : immg=tarma[0];    break;
                        case 'M' : immg=masso;       break;
                        case 'U' : immg=uscita[0];   break;
                        case 'm' : immg=mostro[0];   break;
                        case 'i' : immg=incostruzione;break;
                        case 'X' : immg=croce;break;
                        case 'n' : immg=navicella;break;
                        default : t--;immg = "";
                     }
                     t++;	
                     break;
          
/*
# se si usa il Mx si possono usare i seguenti tag:
#  r=rimbalzo,t=trasparente,N=nemico,P=pallo,D=diamante,U=Uscita
#  s=serpende,i=insegue,G=Griglia,a=AI,T=tarma
*/
        }
    insobg("O    ",liv11,leggi(1),leggi(2),immg);
    }
  }  
 }
 insobg("  ",0,0,0,vuoto);sparo1 = nobg-1;
 insobg("  ",0,0,0,vuoto);sparo2 = nobg-1;
 ccprint "\033[2J"); colorebase; 
}
