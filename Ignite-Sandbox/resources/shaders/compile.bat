%VULKAN_SDK%/Bin32/glslc.exe shader.vert -o vert.spv
%VULKAN_SDK%/Bin32/glslc.exe shader.frag -o frag.spv
%VULKAN_SDK%/Bin32/glslc.exe unlit.vert -o unlitVert.spv
%VULKAN_SDK%/Bin32/glslc.exe unlit.frag -o unlitFrag.spv

@echo off
xcopy /e /v . ..\..\..\bin\Release-windows-x86_64\Ignite-Sandbox\resources\shaders /Y
xcopy /e /v . ..\..\..\bin\Debug-windows-x86_64\Ignite-Sandbox\resources\shaders /Y
pause