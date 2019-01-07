# opengl_mini_startup
Mini OpenGL program to demonstrate startup issues

Starting the executable multiple times (must be the same executable file path) will cause
the *second* SwapBuffers call to take about 1 additional second for each instance of the program
that is already running.

The frame displayed is inexplicably white instead of black.

Maybe that has something to do with GDI windows being white by default or something.
