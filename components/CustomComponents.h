//67f1d40669d8352484e1c7030e16037d289cbda95843c1cf514d250b31b4a7d6
#include "PlayerInput.h"
#include <goop/Scene.h>
#include <goop/Entity.h>
#include <map> 
#include <variant>
#include <imgui.h>

using CustomComponentVariant = std::variant<
    PlayerInput
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
REGISTER_CUSTOM_COMPONENT("PlayerInput", PlayerInput);
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
        std::visit([s, &name](auto& arg) 
        {
            using T = std::decay_t<decltype(arg)>;
            auto view = s->view<T>();
            for (auto e : view)
            {     
                ImGui::Text("%s", name.c_str());        
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

inline void saveCustomComponents(goop::Scene* scene, goop::Entity e, nlohmann::json& json)  
{
    for (auto& [n, factory] : customComponentFactoryMap)
    { 
        auto variant = factory(entt::null, nullptr);
        std::visit([&e, &json, &n](auto& arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if (e.hasComponent<T>())  
            {     
                nlohmann::json j;
                j["type"] = n; 
                json.push_back(j);
            } 
        }, variant);
    }
}
