#include <stdio.h>          
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "remotectrl.h"

/*
char *vstrcat(char *first, ...)
{
	size_t len = 0;
	char *retbuf;
	va_list argp;
	char *p;
	char *tmp="";
	if(first == NULL)
		first=tmp;
//		return NULL;
	
	len = strlen(first);
		
	va_start(argp, first);

	while((p = va_arg(argp, char *)) != NULL)
		len += strlen(p);

	va_end(argp);
	retbuf = (char *)malloc(len + 1);	// +1 for trailing \0 

	if(retbuf == NULL)
		return NULL;		// error 

	(void)strcpy(retbuf, first);

	va_start(argp, first);

	while((p = va_arg(argp, char *)) != NULL)
		(void)strcat(retbuf, p);

	va_end(argp);

	return retbuf;
}
*/

int rmtctrl_write_msg( struct rmt_socket_t *sckHnd, uint32_t id, uint32_t extra, char *message ){
	msg_head_t header;
	int rslt;
	header.id    = id;
	header.len   = strlen(message);
	header.extra = extra;
  rslt = send(sckHnd->fd,&header,sizeof(struct msg_head_t),0);
	if (rslt != -1 && header.len > 0) {
		sckHnd->Tx += rslt;
		rslt = send(sckHnd->fd, message, header.len, 0);
		if (rslt > 0)
		{
			sckHnd->Tx += rslt;
			rslt += sizeof(struct msg_head_t);
		}
	}
	return rslt;
}

int rmtctrl_read_msg( struct rmt_socket_t *sckHnd, msg_head_t *head, char **message )
{
//	msg_head_t header;
	int rslt;
	char *buffer;
	int reading = 0;
	int aux = 0;
  rslt = recv(sckHnd->fd, head, sizeof(struct msg_head_t), 0);
	if (rslt == sizeof(struct msg_head_t) ) {
		sckHnd->Rx += rslt;
		*message = (char *)malloc(head->len+1);
		memset(*message,'\0', head->len+1);
		while ( reading < head->len ){
			aux = recv(sckHnd->fd, *message, head->len, 0);
			switch ( aux ) 
			{
				case -1:
					switch (errno)
					{
						case EINTR:
						case EAGAIN:
							usleep (100);
							break;
						default:
							return -1;
					}
				break;
				case 0: // mean socket was closed
					sckHnd->Rx += reading;
					return reading;
					break;
				break;
				default:
					reading+=aux;
				break;
			}
		}
/*
		buffer = (char *)malloc(head->len+1);
		while ( reading < head->len ){
			memset(buffer,'\0', head->len+1);
			aux = recv(sckHnd->fd, buffer, head->len, 0);
			switch ( aux ) {
				case -1:
					switch (errno){
						case EINTR:
						case EAGAIN:
							usleep (100);
							break;
						default:
							return -1;
					}
				break;
				case 0: // mean socket was closed
					sckHnd->Rx += reading;
					return reading;
					break;
				break;
				default:
					if (reading == 0) 
						*message=(char *)malloc(aux+1);
					else
						*message=(char*)realloc(*message,(reading+aux+1)*sizeof(char));
//						strcat(*message, buffer);
					memcpy(*message+reading, buffer, aux);
					reading += aux;
					*message[reading]=0;
			}
		}
		free(buffer);
*/
		sckHnd->Rx += reading;
		reading += rslt;
		return reading;
	}
	return rslt;
}

void rmtctrl_newClient(struct rmt_socket_t srv, struct rmt_socket_t *client, int *activeClients)
{
	int rslt;
	int cli = (*activeClients);
	rmtctrl_accept(srv,&client[cli]);
	if (client[(*activeClients)].fd != -1)
	{
		(*activeClients)++;
	}
	if ((*activeClients) >= MAX_CLIENTS)
	{
		(*activeClients)--;
		rslt = rmtctrl_write_msg(&client[(*activeClients)],MSG_END,0, "Sorry Server is too Busy\n	Try more late\n" );
		if (rslt > 0) client[(*activeClients)].Tx += rslt;
		rmtctrl_close(&client[(*activeClients)]);
	}
}

void rmtctrl_close ( struct rmt_socket_t *client )
{
	printf("Desde %s se recibieron %d bytes y se enviaron %d bytes\n",inet_ntoa(client->addr.sin_addr),client->Rx,client->Tx);
	close(client->fd); /* cierra fd_rmt_client */
	printf("Se cerro conexión desde %s\n",inet_ntoa(client->addr.sin_addr) ); 
	client->fd = -1;
}

void rmtctrl_accept (struct rmt_socket_t srv, struct rmt_socket_t *client ) 
{
	int sin_size=sizeof(struct sockaddr_in);
	int int_Send;
	struct sockaddr_in addr;
	
	if ((client->fd = accept(srv.fd,(struct sockaddr *)&client->addr,&sin_size))!=-1) 
	{
		client->Rx = 0;
		client->Tx = 0;
		unsigned char c = sizeof(uint32_t);
		int_Send = send(client->fd, &c, 1, 0);
		if (int_Send > 0) client->Tx += int_Send;
		printf("Se abrió una conexión desde %s\n", inet_ntoa(client->addr.sin_addr)); 
	}
}

//void cleanClients (int *table, int *n)
void rmtctrl_cleanClients (struct rmt_socket_t *client, int *n)
{
	int i,j;

	if ((client == NULL) || ((*n) == 0))
		return;

	j=0;
	for (i=0; i<(*n); i++)
	{
		if (client[i].fd != -1)
		{
			client[j].fd = client[i].fd;
			client[j].addr = client[i].addr;
			client[j].Rx = client[i].Rx;
			client[j].Tx = client[i].Tx;
			j++;
		}
	}
	
	*n = j;
}

int rmtctrl_maxValue (struct rmt_socket_t *client, int n)
{
	int i;
	int max;

	if ((client == NULL) || (n<1))
		return 0;
		
	max = client[0].fd;
	for (i=0; i<n; i++)
		if (client[i].fd > max)
			max = client[i].fd;

	return max;
}

struct rmt_socket_t rmtctrl_initSrv(struct in_addr rmtlisten, int rmtport){
	struct rmt_socket_t srv;
	if ((srv.fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {  
		printf("error en socket()\n");
		exit(1);
	}
	srv.addr.sin_family = AF_INET;
	srv.addr.sin_port = htons(rmtport); 
	srv.addr.sin_addr.s_addr = rmtlisten.s_addr;
	bzero(&(srv.addr.sin_zero),8); 

	if(bind(srv.fd,(struct sockaddr*)&srv.addr,sizeof(struct sockaddr))==-1) {
		printf("error en bind() \n");
		exit(-1);
	}     

	if(listen(srv.fd,BACKLOG) == -1) {
		printf("error en listen()\n");
		exit(-1);
	}
	return srv;
}

void rmtctrl_srv(struct rmt_socket_t srv, struct rmt_socket_t *client, int *activeClients)
{
	fd_set fdRead;
	int maxHnd;
	int i;
	struct timeval  nowait; 
	memset((char *)&nowait,0,sizeof(nowait)); 

	rmtctrl_cleanClients(client, activeClients);
	FD_ZERO (&fdRead);
	FD_SET (srv.fd, &fdRead);

	for (i=0; i<*activeClients; i++)
		FD_SET (client[i].fd, &fdRead);

	maxHnd = rmtctrl_maxValue (client, *activeClients);
		
	if (maxHnd < srv.fd)
		maxHnd = srv.fd;

	select (maxHnd + 1, &fdRead, NULL, NULL,&nowait);
	for (i=0; i<*activeClients; i++)
	{
		if (FD_ISSET (client[i].fd, &fdRead))
		{
				rmtctrl_msg_proccess(&client[i]);
		}
	}
	if (FD_ISSET (srv.fd, &fdRead))
		rmtctrl_newClient(srv,client, &(*activeClients));
}

int send_line( struct rmt_socket_t *client, int msg_type, int msg_extra, char *fmt, char *data)
{
	char str[2048];
	memset(str,'\0',2048);
	sprintf(str,fmt,data);
	return rmtctrl_write_msg(client,msg_type,msg_extra, str);
}

int send_octets( struct rmt_socket_t *client, int msg_type, int msg_extra, char *fmt, uint64_t value)
{
	char *Buffer = (char *)octets2str(value);
	int ret = send_line( client, msg_type, msg_extra, fmt, Buffer);
	free(Buffer);
	return ret;
}

int send_mac( struct rmt_socket_t *client, int msg_type, int msg_extra, char *fmt, uint8_t *value)
{
	char *Buffer = (char*)mac2str(value);
	int ret = send_line( client, msg_type, msg_extra, fmt, Buffer);
	free(Buffer);
	return ret;
}

int send_number( struct rmt_socket_t *client, int msg_type, int msg_extra, char *fmt, uint64_t value)
{
	char Buffer[64];
	sprintf(Buffer,"%d", value);
	return send_line( client, msg_type, msg_extra, fmt, Buffer);
}

const char * mac2str(uint8_t *mac)
{
	char *buffer;
	buffer=(char*)malloc(18*sizeof(char));
	memset(buffer,'\0',18);
	(void) sprintf(buffer,"%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
		    mac[0], mac[1],
		    mac[2], mac[3],
		    mac[4], mac[5]);
	return buffer;
}

const char * octets2str(uint64_t value)
{
	char *buffer;
	buffer=(char*)malloc(13*sizeof(char));
	memset(buffer,'\0',13);
	if (value/8 > 1073741824){ // gigas
		sprintf(buffer,"%.1f(GiB)",((value/8.0)/1073741824.0));
	}else if (value/8 > 1048576){ // megas
		sprintf(buffer,"%.1f(MiB)",((value/8.0)/1048576.0));
	}else if (value/8 > 1024){ // KiloBytes
		sprintf(buffer,"%.1f(KiB)",((value/8.0)/1024.0));
	}else // Bytes
		sprintf(buffer,"%.1f(B)",(value/8));
	return buffer;
}

