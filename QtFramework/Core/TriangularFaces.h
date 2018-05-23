#pragma once

#include "Exceptions.h"
#include <GL/glew.h>
#include <iostream>
#include <vector>

namespace cagd {
class TriangularFace
{
protected:
    GLuint _node[3];

public:
    // default constructor
    TriangularFace();

    // Not needed... default is enough...
    //        // homework: copy constructor
    //        TriangularFace(const TriangularFace& face);

    //        // homework: assignment operator
    //        TriangularFace& operator =(const TriangularFace& rhs);

    // homework: get node identifiers by value
    inline GLuint operator[](GLuint i) const;

    // homework: get node identifiers by reference
    inline GLuint &operator[](GLuint i);
};

// default constructor
inline TriangularFace::TriangularFace() { _node[0] = _node[1] = _node[2] = 0; }



// homework: get node identifiers by value
GLuint TriangularFace::operator[](GLuint i) const { return _node[i]; }

// homework: get node identifiers by reference
GLuint &TriangularFace::operator[](GLuint i) { return _node[i]; }


// output to stream
inline std::ostream &operator<<(std::ostream &lhs, const TriangularFace &rhs)
{
    lhs << 3;
    for (GLuint i = 0; i < 3; ++i)
        lhs << " " << rhs[i];
    return lhs;
}

// homework
inline std::istream &operator>>(std::istream &lhs, TriangularFace &rhs)
{
    unsigned vcount;
    lhs >> vcount;
    if (vcount != 3) {
        throw Exception("Just triangulated meshes, please!");
    }
    lhs >> rhs[0] >> rhs[1] >> rhs[2];
    return lhs;
}
} // namespace cagd
