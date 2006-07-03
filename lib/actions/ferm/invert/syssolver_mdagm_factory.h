// -*- C++ -*-
// $Id: syssolver_mdagm_factory.h,v 3.1 2006-07-03 15:26:09 edwards Exp $
/*! \file
 *  \brief Factory for producing system solvers for MdagM*psi = chi
 */

#ifndef __syssolver_mdagm_factory_h__
#define __syssolver_mdagm_factory_h__

#include "singleton.h"
#include "objfactory.h"
#include "linearop.h"
#include "actions/ferm/invert/syssolver_mdagm.h"

namespace Chroma
{

  //! MdagM system solver factory (foundry)
  /*! @ingroup invert */
  typedef SingletonHolder< 
    ObjectFactory<MdagMSystemSolver<LatticeFermion>, 
		  std::string,
		  TYPELIST_3(XMLReader&, const std::string&, Handle< LinearOperator<LatticeFermion> >),
		  MdagMSystemSolver<LatticeFermion>* (*)(XMLReader&,
							 const std::string&,
							 Handle< LinearOperator<LatticeFermion> >), 
		  StringFactoryError> >
  TheMdagMFermSystemSolverFactory;


  //! MdagM system solver factory (foundry)
  /*! @ingroup invert */
  typedef SingletonHolder< 
    ObjectFactory<MdagMSystemSolverArray<LatticeFermion>, 
		  std::string,
		  TYPELIST_3(XMLReader&, const std::string&, Handle< LinearOperatorArray<LatticeFermion> >),
		  MdagMSystemSolverArray<LatticeFermion>* (*)(XMLReader&,
							      const std::string&,
							      Handle< LinearOperatorArray<LatticeFermion> >), 
		  StringFactoryError> >
  TheMdagMFermSystemSolverArrayFactory;


  //! MdagM system solver factory (foundry)
  /*! @ingroup invert */
  typedef SingletonHolder< 
    ObjectFactory<MdagMSystemSolver<LatticeStaggeredFermion>, 
		  std::string,
		  TYPELIST_3(XMLReader&, const std::string&, Handle< LinearOperator<LatticeStaggeredFermion> >),
		  MdagMSystemSolver<LatticeStaggeredFermion>* (*)(XMLReader&,
								  const std::string&,
								  Handle< LinearOperator<LatticeStaggeredFermion> >), 
		  StringFactoryError> >
  TheMdagMStagFermSystemSolverFactory;

}


#endif
