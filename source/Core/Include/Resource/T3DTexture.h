

#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "Misc/T3DCommon.h"
#include "T3DRect.h"
#include "T3DResource.h"


namespace Tiny3D
{
    /**
     * @brief 纹理资源
     */
    class T3D_ENGINE_API Texture : public Resource
    {
    public:
        enum TexType
        {
            E_TEX_TYPE_1D = 1,
            E_TEX_TYPE_2D,
            E_TEX_TYPE_3D,
            E_TEX_TYPE_CUBE_MAP,
        };

        enum TexUsage
        {
            E_TU_DEFAULT = 0,   /// 从图片加载数据填充纹理
            E_TU_RENDERTARGET,  /// 渲染到纹理
            E_TU_BLANK,         /// 空白纹理
        };

        virtual ~Texture();

        virtual Type getType() const override;

        TexType getTextureType() const  { return mTexType; }
        TexUsage getTextureUsage() const    { return mTexUsage; }

        int32_t getNumMipMaps() const   { return mNumMipMaps; }

        int32_t getTexWidth() const        { return mTexWidth; }
        int32_t getTexHeight() const       { return mTexHeight; }

        int32_t getOriginalWidth() const    { return mImgWidth; }
        int32_t getOriginalHeight() const   { return mImgHeight; }

        PixelFormat getFormat() const   { return mFormat; }

        bool hasAlpha() const           { return mHasAlpha; }

        virtual bool copyToTexture(const TexturePtr &texture, Rect *src = nullptr, Rect *dst = nullptr) const = 0;

        virtual bool loadImage(const Image &src) = 0;

        /**
         * @brief 写数据到纹理
         * @param [in] data : 指向数据缓冲区地址的指针
         * @param [in] size : 数据缓冲大小
         * @param [in] dst : 写入纹理的目标区域，默认为nullptr表示整个纹理，并且src和dst是一样大小的
         * @param [in] src : 源数据区域，默认为nullptr表示整个纹理，并且src要和dst区域一样大或者要比dst区域小
         * @return 写入成功返回写入的字节数，否则返回0
         * @note 本接口无法校验源数据格式，所以写入的数据格式必须跟纹理格式相匹配，否则会出不可预知错误
         */
        virtual size_t writeData(uint8_t *data, size_t size, Rect *dst = nullptr, Rect *src = nullptr) = 0;

        /**
         * @brief 从纹理读取数据
         * @param [in] data : 指向接收获取数据缓冲区地址的指针
         * @param [in] size : 数据缓冲区大小
         * @param [in] dst : 读取出来填充的区域，默认为nullptr表示整个纹理，并且src和dst是一样大小的
         * @param [in] src : 源纹理要读取的区域，默认为nullptr表示整个纹理，并且dst要和src区域一样大或者要比src区域小
         * @return 读取成功返回要读取的字节数，否则返回0
         * @note 
         *  - 本接无法确定目标数据格式，所以读取到的数据格式跟源纹理格式是相同的
         *  - 本接口不负责内存空间分配，请调用者自行分配好data的空间来获取读取到的数据
         */
        virtual size_t readData(uint8_t *data, size_t size, Rect *dst = nullptr, Rect *src = nullptr) = 0;

    protected:
        Texture(const String &name, int32_t numMipMaps, int32_t texWidth = -1, int32_t texHeight = -1, 
            TexUsage texUsage = E_TU_DEFAULT, TexType texType = E_TEX_TYPE_2D, 
            PixelFormat format = E_PF_A8R8G8B8);

        TexType     mTexType;
        TexUsage    mTexUsage;
        int32_t     mNumMipMaps;

        int32_t     mTexWidth;
        int32_t     mTexHeight;

        int32_t     mImgWidth;
        int32_t     mImgHeight;

        PixelFormat mFormat;

        bool        mHasAlpha;
    };
}


#endif  /*__T3D_TEXTURE_H__*/
