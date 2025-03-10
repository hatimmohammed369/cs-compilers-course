#include "environment.hpp"

Environment::Environment() {
    // Globals
    scopes.push_back(new Table{});
}

EnvironmentResult Environment::define(const std::string& s) noexcept {
    if (resolved_names.contains(s)) {
        return EnvironmentResult::Error(
            std::format("Name `{}` already defined!", s)
        );
    }
    resolved_names[s] = scopes.size()-1;
    (*get_current_scope())[s] = nullptr;
    return EnvironmentResult::Ok(nullptr);
}

EnvironmentResult Environment::set(const std::string& s, Object* value) noexcept {
    auto name_depth_ptr =
        resolved_names.find(s);
    if (name_depth_ptr == resolved_names.end()) {
        return EnvironmentResult::Error(
            std::format("Name `{}` not defined!", s)
        );
    }
    Table* name_scope = scopes.at(name_depth_ptr->second);
    (*name_scope)[s] = value;
    return EnvironmentResult::Ok(nullptr);
}

EnvironmentResult Environment::get(const std::string& s) const noexcept {
    auto name_depth_ptr =
        resolved_names.find(s);
    if (name_depth_ptr == resolved_names.end()) {
        return EnvironmentResult::Error(
            std::format("Name `{}` not defined!", s)
        );
    }
    return EnvironmentResult::Ok(
        scopes.at(name_depth_ptr->second)->at(s)
    );
}
