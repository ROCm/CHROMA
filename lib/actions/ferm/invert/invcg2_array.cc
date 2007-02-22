// $Id: invcg2_array.cc,v 3.4 2007-02-22 21:11:46 bjoo Exp $
/*! \file
 *  \brief Conjugate-Gradient algorithm for a generic Linear Operator
 */

#include "chromabase.h"
#include "actions/ferm/invert/invcg2_array.h"

namespace Chroma 
{


  //! Conjugate-Gradient (CGNE) algorithm for a generic Linear Operator
  /*! \ingroup invert
   * This subroutine uses the Conjugate Gradient (CG) algorithm to find
   * the solution of the set of linear equations
   *
   *   	    Chi  =  A . Psi
   *
   * where       A = M^dag . M
   *
   * Algorithm:

   *  Psi[0]  :=  initial guess;    	       Linear interpolation (argument)
   *  r[0]    :=  Chi - M^dag . M . Psi[0] ;     Initial residual
   *  p[1]    :=  r[0] ;	       	       	       Initial direction
   *  IF |r[0]| <= RsdCG |Chi| THEN RETURN;      Converged?
   *  FOR k FROM 1 TO MaxCG DO    	       CG iterations
   *      a[k] := |r[k-1]|**2 / <Mp[k],Mp[k]> ;
   *      Psi[k] += a[k] p[k] ;   	       New solution vector
   *      r[k] -= a[k] M^dag . M . p[k] ;        New residual
   *      IF |r[k]| <= RsdCG |Chi| THEN RETURN;  Converged?
   *      b[k+1] := |r[k]|**2 / |r[k-1]|**2 ;
   *      p[k+1] := r[k] + b[k+1] p[k];          New direction
   *
   * Arguments:
   *
   *  \param M       Linear Operator    	       (Read)
   *  \param chi     Source	               (Read)
   *  \param psi     Solution    	    	       (Modify)
   *  \param RsdCG   CG residual accuracy        (Read)
   *  \param MaxCG   Maximum CG iterations       (Read)
   *  \return res    System solver results
   *
   * Local Variables:
   *
   *  p   	       Direction vector
   *  r   	       Residual vector
   *  cp  	       | r[k] |**2
   *  c   	       | r[k-1] |**2
   *  k   	       CG iteration counter
   *  a   	       a[k]
   *  b   	       b[k+1]
   *  d   	       < p[k], A.p[k] >
   *  Mp  	       Temporary for  M.p
   *
   * Subroutines:
   *                             +               
   *  A       Apply matrix M or M  to vector
   *
   * Operations:
   *
   *  2 A + 2 Nc Ns + N_Count ( 2 A + 10 Nc Ns )
   */

#undef PRINT_5D_RESID
  template<typename T, typename C>
  SystemSolverResults_t 
  InvCG2_a(const C& M,
	   const multi1d<T> & chi,
	   multi1d<T>& psi,
	   const Real& RsdCG, 
	   int MaxCG)
  {
    START_CODE();

    const int N = psi.size();
    const Subset& s = M.subset();

    SystemSolverResults_t res;

    // Move what we can to fast memory
    multi1d<T> mp(N);            moveToFastMemoryHint(mp);
    multi1d<T> mmp(N);           moveToFastMemoryHint(mmp);
    multi1d<T> p(N);             moveToFastMemoryHint(p);

    moveToFastMemoryHint(psi,true);

    multi1d<T> r(N);             moveToFastMemoryHint(r);
    multi1d<T> chi_internal(N);  moveToFastMemoryHint(chi_internal);

    for(int i=0; i < N; i++) {
      chi_internal[i][s] = chi[i];
    }

    QDPIO::cout << "InvCG2: starting" << endl;
    FlopCounter flopcount;
    flopcount.reset();
    StopWatch swatch;
    swatch.reset();
    swatch.start();

    Real chi_sq =  Real(norm2(chi_internal,s));  // 4*Nc*Ns flops per site
    flopcount.addSiteFlops(4*Nc*Ns*N,s);


    //  QDPIO::cout << "chi_norm = " << sqrt(chi_sq) << endl;
    Real rsd_sq = (RsdCG * RsdCG) * chi_sq;

    //                                            +
    //  r[0]  :=  Chi - A . Psi[0]    where  A = M  . M
    
    //                      +
    //  r  :=  [ Chi  -  M(u)  . M(u) . psi ]

    M(mp, psi, PLUS);
    M(mmp, mp, MINUS);
    flopcount.addFlops(2*M.nFlops());

    for(int n=0; n < N; ++n){
      r[n][s] = chi_internal[n] - mmp[n];
    }

    flopcount.addSiteFlops(2*Nc*Ns*N,s);

#ifdef PRINT_5D_RESID 
    for(int n=0; n < N; n++) {
      Double norm_r = norm2(r[n],s);
      if( toBool( norm_r > Double(1.0e-20)) ) {
	QDPIO::cout << "Iteration 0  r[" << n << "] = " << norm_r << endl;
      }
    }
#endif


    //  p[1]  :=  r[0]

    for(int n=0; n < N; ++n)
      p[n][s] = r[n];
  
    //  Cp = |r[0]|^2
    Double cp = norm2(r, s);   	       	   /* 4 Nc Ns  flops/cbsite */
    flopcount.addSiteFlops(4*Nc*Ns*N, s);

    //QDPIO::cout << "InvCG: k = 0  cp = " << cp << "  rsd_sq = " << rsd_sq << endl;

    //  IF |r[0]| <= RsdCG |Chi| THEN RETURN;
    if ( toBool(cp  <=  rsd_sq) )
    {
      res.n_count = 0;
      res.resid   = sqrt(cp);
      swatch.stop();
      QDPIO::cout << "InvCG: k = 0  cp = " << cp << "  rsd_sq = " << rsd_sq << endl;
      // Try it all at the end.
      flopcount.report("invcg2_array", swatch.getTimeInSeconds());
      revertFromFastMemoryHint(psi,true);
      END_CODE();
      return res;
    }

    //
    //  FOR k FROM 1 TO MaxCG DO
    //
    Real a, b;
    Double c, d;
  
    for(int k = 1; k <= MaxCG; ++k)
    {
      //  c  =  | r[k-1] |**2
      c = cp;

      //  a[k] := | r[k-1] |**2 / < p[k], Ap[k] > ;
      //      	       	       	       	       	  +
      //  First compute  d  =  < p, A.p >  =  < p, M . M . p >  =  < M.p, M.p >
      //  Mp = M(u) * p

      M(mp, p, PLUS);  flopcount.addFlops(M.nFlops());
   
      //  d = | mp | ** 2
      d = norm2(mp, s); flopcount.addSiteFlops(4*Nc*Ns*N,s);

      a = Real(c)/Real(d);

      //  Psi[k] += a[k] p[k]
      for(int n=0; n < N; ++n) {
	psi[n][s] += a * p[n];	/* 4 Nc Ns  cbsite flops */
      }
      flopcount.addSiteFlops(4*Nc*Ns*N,s);

      //  r[k] -= a[k] A . p[k] ;
      //      	       +            +
      //  r  =  r  -  M(u)  . Mp  =  M  . M . p  =  A . p

      M(mmp, mp, MINUS);
      flopcount.addFlops(M.nFlops());

      for(int n=0; n < N; ++n) {
	r[n][s] -= a * mmp[n];
      }
      flopcount.addSiteFlops(4*Nc*Ns*N, s);

#ifdef PRINT_5D_RESID
      for(int n=0; n < N; n++) {
	Double norm_r = norm2(r[n],s);
	if( toBool( norm_r > Double(1.0e-20)) )
	  QDPIO::cout << "Iteration " << k << " r[" << n << "] = " << norm_r << endl;
      }
#endif

      //  IF |r[k]| <= RsdCG |Chi| THEN RETURN;

      //  cp  =  | r[k] |**2
      cp = norm2(r, s);	                /* 2 Nc Ns  flops */
      flopcount.addSiteFlops(4*Nc*Ns*N,s);

      //    QDPIO::cout << "InvCG: k = " << k << "  cp = " << cp << endl;

      if ( toBool(cp  <=  rsd_sq) )
      {
	res.n_count = k;
	swatch.stop();
	QDPIO::cout << "InvCG: k = " << k << "  cp = " << cp << endl;
	flopcount.report("invcg2_array", swatch.getTimeInSeconds());
	revertFromFastMemoryHint(psi,true);

	// Compute the actual residual
	{
	  M(mp, psi, PLUS);
	  M(mmp, mp, MINUS);
	  Double actual_res(zero);
	  for(int n=0; n < N; ++n)
	  {
	    Double norm_r = norm2(chi[n] - mmp[n],s);
//	    QDPIO::cout<<"True residual "<<" r[" << n << "] = "<< sqrt(norm_r/chi_sq)<<endl;
	    actual_res += norm_r ;
	  }
	  
	  res.resid = sqrt(actual_res);
	  QDPIO::cout << "Actual residual r = " << sqrt(actual_res) << " Actual relative residual="<< sqrt(actual_res/chi_sq) << endl;
	}

	END_CODE();
	return res;
      }

      //  b[k+1] := |r[k]|**2 / |r[k-1]|**2
      b = Real(cp) / Real(c);
#if 1
      // QDPIO::cout << "InvCGev: k = " << k << "  alpha = " << a << "  beta = " << b << endl;
#endif 
      //  p[k+1] := r[k] + b[k+1] p[k]
      for(int n=0; n < N; ++n) {
	p[n][s] = r[n] + b*p[n];	/* Nc Ns  flops */
      }
      flopcount.addSiteFlops(4*Nc*Ns*N,s);
    }

    res.n_count = MaxCG;
    res.resid   = sqrt(cp);
    swatch.stop();
    QDPIO::cerr << "Nonconvergence Warning" << endl;
    flopcount.report("invcg2_array", swatch.getTimeInSeconds());
    revertFromFastMemoryHint(psi,true);

    END_CODE();
    return res;
  }


  // Fix here for now
  template<>
  SystemSolverResults_t 
  InvCG2(const LinearOperatorArray<LatticeFermion>& M,
	 const multi1d<LatticeFermion>& chi,
	 multi1d<LatticeFermion>& psi,
	 const Real& RsdCG, 
	 int MaxCG)
  {
    return InvCG2_a(M, chi, psi, RsdCG, MaxCG);
  }

}  // end namespace Chroma
