/*######################################################
#                                                      #
# Polygon Paint by Jocelyn Dzuong, CAP5701 Spring 2023 #
# All work here is mine and cited accordingly          #
#                                                      #
########################################################*/

#define IMGUI_DISABLE_STYLIZER
#define IMGUI_DISABLE_DEMO_WINDOWS
#define IMGUI_DISABLE_DOCKING
#define IMGUI_DISABLE_METRICS
#define IMGUI_DISABLE_STYLIZER
#define IMGUI_DISABLE_LOG_CHANNEL
#define X_MAX 1000
#define Y_MAX 600
#define X_POSITION 500
#define Y_POSITION 100
#define MAX_POLYGONS 9999
#define CLOSE_DISTANCE 50.0f
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_glfw.h"
#include "struct_types.h"
#include "utilities.h"
#include <GL/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdio>
#include <stack>
#include <vector>

// Mouse move callback
void onMouseMove(int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(x, y);
    io.MouseDown[0] = glutGetModifiers() & GLUT_LEFT_BUTTON;
    io.MouseDown[1] = glutGetModifiers() & GLUT_RIGHT_BUTTON;
    io.MouseDown[2] = glutGetModifiers() & GLUT_MIDDLE_BUTTON;

    if (!io.WantCaptureMouse)
    {
        if (coordsP == 0 || recentMouseButton == GLUT_RIGHT_BUTTON)
        {
            /*If there are no previous coordinates or if the recent button is the right button,
            update the previous mouse location and return without drawing anything. */
            previousMouseLoc[0] = x;
            previousMouseLoc[1] = y;
            return;
        }

        // Convert mouse coordinates to OpenGL coordinate system
        y = (glutGet(GLUT_WINDOW_HEIGHT) - y) * (float)Y_MAX / glutGet(GLUT_WINDOW_HEIGHT);
        x = x * ((float)X_MAX / glutGet(GLUT_WINDOW_WIDTH));
        glClear(GL_COLOR_BUFFER_BIT);
        // Clear the OpenGL buffer only when starting a new polygon
        if (coordsP == 0)
        {
            glClear(GL_COLOR_BUFFER_BIT);
        }

        // Draw the polygonal lines
        glLineWidth(lWidth);
        glBlendFunc(GL_ONE, GL_ZERO);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glBegin(GL_LINE_STRIP);
        glVertex2i(previousMouseLoc[0], previousMouseLoc[1]);
        glVertex2i(polygons[polygonsP][coordsP - 1].x, polygons[polygonsP][coordsP - 1].y);
        glEnd();

        glColor4f(borderColor[0], borderColor[1], borderColor[2], borderColor[3]);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int j = 0; j <= polygonsP; j++)
        {
            int no_vertex = (j == polygonsP) ? coordsP : no_vertices[j];
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < no_vertex; i++)
                glVertex2i(polygons[j][i].x, polygons[j][i].y);
            if (j == polygonsP)
                glVertex2i(x, y);
            else
                glVertex2i(polygons[j][0].x, polygons[j][0].y);
            glEnd();
        }
        glFlush();
        previousMouseLoc[0] = x;
        previousMouseLoc[1] = y;
    }
}

// Mouse click callback
void onMouseClick(int button, int state, int x, int y)
{
    // Handle mouse button states for ImGui
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(x, y);

    if (state == GLUT_DOWN)
    {
        io.MouseDown[button] = true;
        if (io.WantCaptureMouse) {
            return;
        }
    }
    else if (state == GLUT_UP)
    {
        io.MouseDown[button] = false;
        if (io.WantCaptureMouse) {
            return;
        }
    }

    // set line width, border color, border pattern
    glLineWidth(lWidth);
    glColor4f(border_color.x, border_color.y, border_color.z, border_color.w);
    glLineStipple(1, bPattern);
    glEnable(GL_LINE_STIPPLE);

    // Convert mouse coordinates to OpenGL coordinate system
    y = (glutGet(GLUT_WINDOW_HEIGHT) - y) * (float)Y_MAX / glutGet(GLUT_WINDOW_HEIGHT);
    x = x * ((float)X_MAX / glutGet(GLUT_WINDOW_WIDTH));

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // If there are less than 3 points, simply add the clicked point
        if (coordsP < 3)
        {
            // Add the clicked point to the current polygon
            polygons[polygonsP][coordsP].x = x;
            polygons[polygonsP][coordsP].y = y;
            coordsP++;
        }
        else
        {
            // if last point is close enough to first point, rasterize polygon with settings 
            if (distance(polygons[polygonsP][0], { x, y }) <= CLOSE_DISTANCE)
            {
                // Render the polygon and its fill color
                glBegin(GL_POLYGON);

                for (int i = 0; i < coordsP; i++) {
                    // Add current fill colors to the history
                    fillColorsCache();
                    // Render the polygon using the current fill color
                    if (fillGrad && !fillColorsHistory.empty()) {
                        glEnable(GL_BLEND);
                        std::vector<ImVec4> currentColors = fillColorsHistory.back();
                        ImVec4& color = currentColors[i % currentColors.size()]; // Cycle through colors for each vertex
                        glColor4f(color.x, color.y, color.z, color.w);
                        glVertex2i(polygons[polygonsP][i].x, polygons[polygonsP][i].y);
                        glDisable(GL_BLEND);
                    }
                    else {
                        glColor4f(fill_color.x, fill_color.y, fill_color.z, fill_color.w);
                        glVertex2i(polygons[polygonsP][i].x, polygons[polygonsP][i].y);
                    }
                }

                // Clear the fill colors history
                fillColorsHistory.clear();

                // End the polygon rendering
                glEnd();
                glDisable(GL_POLYGON);

                // Render the polygon's borders with patterns as need be
                glColor4f(border_color.x, border_color.y, border_color.z, border_color.w);
                glLineStipple(1, bPattern);
                glEnable(GL_LINE_STIPPLE);
                glBegin(GL_LINE_LOOP);
                for (int i = 0; i < coordsP; i++) {
                    // Add current border colors to the history
                    borderColorsCache();
                    // Render the polygon border using the current border color
                    if (borderGrad && !borderColorsHistory.empty()) {
                        glEnable(GL_BLEND);
                        std::vector<ImVec4> currentColors = borderColorsHistory.back();
                        ImVec4& color = currentColors[i % currentColors.size()]; // Cycle through colors for each vertex
                        glColor4f(color.x, color.y, color.z, color.w);
                        glVertex2i(polygons[polygonsP][i].x, polygons[polygonsP][i].y);
                        glDisable(GL_BLEND);
                    }
                    else {
                        // Use a single border color if gradient is not enabled or history is empty
                        glColor4f(border_color.x, border_color.y, border_color.z, border_color.w);
                        glVertex2i(polygons[polygonsP][i].x, polygons[polygonsP][i].y);
                    }
                }
                borderColorsHistory.clear();
                glEnd();
                glDisable(GL_LINE_STIPPLE);

                // Render the polygon and its fill pattern
                if (currFillPattern > 0 && currFillPattern <= 3) {
                    glEnable(GL_POLYGON_STIPPLE);
                    glColor4f(0.0, 0.0, 0.0, 1.0);
                    switch (currFillPattern) {
                    case 1:
                        glPolygonStipple(fly);
                        break;
                    case 2:
                        glPolygonStipple(HorHatch);
                        break;
                    case 3:
                        glPolygonStipple(halftone);
                        break;
                    default:
                        glPolygonStipple(halftone);
                        break;
                    }
                    glBegin(GL_POLYGON);
                    for (int i = 0; i < coordsP; i++) {
                        glVertex2i(polygons[polygonsP][i].x, polygons[polygonsP][i].y);
                    }
                    glEnd();
                    glDisable(GL_POLYGON_STIPPLE);
                }

                // Start a new polygon
                polygonsP++;
                coordsP = 0;
            }
            else
            {
                // Add the clicked point to the current polygon
                polygons[polygonsP][coordsP].x = x;
                polygons[polygonsP][coordsP].y = y;
                coordsP++;
            }
        }

        // Update the display
        glutPostRedisplay();
    }
}

void renderUI()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    // Render ImGui UI
    {
        ImGui::SetNextWindowSize(ImVec2(320, 395));
        ImGui::SetNextWindowPos(ImVec2(glutGet(GLUT_WINDOW_WIDTH) - 308, 0));
        ImGui::Begin("Polygon Paint");

        ImGui::Text("Fill Color (r,g,b,a)");
        static char fc[64] = ""; // Make it static to persist its value
        ImGui::InputText("##Text1", fc, IM_ARRAYSIZE(fc));
        ImGui::SameLine();
        ImGui::Checkbox("F-Gradnt", &fillGrad);
        if (ImGui::Button("Set Fill Color")) {
            if (fillGrad && parseTextRGBA(fc, fill_color, "Fill")) {
                std::cout << "Using fill gradients.\n";
                fill_grad_colors.push(fill_color);
            }
            else if (!fillGrad && parseTextRGBA(fc, fill_color, "Fill")) {
                std::cout << "No fill gradients.\n";
                while (!fill_grad_colors.empty()) {
                    fill_grad_colors.pop();
                }
                fillColorsHistory.clear();
            }
            else {
                parseTextRGBA(fc, fill_color, "Fill");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Fill Color")) {
            fill_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            fillGrad = false;
            default_fill_grad = fill_color;
            while (!fill_grad_colors.empty()) {
                fill_grad_colors.pop();
            }
            fillColorsHistory.clear();
            std::cout << "Fill color back to white.\n";
        }
        ImGui::NewLine();

        ImGui::Text("Fill Pattern");
        ImGui::Checkbox("Solid", &isSolid); ImGui::SameLine();
        ImGui::Checkbox("Fly", &isFly); ImGui::SameLine(); // 1
        ImGui::Checkbox("Hatched", &isHatch); ImGui::SameLine(); // 2
        ImGui::Checkbox("Halftone", &isHalf); // 3
        if (ImGui::Button("Set Fill Pattern"))
        {
            if (isSolid && ((isFly || isHatch || isHalf) || !(isFly || isHatch || isHalf))) {
                isFly = isHatch = isHalf = false;
                currFillPattern = 0;
                std::cout << "Set fill pattern to solid.\n";
            }
            else if (isFly && ((isSolid || isHatch || isHalf) || !(isSolid || isHatch || isHalf))) {
                isSolid = isHatch = isHalf = false;
                currFillPattern = 1;
                std::cout << "Set fill pattern to fly.\n";
            }
            else if (isHatch && ((isSolid || isFly || isHalf) || !(isSolid || isFly || isHalf))) {
                isSolid = isFly = isHalf = false;
                currFillPattern = 2;
                std::cout << "Set fill pattern to hatched.\n";

            }
            else if (isHalf && ((isSolid || isFly || isHatch) || !(isSolid || isFly || isHatch))) {
                isSolid = isFly = isHatch = false;
                currFillPattern = 3;
                std::cout << "Set fill pattern to halftone.\n";
            }
            else {
                isSolid = true;
                currFillPattern = 0;
                isFly = isHatch = isHalf = false;
                std::cout << "Set fill pattern to solid by default.\n";
            }
        }
        ImGui::NewLine();

        ImGui::Text("Border Color (r,g,b,a)");
        static char bc[64] = ""; // Make it static to persist its value
        ImGui::InputText("##Text2", bc, IM_ARRAYSIZE(bc));
        ImGui::SameLine();
        ImGui::Checkbox("B-Gradnt", &borderGrad);
        if (ImGui::Button("Set Border Color")) {
            if (borderGrad && parseTextRGBA(bc, border_color, "Border")) {
                std::cout << "Using border gradients.\n";
                bord_grad_colors.push(border_color);
            }
            else if (!borderGrad && parseTextRGBA(bc, border_color, "Border")) {
                std::cout << "No border gradients.\n";
                while (!bord_grad_colors.empty()) {
                    bord_grad_colors.pop();
                }
                borderColorsHistory.clear();
            }
            
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Border Color")) {
            borderGrad = false;
            border_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            while (!bord_grad_colors.empty()) {
                bord_grad_colors.pop();
            }
            borderColorsHistory.clear();
            std::cout << "Border color back to black.\n";
        }
        ImGui::NewLine();
        ImGui::Text("Line Width");
        ImGui::SliderFloat("##Slide1", &lWidth, 1.0, 10.0);
        ImGui::NewLine();

        ImGui::Text("Border Pattern (Hex value)");
        static char bp[14] = "";
        ImGui::InputText("##Text3", bp, IM_ARRAYSIZE(bp));
        unsigned int hexValue;
        if (ImGui::Button("Set Border Pattern")) {
            if (sscanf_s(bp, "0x%x", &hexValue) == 1) {
                std::cout << "Success setting " << "Border pattern" << " color.\n";
                bPattern = static_cast<GLushort>(hexValue);
                std::cout << "Pattern set to " << bPattern << std::endl;
            }
            else {
                std::cout << "Please enter in the format 0x00FF\n";
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Border Pattern")) {
            border_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            bPattern = 0xFFFF;
            borderColorsHistory.clear();
            while (!bord_grad_colors.empty()) {
                bord_grad_colors.pop();
            }
            std::cout << "Reset border pattern back to solid.\n";
        }
        ImGui::NewLine();

        ImGui::End();
    }

    // Rendering ImGui
    ImGui::Render();

    // Setup viewport and clear background
    glViewport(0, 0, X_MAX, Y_MAX);

    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

    // Rendering ImGui draw data
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    // Swap buffers and update the display
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    // Initialize GLUT window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_MULTISAMPLE);
    glutInitWindowSize(X_MAX, Y_MAX);
    glutInitWindowPosition(X_POSITION, Y_POSITION);
    glutCreateWindow("Polygon Paint");

    // Setup OpenGL state
    setupGlut();

    // Setup GLUT display function
    glutDisplayFunc(renderUI);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL2_Init();
    ImGui_ImplGLUT_InstallFuncs();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup ImGui colors
    ImGui::StyleColorsDark();

    // Setup mouse callback functions
    glutMotionFunc(onMouseMove);
    glutMouseFunc(onMouseClick);

    // Start main GLUT loop
    std::cout << "Rasterizing polygon with solid black border and white transparent fill.\n";
    glutMainLoop();

    // Cleanup ImGui on exit
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
