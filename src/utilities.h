/*######################################################
#                                                      #
#     Helper functions for parsing colors and math     # 
#                                                      #
########################################################*/

#pragma once
#include "imgui.h"
#include "gl/glut.h"
#include "struct_types.h"
#include <iostream>
#include <stack>
#include <vector>
#include "GL_callbacks.h"
#define X_MAX 1000
#define Y_MAX 600
#define X_POSITION 500
#define Y_POSITION 100
#define MAX_POLYGONS 9999
#define CLOSE_DISTANCE 50.0f

Color getPixelColor(GLint x, GLint y)
{
    Color color;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &color);
    return color;
}

bool parseTextRGBA(const char* input, ImVec4& color, const char* colorName) {
    float r = 0.0, g = 0.0, b = 0.0, a = 1.0;
    if ((sscanf_s(input, "%f,%f,%f,%f", &r, &g, &b, &a) == 4) || (sscanf_s(input, "(%f, %f, %f, %f)", &r, &g, &b, &a) == 4) || (sscanf_s(input, "%f, %f, %f, %f", &r, &g, &b, &a) == 4)) {
        std::cout << "Success setting " << colorName << " color.\n";
        std::cout << "Color set to " << r << ", " << g << ", " << b << ", " << a << std::endl;
        color = ImVec4(r, g, b, a);
        return true;
    }
    else {
        std::cout << "Failed to set " << colorName << " color.\nEnter in format 1.0,1.0,1.0,1.0";
        return false;
    }
}

// Function to add current fill colors to the history
void fillColorsCache() {
    // Create a copy of the current fill colors and add it to the history
    std::vector<ImVec4> currentColors;
    std::stack<ImVec4> tempStack;
    while (!fill_grad_colors.empty()) {
        ImVec4 color = fill_grad_colors.top();
        currentColors.push_back(color);
        tempStack.push(color);
        fill_grad_colors.pop();
    }
    // Restore the original fill colors stack
    while (!tempStack.empty()) {
        fill_grad_colors.push(tempStack.top());
        tempStack.pop();
    }
    fillColorsHistory.push_back(currentColors);
}

void borderColorsCache() {
    // Create a copy of the current border colors and add it to the history
    std::vector<ImVec4> currentColors;
    std::stack<ImVec4> tempStack;
    while (!bord_grad_colors.empty()) {
        ImVec4 color = bord_grad_colors.top();
        currentColors.push_back(color);
        tempStack.push(color);
        bord_grad_colors.pop();
    }
    // Restore the original border colors stack
    while (!tempStack.empty()) {
        bord_grad_colors.push(tempStack.top());
        tempStack.pop();
    }
    borderColorsHistory.push_back(currentColors);
}


GLfloat distance(const Coords& p1, const Coords& p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}