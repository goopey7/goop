#include "Circle.h"
#include <goop/Scene.h>
#include <map>
#include <variant>

using CustomComponentVariant = std::variant<
    Circle
>;
static std::map<std::string, std::function<CustomComponentVariant(entt::entity, goop::Scene*)>> customComponentFactoryMap;

#define REGISTER_CUSTOM_COMPONENT(name, type) \
    static CustomComponentVariant create##type(entt::entity e, goop::Scene* s) \
    { \
        return type(goop::Entity(e, s)); \
    } \
    static bool registered##type = []() \
    { \
        customComponentFactoryMap[name] = create##type; \
        return true; \
    }()

REGISTER_CUSTOM_COMPONENT("Circle", Circle);
inline void initCustomComponents(goop::Scene* s)
{
    for (auto& [name, factory] : customComponentFactoryMap)
    {
 auto variant = factory(entt::null, s);
 std::visit([s](auto& arg)
 {
     using T = std::decay_t<decltype(arg)>;
     auto view = s->view<T>();
     for (auto e : view)
     {
         goop::Entity(e, s).getComponent<T>().init();
     }
 }, variant);
    }
}
inline void updateCustomComponents(goop::Scene* s, float dt)
{
    for (auto& [name, factory] : customComponentFactoryMap)
    {
 auto variant = factory(entt::null, s);
 std::visit([s, dt](auto& arg)
 {
     using T = std::decay_t<decltype(arg)>;
     auto view = s->view<T>();
     for (auto e : view)
     {
         goop::Entity(e, s).getComponent<T>().update(dt);
     }
 }, variant);
    }
}
inline void guiCustomComponents(goop::Scene* s)
{
    for (auto& [name, factory] : customComponentFactoryMap)
    {
 auto variant = factory(entt::null, s);
 std::visit([s](auto& arg)
 {
     using T = std::decay_t<decltype(arg)>;
     auto view = s->view<T>();
     for (auto e : view)
     {
         goop::Entity(e, s).getComponent<T>().gui();
     }
 }, variant);
    }
}
inline void addCustomComponent(const std::string& name, goop::Entity e, goop::Scene* scene)
{
    auto variant = customComponentFactoryMap[name](e.getEntity(), scene);
    std::visit([e](auto& arg)
    {
        using T = std::decay_t<decltype(arg)>;
        e.addComponent<T>(arg);
    }, variant);
}
