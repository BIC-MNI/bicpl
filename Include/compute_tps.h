/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_tps.h
@DESCRIPTION: Header file for get_nonlinear_warp
@GLOBALS    :
@CALLS      :
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : $Log: compute_tps.h,v $
@MODIFIED   : Revision 1.1  1994-11-04 13:11:34  david
@MODIFIED   : Initial revision
@MODIFIED   :
 * Revision 1.1  93/09/01  15:25:35  neelin
 * Initial revision
 * 
 * Revision 1.1  93/09/01  13:22:13  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

/* Function prototype */
public void  get_nonlinear_warp(float **bdefor, /* num_marks x dim */
                                float **adefor, /* num_marks x dim */
                                float **INVMLY, /* num_marks+1+dim x dim */
                                int num_marks,
                                int dim);


