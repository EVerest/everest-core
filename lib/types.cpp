#include <utils/types.hpp>

TypedHandler::TypedHandler(const std::string& name_, const std::string& id_, HandlerType type_,
                           std::shared_ptr<Handler> handler_) :
    name(name_), id(id_), type(type_), handler(handler_) {
}

TypedHandler::TypedHandler(const std::string& name_, HandlerType type_, std::shared_ptr<Handler> handler_) :
    TypedHandler(name_, "", type_, handler_) {
}

TypedHandler::TypedHandler(HandlerType type_, std::shared_ptr<Handler> handler_) :
    TypedHandler("", "", type_, handler_) {
}

Requirement::Requirement(const std::string& requirement_id_, size_t index_) : id(requirement_id_), index(index_) {
}

bool Requirement::operator<(const Requirement& rhs) const {
    if (id < rhs.id) {
        return true;
    } else if (id == rhs.id) {
        return (index < rhs.index);
    } else {
        return false;
    }
}

ImplementationIdentifier::ImplementationIdentifier(const std::string& module_id_,
                                                   const std::string& implementation_id_) :
    module_id(module_id_), implementation_id(implementation_id_) {
}

std::string ImplementationIdentifier::to_string() const {
    return this->module_id + "->" + this->implementation_id;
}

bool operator==(const ImplementationIdentifier& lhs, const ImplementationIdentifier& rhs) {
    return lhs.module_id == rhs.module_id && lhs.implementation_id == rhs.implementation_id;
}

bool operator!=(const ImplementationIdentifier& lhs, const ImplementationIdentifier& rhs) {
    return !(lhs == rhs);
}
