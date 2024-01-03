#include <goop/Components.h>
#include <map>

static std::map<std::string, std::function<goop::CustomComponent*(entt::entity, goop::Scene*)>> customComponentMap;

#define REGISTER_CUSTOM_COMPONENT(name, type) \
    static goop::CustomComponent* create##type(entt::entity e, goop::Scene* s) \
    { \
        return new type(goop::Entity(e, s)); \
    } \
    static bool registered##type = []() \
    { \
        customComponentMap[name] = create##type; \
        return true; \
    }()

