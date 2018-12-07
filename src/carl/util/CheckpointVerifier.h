#pragma once

#include "../core/logging.h"
#include "../core/VariablePool.h"
#include "../formula/Formula.h"
#include "debug.h"

#include <any>

#include <queue>
#include <tuple>
#include <typeinfo>
#include <type_traits>
#include <utility>

namespace carl {
namespace checkpointverifier {
	class CheckpointVector {
		template<typename... Args>
		using Tuple = std::tuple<std::decay_t<Args>...>;
		using Checkpoint = std::tuple<std::string,bool,std::any>;
	private:
		std::vector<Checkpoint> mData;
		std::size_t mCurrent = 0;
		
		const Checkpoint& cur() const {
			assert(valid());
			return mData[mCurrent];
		}
	public:
		CheckpointVector() {
			VariablePool::getInstance();
		}
		const std::string& description() const {
			return std::get<0>(cur());
		}
		bool forced() const {
			return std::get<1>(cur());
		}
		template<typename T>
		const T& data() const {
			return std::any_cast<T>(std::get<2>(cur()));
		}
		template<typename T>
		const T* try_data() const {
			return std::any_cast<T>(&std::get<2>(cur()));
		}
		bool valid() const {
			return mData.size() > mCurrent;
		}
		void next() {
			mCurrent++;
		}
		
		template<typename... Args>
		void add(const std::string& description, bool forced, Args&&... args) {
			mData.emplace_back(description, forced, std::any(Tuple<Args...>(args...)));
		}
		void clear() {
			mData.clear();
			mCurrent = 0;
		}
		
		bool mayExceed = true;
		bool printDebug = true;
	};
}

	class CheckpointVerifier {
		struct ChannelWrapper: std::map<std::string, checkpointverifier::CheckpointVector> {
			ChannelWrapper() {
				carl::VariablePool::getInstance();
				carl::ConstraintPool<carl::MultivariatePolynomial<mpq_class>>::getInstance();
				carl::FormulaPool<carl::MultivariatePolynomial<mpq_class>>::getInstance();
			}
		};
		template<typename... Args>
		using Tuple = checkpointverifier::CheckpointVector::Tuple<Args...>;
		using Channels = ChannelWrapper;
	private:
		static Channels mChannels;
	public:
		template<typename... Args>
		static void push(const std::string& channel, const std::string& description, bool forced, Args&&... args) {
			mChannels[channel].add(description, forced, std::forward<Args>(args)...);
			CARL_LOG_DEBUG("carl.checkpoint", "Added " << (forced ? "forced " : "") << description << ": " << Tuple<Args...>(args...) << " in " << channel);
			CARL_LOG_DEBUG("carl.checkpoint", "Type: " << carl::demangle(typeid(Tuple<Args...>).name()));
		}
		template<typename... Args>
		static bool check(const std::string& channel, const std::string& description, Args&&... args) {
			
			if (!mChannels[channel].valid()) {
				CARL_LOG_WARN("carl.checkpoint", "No Checkpoints left in " << channel << ", got " << Tuple<Args...>(args...));
				return mChannels[channel].mayExceed;
			}
			if (mChannels[channel].description() != description) {
				CARL_LOG_ERROR("carl.checkpoint", "Expected " << mChannels[channel].description() << " but got " << description << " in " << channel);
				return !mChannels[channel].forced();
			}
			const Tuple<Args...>* checkpoint = mChannels[channel].try_data<Tuple<Args...>>();
			if (checkpoint == nullptr) {
				CARL_LOG_ERROR("carl.checkpoint", "Got " << description << " with offending data type: " << Tuple<Args...>(args...));
				CARL_LOG_ERROR("carl.checkpoint", "Offending type: " << carl::demangle(typeid(Tuple<Args...>).name()));
				return !mChannels[channel].forced();
			}
			bool res = *checkpoint == Tuple<Args...>(args...);
			if (!res) {
				CARL_LOG_ERROR("carl.checkpoint", "Expected " << *checkpoint << " -> " << std::boolalpha << res);
				CARL_LOG_ERROR("carl.checkpoint", "Expectation failed, got " << Tuple<Args...>(args...));
			} else if (mChannels[channel].printDebug) {
				CARL_LOG_ERROR("carl.checkpoint", "Expected " << *checkpoint << " -> " << std::boolalpha << res);
			}
			res = !mChannels[channel].forced() || res;
			mChannels[channel].next();
			return res;
		}
		template<typename... Args>
		static void expect(const std::string& channel, const std::string& description, Args&&... args) {
			if (mChannels[channel].printDebug) {
				CARL_LOG_ERROR("carl.checkpoint", "*****");
				CARL_LOG_ERROR("carl.checkpoint", "***** Checkpoint for " << channel);
				CARL_LOG_ERROR("carl.checkpoint", "***** Data provided: " << description << " " << Tuple<Args...>(args...));
				CARL_LOG_ERROR("carl.checkpoint", "*****");
			}
			auto res = check(channel, description, args...);
			if (mChannels[channel].printDebug) {
				CARL_LOG_ERROR("carl.checkpoint", "*****");
				CARL_LOG_ERROR("carl.checkpoint", "*****");
				CARL_LOG_ERROR("carl.checkpoint", "*****");
			}
			assert(res);
		}
		static void clear(const std::string& channel) {
			CARL_LOG_DEBUG("carl.checkpoint", "Clearing " << channel);
			mChannels[channel].clear();
		}
		static bool& mayExceed(const std::string& channel) {
			return mChannels[channel].mayExceed;
		}
		static bool& printDebug(const std::string& channel) {
			return mChannels[channel].printDebug;
		}
	};
}

#if false
#define CARL_ADD_CHECKPOINT(channel,description,forced,...) carl::CheckpointVerifier::push(channel, description, forced, __VA_ARGS__);
#define CARL_CHECKPOINT(channel,description,...) carl::CheckpointVerifier::expect(channel, description, __VA_ARGS__);
#define CARL_CLEAR_CHECKPOINT(channel) carl::CheckpointVerifier::clear(channel);
#else
#define CARL_ADD_CHECKPOINT(channel,description,forced,...)
#define CARL_CHECKPOINT(channel,description,...)
#define CARL_CLEAR_CHECKPOINT(channel)
#endif
