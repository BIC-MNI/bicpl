#ifndef  _COMPUTE_XFM_H
#define  _COMPUTE_XFM_H

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_xfm.h
@DESCRIPTION: Header file for compute_transform_from_tags
@GLOBALS    :
@CALLS      :
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : $Log: compute_xfm.h,v $
@MODIFIED   : Revision 1.2  1995-07-12 18:07:27  david
@MODIFIED   : *** empty log message ***
@MODIFIED   :
 * Revision 1.1  94/11/04  14:09:05  david
 * Initial revision
 * 
 * Revision 1.1  94/11/04  13:11:36  david
 * Initial revision
 * 
 * Revision 1.2  93/09/08  15:47:44  neelin
 * Added 9 and 10 parameter linear fitting.
 * 
 * Revision 1.1  93/09/01  15:25:37  neelin
 * Initial revision
 * 
 * Revision 1.1  93/09/01  13:22:24  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

/* Transformation types */
typedef enum {
   TRANS_LSQ6, TRANS_LSQ7, TRANS_LSQ9, TRANS_LSQ10, TRANS_LSQ12, TRANS_TPS
} Trans_type;

/* Minimum number of points needed */
#define MIN_POINTS_LINEAR 4
#define MIN_POINTS_TPS    5

#endif
