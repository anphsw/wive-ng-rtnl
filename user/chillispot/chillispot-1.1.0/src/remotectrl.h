#include <stdarg.h>
#ifndef _RMTCTRL_H
#define _RMTCTRL_H

enum 
{
	MSG_OK              = 0,
	MSG_START           = 1,
	MSG_PART            = 2,
	MSG_END             = 3,
	QRY_STATUS          = 100,
	QRY_CONNECTED_LIST	= 101,
	QRY_CONNECTED_FULL_LIST	= 102,
	QRY_MACADDR         = 103,
	QRY_IPADDR          = 104,
	QRY_USERNAME        = 105,

	CMD_AUTHORIZE				= 200,
	CMD_DISCONNECT			= 201,
	
	EXTRA_ALL_OP				= 300,
	EXTRA_MAC_OP				= 301,
	EXTRA_IP_OP					= 302,
	EXTRA_USER_OP				= 303,
};

enum
{
	STRING,
	IP_ADDR,
	MAC_ADDR,
	NUMBER,
	OCTETS,
};

#define PORT 15557 /* El puerto que ser? abierto */
#define BACKLOG 2 /* El n?mero de conexiones permitidas */
#define MAX_CLIENTS 10
static char CONVERT_BUFF[1024];

typedef struct msg_head_t {
	uint32_t id;
	uint32_t extra;
	uint32_t len;
} msg_head_t;

typedef struct rmt_socket_t {
	int fd;
	struct sockaddr_in addr;
	int Rx;
	int Tx;
} rmt_socket_t;

//char *vstrcat(char *first, ...);

	
int rmtctrl_write_msg( struct rmt_socket_t *sckHnd, uint32_t id, uint32_t extra, char *message );
int rmtctrl_read_msg( struct rmt_socket_t *sckHnd, msg_head_t *head, char **message );
void rmtctrl_newClient(struct rmt_socket_t srv, struct rmt_socket_t *client, int *activeClients);
void rmtctrl_close ( struct rmt_socket_t *client );
void rmtctrl_accept (struct rmt_socket_t srv, struct rmt_socket_t *client );
void rmtctrl_cleanClients (struct rmt_socket_t *client, int *n);

//inicializa las variables y arranca el servidor
struct rmt_socket_t rmtctrl_initSrv(struct in_addr rmtlisten, int rmtport);
//esta funcion es la que va dentro del loop del programa que lo utiliza
void rmtctrl_srv(struct rmt_socket_t srv, struct rmt_socket_t *client, int *activeClients);
//En esta funcion es donde se define como se procesan los mensajes
void rmtctrl_msg_proccess(struct rmt_socket_t *client);

//char * pepe(char *dest, char *sep, char *src);

//char * send_value(char *name, char *value, int len, struct in_addr *addr,
//	    uint8_t *mac, long int *integer);
//int addfield(char* reg, char *field, char sep);
//int addfield1(char** reg,int rlen, char *field, int flen, char sep);
//int sendConnectedInfo(struct app_conn_t *appconn, struct rmt_socket_t *client); 
//const char * value2str( int type, void *value);
const char * octets2str(uint64_t value);
const char * mac2str(uint8_t *mac);
int send_line( struct rmt_socket_t *client, int msg_type, int msg_extra, char *fmt, char *data);
int send_octets( struct rmt_socket_t *client, int msg_type, int msg_extra, char *fmt, uint64_t value);
int send_mac( struct rmt_socket_t *client, int msg_type, int msg_extra, char *fmt, uint8_t *value);
int send_number( struct rmt_socket_t *client, int msg_type, int msg_extra, char *fmt, uint64_t value);

#endif	/* !_RMTCTRL_H */
