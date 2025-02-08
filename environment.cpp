#include "environment.hpp"

Environment::Environment() {
    // Globals
    scopes.push_back(Table{});
}

EnvironmentResult Environment::define(const std::string& s) noexcept {
    if (resolved_names.contains(s)) {
        return EnvironmentResult::Error(
            std::format("Name `{}` already defined!\n", s)
        );
    }
    resolved_names[s] = scopes.size()-1;
    get_current_scope()[s] = nullptr;
    return EnvironmentResult::Ok(nullptr);
}

EnvironmentResult Environment::set(const std::string& s, Object* value) noexcept {
    auto name_depth_ptr =
        resolved_names.find(s);
    if (name_depth_ptr == resolved_names.end()) {
        return EnvironmentResult::Error(
            std::format("Name `{}` not defined!\n", s)
        );
    }
    scopes.at(name_depth_ptr->second).insert({s, value});
    return EnvironmentResult::Ok(nullptr);
}

EnvironmentResult Environment::get(const std::string& s) const noexcept {
    auto name_depth_ptr =
        resolved_names.find(s);
    if (name_depth_ptr == resolved_names.end()) {
        return EnvironmentResult::Error(
            std::format("Name `{}` not defined!\n", s)
        );
    }
    return EnvironmentResult::Ok(
        scopes.at(name_depth_ptr->second).at(s)
    );
}
