#pragma once

#include  <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <carl-arith/core/Variable.h>
#include <carl-arith/core/Common.h>
#include <carl-common/meta/SFINAE.h>
#include <carl-common/memory/Singleton.h>

#include <map>

#include <poly/polyxx.h>

namespace carl {

class VariableMapper : public Singleton<VariableMapper> {
	//mapping from carl variables to libpoly variables
	std::map<carl::Variable, poly::Variable> mVarsCarlLibpoly;
	//mapping from libpoly variables to carl variables
	std::map<poly::Variable, carl::Variable> mVarsLibolyCarl;

public:
	VariableMapper();

	/**
	 * Convert a carl variable to a libpoly variable
	 * If the variable has not been converted before, a new libpoly variable with the same name is created
	 * @return The corresponding libpoly variable
	 */
	poly::Variable getLibpolyVariable(const carl::Variable& var);

	/**
	 * Convert a Libpoly Variable to the corresponding Carl Variable
	 * Asserts that the libpoly variable has been converted from a carl variable before
	 * @return The corresponding carl variable
	 */
	carl::Variable getCarlVariable(const poly::Variable& var);
};

} // namespace carl

#endif