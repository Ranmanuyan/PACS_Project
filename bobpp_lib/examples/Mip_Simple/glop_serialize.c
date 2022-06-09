/*
 * serialize.c - Glop generic object serialization code
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glop.h>
#include "glop_serialize.h"

static void *umalloc( size_t size )
{
  void *ptr = malloc( size );

  if ( ptr == NULL )
  {
    perror( "umalloc" );
    exit( 1 );
  }
  return ptr;
}

static void ufree( void *ptr )
{
  if ( ptr )
    free( ptr );
}

void lp_pack( GSP * lp, serialize * s )
{
  int i, j;
  const char *prob_name = gsp_get_prob_name( lp );
  int prob_class = gsp_get_class( lp );
  int num_rows = gsp_get_num_rows( lp );
  int row_status[num_rows];
  double rhs[num_rows];
  int row_types[num_rows];
  int num_cols = gsp_get_num_cols( lp );
  int column_kinds[num_cols];
  double lb[num_cols], ub[num_cols];
  double obj_coef[num_cols];
  int column_status[num_cols];
  int sense;
  double const_term;
  int *mstart, *mrwind;
  double *dmatval;

  /* Pack the problem name */
  pack_string( s, prob_name );

  /* Pack the object class */
  pack_int( s, &prob_class, 1 );

  /* Pack the number of rows */
  pack_int( s, &num_rows, 1 );

  /* Pack the rows status */
  gsp_get_row_stat( lp, row_status, 0, num_rows - 1 );
  pack_int( s, row_status, num_rows );

  /* Pack the rhs of the rows */
  gsp_get_rhs( lp, rhs, 0, num_rows - 1 );
  pack_double( s, rhs, num_rows );

  /* Pack the row types */
  gsp_get_row_type( lp, row_types, 0, num_rows - 1 );
  pack_int( s, row_types, num_rows );

  /* Pack the row names */
  for ( i = 0; i < num_rows; i++ )
    pack_string( s, gsp_get_row_name( lp, i ) );

  /* Pack the number of columns */
  pack_int( s, &num_cols, 1 );

  /* Pack the column names */
  for ( j = 0; j < num_cols; j++ )
    pack_string( s, gsp_get_col_name( lp, j ) );

  /* Pack the columns kinds */
  if ( prob_class == GSP_MIP )
  {
    gsp_get_col_kind( lp, column_kinds, 0, num_cols - 1 );
    pack_int( s, column_kinds, num_cols );
  }

  /* Pack the columns bounds */
  gsp_get_lb( lp, lb, 0, num_cols - 1 );
  pack_double( s, lb, num_cols );
  gsp_get_ub( lp, ub, 0, num_cols - 1 );
  pack_double( s, ub, num_cols );

  /* Pack the objective value coefficients */
  gsp_get_obj_coef( lp, obj_coef, 0, num_cols - 1 );
  pack_double( s, obj_coef, num_cols );

  /* Pack the columns status */
  gsp_get_col_stat( lp, column_status, 0, num_cols - 1 );
  pack_int( s, column_status, num_cols );

  /* Pack the objective function name */
  pack_string( s, gsp_get_obj_name( lp ) );

  /* Pack the objective sense */
  sense = gsp_get_obj_dir( lp );
  pack_int( s, &sense, 1 );

  /* Pack the constant term */
  const_term = gsp_get_const_term( lp );
  pack_double( s, &const_term, 1 );

  /* Pack the constraint matrix */
  gsp_get_matrix( lp, 0, &mstart, &mrwind, &dmatval );
  pack_int( s, mstart, num_rows + 1 );
  pack_int( s, mrwind, mstart[num_rows] );
  pack_double( s, dmatval, mstart[num_rows] );
  free( mstart );
  free( mrwind );
  free( dmatval );
}

GSP *lp_unpack( deserialize * s )
{
  GSP *lp;
  int i, j;
  char *prob_name;
  int prob_class;
  int num_rows;
  int *row_status;
  double *rhs;
  int *row_types;
  char **row_names;
  int num_cols;
  char **column_names;
  int *column_kinds = NULL;
  double *lb, *ub;
  double *obj_coef;
  int *column_status;
  char *obj_name;
  int sense;
  double const_term;
  int *mstart, *mrwind;
  double *dmatval;

  /* Unpack the problem name */
  unpack_string( s, &prob_name );
  lp = gsp_create_prob( prob_name );

  /* Unpack the object class */
  unpack_int( s, &prob_class, 1 );
  gsp_set_class( lp, prob_class );

  /* Unpack the rows */
  unpack_int( s, &num_rows, 1 );

  /* Unpack the rows status */
  row_status = umalloc( num_rows * sizeof( int ) );
  unpack_int( s, row_status, num_rows );

  /* Unpack the rhs of the rows */
  rhs = umalloc( num_rows * sizeof( double ) );
  unpack_double( s, rhs, num_rows );

  /* UnPack the row types */
  row_types = umalloc( num_rows * sizeof( int ) );
  unpack_int( s, row_types, num_rows );

  /* Unpack the row names */
  row_names = umalloc( num_rows * sizeof( char * ) );
  for ( i = 0; i < num_rows; i++ )
    unpack_string( s, row_names + i );

  /* Unpack the columns */
  unpack_int( s, &num_cols, 1 );

  /* Unpack the column names */
  column_names = umalloc( num_cols * sizeof( char * ) );
  for ( j = 0; j < num_cols; j++ )
    unpack_string( s, column_names + j );

  /* Unpack the columns kinds */
  if ( prob_class == GSP_MIP )
  {
    column_kinds = umalloc( num_cols * sizeof( int ) );
    unpack_int( s, column_kinds, num_cols );
  }

  /* Unpack the columns bounds */
  lb = umalloc( num_cols * sizeof( double ) );
  ub = umalloc( num_cols * sizeof( double ) );
  unpack_double( s, lb, num_cols );
  unpack_double( s, ub, num_cols );

  /* Unpack the objective value coefficients */
  obj_coef = umalloc( num_cols * sizeof( double ) );
  unpack_double( s, obj_coef, num_cols );

  /* Unpack the columns status */
  column_status = umalloc( num_cols * sizeof( int ) );
  unpack_int( s, column_status, num_cols );

  /* Unpack the objective function name */
  unpack_string( s, &obj_name );

  /* Unpack the objective sense */
  unpack_int( s, &sense, 1 );

  /* Unpack the constant term */
  unpack_double( s, &const_term, 1 );

  /* Unpack the constraint matrix */
  mstart = umalloc( ( num_rows + 1 ) * sizeof( int ) );
  unpack_int( s, mstart, num_rows + 1 );
  mrwind = umalloc( mstart[num_rows] * sizeof( int ) );
  dmatval = umalloc( mstart[num_rows] * sizeof( double ) );
  unpack_int( s, mrwind, mstart[num_rows] );
  unpack_double( s, dmatval, mstart[num_rows] );

  /* Populate the LP */
  gsp_load_lp( lp, prob_name, obj_name, num_cols, num_rows, row_types, rhs,
               obj_coef, 0, mstart, mrwind, dmatval, lb, ub,
               ( const char ** ) row_names, ( const char ** ) column_names );
  if ( prob_class == GSP_MIP )
    gsp_set_col_kind( lp, column_kinds, 0, num_cols - 1 );
  gsp_set_const_term( lp, const_term );
  gsp_set_obj_dir( lp, sense );
  /* Set the row and column status */
  gsp_set_row_stat( lp, row_status, 0, num_rows - 1 );
  gsp_set_col_stat( lp, column_status, 0, num_cols - 1 );

  if ( gsp_warmup_basis( lp ) != GSP_PROB_OK )
  {
    printf( "Impossible to unpack the problem\n" );
    exit( 1 );
  }

  ufree( mstart );
  ufree( mrwind );
  ufree( dmatval );
  free( obj_name );
  ufree( column_status );
  ufree( obj_coef );
  ufree( ub );
  ufree( lb );
  if ( prob_class == GSP_MIP )
    ufree( column_kinds );
  for ( j = 0; j < num_cols; j++ )
    free( column_names[j] );
  for ( i = 0; i < num_rows; i++ )
    free( row_names[i] );
  ufree( row_names );
  ufree( row_types );
  ufree( rhs );
  ufree( row_status );
  free( prob_name );
  return lp;
}
