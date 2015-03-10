/**
 * @file VariablePool.h 
 * @author Sebastian Junges
 */

#pragma once

#include <array>
#include <memory>
#include <mutex>
#include <map>
#include <string>
#include "Variable.h"
#include "../util/Singleton.h"


namespace carl 
{

/**
 * This class generates new variables and stores human-readable names for them.
 *
 * As we want only a single unique VariablePool and need global access to it, it is implemented as a singleton.
 *
 * All methods that modify the pool, that are getInstance(), getFreshVariable() and setName(), are thread-safe.
 */
class VariablePool : public Singleton<VariablePool>
{
friend Singleton<VariablePool>;
private:
	/**
	 * Contains the id of the next variable to be created for each type.
	 * As such, is also a counter of the variables that exist.
	 */
	std::array<std::size_t, (std::size_t)VariableType::TYPE_SIZE> mNextIDs;

	/**
	 * Mutex for calling getFreshVariable().
	 */
	std::mutex freshVarMutex;

	/**
	 * Mutex for calling setVariableName().
	 */
	std::mutex setNameMutex;

	std::size_t& nextID(const VariableType& vt) {
		assert((std::size_t)vt < mNextIDs.size());
		return mNextIDs[(std::size_t)vt];
	}
	const std::size_t& nextID(const VariableType& vt) const {
		assert((std::size_t)vt < mNextIDs.size());
		return mNextIDs[(std::size_t)vt];
	}

	/**
	 * Stores human-readable names for variables that can be set via setVariableName().
	 */
	std::map<Variable, std::string> mVariableNames;

	/**
	 * Stores a prefix for printing variables that have no human-readable name.
	 */
	std::string mVariablePrefix;

protected:
	/**
	 * Private default constructor.
	 */
	VariablePool();

public:

	/**
	 * Clears everything already created in this pool.
	 */
	void clear()
    {
        mVariableNames.clear();
		mNextIDs.fill(1);
    }

	/**
	 * Get a variable which was not used before.
	 * This method is thread-safe.
	 * @param type Type for the new variable.
	 * @return A new variable.
	 */
	Variable getFreshVariable(VariableType type = VariableType::VT_REAL);

	/**
	 * Get a variable with was not used before and set a name for it.
	 * This method is thread-safe.
	 * @param name Name for the new variable.
	 * @param type Type for the new variable.
	 * @return A new variable.
	 */
	Variable getFreshVariable(const std::string& name, VariableType type = VariableType::VT_REAL);

	/**
	 * Searches in the friendly names list for a variable with the given name.
	 * @param name The friendly variable name to look for.
	 * @return The first variable with that friendly name.
	 */
	Variable findVariableWithName(const std::string& name) const;
	/**
	 * Get a human-readable name for the given variable.
	 * If the given Variable is Variable::NO_VARIABLE, "NO_VARIABLE" is returned.
	 * If friendlyVarName is true, the name that was set via setVariableName() for this Variable, if there is any, is returned.
	 * Otherwise "x_<id>" is returned, id being the internal id of the Variable.
	 * @param v Variable.
	 * @param variableName Flag, if a name set via setVariableName shall be considered.
	 * @return Some name for the Variable.
	 */
	const std::string getName(Variable::Arg v, bool variableName = true) const;
	/**
	 * Add a name for a given Variable.
	 * This method is thread-safe.
	 * @param v Variable.
	 * @param name Some string naming the variable.
	 */
	void setName(Variable::Arg v, const std::string& name);

	/**
	 * Sets the prefix used when printing anonymous variables.
	 * The default is "_", hence they look like "_x_5".
     * @param prefix Prefix for anonymous variable names.
     */
	void setPrefix(const std::string& prefix = "_") {
		this->mVariablePrefix = prefix;
	}
   
	/**
	 * Returns the number of variables initialized by the pool.
	 * @return Number of variables.
	 */
	std::size_t nrVariables(VariableType type = VariableType::VT_REAL) const {
		return nextID(type) - 1;
	}
};

inline Variable freshVariable(const VariableType& vt) {
	return VariablePool::getInstance().getFreshVariable(vt);
}
inline Variable freshVariable(const std::string& name, const VariableType& vt) {
	return VariablePool::getInstance().getFreshVariable(name, vt);
}

inline Variable freshBooleanVariable() {
	return freshVariable(VariableType::VT_BOOL);
}
inline Variable freshBooleanVariable(const std::string& name) {
	return freshVariable(name, VariableType::VT_BOOL);
}
inline Variable freshRealVariable() {
	return freshVariable(VariableType::VT_REAL);
}
inline Variable freshRealVariable(const std::string& name) {
	return freshVariable(name, VariableType::VT_REAL);
}
inline Variable freshIntegerVariable() {
	return freshVariable(VariableType::VT_INT);
}
inline Variable freshIntegerVariable(const std::string& name) {
	return freshVariable(name, VariableType::VT_INT);
}
inline Variable freshUninterpretedVariable() {
	return freshVariable(VariableType::VT_UNINTERPRETED);
}
inline Variable freshUninterpretedVariable(const std::string& name) {
	return freshVariable(name, VariableType::VT_UNINTERPRETED);
}

}
