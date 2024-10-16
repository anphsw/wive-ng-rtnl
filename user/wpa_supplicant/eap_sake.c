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
/*
 * EAP peer method: EAP-SAKE (RFC 4763)
 * Copyright (c) 2006, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"

#include "common.h"
#include "eap_i.h"
#include "config_ssid.h"
#include "eap_sake_common.h"

struct eap_sake_data {
	enum { IDENTITY, CHALLENGE, CONFIRM, SUCCESS, FAILURE } state;
	u8 root_secret_a[EAP_SAKE_ROOT_SECRET_LEN];
	u8 root_secret_b[EAP_SAKE_ROOT_SECRET_LEN];
	u8 rand_s[EAP_SAKE_RAND_LEN];
	u8 rand_p[EAP_SAKE_RAND_LEN];
	struct {
		u8 auth[EAP_SAKE_TEK_AUTH_LEN];
		u8 cipher[EAP_SAKE_TEK_CIPHER_LEN];
	} tek;
	u8 msk[EAP_MSK_LEN];
	u8 emsk[EAP_EMSK_LEN];
	u8 session_id;
	int session_id_set;
	u8 *peerid;
	size_t peerid_len;
	u8 *serverid;
	size_t serverid_len;
};


static const char * eap_sake_state_txt(int state)
{
	switch (state) {
	case IDENTITY:
		return "IDENTITY";
	case CHALLENGE:
		return "CHALLENGE";
	case CONFIRM:
		return "CONFIRM";
	case SUCCESS:
		return "SUCCESS";
	case FAILURE:
		return "FAILURE";
	default:
		return "?";
	}
}


static void eap_sake_state(struct eap_sake_data *data, int state)
{
	wpa_printf(MSG_DEBUG, "EAP-SAKE: %s -> %s",
		   eap_sake_state_txt(data->state),
		   eap_sake_state_txt(state));
	data->state = state;
}


static void eap_sake_deinit(struct eap_sm *sm, void *priv);


static void * eap_sake_init(struct eap_sm *sm)
{
	struct wpa_ssid *config = eap_get_config(sm);
	struct eap_sake_data *data;

	if (config == NULL) {
		wpa_printf(MSG_INFO, "EAP-SAKE: No configuration found");
		return NULL;
	}

	if (!config->eappsk ||
	    config->eappsk_len != 2 * EAP_SAKE_ROOT_SECRET_LEN) {
		wpa_printf(MSG_INFO, "EAP-SAKE: No key (eappsk) of correct "
			   "length configured");
		return NULL;
	}

	data = os_zalloc(sizeof(*data));
	if (data == NULL)
		return NULL;
	data->state = IDENTITY;

	if (config->nai) {
		data->peerid = os_malloc(config->nai_len);
		if (data->peerid == NULL) {
			eap_sake_deinit(sm, data);
			return NULL;
		}
		os_memcpy(data->peerid, config->nai, config->nai_len);
		data->peerid_len = config->nai_len;
	}

	os_memcpy(data->root_secret_a, config->eappsk,
		  EAP_SAKE_ROOT_SECRET_LEN);
	os_memcpy(data->root_secret_b,
		  config->eappsk + EAP_SAKE_ROOT_SECRET_LEN,
		  EAP_SAKE_ROOT_SECRET_LEN);

	return data;
}


static void eap_sake_deinit(struct eap_sm *sm, void *priv)
{
	struct eap_sake_data *data = priv;
	os_free(data->serverid);
	os_free(data->peerid);
	os_free(data);
}


static u8 * eap_sake_build_msg(struct eap_sake_data *data, u8 **payload,
			       int id, size_t *length, u8 subtype)
{
	struct eap_sake_hdr *req;
	u8 *msg;

	*length += sizeof(struct eap_sake_hdr);

	msg = os_zalloc(*length);
	if (msg == NULL) {
		wpa_printf(MSG_ERROR, "EAP-SAKE: Failed to allocate memory "
			   "request");
		return NULL;
	}

	req = (struct eap_sake_hdr *) msg;
	req->code = EAP_CODE_RESPONSE;
	req->identifier = id;
	req->length = htons((u16) *length);
	req->type = EAP_TYPE_SAKE;
	req->version = EAP_SAKE_VERSION;
	req->session_id = data->session_id;
	req->subtype = subtype;
	*payload = (u8 *) (req + 1);

	return msg;
}


static u8 * eap_sake_process_identity(struct eap_sm *sm,
				      struct eap_sake_data *data,
				      struct eap_method_ret *ret,
				      const u8 *reqData, size_t reqDataLen,
				      const u8 *payload, size_t payload_len,
				      size_t *respDataLen)
{
	struct eap_sake_parse_attr attr;
	u8 *resp, *rpos;
	const struct eap_hdr *hdr = (const struct eap_hdr *) reqData;

	if (data->state != IDENTITY) {
		ret->ignore = TRUE;
		return NULL;
	}

	wpa_printf(MSG_DEBUG, "EAP-SAKE: Received Request/Identity");

	if (eap_sake_parse_attributes(payload, payload_len, &attr))
		return NULL;

	if (!attr.perm_id_req && !attr.any_id_req) {
		wpa_printf(MSG_INFO, "EAP-SAKE: No AT_PERM_ID_REQ or "
			   "AT_ANY_ID_REQ in Request/Identity");
		return NULL;
	}

	wpa_printf(MSG_DEBUG, "EAP-SAKE: Sending Response/Identity");

	*respDataLen = 2 + data->peerid_len;
	resp = eap_sake_build_msg(data, &rpos, hdr->identifier, respDataLen,
				  EAP_SAKE_SUBTYPE_IDENTITY);
	if (resp == NULL)
		return NULL;

	wpa_printf(MSG_DEBUG, "EAP-SAKE: * AT_PEERID");
	*rpos++ = EAP_SAKE_AT_PEERID;
	*rpos++ = 2 + data->peerid_len;
	if (data->peerid)
		os_memcpy(rpos, data->peerid, data->peerid_len);

	eap_sake_state(data, CHALLENGE);

	return resp;
}


static u8 * eap_sake_process_challenge(struct eap_sm *sm,
				       struct eap_sake_data *data,
				       struct eap_method_ret *ret,
				       const u8 *reqData, size_t reqDataLen,
				       const u8 *payload, size_t payload_len,
				       size_t *respDataLen)
{
	struct eap_sake_parse_attr attr;
	u8 *resp, *rpos;
	const struct eap_hdr *hdr = (const struct eap_hdr *) reqData;

	if (data->state != IDENTITY && data->state != CHALLENGE) {
		wpa_printf(MSG_DEBUG, "EAP-SAKE: Request/Challenge received "
			   "in unexpected state (%d)", data->state);
		ret->ignore = TRUE;
		return NULL;
	}
	if (data->state == IDENTITY)
		eap_sake_state(data, CHALLENGE);

	wpa_printf(MSG_DEBUG, "EAP-SAKE: Received Request/Challenge");

	if (eap_sake_parse_attributes(payload, payload_len, &attr))
		return NULL;

	if (!attr.rand_s) {
		wpa_printf(MSG_INFO, "EAP-SAKE: Request/Challenge did not "
			   "include AT_RAND_S");
		return NULL;
	}

	os_memcpy(data->rand_s, attr.rand_s, EAP_SAKE_RAND_LEN);
	wpa_hexdump(MSG_MSGDUMP, "EAP-SAKE: RAND_S (server rand)",
		    data->rand_s, EAP_SAKE_RAND_LEN);

	if (hostapd_get_rand(data->rand_p, EAP_SAKE_RAND_LEN)) {
		wpa_printf(MSG_ERROR, "EAP-SAKE: Failed to get random data");
		return NULL;
	}
	wpa_hexdump(MSG_MSGDUMP, "EAP-SAKE: RAND_P (peer rand)",
		    data->rand_p, EAP_SAKE_RAND_LEN);

	os_free(data->serverid);
	data->serverid = NULL;
	data->serverid_len = 0;
	if (attr.serverid) {
		wpa_hexdump_ascii(MSG_MSGDUMP, "EAP-SAKE: SERVERID",
				  attr.serverid, attr.serverid_len);
		data->serverid = os_malloc(attr.serverid_len);
		if (data->serverid == NULL)
			return NULL;
		os_memcpy(data->serverid, attr.serverid, attr.serverid_len);
		data->serverid_len = attr.serverid_len;
	}

	eap_sake_derive_keys(data->root_secret_a, data->root_secret_b,
			     data->rand_s, data->rand_p,
			     (u8 *) &data->tek, data->msk, data->emsk);

	wpa_printf(MSG_DEBUG, "EAP-SAKE: Sending Response/Challenge");

	*respDataLen = 2 + EAP_SAKE_RAND_LEN + 2 + EAP_SAKE_MIC_LEN;
	if (data->peerid)
		*respDataLen += 2 + data->peerid_len;
	resp = eap_sake_build_msg(data, &rpos, hdr->identifier, respDataLen,
				  EAP_SAKE_SUBTYPE_CHALLENGE);
	if (resp == NULL)
		return NULL;

	wpa_printf(MSG_DEBUG, "EAP-SAKE: * AT_RAND_P");
	*rpos++ = EAP_SAKE_AT_RAND_P;
	*rpos++ = 2 + EAP_SAKE_RAND_LEN;
	os_memcpy(rpos, data->rand_p, EAP_SAKE_RAND_LEN);
	rpos += EAP_SAKE_RAND_LEN;

	if (data->peerid) {
		wpa_printf(MSG_DEBUG, "EAP-SAKE: * AT_PEERID");
		*rpos++ = EAP_SAKE_AT_PEERID;
		*rpos++ = 2 + data->peerid_len;
		os_memcpy(rpos, data->peerid, data->peerid_len);
		rpos += data->peerid_len;
	}

	wpa_printf(MSG_DEBUG, "EAP-SAKE: * AT_MIC_P");
	*rpos++ = EAP_SAKE_AT_MIC_P;
	*rpos++ = 2 + EAP_SAKE_MIC_LEN;
	if (eap_sake_compute_mic(data->tek.auth, data->rand_s, data->rand_p,
				 data->serverid, data->serverid_len,
				 data->peerid, data->peerid_len, 1,
				 resp, *respDataLen, rpos, rpos)) {
		wpa_printf(MSG_INFO, "EAP-SAKE: Failed to compute MIC");
		os_free(resp);
		return NULL;
	}

	eap_sake_state(data, CONFIRM);

	return resp;
}


static u8 * eap_sake_process_confirm(struct eap_sm *sm,
				     struct eap_sake_data *data,
				     struct eap_method_ret *ret,
				     const u8 *reqData, size_t reqDataLen,
				     const u8 *payload, size_t payload_len,
				     size_t *respDataLen)
{
	struct eap_sake_parse_attr attr;
	u8 mic_s[EAP_SAKE_MIC_LEN];
	u8 *resp, *rpos;
	const struct eap_hdr *hdr = (const struct eap_hdr *) reqData;

	if (data->state != CONFIRM) {
		ret->ignore = TRUE;
		return NULL;
	}

	wpa_printf(MSG_DEBUG, "EAP-SAKE: Received Request/Confirm");

	if (eap_sake_parse_attributes(payload, payload_len, &attr))
		return NULL;

	if (!attr.mic_s) {
		wpa_printf(MSG_INFO, "EAP-SAKE: Request/Confirm did not "
			   "include AT_MIC_S");
		return NULL;
	}

	eap_sake_compute_mic(data->tek.auth, data->rand_s, data->rand_p,
			     data->serverid, data->serverid_len,
			     data->peerid, data->peerid_len, 0,
			     reqData, reqDataLen, attr.mic_s, mic_s);
	if (os_memcmp(attr.mic_s, mic_s, EAP_SAKE_MIC_LEN) != 0) {
		wpa_printf(MSG_INFO, "EAP-SAKE: Incorrect AT_MIC_S");
		eap_sake_state(data, FAILURE);
		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_FAIL;
		ret->allowNotifications = FALSE;
		*respDataLen = 0;
		wpa_printf(MSG_DEBUG, "EAP-SAKE: Sending "
			   "Response/Auth-Reject");
		return eap_sake_build_msg(data, &rpos, hdr->identifier,
					  respDataLen,
					  EAP_SAKE_SUBTYPE_AUTH_REJECT);
	}

	wpa_printf(MSG_DEBUG, "EAP-SAKE: Sending Response/Confirm");

	*respDataLen = 2 + EAP_SAKE_MIC_LEN;
	resp = eap_sake_build_msg(data, &rpos, hdr->identifier, respDataLen,
				  EAP_SAKE_SUBTYPE_CONFIRM);
	if (resp == NULL)
		return NULL;

	wpa_printf(MSG_DEBUG, "EAP-SAKE: * AT_MIC_P");
	*rpos++ = EAP_SAKE_AT_MIC_P;
	*rpos++ = 2 + EAP_SAKE_MIC_LEN;
	if (eap_sake_compute_mic(data->tek.auth, data->rand_s, data->rand_p,
				 data->serverid, data->serverid_len,
				 data->peerid, data->peerid_len, 1,
				 resp, *respDataLen, rpos, rpos)) {
		wpa_printf(MSG_INFO, "EAP-SAKE: Failed to compute MIC");
		os_free(resp);
		return NULL;
	}

	eap_sake_state(data, SUCCESS);
	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_UNCOND_SUCC;
	ret->allowNotifications = FALSE;

	return resp;
}


static u8 * eap_sake_process(struct eap_sm *sm, void *priv,
			    struct eap_method_ret *ret,
			    const u8 *reqData, size_t reqDataLen,
			    size_t *respDataLen)
{
	struct eap_sake_data *data = priv;
	const struct eap_sake_hdr *req;
	u8 *resp;
	const u8 *pos, *end;
	size_t len;
	u8 subtype, session_id;

	pos = eap_hdr_validate(EAP_VENDOR_IETF, EAP_TYPE_SAKE,
			       reqData, reqDataLen, &len);
	if (pos == NULL || len < 3) {
		ret->ignore = TRUE;
		return NULL;
	}

	req = (const struct eap_sake_hdr *) reqData;
	subtype = req->subtype;
	session_id = req->session_id;
	pos = (const u8 *) (req + 1);
	end = reqData + be_to_host16(req->length);

	wpa_printf(MSG_DEBUG, "EAP-SAKE: Received frame: subtype %d "
		   "session_id %d", subtype, session_id);
	wpa_hexdump(MSG_DEBUG, "EAP-SAKE: Received attributes",
		    pos, end - pos);

	if (data->session_id_set && data->session_id != session_id) {
		wpa_printf(MSG_INFO, "EAP-SAKE: Session ID mismatch (%d,%d)",
			   session_id, data->session_id);
		ret->ignore = TRUE;
		return NULL;
	}
	data->session_id = session_id;
	data->session_id_set = 1;

	ret->ignore = FALSE;
	ret->methodState = METHOD_MAY_CONT;
	ret->decision = DECISION_FAIL;
	ret->allowNotifications = TRUE;

	switch (subtype) {
	case EAP_SAKE_SUBTYPE_IDENTITY:
		resp = eap_sake_process_identity(sm, data, ret, reqData,
						 reqDataLen, pos, end - pos,
						 respDataLen);
		break;
	case EAP_SAKE_SUBTYPE_CHALLENGE:
		resp = eap_sake_process_challenge(sm, data, ret, reqData,
						  reqDataLen, pos, end - pos,
						  respDataLen);
		break;
	case EAP_SAKE_SUBTYPE_CONFIRM:
		resp = eap_sake_process_confirm(sm, data, ret, reqData,
						reqDataLen, pos, end - pos,
						respDataLen);
		break;
	default:
		wpa_printf(MSG_DEBUG, "EAP-SAKE: Ignoring message with "
			   "unknown subtype %d", subtype);
		ret->ignore = TRUE;
		return NULL;
	}

	if (ret->methodState == METHOD_DONE)
		ret->allowNotifications = FALSE;

	return resp;
}


static Boolean eap_sake_isKeyAvailable(struct eap_sm *sm, void *priv)
{
	struct eap_sake_data *data = priv;
	return data->state == SUCCESS;
}


static u8 * eap_sake_getKey(struct eap_sm *sm, void *priv, size_t *len)
{
	struct eap_sake_data *data = priv;
	u8 *key;

	if (data->state != SUCCESS)
		return NULL;

	key = os_malloc(EAP_MSK_LEN);
	if (key == NULL)
		return NULL;
	os_memcpy(key, data->msk, EAP_MSK_LEN);
	*len = EAP_MSK_LEN;

	return key;
}


static u8 * eap_sake_get_emsk(struct eap_sm *sm, void *priv, size_t *len)
{
	struct eap_sake_data *data = priv;
	u8 *key;

	if (data->state != SUCCESS)
		return NULL;

	key = os_malloc(EAP_EMSK_LEN);
	if (key == NULL)
		return NULL;
	os_memcpy(key, data->emsk, EAP_EMSK_LEN);
	*len = EAP_EMSK_LEN;

	return key;
}


int eap_peer_sake_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_peer_method_alloc(EAP_PEER_METHOD_INTERFACE_VERSION,
				    EAP_VENDOR_IETF, EAP_TYPE_SAKE, "SAKE");
	if (eap == NULL)
		return -1;

	eap->init = eap_sake_init;
	eap->deinit = eap_sake_deinit;
	eap->process = eap_sake_process;
	eap->isKeyAvailable = eap_sake_isKeyAvailable;
	eap->getKey = eap_sake_getKey;
	eap->get_emsk = eap_sake_get_emsk;

	ret = eap_peer_method_register(eap);
	if (ret)
		eap_peer_method_free(eap);
	return ret;
}
