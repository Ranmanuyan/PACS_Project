/*
 * serialize.h - Glop generic object serialization code
 */

/*----------------------------------------------------------------------
-- Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007 François Galea,
-- Laboratoire PRiSM, Univerité de Versailles, France.
-- All rights reserved. E-mail: <francois.galea@prism.uvsq.fr>.
--
-- This file is part of Glop (Generic Linear Optimization Package).
--
-- Glop is free software; you can redistribute it and/or modify it under
-- the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2, or (at your option)
-- any later version.
--
-- Glop is distributed in the hope that it will be useful, but WITHOUT
-- ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
-- or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
-- Public License for more details.
--
-- You should have received a copy of the GNU General Public
-- License along with Glop; see the file COPYING. If not, write to the
-- Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
-- Boston, MA  02110-1301, USA.
----------------------------------------------------------------------*/

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* User-defined structures */
typedef struct _serialize serialize;
typedef struct _deserialize deserialize;

/* Provided Glop object serialization code */
void lp_pack(GSP * lp, serialize * s);
GSP *lp_unpack(deserialize * s);

/* User-defined functions */
int pack_int( serialize * s, const int *p, int count );
int pack_double( serialize * s, const double *p, int count );
int pack_string( serialize * s, const char *p );
int unpack_int( deserialize * s, int *p, int count );
int unpack_double( deserialize * s, double *p, int count );
int unpack_string( deserialize * s, char **p );

#ifdef __cplusplus
}
#endif

#endif
