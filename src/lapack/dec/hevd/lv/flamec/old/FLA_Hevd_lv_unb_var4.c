
#include "FLAME.h"

FLA_Error FLA_Hevd_lv_unb_var4( FLA_Obj A, FLA_Obj l )
{
	FLA_Uplo     uplo = FLA_LOWER_TRIANGULAR;
	FLA_Datatype dt;
	FLA_Datatype dt_real;
	FLA_Obj      T, r, d, e, C, S, G;
	dim_t        mn_A;
	dim_t        k_max = 30;

	mn_A    = FLA_Obj_length( A );
	dt      = FLA_Obj_datatype( A );
	dt_real = FLA_Obj_datatype_proj_to_real( A );

	// If the matrix is a scalar, then the EVD is easy.
	if ( mn_A == 1 )
	{
		FLA_Copy( A, l );
		FLA_Obj_set_to_scalar( FLA_ONE, A );

		return FLA_SUCCESS;
	}

	// Create a matrix to hold block Householder transformations.
	FLA_Tridiag_UT_create_T( A, &T );

	// Create a vector to hold the realifying scalars.
	FLA_Obj_create( dt, mn_A, 1, 0, 0, &r );

	// Create vectors to hold the diagonal and sub-diagonal.
	FLA_Obj_create( dt_real, mn_A,       1, 0, 0, &d );
	FLA_Obj_create( dt_real, mn_A-1,     1, 0, 0, &e );
	FLA_Obj_create( dt_real, mn_A-1, k_max, 0, 0, &C );
	FLA_Obj_create( dt_real, mn_A-1, k_max, 0, 0, &S );
	FLA_Obj_create( dt_real, mn_A,    mn_A, 0, 0, &G );

	// Reduce the matrix to tridiagonal form.
	FLA_Tridiag_UT( uplo, A, T );

	// Apply scalars to rotate elements on the sub-diagonal to the real domain.
	FLA_Tridiag_UT_realify( uplo, A, r );

	// Extract the diagonal and sub-diagonal from A.
	FLA_Tridiag_UT_extract_diagonals( uplo, A, d, e );

	// Form Q, overwriting A.
	FLA_Tridiag_UT_form_Q( uplo, A, T );

	// Apply the scalars in r to Q.
	FLA_Apply_diag_matrix( FLA_RIGHT, FLA_CONJUGATE, r, A );

	// Initialize G to identity.
	FLA_Obj_set_to_identity( G );

	// Perform an eigenvalue decomposition on the tridiagonal matrix.
	FLA_Tevd_v_opt_var4( d, e, C, S, G, A );

	// Copy the converged eigenvalues to the output vector.
	FLA_Copy( d, l );

	// Sort the eigenvalues and eigenvectors in ascending order.
	FLA_Sort_evd( FLA_FORWARD, l, A );

	FLA_Obj_free( &T );
	FLA_Obj_free( &r );
	FLA_Obj_free( &d );
	FLA_Obj_free( &e );
	FLA_Obj_free( &C );
	FLA_Obj_free( &S );
	FLA_Obj_free( &G );

	return FLA_SUCCESS;
}
