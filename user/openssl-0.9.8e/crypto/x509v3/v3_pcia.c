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
/* v3_pcia.c -*- mode:C; c-file-style: "eay" -*- */
/* Contributed to the OpenSSL Project 2004
 * by Richard Levitte (richard@levitte.org)
 */
/* Copyright (c) 2004 Kungliga Tekniska Högskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/x509v3.h>

ASN1_SEQUENCE(PROXY_POLICY) =
	{
	ASN1_SIMPLE(PROXY_POLICY,policyLanguage,ASN1_OBJECT),
	ASN1_OPT(PROXY_POLICY,policy,ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(PROXY_POLICY)

IMPLEMENT_ASN1_FUNCTIONS(PROXY_POLICY)

ASN1_SEQUENCE(PROXY_CERT_INFO_EXTENSION) =
	{
	ASN1_OPT(PROXY_CERT_INFO_EXTENSION,pcPathLengthConstraint,ASN1_INTEGER),
	ASN1_SIMPLE(PROXY_CERT_INFO_EXTENSION,proxyPolicy,PROXY_POLICY)
} ASN1_SEQUENCE_END(PROXY_CERT_INFO_EXTENSION)

IMPLEMENT_ASN1_FUNCTIONS(PROXY_CERT_INFO_EXTENSION)
