/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef RADIUS_CLIENT_H
#define RADIUS_CLIENT_H

typedef enum {
	RADIUS_AUTH
} RadiusType;

/* RADIUS message retransmit list */
struct radius_msg_list {
	struct radius_msg *msg;
	RadiusType msg_type;
	time_t first_try;
	time_t next_try;
	int attempts;
	int next_wait;

	u8 *shared_secret;
	size_t shared_secret_len;

	u8	ApIdx;	// Multiple SSID interface
	/* TODO: server config with failover to backup server(s) */

	struct radius_msg_list *next;
};


typedef enum {
	RADIUS_RX_PROCESSED,
	RADIUS_RX_QUEUED,
	RADIUS_RX_UNKNOWN
} RadiusRxResult;

struct radius_rx_handler {
	RadiusRxResult (*handler)(rtapd *apd, struct radius_msg *msg, struct radius_msg *req,
				  u8 *shared_secret, size_t shared_secret_len, void *data);
	void *data;
};

struct radius_client_data {

#if MULTIPLE_RADIUS
	int mbss_auth_serv_sock[MAX_MBSSID_NUM]; /* socket for authentication RADIUS messages */
#else
	int auth_serv_sock; /* socket for authentication RADIUS messages */
#endif

	struct radius_rx_handler *auth_handlers;
	size_t num_auth_handlers;

	struct radius_msg_list *msgs;
	size_t num_msgs;

	u8 next_radius_identifier;

};

int Radius_client_register(rtapd *apd, RadiusType msg_type,
			   RadiusRxResult (*handler) (rtapd *apd,  struct radius_msg *msg, struct radius_msg *req,
			    u8 *shared_secret, size_t shared_secret_len, void *data),  void *data);
int Radius_client_send(rtapd *rtapd, struct radius_msg *msg, RadiusType msg_type, u8 ApIdx);
u8 Radius_client_get_id(rtapd *rtapd);
void Radius_client_flush(rtapd *rtapd);
int Radius_client_init(rtapd *rtapd);
void Radius_client_deinit(rtapd *rtapd);

#endif /* RADIUS_CLIENT_H */
