@echo off
setlocal enabledelayedexpansion

:: Get the directory of the script
for %%I in ("%~dp0") do set "script_dir=%%~fI"

:: Move to the script's directory
cd /d "%script_dir%" || exit /b

:: Find .cpp files
set "cpp_files="
for %%F in ("%script_dir%\*.cpp") do (
    set "cpp_files=!cpp_files! %%~nxF"
)

:: Calculate hash of CustomComponents.h if it exists
set "existing_hash="
if exist CustomComponents.h (
    for /f %%H in ('CertUtil -hashfile CustomComponents.h SHA256 ^| findstr /v /c:SHA256') do (
        set "existing_hash=%%H"
    )
)

:: Function to calculate hash for a file
:calculate_hash
set "file=%~1"
for /f %%H in ('CertUtil -hashfile "!file!" SHA256 ^| findstr /v /c:SHA256') do (
    set "hash_result=%%H"
)
exit /b

:: Concatenate all .cpp file names together to form a string
set "file_names_concatenated="
for %%F in (%cpp_files%) do (
    set "file_names_concatenated=!file_names_concatenated! %%~nxF"
)

:: Calculate the hash of the concatenated file names
set "updated_hash="
echo !file_names_concatenated! | CertUtil -hashfile - SHA256 | findstr /v /c:SHA256 > hash_result.txt
set /p updated_hash=<hash_result.txt
del hash_result.txt

:: Get the existing hash from the first line comment
set "existing_hash_comment="
for /f "tokens=*" %%A in (CustomComponents.h) do (
    set "existing_hash_comment=%%A"
    goto :hash_comment_found
)
:hash_comment_found

set "existing_hash="
for /f "tokens=2 delims=/" %%B in ("!existing_hash_comment!") do (
    set "existing_hash=%%B"
)

:: Check if the file needs updating based on the hash
set "needs_update=false"
if "!existing_hash!" neq "!updated_hash!" (
    set "needs_update=true"
)

if not !needs_update! == false (
    echo CustomComponents.h is up to date
    exit /b 0
)

:: Clear the contents of CustomComponents.h
type nul > CustomComponents.h

:: Loop through each .cpp file and generate include statements
for %%F in (%cpp_files%) do (
    set "filename=%%~nF"
    echo #include "!filename!.h"
) >> CustomComponents.h

:: Add remaining content to CustomComponents.h
(
echo #include ^<goop/Scene.h^>
echo #include ^<goop/Entity.h^>
echo #include ^<map^>
echo #include ^<variant^>
echo #include ^<imgui.h^>
echo.
echo using CustomComponentVariant = std::variant^< 
)

:: Check if there are any .cpp files
set "file_count=0"
for %%F in (%cpp_files%) do (
    set /a "file_count+=1"
)

:: Generate variant types or default if no .cpp files are found
if %file_count% neq 0 (
    for %%F in (%cpp_files%) do (
        set "filename=%%~nF"
        echo     !filename!,
    )
) else (
    echo std::monostate,
)

:: Remove the last comma
echo ^>^; 
echo.
echo static std::map^<std::string, std::function^<CustomComponentVariant(entt::entity, goop::Scene*)^>^> customComponentFactoryMap;
echo.

:: Check if there are any .cpp files for generating additional content
if %file_count% neq 0 (
    echo #define REGISTER_CUSTOM_COMPONENT(name, type) \\
    echo     static CustomComponentVariant create##type(entt::entity e, goop::Scene* s) \\
    echo     ^{ \\
    echo         return type(goop::Entity(e, s)); \\
    echo     ^} \\
    echo     static bool registered##type = ^[^] \\
    echo     ^{ \\
    echo         customComponentFactoryMap[name] = create##type; \\
    echo         return true; \\
    echo     ^}^(
    
    :: Register components
    for %%F in (%cpp_files%) do (
        set "filename=%%~nF"
        echo REGISTER_CUSTOM_COMPONENT^("!filename!", !filename!^);   
    )
)

) >> CustomComponents.h

(
echo inline void initCustomComponents(goop::Scene* s^)
echo {
) > CustomComponents.h

:: Checking if there are .cpp files
if %file_count% neq 0 (
    (
    echo    for (auto^& [name, factory] : customComponentFactoryMap^)
    echo    {
    echo        auto variant = factory(entt::null, s^);
    echo        std::visit([s](auto^& arg^)  
    echo        {     
    echo            using T = std::decay_t<decltype(arg^)>;
    echo            auto view = s->view<T^>();
    echo            for (auto e : view^)  
    echo            {        
    echo                goop::Entity(e, s).getComponent<T^>().init^();
    echo            }
    echo        ^}, variant^);
    echo    }
    ) >> CustomComponents.h
)

(
echo }
echo.
echo inline void updateCustomComponents(goop::Scene* s, float dt^)  
echo {
) >> CustomComponents.h

:: Checking if there are .cpp files
if %file_count% neq 0 (
    (
    echo    for (auto^& [name, factory] : customComponentFactoryMap^)
    echo    {
    echo        auto variant = factory(entt::null, s^);
    echo        std::visit([s, dt](auto^& arg^) 
    echo        {  
    echo            using T = std::decay_t<decltype(arg^)>;
    echo            auto view = s->view<T^>();
    echo            for (auto e : view^)   
    echo            {         
    echo                goop::Entity(e, s).getComponent<T^>().update(dt^);
    echo            }
    echo        ^}, variant^);
    echo    }
    ) >> CustomComponents.h
)

(
echo }
echo.
echo inline void guiCustomComponents(goop::Scene* s^)
echo {
) >> CustomComponents.h

:: Checking if there are .cpp files
if %file_count% neq 0 (
    (
    echo    for (auto^& [name, factory] : customComponentFactoryMap^)  
    echo    {
    echo        auto variant = factory(entt::null, s^);
    echo        std::visit([s, ^&name](auto^& arg^) 
    echo        {
    echo            using T = std::decay_t<decltype(arg^)>;
    echo            auto view = s->view<T^>();
    echo            for (auto e : view^)
    echo            {     
    echo                ImGui::Text("^%s^", name.c_str()^);        
    echo                goop::Entity(e, s).getComponent<T^>().gui^(); 
    echo            }
    echo        ^}, variant^);
    echo    }
    ) >> CustomComponents.h
)

(
echo }
echo.
echo inline void addCustomComponent(const std::string^& name, goop::Entity e, goop::Scene* scene^) 
echo {
) >> CustomComponents.h

:: Checking if there are .cpp files
if %file_count% neq 0 (
    (
    echo    auto variant = customComponentFactoryMap[name](e.getEntity(), scene^);
    echo    std::visit([e](auto^& arg^)   
    echo    {
    echo        using T = std::decay_t<decltype(arg^)>;
    echo        e.addComponent<T>(arg^);
    echo    ^}, variant^);
    ) >> CustomComponents.h
)

(
echo }
echo.
echo inline void saveCustomComponents(goop::Scene* scene, goop::Entity e, nlohmann::json^& json^)  
echo {
) >> CustomComponents.h

:: Checking if there are .cpp files
if %file_count% neq 0 (
    (
    echo    for (auto^& [n, factory] : customComponentFactoryMap^)
    echo    { 
    echo        auto variant = factory(entt::null, nullptr^);
    echo        std::visit([&e, ^&json, ^&n](auto^& arg^)
    echo        {
    echo            using T = std::decay_t<decltype(arg^)>;
    echo            if (e.hasComponent<T>()^)  
    echo            {     
    echo                nlohmann::json j;
    echo                j["type"] = n; 
    echo                json.push_back(j^);
    echo            } 
    echo        ^}, variant^);
    echo    }
    ) >> CustomComponents.h
)

(
echo }
) >> CustomComponents.h

:: Update the hash in the first line comment of CustomComponents.h
powershell -Command "(Get-Content CustomComponents.h) | ForEach-Object { $_ -replace '\/\/.*', '\/\/!updated_hash!' } | Set-Content CustomComponents.h"

echo CustomComponents.h updated
