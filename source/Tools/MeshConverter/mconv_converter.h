
#ifndef __MCONV_CONVERTER_H__
#define __MCONV_CONVERTER_H__


#include "mconv_prerequisites.h"
#include "mconv_settings.h"


namespace mconv
{
    class ConverterImpl;
    class Serializer;

    class Converter
    {
    public:
        Converter();
        virtual ~Converter();

        bool execute(int argc, char *argv[]);

    protected:
        ConverterImpl   *mConverter;
    };

    class ConverterImpl
    {
    public:
        ConverterImpl(const Settings &settings);
        virtual ~ConverterImpl();

        virtual bool convert();

    protected:
        virtual bool importScene() = 0;
        virtual bool convertToT3D() = 0;
        virtual bool exportScene() = 0;
        virtual void cleanup() = 0;

    protected:
        const Settings    &mSettings;

        Serializer  *mImporter;
        Serializer  *mExporter;
    };
}


#endif  /*__MCONV_CONVERTER_H__*/
