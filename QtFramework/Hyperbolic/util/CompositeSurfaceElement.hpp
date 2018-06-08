#pragma once

#include <array>
#include <memory>

#include "../../Core/Materials.h"
#include "../../Core/ShaderPrograms.h"
#include "../../Core/TriangulatedMeshes3.h"
#include "../SecondOrderHyperbolicPatch.h"


namespace cagd {

class CompositeSurfaceElement
{
public:
    typedef unsigned SurfaceId;

    friend class CompositeSurfaceProvider;

    enum Direction
    {
        NORTH = 0,
        NORTH_EAST,
        EAST,
        SOUTH_EAST,
        SOUTH,
        SOUTH_WEST,
        WEST,
        NORTH_WEST,
        DIR_COUNT
    };

    typedef void(evaluator)(std::unique_ptr<SecondOrderHyperbolicPatch> &,
                            GLuint, GLuint, GLuint, GLuint,
                            std::unique_ptr<SecondOrderHyperbolicPatch> &,
                            GLuint, GLuint, GLuint, GLuint);


    static double default_tension;

private:
    unsigned _use_count;

    std::array<CompositeSurfaceElement *, DIR_COUNT> _neighbors;
    std::array<Direction, DIR_COUNT>                 _neighbor_back_references;
    std::unique_ptr<SecondOrderHyperbolicPatch>      _own_surface_ptr;

    std::unique_ptr<TriangulatedMesh3>          _surf_image;
    std::vector<std::unique_ptr<GenericCurve3>> _u_parametric_lines;
    std::vector<std::unique_ptr<GenericCurve3>> _v_parametric_lines;
    bool                                        _update_needed;

    std::shared_ptr<ShaderProgram> _shader;
    Material                       _material;

    GLfloat _wireframe_red_component;
    GLfloat _wireframe_green_component;
    GLfloat _wireframe_blue_component;



    void forceBorderCondition(Direction, evaluator eval);

public:
    CompositeSurfaceElement();
    CompositeSurfaceElement(SecondOrderHyperbolicPatch *);
    ~CompositeSurfaceElement();

    CompositeSurfaceElement(const CompositeSurfaceElement &) = delete;
    CompositeSurfaceElement &operator=(CompositeSurfaceElement &) = delete;

    CompositeSurfaceElement(CompositeSurfaceElement &&);
    CompositeSurfaceElement &operator=(CompositeSurfaceElement &&);



    // Join methods:
    void joinWith(Direction, Direction, CompositeSurfaceElement *);
    void splitFrom(Direction);

    // Merge methods:
    void mergeWith(Direction, Direction, CompositeSurfaceElement *);

    // Continue:
    void continuePatch(CompositeSurfaceElement *, Direction, Direction);

    // Both:
    void forceConditions();

    // Test methods:
    bool isNeighbor(const CompositeSurfaceElement &other,
                    Direction                      directionThis) const;


    // Render methods:
    bool updateVBOs(GLuint, GLuint, bool);
    void renderMesh(GLenum);
    void renderWireframe(GLenum) const;
    void renderControlPoints(std::shared_ptr<TriangulatedMesh3> &) const;
    void renderControlPoints(std::shared_ptr<TriangulatedMesh3> &,
                             GLuint) const;
    void renderUVParametricLines() const;
    void renderNormals() const;
    void renderTexture() const;

    // Utility methods:
    SecondOrderHyperbolicPatch *releaseOwnSurface();

    GLdouble getAlphaTension() const;


    friend void swap(CompositeSurfaceElement &, CompositeSurfaceElement &);
};

} // namespace cagd
