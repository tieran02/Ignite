%VULKAN_SDK%/Bin32/glslc.exe shader.vert -o vert.spv
%VULKAN_SDK%/Bin32/glslc.exe shader.frag -o frag.spv
%VULKAN_SDK%/Bin32/glslc.exe unlit.vert -o unlitVert.spv
%VULKAN_SDK%/Bin32/glslc.exe unlit.frag -o unlitFrag.spv
%VULKAN_SDK%/Bin32/glslc.exe normal.vert -o normalVert.spv
%VULKAN_SDK%/Bin32/glslc.exe normal.frag -o normalFrag.spv
%VULKAN_SDK%/Bin32/glslc.exe normalMapping.vert -o normalMappingVert.spv
%VULKAN_SDK%/Bin32/glslc.exe normalMapping.frag -o normalMappingFrag.spv
%VULKAN_SDK%/Bin32/glslc.exe ui.vert -o uiVert.spv
%VULKAN_SDK%/Bin32/glslc.exe ui.frag -o uiFrag.spv
%VULKAN_SDK%/Bin32/glslc.exe debugNormal.vert -o debugNormalVert.spv
%VULKAN_SDK%/Bin32/glslc.exe debugNormal.frag -o debugNormalFrag.spv
%VULKAN_SDK%/Bin32/glslc.exe debugNormal.geom -o debugNormalGeom.spv


@echo off
xcopy /e /v . ..\..\..\bin\Release-windows-x86_64\Ignite-Sandbox\resources\shaders /Y
xcopy /e /v . ..\..\..\bin\Debug-windows-x86_64\Ignite-Sandbox\resources\shaders /Y
pause