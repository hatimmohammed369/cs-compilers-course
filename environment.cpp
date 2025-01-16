#include "environment.hpp"

Environment::Environment() {
    // Globals
    scopes.push_back(Table{});
}

void Environment::define(const std::string& s) noexcept {
    if (resolved_names.contains(s)) {
        std::cerr << std::format("Name `{}` already defined!\n", s) ;
        exit(1);
    }
    resolved_names[s] = scopes.size()-1;
    get_current_scope()[s] = nullptr;
}

void Environment::set(const std::string& s, Object* value) noexcept {
    auto name_depth_ptr =
        resolved_names.find(s);
    if (name_depth_ptr == resolved_names.end()) {
        std::cerr << std::format("Name `{}` not defined!\n", s) ;
        exit(1);
    }
    scopes.at(name_depth_ptr->second).insert({s, value});
}

Object* Environment::get(const std::string& s) const noexcept {
    auto name_depth_ptr =
        resolved_names.find(s);
    if (name_depth_ptr == resolved_names.end()) {
        std::cerr << std::format("Name `{}` not defined!\n", s) ;
        exit(1);
    }
    return scopes.at(name_depth_ptr->second).at(s);
}
