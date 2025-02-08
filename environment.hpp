#ifndef ENVIRONMENT_H_INCLUDED
#define ENVIRONMENT_H_INCLUDED

#include <unordered_map>
#include "object.hpp"
#include "result.hpp"

using EnvironmentResult =
    PointerValueResult<Object*/*value type*/, std::string/*error type*/>;

class Environment {
public:
    using depth = std::uint64_t;
    using Table = std::unordered_map<std::string, Object*>;
    using Names = std::unordered_map<std::string, depth>;

private:
    std::vector<Table*> scopes{};
    Names resolved_names{};

public:
    Environment();

    inline Table* get_current_scope() const noexcept { return scopes.back(); }
    inline Table* globals() const noexcept { return scopes[0]; }
    inline void begin_scope() noexcept { scopes.push_back(new Table{}); }
    inline void end_scope() noexcept {
        for (const auto& [key, _] : *scopes.back())
            resolved_names.erase(key);
        scopes.pop_back();
    }
    EnvironmentResult define(const std::string& s) noexcept;
    EnvironmentResult set(const std::string& s, Object* value) noexcept;
    EnvironmentResult get(const std::string& s) const noexcept;
};

#endif
