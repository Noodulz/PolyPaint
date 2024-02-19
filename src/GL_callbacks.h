/*######################################################
#                                                      #
#             GLUT rasterizing setup                   #
#                                                      #
########################################################*/

#pragma once
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_glfw.h"
#include "utilities.h"
#include "struct_types.h"
#include <gl/glut.h>
#include <iostream>
#include <string>
#include <stack>
#include <vector>

#define X_MAX 1000
#define Y_MAX 600
#define X_POSITION 500
#define Y_POSITION 100
#define MAX_POLYGONS 9999
#define CLOSE_DISTANCE 50.0f

extern void setupGlut()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, X_MAX, 0, Y_MAX);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}


