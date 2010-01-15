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
/* -*- buffer-read-only: t -*- vi: set ro:
 *
 * Prototypes for autoopts
 * Generated Thu Oct 12 17:44:48 PDT 2006
 */
#ifndef AUTOOPTS_PROTO_H_GUARD
#define AUTOOPTS_PROTO_H_GUARD 1
#ifndef LOCAL
#  define LOCAL extern
#  define REDEF_LOCAL 1
#else
#  undef  REDEF_LOCAL
#endif
/*
 *  Extracted from autoopts.c
 */
LOCAL void *
ao_malloc( size_t sz );

LOCAL void *
ao_realloc( void *p, size_t sz );

LOCAL void
ao_free( void *p );

LOCAL char *
ao_strdup( char const *str );

LOCAL tSuccess
handleOption( tOptions* pOpts, tOptState* pOptState );

LOCAL tSuccess
longOptionFind( tOptions* pOpts, char* pzOptName, tOptState* pOptState );

LOCAL tSuccess
shortOptionFind( tOptions* pOpts, uint_t optValue, tOptState* pOptState );

LOCAL tSuccess
doImmediateOpts( tOptions* pOpts );

LOCAL tSuccess
doRegularOpts( tOptions* pOpts );

/*
 *  Extracted from configfile.c
 */
LOCAL void
internalFileLoad( tOptions* pOpts );

LOCAL char*
parseAttributes(
    tOptions*           pOpts,
    char*               pzText,
    tOptionLoadMode*    pMode,
    tOptionValue*       pType );

LOCAL tSuccess
validateOptionsStruct( tOptions* pOpts, char const* pzProgram );

/*
 *  Extracted from environment.c
 */
LOCAL void
doPrognameEnv( tOptions* pOpts, teEnvPresetType type );

LOCAL void
doEnvPresets( tOptions* pOpts, teEnvPresetType type );

/*
 *  Extracted from load.c
 */
LOCAL void
mungeString( char* pzTxt, tOptionLoadMode mode );

LOCAL void
loadOptionLine(
    tOptions*   pOpts,
    tOptState*  pOS,
    char*       pzLine,
    tDirection  direction,
    tOptionLoadMode   load_mode );

/*
 *  Extracted from sort.c
 */
LOCAL void
optionSort( tOptions* pOpts );

/*
 *  Extracted from stack.c
 */
LOCAL void
addArgListEntry( void** ppAL, void* entry );

#ifdef REDEF_LOCAL
#  undef LOCAL
#  define LOCAL
#endif

#endif /* AUTOOPTS_PROTO_H_GUARD */
