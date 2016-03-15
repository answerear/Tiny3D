

#include <stdio.h>
#include "mconv_converter.h"


int main(int argc, char *argv[])
{
    Tiny3D::System *pSystem = new Tiny3D::System();

    Tiny3D::Logger *pLogger = new Tiny3D::Logger();
    T3D_LOG_STARTUP(1001, "MeshConverter", true, false);

    mconv::Converter *pConverter = new mconv::Converter();
    bool result = pConverter->execute(argc, argv);
    delete pConverter;

    T3D_LOG_SHUTDOWN();
    delete pLogger;

    delete pSystem;

	return (result ? 0 : -1);
}
