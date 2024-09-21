project(lilac-csharp)

#
# 2024-09-06 - NOTE: include_external_msproject not supported on UNIXen
#

#include_external_msproject(
#        lilac-csharp
#        lilac.csharp/lilac.csharp.csproj
#        TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC
#        GUID 26CAC20C-8E1E-4140-B7C1-37B774480C7D
#)

add_custom_target(
        lilac-csharp-managed
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/Lilac.Generator"
        COMMAND dotnet publish -c "$<CONFIG>" -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

add_executable(lilac-csharp)
add_dependencies(lilac-csharp lilac-csharp-managed)
