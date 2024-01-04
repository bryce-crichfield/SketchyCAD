#include <cad/object/ObjectRegistry.h>

namespace Cad {

ObjectRegistry::Entry::Entry() : object(nullptr) {}
ObjectRegistry::Entry::Entry(std::unique_ptr<Object> object) : object(std::move(object)) {}

void ObjectRegistry::Entry::Invalidate()
{
    if (object == nullptr) return;
    Object* ptr = object.release();
    delete ptr;
    object = nullptr;
}
Object& ObjectRegistry::Entry::GetObject() { return *object; }
bool ObjectRegistry::Entry::NotValid() const { return object == nullptr; }

ObjectRegistry::Reference ObjectRegistry::Null() { return std::make_shared<ObjectRegistry::Entry>(); }

ObjectRegistry::~ObjectRegistry()
{
    for (auto& reference : references) {
        reference->Invalidate();
    }

    references.clear();
}

ObjectRegistry::Reference ObjectRegistry::CreateObject(ObjectBuilder& builder)
{
    auto object = builder.Build();
    auto reference = std::make_shared<Entry>(std::move(object));
    references.push_back(reference);
    return reference;
}

void ObjectRegistry::DeleteObject(ObjectRegistry::Reference reference)
{
    if (reference->NotValid()) return;

    reference->Invalidate();

    // Purge invalid references (move to a flush method?)
    references.remove_if([](const Reference& reference) { return reference->NotValid(); });
}

void ObjectRegistry::DeleteObjects(std::vector<ObjectRegistry::Reference> refs)
{
    for (auto& reference : refs) {
        if (reference->NotValid()) return;

        reference->Invalidate();
    }

    references.remove_if([](const ObjectRegistry::Reference& reference) { return reference->NotValid(); });
}

void ObjectRegistry::VisitObjects(ObjectVisitor& visitor)
{
    for (auto& reference : references) {
        VisitObject(reference, visitor);
    }
}

void ObjectRegistry::VisitObject(ObjectRegistry::Reference reference, ObjectVisitor& visitor)
{
    if (reference->NotValid()) return;
    reference->GetObject().Accept(visitor);
}

void ObjectRegistry::VisitObjects(std::vector<ObjectRegistry::Reference> refs, ObjectVisitor& visitor)
{
    for (auto& reference : refs) {
        VisitObject(reference, visitor);
    }
}

std::vector<ObjectRegistry::Reference> ObjectRegistry::QueryObjects(ObjectPredicate& predicate)
{
    std::vector<Reference> result;
    for (auto& reference : references) {
        if (predicate.Match(reference->GetObject())) {
            result.push_back(reference);
        }
    }
    return result;
}

unsigned int ObjectRegistry::Count() const { return references.size(); }
}; // namespace Cad::Core
