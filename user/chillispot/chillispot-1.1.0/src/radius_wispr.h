/* 
 * Radius client functions.
 *
 * Copyright (c) 2006, Jens Jakobsen 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   Neither the names of copyright holders nor the names of its contributors
 *   may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Copyright (C) 2003, 2004, 2005 Mondru AB.
 * 
 * The contents of this file may be used under the terms of the GNU
 * General Public License Version 2, provided that the above copyright
 * notice and this permission notice is included in all copies or
 * substantial portions of the software.
 * 
 */

#ifndef _RADIUS_WISPR_H
#define _RADIUS_WISPR_H

#define RADIUS_VENDOR_WISPR                         14122
#define	RADIUS_ATTR_WISPR_LOCATION_ID	                1
#define	RADIUS_ATTR_WISPR_LOCATION_NAME		        2 /* string */
#define	RADIUS_ATTR_WISPR_LOGOFF_URL		        3 /* string */
#define	RADIUS_ATTR_WISPR_REDIRECTION_URL		4 /* string */
#define	RADIUS_ATTR_WISPR_BANDWIDTH_MIN_UP		5 /* integer */
#define	RADIUS_ATTR_WISPR_BANDWIDTH_MIN_DOWN	        6 /* integer */
#define	RADIUS_ATTR_WISPR_BANDWIDTH_MAX_UP		7 /* integer */
#define	RADIUS_ATTR_WISPR_BANDWIDTH_MAX_DOWN	        8 /* integer */
#define	RADIUS_ATTR_WISPR_SESSION_TERMINATE_TIME	9 /* string */
#define	RADIUS_ATTR_WISPR_SESSION_TERMINATE_END_OF_DAY 10 /* string */
#define	RADIUS_ATTR_WISPR_BILLING_CLASS_OF_SERVICE     11 /* string */

#endif	/* !_RADIUS_WISPR_H */
