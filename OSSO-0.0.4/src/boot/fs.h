/*
 Questo file dovra' essere usato per scegliere il file system
 nella procedura di installazione. Bastera' scrivere il fs giusto
 in questo file e ricompilare la parte di boot prima di installare
 OSSO.

 La doppia inclusione e' necessaria perche' non so come si fa il
 linking in bcc e non mi sembra il caso di cercare il modo visto
 che il bcc ha gia' dei grossi problemi con sorgenti unici!!!
*/

#include "fat.c"
