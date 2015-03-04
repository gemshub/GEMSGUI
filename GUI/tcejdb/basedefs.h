/*************************************************************************************************
 * The abstract database API of EJDB
 *                                                               Copyright (C) 2012-2013 Softmotions Ltd <info@softmotions.com>
 * This file is part of EJDB.
 * EJDB is free software; you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation; either
 * version 2.1 of the License or any later version.  EJDB is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with Tokyo
 * Cabinet; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA.
 *************************************************************************************************/

#ifndef BASIC_H
#define	BASIC_H

#ifdef __cplusplus
#define EJDB_EXTERN_C_START extern "C" {
#define EJDB_EXTERN_C_END }
#else
#define EJDB_EXTERN_C_START
#define EJDB_EXTERN_C_END
#endif

EJDB_EXTERN_C_START

#ifdef __GNUC__
#define EJDB_INLINE static inline
#else
#define EJDB_INLINE static
#endif

#ifdef _WIN32
#ifdef EJDB_DLL
#define EJDB_EXPORT __declspec(dllexport)
#elif !defined(EJDB_STATIC)
#define EJDB_EXPORT __declspec(dllimport)
#else
#define EJDB_EXPORT
#endif
#else
#define EJDB_EXPORT
#endif

#ifdef _WIN32
#include <windows.h>
#define INVALIDHANDLE(_HNDL) (((_HNDL) == INVALID_HANDLE_VALUE) || (_HNDL) == NULL)
#else
typedef int HANDLE;
#define INVALID_HANDLE_VALUE (-1)
#define INVALIDHANDLE(_HNDL) ((_HNDL) < 0 || (_HNDL) == UINT16_MAX)
#endif


#define JDBIDKEYNAME "_id"  /**> Name of PK _id field in BSONs */
#define JDBIDKEYNAMEL 3

EJDB_EXTERN_C_END
#endif	/* BASIC_H */

