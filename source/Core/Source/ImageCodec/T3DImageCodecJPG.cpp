// 
// 
// #include "ImageCodec/T3DImageCodecJPG.h"
// 
// 
// namespace Tiny3D
// {
//     ImageCodecJPGPtr ImageCodecJPG::create()
//     {
//         ImageCodecJPGPtr codec = new ImageCodecJPG();
//         codec->release();
//         return codec;
//     }
// 
//     ImageCodecJPG::ImageCodecJPG()
//     {
// 
//     }
// 
//     ImageCodecJPG::~ImageCodecJPG()
//     {
// 
//     }
// 
//     ImageCodecBase::FileType ImageCodecJPG::getFileType() const
//     {
//         return E_TYPE_JPG;
//     }
// 
//     bool ImageCodecJPG::encode(const String &name, const Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecJPG::encode(DataStream &stream, const Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecJPG::encode(uint8_t *&data, size_t &size, const Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecJPG::decode(const String &name, Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecJPG::decode(DataStream &stream, Image &image)
//     {
//         return true;
//     }
// 
//     bool ImageCodecJPG::decode(const uint8_t *data, size_t size, Image &image)
//     {
//         return true;
//     }
// }
