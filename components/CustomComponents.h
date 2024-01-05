//d9f223827793ea3f823538c76b74b4ce608e210157e0b329f1f700a44bff59e8
#include "PlayerInput.h"
#include "WindowsTest.h"
#include <goop/Scene.h>
#include <goop/Entity.h>
#include <map> 
#include <variant>
#include <imgui.h>

using CustomComponentVariant = std::variant<
    PlayerInput,
    WindowsTest
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
REGISTER_CUSTOM_COMPONENT("WindowsTest", WindowsTest);
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

inline void guiCustomComponents(goop::Entity e)
{
    for (auto& [name, factory] : customComponentFactoryMap)  
    {
        auto variant = factory(entt::null, nullptr);
        std::visit([e, &name](auto& arg) 
        {
			goop::Scene* s = e.getScene();
            using T = std::decay_t<decltype(arg)>;
            auto view = s->view<T>();
            for (auto en : view)
            {     
				if (e.getEntity() != en) continue;
                ImGui::Text("%s", name.c_str());        
                goop::Entity(en, s).getComponent<T>().gui(); 
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
