Export File Description
-----------------------

Extension: *.M3D (Custom Max Exporter)

Exports the version number, materials, vertices, normals, texture coordinates, fat indices as numbers 
pointing to the vertex, normal, and texture coordinate used for each vertex per object. Supports
normal mapping, multiple textures, keyframe animations, and skinning.


dll-examples.dle should be placed in the plugins folder of your 3DStudio Max,
e.g. C:\Program Files\Autodesk\3ds Max 2012\plugins


Run the DX11Basics.exe in ..\McLaine_Exporter\McLaine_DualRenderer\DX11Basics\DX11Basics




Controls
--------

WASD - Movement

Space/C - Up/Down

L - Place/Grab Light

Page Up/Page Down - Cycle Scenes / Animation Sets



Notes
--------

Visual Studio should be opened in Admin mode to compile exporter as it will send the .DLE
directly to the 3DStudio Max plugins folder.