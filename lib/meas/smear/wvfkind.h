// -*- C++ -*-
// $Id: wvfkind.h,v 3.1 2006-08-11 16:13:30 edwards Exp $
/*! \file
 *  \brief Wave-function types for smearing
 */

#ifndef __wvftype_h__
#define __wvftype_h__

namespace Chroma 
{ 
  //! Wave-function types for smearing
  /*! \ingroup smear */
  enum WvfKind {
    WVF_KIND_GAUSSIAN,
    WVF_KIND_EXPONENTIAL,
    WVF_KIND_GAUGE_INV_GAUSSIAN,
    WVF_KIND_WUPPERTAL,
    WVF_KIND_JACOBI
  };

}

#endif
