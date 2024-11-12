#include "mesh/include/primitives.h"

namespace Innsmouth {

std::array<Vertex, 36> MakeBox() {
  return std::array<Vertex, 36>{
    // Right face
    Vertex{{+0.5f, +0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}, {1.0f, 0.0f}}, // top-left
    Vertex{{+0.5f, -0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}, {0.0f, 1.0f}}, // bottom-right
    Vertex{{+0.5f, +0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}, {1.0f, 1.0f}}, // top-right
    Vertex{{+0.5f, -0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}, {0.0f, 1.0f}}, // bottom-right
    Vertex{{+0.5f, +0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}, {1.0f, 0.0f}}, // top-left
    Vertex{{+0.5f, -0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}, {0.0f, 0.0f}}, // bottom-left
    // Left face
    Vertex{{-0.5f, +0.5f, +0.5f}, {-1.0f, +0.0f, +0.0f}, {1.0f, 0.0f}}, // top-right
    Vertex{{-0.5f, +0.5f, -0.5f}, {-1.0f, +0.0f, +0.0f}, {1.0f, 1.0f}}, // top-left
    Vertex{{-0.5f, -0.5f, -0.5f}, {-1.0f, +0.0f, +0.0f}, {0.0f, 1.0f}}, // bottom-left
    Vertex{{-0.5f, -0.5f, -0.5f}, {-1.0f, +0.0f, +0.0f}, {0.0f, 1.0f}}, // bottom-left
    Vertex{{-0.5f, -0.5f, +0.5f}, {-1.0f, +0.0f, +0.0f}, {0.0f, 0.0f}}, // bottom-right
    Vertex{{-0.5f, +0.5f, +0.5f}, {-1.0f, +0.0f, +0.0f}, {1.0f, 0.0f}}, // top-right
    // Top face
    Vertex{{-0.5f, +0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}, {0.0f, 1.0f}}, // top-left
    Vertex{{+0.5f, +0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}, {1.0f, 0.0f}}, // bottom-right
    Vertex{{+0.5f, +0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}, {1.0f, 1.0f}}, // top-right
    Vertex{{+0.5f, +0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}, {1.0f, 0.0f}}, // bottom-right
    Vertex{{-0.5f, +0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}, {0.0f, 1.0f}}, // top-left
    Vertex{{-0.5f, +0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}, {0.0f, 0.0f}}, // bottom-left
    // Bottom face
    Vertex{{-0.5f, -0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}, {0.0f, 1.0f}}, // top-right
    Vertex{{+0.5f, -0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}, {1.0f, 1.0f}}, // top-left
    Vertex{{+0.5f, -0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}, {1.0f, 0.0f}}, // bottom-left
    Vertex{{+0.5f, -0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}, {1.0f, 0.0f}}, // bottom-left
    Vertex{{-0.5f, -0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}, {0.0f, 0.0f}}, // bottom-right
    Vertex{{-0.5f, -0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}, {0.0f, 1.0f}}, // top-right
    // Front face
    Vertex{{-0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}, {0.0f, 0.0f}}, // bottom-left
    Vertex{{+0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}, {1.0f, 0.0f}}, // bottom-right
    Vertex{{+0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}, {1.0f, 1.0f}}, // top-right
    Vertex{{+0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}, {1.0f, 1.0f}}, // top-right
    Vertex{{-0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}, {0.0f, 1.0f}}, // top-left
    Vertex{{-0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}, {0.0f, 0.0f}}, // bottom-left
    // Back face
    Vertex{{-0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}, {0.0f, 0.0f}},  // Bottom-left
    Vertex{{+0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}, {1.0f, 1.0f}},  // top-right
    Vertex{{+0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}, {1.0f, 0.0f}},  // bottom-right
    Vertex{{+0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}, {1.0f, 1.0f}},  // top-right
    Vertex{{-0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}, {0.0f, 0.0f}},  // bottom-left
    Vertex{{-0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}, {0.0f, 1.0f}}}; // top-left
}

} // namespace Innsmouth