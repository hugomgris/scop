/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UIManager.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:30:00 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/06 13:52:56 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/UIManager.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>

UIManager::UIManager(GLFWwindow* window) : _window(window) {
    int width, height;
    glfwGetWindowSize(_window, &width, &height);
    _layout.windowWidth = static_cast<float>(width);
    _layout.windowHeight = static_cast<float>(height);
    
    _layout.renderAreaPos = ImVec2(_layout.leftPanelWidth + _layout.panelPadding, _layout.panelPadding);
    _layout.renderAreaSize = ImVec2(
        _layout.windowWidth - _layout.leftPanelWidth - (_layout.panelPadding * 2),
        _layout.windowHeight - (_layout.panelPadding * 2)
    );
}

UIManager::~UIManager() {
    shutdown();
}

bool UIManager::initialize() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


    io.Fonts->AddFontDefault();
    ImFont *boldFont = io.Fonts->AddFontFromFileTTF("resources/fonts/jetBrainsMono/JetBrainsMonoNL-ExtraBold.ttf", 24.0f);
    ImFont *regularFont = io.Fonts->AddFontFromFileTTF("resources/fonts/jetBrainsMono/JetBrainsMono-Light.ttf", 18.0f);
    _boldFont = boldFont;
    _regularFont = regularFont;
    
    
    setupStyle();
    
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return true;
}

void UIManager::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::setupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Colors
    style.Colors[ImGuiCol_WindowBg] = _panelColor;
    style.Colors[ImGuiCol_MenuBarBg] = _panelColor;
    style.Colors[ImGuiCol_Text] = _textColor;
    style.Colors[ImGuiCol_Button] = _buttonColor;
    style.Colors[ImGuiCol_ButtonHovered] = _buttonHoveredColor;
    style.Colors[ImGuiCol_ButtonActive] = _buttonActiveColor;
    style.Colors[ImGuiCol_Border] = _borderColor;
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.8f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 0.8f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 0.8f);
    style.Colors[ImGuiCol_CheckMark] = _borderColor;
    style.Colors[ImGuiCol_SliderGrab] = _borderColor;
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = _panelColor;
    style.Colors[ImGuiCol_MenuBarBg] = _panelColor;
    style.Colors[ImGuiCol_Header] = _panelColor;
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(_panelColor.x + 0.05f, _panelColor.y + 0.05f, _panelColor.z + 0.05f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = _borderColor;
    
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); 
    
    // Style
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 3.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.WindowBorderSize = 5.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
}

void UIManager::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    int width, height;
    glfwGetWindowSize(_window, &width, &height);
    _layout.windowWidth = static_cast<float>(width);
    _layout.windowHeight = static_cast<float>(height);
    
    _layout.renderAreaPos = ImVec2(_layout.leftPanelWidth + _layout.panelPadding, _layout.panelPadding - 8);
    _layout.renderAreaSize = ImVec2(
        _layout.windowWidth - _layout.leftPanelWidth - (_layout.panelPadding * 2),
        _layout.windowHeight - (_layout.panelPadding) + 6
    );
}

void UIManager::render() {
    renderControlPanel();
    renderMainViewport();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::renderControlPanel() {
    ImVec2 panelPos = ImVec2(_layout.panelPadding, _layout.panelPadding);
    ImVec2 panelSize = ImVec2(_layout.leftPanelWidth, _layout.windowHeight - (_layout.panelPadding * 2));
    
    ImGui::SetNextWindowPos(panelPos);
    ImGui::SetNextWindowSize(panelSize);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    
    ImGui::Begin("Control Panel", nullptr, window_flags);
    
    float headerHeight = 35.0f;
    ImU32 headerColor = IM_COL32(229, 229, 217, 255);
    ImU32 textColor = IM_COL32(31, 31, 33, 255);
    
    drawCustomFrameHeader("CONTROL PANEL", panelPos, _layout.leftPanelWidth, 
                         headerHeight, headerColor, textColor);
    
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + headerHeight + 10.0f);
    
    renderMeshInfo();
    ImGui::Spacing();
    
    renderRenderingControls();
    ImGui::Spacing();
    
    renderModelControls();
    ImGui::Spacing();
    
    renderPerformanceStats();
    
    ImGui::End();
}

void UIManager::renderMeshInfo() {
    if (renderCustomCollapsingHeader("Mesh Information", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (_regularFont) {
            ImGui::PushFont(_regularFont);
        }
        
        ImGui::Text("File: %s", _state.currentFile.c_str());
        ImGui::Text("Vertices: %d", _state.vertexCount);
        ImGui::Text("Indices: %d", _state.indexCount);
        ImGui::Text("Triangles: %d", _state.triangleCount);
        ImGui::Text("Materials: %d", _state.materialCount);
        
        if (_regularFont) {
            ImGui::PopFont();
        }
        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
    }
}

void UIManager::renderRenderingControls() {
    if (renderCustomCollapsingHeader("Rendering Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (_regularFont) {
            ImGui::PushFont(_regularFont);
        }
        
        bool wireframe = _state.wireframeMode;
        if (ImGui::Checkbox("Wireframe Mode [V]", &wireframe)) {
            _state.wireframeMode = wireframe;
            if (onWireframeModeChanged) {
                onWireframeModeChanged(wireframe);
            }
        }
        
        bool vertices = _state.showVertices;
        if (ImGui::Checkbox("Show Vertices [X]", &vertices)) {
            _state.showVertices = vertices;
            if (onVertexModeChanged) {
                onVertexModeChanged(vertices);
            }
        }
        
        bool ortho = _state.orthographicProjection;
        if (ImGui::Checkbox("Orthographic [P]", &ortho)) {
            _state.orthographicProjection = ortho;
            if (onProjectionModeChanged) {
                onProjectionModeChanged(ortho);
            }
        }
        
        bool autoRot = _state.autoRotation;
        if (ImGui::Checkbox("Auto Rotation [1]", &autoRot)) {
            _state.autoRotation = autoRot;
            if (onAutoRotationChanged) {
                onAutoRotationChanged(autoRot);
            }
        }

        bool crtEffect = _state.enableCRT;
        if (ImGui::Checkbox("CRT Effect [C]", &crtEffect)) {
            _state.enableCRT = crtEffect;
            if (onCRTModeChanged) {
                onCRTModeChanged(crtEffect);
            }
        }

        if (_regularFont) {
            ImGui::PopFont();
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
    }
}

void UIManager::renderModelControls() {
    if (renderCustomCollapsingHeader("Model Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (_regularFont) {
            ImGui::PushFont(_regularFont);
        }
        
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", 
                   _state.cameraPosition.x, 
                   _state.cameraPosition.y, 
                   _state.cameraPosition.z);
        
        if (_regularFont) {
            ImGui::PopFont();
        }
        
        if (renderCustomButton("Reset Model [R]")) {
            if (onResetCamera) {
                onResetCamera();
            }
        }
        
        if (_regularFont) {
            ImGui::PushFont(_regularFont);
        }
        
        ImGui::Text("\nInteraction Controls:");
        ImGui::Text("Left-click + drag - Pan model");
        ImGui::Text("Right-click + drag - Rotate model");
        ImGui::Text("Scroll wheel - Zoom in/out");

        if (_regularFont) {
            ImGui::PopFont();
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
    }
}

void UIManager::renderPerformanceStats() {
    if (renderCustomCollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (_regularFont) {
            ImGui::PushFont(_regularFont);
        }
        
        ImGui::Text("FPS: %.1f", _state.fps);
        ImGui::Text("Frame Time: %.3f ms", _state.frameTime * 1000.0f);
        
        if (_regularFont) {
            ImGui::PopFont();
        }
        
        static float fps_history[100] = {};
        static int fps_history_offset = 0;
        fps_history[fps_history_offset] = _state.fps;
        fps_history_offset = (fps_history_offset + 1) % 100;
        
        ImGui::PlotLines("##FPS", fps_history, 100, fps_history_offset, 
                        nullptr, 0.0f, 120.0f, ImVec2(0, 80));
    }
}

void UIManager::renderMainViewport() {
    ImVec2 viewportPos = _layout.renderAreaPos;
    ImVec2 viewportSize = _layout.renderAreaSize;
    
    ImGui::SetNextWindowPos(viewportPos);
    ImGui::SetNextWindowSize(viewportSize);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                                   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                                   ImGuiWindowFlags_NoBackground;
    
    ImGui::Begin("3D Viewport", nullptr, window_flags);

    float headerHeight = 35.0f;
    ImU32 headerColor = IM_COL32(229, 229, 217, 255);
    ImU32 textColor = IM_COL32(31, 31, 33, 255);
    
    ImVec2 headerStartPos = ImGui::GetCursorScreenPos();
    headerStartPos.x += 7.0f;
    ImVec2 availableRegion = ImGui::GetContentRegionAvail();
    
    ImGui::InvisibleButton("##header_space", ImVec2(availableRegion.x, headerHeight - 5));
    
    drawCustomFrameHeader("VIEWPORT", headerStartPos, availableRegion.x - 7.0f, 
                         headerHeight, headerColor, textColor);

    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 border_start = ImVec2(canvas_pos.x + 9.0f, canvas_pos.y);
    ImVec2 border_end = ImVec2(canvas_pos.x + canvas_size.x - 1.0f, canvas_pos.y + canvas_size.y - 5.0f);

    draw_list->AddRect(border_start, border_end, 
                    IM_COL32(230, 230, 230, 255),
                    0.0f, 0, 5.0f);
        
    ImGui::End();
}

void UIManager::updateState(const UIState& newState) {
    _state = newState;
}

void UIManager::updateMeshInfo(const Parser* parser) {
    if (parser) {
        _state.vertexCount = parser->getVertices().size();
        _state.indexCount = parser->getIndices().size();
        _state.triangleCount = _state.indexCount / 3;
        _state.materialCount = parser->getMaterials().size();
    }
}

void UIManager::updateCameraInfo(const InputManager* inputManager) {
    if (inputManager) {
        _state.cameraPosition = inputManager->getCameraPosition();
        _state.autoRotation = inputManager->getAutorotationStatus();
        _state.orthographicProjection = inputManager->isUsingOrthographic();
    }
}

void UIManager::updatePerformanceStats(float deltaTime) {
    _state.frameTime = deltaTime;
    _state.fps = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;
}

void UIManager::setCurrentFile(const std::string& filename) {
    _state.currentFile = filename;
}

void UIManager::drawCustomFrameHeader(const char* title, ImVec2 framePos, float frameWidth, 
                                     float headerHeight, ImU32 headerColor, ImU32 textColor) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImVec2 headerMin = ImVec2(framePos.x, framePos.y - 2);
    ImVec2 headerMax = ImVec2(framePos.x + frameWidth, framePos.y + headerHeight);
    draw_list->AddRectFilled(headerMin, headerMax, headerColor);
    
    draw_list->AddLine(ImVec2(headerMin.x, headerMin.y), ImVec2(headerMax.x, headerMin.y), IM_COL32(229, 229, 217, 255), 1.0f);
    draw_list->AddLine(ImVec2(headerMin.x, headerMin.y), ImVec2(headerMin.x, headerMax.y), IM_COL32(229, 229, 217, 255), 1.0f);  
    draw_list->AddLine(ImVec2(headerMax.x, headerMin.y), ImVec2(headerMax.x, headerMax.y), IM_COL32(229, 229, 217, 255), 1.0f);
    
    if (_boldFont) {
        ImGui::PushFont(_boldFont);
    }

    ImVec2 textSize = ImGui::CalcTextSize(title);
    ImVec2 textPos = ImVec2(
        framePos.x + (frameWidth - textSize.x) * 0.5f,
        framePos.y + (headerHeight - textSize.y) * 0.5f
    );
    
    draw_list->AddText(textPos, textColor, title);
    
    if (_boldFont) {
        ImGui::PopFont();
    }
}

bool UIManager::renderCustomCollapsingHeader(const char* label, ImGuiTreeNodeFlags flags) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(_layout.leftPanelWidth - (_layout.panelPadding * 2) + 15.0f, ImGui::GetTextLineHeightWithSpacing() + 14.0f);
    
    ImVec2 mousePos = ImGui::GetMousePos();
    bool isHovered = (mousePos.x >= cursorPos.x && mousePos.x <= cursorPos.x + size.x &&
                     mousePos.y >= cursorPos.y && mousePos.y <= cursorPos.y + size.y);
    
    bool isClicked = isHovered && ImGui::IsMouseDown(0);

    ImVec4 textColor;
    if (isClicked) {
        textColor = ImVec4(_panelColor.x, _panelColor.y, _panelColor.z, 1.0f);
    } else {
        textColor = _textColor;
    }

    if (_boldFont) {
        ImGui::PushFont(_boldFont);
    }
    
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    bool result = ImGui::CollapsingHeader(label, flags);
    ImGui::PopStyleColor();
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 headerMin = ImVec2(cursorPos.x, cursorPos.y);
    ImVec2 headerMax = ImVec2(cursorPos.x + size.x, cursorPos.y + size.y);
    
    ImU32 borderColor = IM_COL32(
        static_cast<int>(_borderColor.x * 255),
        static_cast<int>(_borderColor.y * 255),
        static_cast<int>(_borderColor.z * 255),
        255
    );
    
    draw_list->AddRect(headerMin, headerMax, borderColor, 0.0f, 0, 2.0f);

    if (_boldFont) {
        ImGui::PopFont();
    }
    
    return result;
}

bool UIManager::renderCustomButton(const char* label) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 buttonSize = ImGui::CalcTextSize(label);
    buttonSize.x += ImGui::GetStyle().FramePadding.x * 2.0f;
    buttonSize.y += ImGui::GetStyle().FramePadding.y * 2.0f;
    
    ImVec2 mousePos = ImGui::GetMousePos();
    bool isHovered = (mousePos.x >= cursorPos.x && mousePos.x <= cursorPos.x + buttonSize.x &&
                     mousePos.y >= cursorPos.y && mousePos.y <= cursorPos.y + buttonSize.y);
    
    bool isClicked = isHovered && ImGui::IsMouseDown(0);

    ImVec4 textColor;
    if (isClicked) {
        textColor = ImVec4(_panelColor.x, _panelColor.y, _panelColor.z, 1.0f);
    } else {
        textColor = _textColor;
    }
    
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    bool result = ImGui::Button(label);
    ImGui::PopStyleColor();
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 buttonMin = ImVec2(cursorPos.x, cursorPos.y);
    ImVec2 buttonMax = ImVec2(cursorPos.x + buttonSize.x, cursorPos.y + buttonSize.y + 2.0f);
    
    ImU32 borderColor = IM_COL32(
        static_cast<int>(_borderColor.x * 255),
        static_cast<int>(_borderColor.y * 255),
        static_cast<int>(_borderColor.z * 255),
        255
    );
    
    draw_list->AddRect(buttonMin, buttonMax, borderColor, 0.0f, 0, 2.0f);
    
    return result;
}