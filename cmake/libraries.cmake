include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.1
)

FetchContent_Declare(
	volk
	GIT_REPOSITORY https://github.com/zeux/volk.git
	GIT_TAG 1.4.304
)

FetchContent_Declare(
	spirv_reflect
	GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
	GIT_TAG vulkan-sdk-1.3.290.0
)

FetchContent_Declare(
	vulkan_memory_allocator
	GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
	GIT_TAG v3.2.0
)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG v1.91.7
)

FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb
	GIT_TAG master
)

FetchContent_Declare(
	tinyobjloader
	GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
	GIT_TAG release
)