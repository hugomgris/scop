/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UIManager.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:30:00 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/07 16:50:13 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file UIManager.hpp
 * @brief Declaration of the UIManager class for ImGui-based user interface.
 *
 * The UIManager class handles all user interface operations using Dear ImGui,
 * providing a modern, responsive control panel for the SCOP 3D viewer. It manages
 * rendering controls, mesh information display, performance statistics, and
 * custom styled UI components with a professional dark theme.
 */

#ifndef UIMANAGER_HPP
# define UIMANAGER_HPP

# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include <imgui.h>
# include <imgui_impl_glfw.h>
# include <imgui_impl_opengl3.h>
# include <string>
# include <memory>
# include <functional>
# include "./Parser.hpp"
# include "./InputManager.hpp"
# include "./Colors.hpp"

/**
 * @struct UIState
 * @brief Holds the current state of all UI controls and application settings.
 */
struct UIState {
    bool wireframeMode = false;
    bool showVertices = false;
    bool orthographicProjection = false; 
    bool autoRotation = false;
    bool enableCRT = false;
    bool useTexture = false;
    glm::vec3 cameraPosition{0.0f};

    int vertexCount = 0;
    int indexCount = 0;
    int triangleCount = 0;
    int materialCount = 0;
    std::string currentFile = "";
    
    float frameTime = 0.0f;
    float fps = 0.0f;
};

/**
 * @struct UILayout
 * @brief Defines the layout and sizing parameters for the user interface.
 */
struct UILayout {
    float leftPanelWidth = 350.0f;        ///< Width of the left control panel
    float windowWidth = 1920.0f;          ///< Total window width
    float windowHeight = 1080.0f;         ///< Total window height
    float panelPadding = 15.0f;           ///< Padding around panel content
    ImVec2 renderAreaPos{};               ///< Position of 3D rendering area
    ImVec2 renderAreaSize{};              ///< Size of 3D rendering area
};

/**
 * @class UIManager
 * @brief Manages the Dear ImGui user interface for the SCOP application.
 *
 * The UIManager provides a comprehensive user interface featuring:
 * - Control panel with rendering options (wireframe, texture, projection modes)
 * - Real-time mesh information display (vertex/triangle counts, materials)
 * - Performance monitoring (FPS, frame time)
 * - Camera position tracking
 * - Custom styled UI components with professional dark theme
 * - Callback system for communicating with application logic
 * - Responsive layout that adapts to window resizing
 *
 * The UI is designed to be non-intrusive while providing full control over
 * all rendering parameters and detailed information about the loaded model.
 */
class UIManager {
    private:
        GLFWwindow* _window;
        UIState _state;
        UILayout _layout;
        ImFont *_boldFont;
        ImFont *_regularFont;
        
        ImVec4 _backgroundColor{0.1f, 0.1f, 0.1f, 1.0f};
        ImVec4 _panelColor{0.15f, 0.15f, 0.15f, 1.0f};
        ImVec4 _borderColor{0.9f, 0.9f, 0.9f, 1.0f};
        ImVec4 _textColor{0.9f, 0.9f, 0.9f, 1.0f};
        ImVec4 _buttonColor{0.2f, 0.2f, 0.2f, 1.0f};
        ImVec4 _buttonHoveredColor{0.4f, 0.4f, 0.4f, 1.0f};
        ImVec4 _buttonActiveColor{0.9f, 0.9f, 0.9f, 1.0f};
        
        void setupStyle();
        void renderControlPanel();
        void renderMeshInfo();
        void renderRenderingControls();
        void renderModelControls();
        void renderPerformanceStats();
        void renderMainViewport();
        void drawCustomFrameHeader(const char* title, ImVec2 framePos, float frameWidth, 
                                float headerHeight, ImU32 headerColor, ImU32 textColor);
        void renderStyledSection(const char* title, std::function<void()> contentCallback);
        void renderMeshInfoStyled();
        bool renderCustomCollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0);
        bool renderCustomButton(const char* label);

    public:
        UIManager(GLFWwindow* window);

        ~UIManager();

        const UIState& getState() const { return _state; }
        const UILayout& getLayout() const { return _layout; }

        bool initialize();
        void shutdown();
        void newFrame();
        void render();
        void updateState(const UIState& newState);
        void updateMeshInfo(const Parser* parser);
        void updateCameraInfo(const InputManager* inputManager);
        void updatePerformanceStats(float deltaTime);
        void setCurrentFile(const std::string& filename);

        std::function<void(bool)> onWireframeModeChanged;
        std::function<void(bool)> onVertexModeChanged;
        std::function<void(bool)> onProjectionModeChanged;
        std::function<void(bool)> onAutoRotationChanged;
        std::function<void(bool)> onCRTModeChanged;
        std::function<void(bool)> onTextureModeChanged;
        std::function<void()> onResetCamera;
        std::function<void(const std::string&)> onLoadFile;
};

#endif
