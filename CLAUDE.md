# 3DCast Codebase Guidelines

## Build System
- CMake (v3.7-3.12) with Visual Studio integration
- Build commands:
  - Debug: `cmake --build build --config Debug`
  - Release: `cmake --build build --config Release`
- Installation: Run `install.bat` (Windows) or `install.sh` (Linux)

## Code Organization
- Namespaces: `Cast::*` for engine code, `Runtime::*` for runtime
- Components: Entity-Component System using EnTT
- Layered architecture with separate modules (3DCast, GLWrapper, Runtime)

## Naming Conventions
- Classes: PascalCase (e.g., `OrthographicCamera`)
- Methods: PascalCase (e.g., `GetPosition()`)
- Variables: camelCase for local/parameters, m_PascalCase for members
- Constants/Macros: UPPERCASE with underscores (e.g., `CAST_ASSERT`)

## Error Handling
- Assertion macros: `CAST_ASSERT`, `CAST_CORE_ASSERT`
- Logging: `LOG_*` macros with different severity levels
- Debug/Release configurations determine assert behavior

## Dependencies
- Graphics: GLFW, GLEW, GLM
- UI: ImGui
- Utilities: spdlog, yaml-cpp, assimp