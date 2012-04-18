#ifndef FUNCTOR_HH_
#define FUNCTOR_HH_
#if defined(_MSC_VER)
#pragma once
#endif


/*
* LEGAL NOTICE
* This computer software was prepared by Battelle Memorial Institute,
* hereinafter the Contractor, under Contract No. DE-AC05-76RL0 1830
* with the Department of Energy (DOE). NEITHER THE GOVERNMENT NOR THE
* CONTRACTOR MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
* LIABILITY FOR THE USE OF THIS SOFTWARE. This notice including this
* sentence must appear on any copies of this computer software.
* 
* EXPORT CONTROL
* User agrees that the Software will not be shipped, transferred or
* exported into any country or used in any manner prohibited by the
* United States Export Administration Act or any other applicable
* export laws, restrictions or regulations (collectively the "Export Laws").
* Export of the Software may require some form of license or other
* authority from the U.S. Government, and failure to obtain such
* export control license may result in criminal liability under
* U.S. laws. In addition, if the Software is identified as export controlled
* items under the Export Laws, User represents and warrants that User
* is not a citizen, or otherwise located within, an embargoed nation
* (including without limitation Iran, Syria, Sudan, Cuba, and North Korea)
*     and that User is not otherwise prohibited
* under the Export Laws from receiving the Software.
* 
* Copyright 2011 Battelle Memorial Institute.  All Rights Reserved.
* Distributed as open-source under the terms of the Educational Community 
* License version 2.0 (ECL 2.0). http://www.opensource.org/licenses/ecl2.php
* 
* For further details, see: http://www.globalchange.umd.edu/models/gcam/
*
*/


/*!
 * @file functor.hh
 * @ingroup Solution
 * @brief Base class definitions for function-like objects
 */

#include <boost/numeric/ublas/vector.hpp> 

#define UBVECTOR boost::numeric::ublas::vector

/*!
 * @class VecFVec
 * @brief Base class template for vector function of a vector argument 
 * @author Robert Link
 * @tparam Tr: return type -- generally a floating point type
 * @tparam Ta: argument type -- generally a floating point type
 */ 
template <class Tr, class Ta>
class VecFVec {
protected:
  /*!
   * @var na: length of the argument vector
   * @var nr: length of the return vector
   * @remark Subclasses of VecFVec are responsible for setting these values
   * appropriately on initialization.
   */
  int na,nr;
public:
  /*!
   * Paren operator -- evaluates the function F(x)
   * @param[in] arg: argument vector - caller is responsible for
   *            sizing it correctly (you'll need to do that to supply reasonable
   *            input values anyhow).
   * @param[out] rval: return value vector. Will be resized if necessary.
   * @remark Not declared as const because a functor may maintain some internal
   *         state that is modified by a normal call.
   */
  virtual void operator()(const UBVECTOR<Ta> &arg, UBVECTOR<Tr> &rval) = 0;
  /*!
   * Returns the length of the argument vector required by the function
   */
  int narg() const {return na;}
  /*!
   * Returns the length of the return vector produced by the function
   */
  int nrtn() const {return nr;}
  
  /*!
   * Indicates that the next call will be for a partial derivative evaulation.
   *
   * When calculating a partial derivative, only one element of the
   * input vector changes.  Knowing this might allow a function to
   * take some shortcuts in evaluation.  The default implementation
   * ignores this hint, but subclasses are free to use the information
   * in any way that increases their efficiency in a subroutine like
   * fdjac.
   *
   * \param ip: The index of the element of the input vector that has changed.
   */
  virtual void partial(int ip) {}
};



/*!
 * @class SclFVec
 * @brief Base class template for scalar function of a vector argument 
 * @author Robert Link
 * @tparam Tr: return type -- generally a floating point type
 * @tparam Ta: argument type -- generally a floating point type
 */ 
template <class Tr, class Ta>
class SclFVec {
protected:
  /*!
   * @var na: length of the argument vector
   * @remark Subclasses of SclFVec are responsible for setting this value
   *         appropriately on initialization.
   */
  int na;

public: 
  /*!
   * Paren operator -- calls the function
   * @param[in] arg: argument vector - caller is responsible for
   *            sizing it correctly (you'll need to do that to supply reasonable
   *            input values anyhow).
   * @return Scalar function output.
   * @remark Not declared as const because a functor may maintain some internal
   *         state that is modified by a normal call.
   */
  virtual Tr operator()(const UBVECTOR<Ta> &arg) = 0;
  /*!
   * Returns the length of the argument vector required by the function
   */
  int narg() const {return na;}
};


/*!
 * @brief Base class template for scalar function of a scalar argument 
 * @author Robert Link
 * @tparam Tr: return type -- generally a floating point type
 * @tparam Ta: argument type -- generally a floating point type
 */ 
template <class Tr, class Ta>
class SclFScl {
protected:

public:
  /*!
   * Paren operator -- calls the function
   * @param[in] arg: argument to the function
   * @return Scalar function output.
   * @remark Not declared as const because a functor may maintain some internal
   *         state that is modified by a normal call.
   */
  virtual Tr operator()(Ta &arg) = 0;
};

#undef UBVECTOR

#endif
