/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UIManager.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:30:00 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/06 12:47:50 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

struct UIState {
    bool wireframeMode = false;
    bool showVertices = false;
    bool orthographicProjection = false;
    bool autoRotation = false;
    glm::vec3 cameraPosition{0.0f};
    
    // Mesh info
    int vertexCount = 0;
    int indexCount = 0;
    int triangleCount = 0;
    int materialCount = 0;
    std::string currentFile = "";
    
    // Rendering stats
    float frameTime = 0.0f;
    float fps = 0.0f;
};

struct UILayout {
    float leftPanelWidth = 350.0f;
    float windowWidth = 1920.0f;
    float windowHeight = 1080.0f;
    float panelPadding = 15.0f;
    ImVec2 renderAreaPos{};
    ImVec2 renderAreaSize{};
};

class UIManager {
private:
    GLFWwindow* _window;
    UIState _state;
    UILayout _layout;
    ImFont *_boldFont;
    ImFont *_regularFont;
    
    // Style colors
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
    
    bool initialize();
    void shutdown();
    
    void newFrame();
    void render();
    
    void updateState(const UIState& newState);
    void updateMeshInfo(const Parser* parser);
    void updateCameraInfo(const InputManager* inputManager);
    void updatePerformanceStats(float deltaTime);
    void setCurrentFile(const std::string& filename);
    
    const UIState& getState() const { return _state; }
    const UILayout& getLayout() const { return _layout; }
    
    // Callback setters
    std::function<void(bool)> onWireframeModeChanged;
    std::function<void(bool)> onVertexModeChanged;
    std::function<void(bool)> onProjectionModeChanged;
    std::function<void(bool)> onAutoRotationChanged;
    std::function<void()> onResetCamera;
    std::function<void(const std::string&)> onLoadFile;
};

#endif
