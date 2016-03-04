

#ifndef __MESH_CONVERTER_H__
#define __MESH_CONVERTER_H__


#include <Tiny3D.h>


class MeshConverter
{
public:
    MeshConverter();
    virtual ~MeshConverter();

    bool execute(int argc, const char **argv);

protected:
    bool print();
};


#endif  /*__MESH_CONVERTER_H__*/
