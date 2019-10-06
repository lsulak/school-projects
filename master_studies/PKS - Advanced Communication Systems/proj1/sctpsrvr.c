/*
 *  sctpsrvr.c
 *
 *  SCTP multi-stream server.
 *
 *  M. Tim Jones <mtj@mtjones.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
 #include <sys/stat.h>

#include <netinet/in.h>
#include <netinet/sctp.h>

#include "common.h"


int main()
{
  int listenSock, ret, connSock = 0;
  struct sockaddr_in servaddr;
  struct sctp_initmsg initmsg;
 
  struct stat fst; //file time stamp of last modif of makefile
  bzero(&fst,sizeof(fst));
  stat("Makefile",&fst);

  FILE *fr;
  fr = fopen ("Makefile", "rt");

  char buffer[MAX_BUFFER+1];
  memset( &buffer, 0, MAX_BUFFER+1 );

  char ch;
  int i = 0;
  int size;

  fseek(fr, 0L, SEEK_END);
  size = ftell(fr);
  fseek(fr, 0L, SEEK_SET);

  char buffer4file[size];

   while( ( ch = fgetc(fr) ) != EOF ) {
      buffer4file[i] = ch;
      i++;
   }

   fclose(fr);

  /* Create SCTP TCP-Style Socket */
  listenSock = socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );

  /* Accept connections from any interface */
  bzero( (void *)&servaddr, sizeof(servaddr) );
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
  servaddr.sin_port = htons(MY_PORT_NUM);

  ret = bind( listenSock, (struct sockaddr *)&servaddr, sizeof(servaddr) );
  if (ret < 0) {
    fprintf(stderr, "Binding socket have failed!\n");
    return 1;   
  }

  /* Specify that a maximum of 5 streams will be available per socket */
  memset( &initmsg, 0, sizeof(initmsg) );
  initmsg.sinit_num_ostreams = 5;
  initmsg.sinit_max_instreams = 5;
  initmsg.sinit_max_attempts = 4;
  ret = setsockopt( listenSock, IPPROTO_SCTP, SCTP_INITMSG, 
                     &initmsg, sizeof(initmsg) );
  if (ret < 0) {
    fprintf(stderr, "Socket options setting have failed!\n");
    return 1;   
  }

  /* Place the server socket into the listening state */
  listen( listenSock, 5 );

  /* Server loop... */
  while( 1 ) {

    /* Await a new client connection */
    printf("Awaiting a new connection\n");
    connSock = accept( listenSock, (struct sockaddr *)NULL, (int *)NULL );

    /* Send the whole Makefile */
    ret = sctp_sendmsg( connSock, (void *)buffer4file, (size_t)strlen(buffer4file),
                         NULL, 0, 0, 0, MAKEFILE_STREAM, 0, 0 );

    if (ret < 0) {
      fprintf(stderr, "Sending Makefile have failed!\n");
      close( connSock );
      return 1;   
    }

    /* Send information about last modification time of Makefile on stream 0*/
    snprintf( buffer, MAX_BUFFER, "%s\n", ctime(&fst.st_mtime) );
    ret = sctp_sendmsg( connSock, (void *)buffer, (size_t)strlen(buffer),
                         NULL, 0, 0, 0, TS_STREAM, 0, 0 );
    if (ret < 0) {
      fprintf(stderr, "Sending timestamp of Makefile have failed!\n");
      close( connSock );
      return 1;   
    }

    /* Close the client connection */
    close( connSock );
  }

  return 0;
}
