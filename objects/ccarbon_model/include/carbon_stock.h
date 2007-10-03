#ifndef _CARBON_STOCK_H_
#define _CARBON_STOCK_H_
#if defined(_MSC_VER)
#pragma once
#endif

/*
 * This software, which is provided in confidence, was prepared by employees of
 * Pacific Northwest National Laboratory operated by Battelle Memorial Institute.
 * Battelle has certain unperfected rights in the software which should not be
 * copied or otherwise disseminated outside your organization without the
 * express written authorization from Battelle. All rights to the software are
 * reserved by Battelle. Battelle makes no warranty, express or implied, and
 * assumes no liability or responsibility for the use of this software.
 */

/*! 
 * \file carbon_stock.h
 * \ingroup Objects
 * \brief CarbonStock class header file.
 * \author Jim Naslund and Ming Chang
 */

#include "ccarbon_model/include/acarbon_stock.h"
#include "util/base/include/time_vector.h"

/*!
 * \brief An implementation of the ICarbonStock Interface.
 * \details A basic implementation of the ICarbonStock Interface that supports
 *          all interface functions.  This class is essentially a wrapper around
 *          a double representing a carbon stock.
 */
class CarbonStock : public ACarbonStock {

public:
    CarbonStock();
	CarbonStock( const CarbonStock& aCarbonStock );
	~CarbonStock();

	virtual CarbonStock* clone() const;

    // IParsable Interface
    virtual bool XMLParse( const xercesc::DOMNode* aNode );
    static const std::string& getXMLNameStatic();

    virtual double transfer( const EnvironmentalInfo* aEnvInfo,
                             FlowType aFlowType, const int aYear );
    virtual void addToStock( double aValueToAdd, const int aYear );
	virtual void toInputXML( std::ostream& aOut, Tabs* aTabs ) const;
	virtual void toDebugXML( const int aPeriod, std::ostream& aOut,
                             Tabs* aTabs ) const;
	virtual void deductFromCurrentCarbonStock( const double aDeductValue);
	virtual double getCurrentStockValue();
	virtual double getTurnoverTimescale();
private:
    //! The number of years it takes for the store to lose its stock.
    double mTurnoverTimescale;

};

#endif // _CARBON_STOCK_H_
