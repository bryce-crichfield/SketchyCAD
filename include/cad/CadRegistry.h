#pragma once

#include <cad/CadObject.h>

namespace Cad {
class Registry;

class RegistryEntry {
    friend class Registry;
    // The "underlying" object, an entry will only be considered valid if 
    // this is not null
    std::unique_ptr<Object> object = nullptr;

    // Only the registry can invalidate an entry
    void Invalidate();

    // The user cannot access the underlying object directly, they must use
    // the registry and visit it with a visitor
    Object& GetObject();

  public:
    RegistryEntry();
    RegistryEntry(std::unique_ptr<Object> object);
    bool NotValid() const;
};

using RegistryReference = std::shared_ptr<RegistryEntry>;

class Registry {
    std::list<RegistryReference> references;

  public:
    Registry() = default;
    ~Registry();

    // Create a new object and return a reference to it
    RegistryReference CreateObject(ObjectBuilder& builder);

    // Given a reference, assuming it is still valid, invalidate it and delete the underlying object
    void DeleteObject(RegistryReference reference);
    void DeleteObjects(std::vector<RegistryReference> references);

    // Given a reference, assuming it is still valid, use a visitor to visit the underlying object
    void VisitObject(RegistryReference reference, ObjectVisitor& visitor);
    void VisitObjects(ObjectVisitor& visitor);
    void VisitObjects(std::vector<RegistryReference> refs, ObjectVisitor& visitor);

    // Test all underlying objects against a predicate and return a list of references to those that match
    std::vector<RegistryReference> QueryObjects(ObjectPredicate& predicate);

    unsigned int Count() const;
    static RegistryReference Null();
};

} // namespace Cad::Core