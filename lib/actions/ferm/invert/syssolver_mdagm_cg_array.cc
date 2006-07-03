// $Id: syssolver_mdagm_cg_array.cc,v 3.1 2006-07-03 15:26:09 edwards Exp $
/*! \file
 *  \brief Solve a MdagM*psi=chi linear system by CG2
 */

#include "actions/ferm/invert/syssolver_mdagm_factory.h"
#include "actions/ferm/invert/syssolver_mdagm_aggregate.h"

#include "actions/ferm/invert/syssolver_mdagm_cg_array.h"

namespace Chroma
{

  //! CG2 system solver namespace
  namespace MdagMSysSolverCGArrayEnv
  {
    //! Callback function
    MdagMSystemSolverArray<LatticeFermion>* createFerm(XMLReader& xml_in,
						       const std::string& path,
						       Handle< LinearOperatorArray<LatticeFermion> > A)
    {
      return new MdagMSysSolverCGArray<LatticeFermion>(A, SysSolverCGParams(xml_in, path));
    }

    //! Name to be used
    const std::string name("CG_INVERTER");

    //! Register all the factories
    bool registerAll()
    {
      bool foo = true;
      foo &= Chroma::TheMdagMFermSystemSolverArrayFactory::Instance().registerObject(name, createFerm);
      return foo;
    }

    //! Register the source construction
    const bool registered = registerAll();
  }
}
