#pragma once

#include <cad/object/Object.h>
#include <cad/object/ObjectBuilder.h>

#include <list>

namespace Cad {

class ObjectRegistry {
    struct Entry {
        friend class ObjectRegistry;
        // The "underlying" object, an entry will only be considered valid if
        // this is not null
        std::unique_ptr<Object> object = nullptr;

        // Only the registry can invalidate an entry
        void Invalidate();

        // The user cannot access the underlying object directly, they must use
        // the registry and visit it with a visitor
        Object& GetObject();

        Entry();
        Entry(std::unique_ptr<Object> object);
        bool NotValid() const;
    };

  public:
    using Reference = std::shared_ptr<Entry>;

  private:
    std::list<Reference> references;

  public:
    ObjectRegistry() = default;
    ~ObjectRegistry();

    // Create a new object and return a reference to it
    Reference CreateObject(ObjectBuilder& builder);

    // Given a reference, assuming it is still valid, invalidate it and delete the underlying object
    void DeleteObject(Reference reference);
    void DeleteObjects(std::vector<Reference> references);

    // Given a reference, assuming it is still valid, use a visitor to visit the underlying object
    void VisitObject(Reference reference, ObjectVisitor& visitor);
    void VisitObjects(ObjectVisitor& visitor);
    void VisitObjects(std::vector<Reference> refs, ObjectVisitor& visitor);

    // Test all underlying objects against a predicate and return a list of references to those that match
    std::vector<Reference> QueryObjects(ObjectPredicate& predicate);

    unsigned int Count() const;
    static Reference Null();
};
}