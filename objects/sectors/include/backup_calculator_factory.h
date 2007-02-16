/*
 * This software, which is provided in confidence, was prepared by employees of
 * Pacific Northwest National Laboratory operated by Battelle Memorial Institute.
 * Battelle has certain unperfected rights in the software which should not be
 * copied or otherwise disseminated outside your organization without the
 * express written authorization from Battelle. All rights to the software are
 * reserved by Battelle. Battelle makes no warranty, express or implied, and
 * assumes no liability or responsibility for the use of this software.
 */

#ifndef _BACKUP_CALCULATOR__FACTORY_H_
#define _BACKUP_CALCULATOR__FACTORY_H_
#if defined(_MSC_VER)
#pragma once
#endif

/*! 
 * \file backup_calculator_factory.h
 * \ingroup Objects
 * \brief BackupCalculatorFactory header file.
 * \author Josh Lurz
 */

#include <string>
#include <memory>

class IBackupCalculator;

/*! 
 * \ingroup Objects
 * \brief A factory which is used to create the various types of backup
 *        calculators.
 * \details The factory encapsulates the creation of various types of backup
 *          calculators so that the IntermediateSubsector does not need to be
 *          aware of all types. This simplifies adding new types and also
 *          minimizes recompilation.
 * \author Josh Lurz
 */
class BackupCalculatorFactory { 
public:
    static bool isOfType( const std::string& aType );
    static std::auto_ptr<IBackupCalculator> create( const std::string& aType );
};

#endif // _BACKUP_CALCULATOR__FACTORY_H_
