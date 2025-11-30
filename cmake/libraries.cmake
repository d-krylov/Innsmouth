include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.1
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

FetchContent_Declare(
	fastgltf
	GIT_REPOSITORY https://github.com/spnda/fastgltf
	GIT_TAG main
)

set(FASTGLTF_ENABLE_DEPRECATED_EXT ON)