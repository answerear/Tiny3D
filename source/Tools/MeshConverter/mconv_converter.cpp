

#include "mconv_converter.h"
#include "mconv_command.h"
#include "mconv_settings.h"
#include "mconv_fbxconverter.h"
#include "mconv_ogreconverter.h"
#include "mconv_serializer.h"


namespace mconv
{
    Converter::Converter()
        : mConverter(nullptr)
    {

    }

    Converter::~Converter()
    {
        delete mConverter;
        mConverter = nullptr;
    }

    bool Converter::execute(int argc, char *argv[])
    {
        bool result = false;

        Settings settings;
        Command command;
        result = command.parse(argc, argv, settings);

        ConverterImpl *converter = nullptr;

        if (result)
        {
            switch (settings.mSrcType)
            {
            case E_FILETYPE_FBX:
            case E_FILETYPE_DAE:
                {
                    converter = new FBXConverter(settings);
                }
                break;
            case E_FILETYPE_OGRE:
                {
                    converter = new OgreConverter(settings);
                }
                break;
            }
        }

        result = result && converter->convert();

        delete converter;
        converter = nullptr;

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////

    ConverterImpl::ConverterImpl(const Settings &settings)
        : mSettings(settings)
        , mImporter(nullptr)
        , mExporter(nullptr)
    {

    }

    ConverterImpl::~ConverterImpl()
    {
        delete mImporter;
        mImporter = nullptr;

        delete mExporter;
        mExporter = nullptr;
    }

    bool ConverterImpl::convert()
    {
        bool ret = true;

        ret = ret && importScene();
        ret = ret && convertToT3D();
        ret = ret && exportScene();

        cleanup();

        return ret;
    }
}
