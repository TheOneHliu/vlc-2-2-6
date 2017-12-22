/*****************************************************************************
 * getaddrinfo.c: getaddrinfo/getnameinfo replacement functions
 *****************************************************************************
 * Copyright (C) 2005 VLC authors and VideoLAN
 * Copyright (C) 2002-2007 Rémi Denis-Courmont
 * $Id: 16dd68e3a3e21dc7aaf2c96dba675b112a29ea19 $
 *
 * Author: Rémi Denis-Courmont <rem # videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>

#include <stddef.h> /* size_t */
#include <string.h> /* strlen(), memcpy(), memset(), strchr() */
#include <stdlib.h> /* malloc(), free(), strtoul() */
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <vlc_network.h>
#include <vlc_net_data_notify.h>
#ifndef AF_UNSPEC
#   define AF_UNSPEC   0
#endif

int vlc_getnameinfo( const struct sockaddr *sa, int salen,
                     char *host, int hostlen, int *portnum, int flags )
{
    char psz_servbuf[6], *psz_serv;
    int i_servlen, i_val;

    flags |= NI_NUMERICSERV;
    if( portnum != NULL )
    {
        psz_serv = psz_servbuf;
        i_servlen = sizeof( psz_servbuf );
    }
    else
    {
        psz_serv = NULL;
        i_servlen = 0;
    }

    i_val = getnameinfo(sa, salen, host, hostlen, psz_serv, i_servlen, flags);

    if( portnum != NULL )
        *portnum = atoi( psz_serv );

    return i_val;
}


/**
 * Resolves a host name to a list of socket addresses (like getaddrinfo()).
 *
 * @param node host name to resolve (encoded as UTF-8), or NULL
 * @param i_port port number for the socket addresses
 * @param p_hints parameters (see getaddrinfo() manual page)
 * @param res pointer set to the resulting chained list.
 * @return 0 on success, a getaddrinfo() error otherwise.
 * On failure, *res is undefined. On success, it must be freed with
 * freeaddrinfo().
 */
int vlc_getaddrinfo (const char *node, unsigned port,
                     const struct addrinfo *hints, struct addrinfo **res)
{
    char hostbuf[NI_MAXHOST], portbuf[6], *servname;

    /*
     * In VLC, we always use port number as integer rather than strings
     * for historical reasons (and portability).
     */
    if (port != 0)
    {
        if (port > 65535)
            return EAI_SERVICE;
        /* cannot overflow */
        snprintf (portbuf, sizeof (portbuf), "%u", port);
        servname = portbuf;
    }
    else
        servname = NULL;

    /*
     * VLC extensions :
     * - accept the empty string as unspecified host (i.e. NULL)
     * - ignore square brackets (for IPv6 numerals)
     */
    if (node != NULL)
    {
        if (node[0] == '[')
        {
            size_t len = strlen (node + 1);
            if ((len <= sizeof (hostbuf)) && (node[len] == ']'))
            {
                assert (len > 0);
                memcpy (hostbuf, node + 1, len - 1);
                hostbuf[len - 1] = '\0';
                node = hostbuf;
            }
        }
        if (node[0] == '\0')
            node = NULL;
    }

	
{
		unsigned long iIP = 0;
		if (hints && (hints->ai_family == AF_INET6 || hints->ai_flags & AI_NUMERICHOST))
		{
			return getaddrinfo (node, servname, hints, res);
		}
		else if (GetCustomeHost(node, &iIP))
		{
			msg_Info_Br("GetCustomeHost %s:%d",node, iIP);
			int buf_size = sizeof(struct addrinfo);
			struct addrinfo* addr_info = (struct addrinfo*)malloc(buf_size);
			*res = 	addr_info;	
			memset(addr_info, 0, buf_size);
			addr_info->ai_flags = 0;
			addr_info->ai_family = AF_INET;
			if (hints)
				addr_info->ai_socktype = hints->ai_socktype;
			else
				addr_info->ai_socktype = SOCK_STREAM;
			if (hints)
				addr_info->ai_protocol = hints->ai_protocol;
			else
				addr_info->ai_protocol = IPPROTO_IP;

			addr_info->ai_addrlen = sizeof(struct sockaddr);
			addr_info->ai_canonname = NULL;
			addr_info->ai_addr =(struct sockaddr*)malloc(addr_info->ai_addrlen);

			struct sockaddr_in * p_addr = (struct sockaddr_in *)addr_info->ai_addr;
			p_addr->sin_family = AF_INET;
			p_addr->sin_port = htons(port);
			p_addr->sin_addr.s_addr = iIP;

			addr_info->ai_next = NULL;
		}
		else
		{
			return getaddrinfo (node, servname, hints, res);
		}
}
	
    return getaddrinfo (node, servname, hints, res);
}
