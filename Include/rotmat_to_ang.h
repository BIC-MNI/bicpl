#ifndef  _ROTMAT_TO_ANG_H
#define  _ROTMAT_TO_ANG_H

/* ----------------------------- MNI Header -----------------------------------
@NAME       : romat_to_ang.h
@DESCRIPTION: Header file for romat_to_ang.c
@GLOBALS    :
@CALLS      :
@CREATED    : Sept 2, 93 Louis
@MODIFIED   : $Log: rotmat_to_ang.h,v $
@MODIFIED   : Revision 1.1  1994-11-04 13:11:44  david
@MODIFIED   : Initial revision
@MODIFIED   :
 * Revision 1.1  93/09/08  15:48:03  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */
public BOOLEAN rotmat_to_ang(float **rot, float *ang);

public void nr_identd(double **A, int m1, int m2, int n1, int n2 );
public void nr_identf(float **A, int m1, int m2, int n1, int n2 );

public void nr_copyd(double **A, int m1, int m2, int n1, int n2, double **B );
public void nr_copyf(float  **A, int m1, int m2, int n1, int n2, float **B );

public void nr_rotxd(double **M, double a);
public void nr_rotxf(float **M, float a);

public void nr_rotyd(double **M,double a);
public void nr_rotyf(float **M, float a);

public void nr_rotzd(double **M,double a);
public void nr_rotzf(float **M, float a);

public void nr_multd(double **A, int mA1, int mA2, int nA1, int nA2,
		     double **B, int mB1, int mB2, int nB1, int nB2, 
		     double **C);
public void nr_multf(float **A, int mA1, int mA2, int nA1, int nA2,
		     float **B, int mB1, int mB2, int nB1, int nB2, 
		     float **C);

#endif
