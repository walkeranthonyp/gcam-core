#ifndef _GET_DISTRIBUTED_INVESTMENT_VISITOR_H_
#define _GET_DISTRIBUTED_INVESTMENT_VISITOR_H_
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
* \file get_distributed_investment_visitor.h
* \ingroup Objects
* \brief GetDistributedInvestmentVisitor class header file.
* \author Sonny Kim
*/

#include "util/base/include/default_visitor.h"
#include <vector>
#include <string>

/*! 
* \ingroup Objects
* \brief A visitor which visits all subsectors and technologies for each sector 
*          to determine how much investment was recieved and set it into the
*          calibration market for that investable.
* \details This visitor when passed to a Region will tabulate individually the
*          amount of investment recieved including fixed using the 
*          getAnnualInvestment method.  It will then set that as the supply in the
*          calibration market using the naming convention "SubSec-"+Subsector::getName()
*          or "Tech-"+BaseTechnology::getName().
* \warning When subsectors or technologies have the same name accross sectors/subsectors
*          this calibration naming scheme breaks.  Using the full path in the naming scheme
*          is a somewhat better solution.
* \see     InvestableCounterVisitor
* \see     SetShareWeightVisitor
* \see     MarketBasedInvestor
*
* \author Sonny Kim
*/
class GetDistributedInvestmentVisitor : public DefaultVisitor {
public:

    GetDistributedInvestmentVisitor( const std::string& aRegionName );

    virtual void startVisitSector( const Sector* aSector,
                                   const int aPeriod );

    virtual void endVisitSector( const Sector* aSector,
                                 const int aPeriod );

    virtual void startVisitSubsector( const Subsector* aSubsector,
                                      const int aPeriod );

    virtual void endVisitSubsector( const Subsector* aSubsector,
                                    const int aPeriod );

    virtual void startVisitBaseTechnology( const BaseTechnology* aTechnology,
                                       const int aPeriod );

    virtual void endVisitBaseTechnology( const BaseTechnology* aTechnology,
                                     const int aPeriod );
    
private:
    //! Name of the Region the calibration and fixed outputs are currently being
    //! summed for.
    std::string mCurrentRegionName;

    //! Name of the sector currently being visited.
    std::string mCurrentSectorName;

    //! Name of the subsector currently being visited.
    std::string mCurrentSubSectorName;

};

#endif // _GET_DISTRIBUTED_INVESTMENT_VISITOR_H_
