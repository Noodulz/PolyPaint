# PolyPaint - A Polygon Shader Made with OpenGL + ImGui

![/assets/"PolyPaint Demo.gif"](https://github.com/Noodulz/PolyPaint/blob/main/assets/PolyPaint%20Demo.gif)

## How to Run
Install the .exe and freeglut.dll file in Releases, and run the .exe with the .dll in the same directory. To edit and build the project, clone this repository, and open PolyPaint.sln in Visual Studio 2022.

## Customizing Colors, Borders, Fill etc.

### Fill and Border
To change fill and/or border colors, insert a quadruplet of RGBA values ranging from 0 to 1.0 in either the format `(1.0, 1.0, 1.0, 1.0)` or `1.0,1.0,1.0,1.0` and click their respective set color buttons to confirm changes. To change the border width, adjust the line width slide in the middle of the UI (up to size 10). 

### Fill Pattern
Choose from a range of 4 byte map patterns, choosing only one pattern at a time. Customized byte map patterns will be added in the future.

### Fill and Border Gradients
To rasterize polygons with gradients, be sure to have the fill and/or border gradient boxes checked (f-gradnt and b-gradnt), and upon enabling this, any future quadruplet of RGBA values inserted and set will be blended together on each vertices of the polygon. It is recommended you add at least 3 quadruplets of colors for a fuller effect, and to increase line width to see border gradients more clearly.

### Border Patterns
In OpenGL/GLUT, border patterns are rasterized through hexadecimal values in the form of `0x00FF`. Feel free to experiment with different hex values to see results. Some common patterns include dotted `0x0101 or 0x5555`, dashed `0x00FF`, and solid `0xFFFF`. 
