@echo off

if "%1"=="" (
    echo Usage: %0 ClassName
    exit /b 1
)

set CLASS_NAME=%1
set SCRIPT_DIR=%~dp0

set HEADER_FILE=%SCRIPT_DIR%\%CLASS_NAME%.h 
set CPP_FILE=%SCRIPT_DIR%\%CLASS_NAME%.cpp

echo Creating C++ class: %CLASS_NAME%...

( 
    echo #pragma once
    
    echo #include ^<goop/Components.h^>
    echo.
    echo #include ^<imgui.h^>
    echo. 
    echo class %CLASS_NAME% : public goop::CustomComponent
    echo {
    echo public:
    echo     %CLASS_NAME%(goop::Entity e) : goop::CustomComponent(e) {name = "%CLASS_NAME%";}
    echo.
    echo     void init();
    echo     void update(float dt);
    echo     void gui();
    echo. 
    echo private:
    echo     void onCollisionEnter(goop::Entity other) final;
    echo     void onCollisionExit(goop::Entity other) final;
    echo };
) > "%HEADER_FILE%"

echo Created %HEADER_FILE%

( 
    echo #include "%CLASS_NAME%.h"
    echo.
    echo // Gets called when the game starts
    echo void %CLASS_NAME%::init^()
    echo {
    echo     /\* do not remove this line \*/
    echo     CustomComponent::init();
    echo.
    echo     //...
    echo }
    echo.
    echo // Gets called every frame
    echo void %CLASS_NAME%::update(float dt^)
    echo {
    echo     //...
    echo }
    echo. 
    echo // Collision callbacks
    echo void PlayerInput::onCollisionEnter(goop::Entity other^) 
    echo {
    echo     // std::cout ^<^< "Player started colliding with " ^<^< other.getComponent^<goop::TagComponent^>().tag ^<^< std::endl;
    echo     //...
    echo }
    echo.
    echo void PlayerInput::onCollisionExit(goop::Entity other^)
    echo {   
    echo     // std::cout ^<^< "Player stopped colliding with " ^<^< other.getComponent^<goop::TagComponent^>().tag ^<^< std::endl;
    echo     //... 
    echo }
    echo.
    echo // Editor GUI - Shown in inspector view 
    echo // Refer to ImGui documentation for more info
    echo void %CLASS_NAME%::gui^()
    echo {
    echo     // ImGui::Text("Hello from %s", name.c_str());
    echo     //...
    echo }
) > "%CPP_FILE%"

echo Created %CPP_FILE%

echo C++ class '%CLASS_NAME%' created successfully!
