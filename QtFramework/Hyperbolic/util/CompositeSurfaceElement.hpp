#pragma once

#include <array>
#include <memory>

#include "../../Core/TriangulatedMeshes3.h"
#include "../SecondOrderHyperbolicPatch.h"


namespace cagd {

class CompositeSurfaceElement
{
public:
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


private:
    static double default_tension;

    unsigned _use_count;

    std::array<CompositeSurfaceElement *, DIR_COUNT> _neighbors;
    std::array<Direction, DIR_COUNT>                 _neighbor_back_references;
    std::unique_ptr<SecondOrderHyperbolicPatch>      _own_surface_ptr;

    std::unique_ptr<TriangulatedMesh3> _surf_image;



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
    CompositeSurfaceElement mergeWith(Direction, Direction,
                                      CompositeSurfaceElement *);

    // Test methods:
    bool isNeighbor(const CompositeSurfaceElement &other,
                    Direction                      directionThis) const;


    // Render methods:
    bool updateVBOs(GLuint, GLuint);
    void renderMesh() const;

    // Utility methods:
    SecondOrderHyperbolicPatch *releaseOwnSurface();

    GLdouble getAlphaTension() const;


    friend void swap(CompositeSurfaceElement &, CompositeSurfaceElement &);
};

} // namespace cagd
