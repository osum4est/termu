//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_MISSINGMAPPEREXCEPTION_H
#define TERMU_MISSINGMAPPEREXCEPTION_H

#include <stdexcept>

class missing_mapper_exception : public std::runtime_error {
public:
    missing_mapper_exception() : std::runtime_error("Could not find correct mapper!") {};
};

#endif //TERMU_MISSINGMAPPEREXCEPTION_H
