

#ifndef __T3D_ACTION_DATA_H__
#define __T3D_ACTION_DATA_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DObject.h"
#include "T3DVector3.h"
#include "T3DQuaternion.h"


namespace Tiny3D
{
    /**
     * @brief 关键帧数据
     */
    class KeyFrameData : public Object
    {
    public:
        /**
         * @brief 关键帧类型
         */
        enum Type
        {
            E_TYPE_UNKNOWN = 0,
            E_TYPE_TRANSLATION,     /// 平移变换
            E_TYPE_ROTATION,        /// 旋转变换
            E_TYPE_SCALING,         /// 缩放变换
        };

        /**
         * @brief 返回关键帧类型
         * @note 子类实现返回具体类型
         */
        virtual Type getType() const = 0;

    protected:
        /** 构造函数 */
        KeyFrameData(int64_t timestamp);

    private:
        KeyFrameData(const KeyFrameData &);
        KeyFrameData &operator =(const KeyFrameData &);

    public:
        int64_t     mTimestamp;     /// 当前关键帧时间戳
    };

    /**
     * @brief 平移关键帧数据
     */
    class KeyFrameDataT : public KeyFrameData
    {
    public:
        static KeyFrameDataTPtr create(int64_t timestamp, const Vector3 &translation);

        virtual Type getType() const override;

    protected:
        KeyFrameDataT(int64_t timestamp, const Vector3 &translation);

    private:
        KeyFrameDataT(const KeyFrameDataT &);
        KeyFrameDataT &operator =(const KeyFrameDataT &);

    public:
        Vector3     mTranslation;
    };

    /**
     * @brief 旋转关键帧数据
     */
    class KeyFrameDataR : public KeyFrameData
    {
    public:
        static KeyFrameDataRPtr create(int64_t timestamp, const Quaternion &orientation);

        virtual Type getType() const override;

    protected:
        KeyFrameDataR(int64_t timestamp, const Quaternion &orientation);

    private:
        KeyFrameDataR(const KeyFrameDataR &);
        KeyFrameDataR &operator =(const KeyFrameDataR &);

    public:
        Quaternion  mOrientation;
    };

    /**
     * @brief 缩放关键帧数据
     */
    class KeyFrameDataS : public KeyFrameData
    {
    public:
        static KeyFrameDataSPtr create(int64_t timestamp, const Vector3 &scaling);

        virtual Type getType() const override;

    protected:
        KeyFrameDataS(int64_t timestamp, const Vector3 &scaling);

    private:
        KeyFrameDataS(const KeyFrameDataS &);
        KeyFrameDataS &operator =(const KeyFrameDataS &);

    public:
        Vector3     mScaling;
    };

    /**
     * @brief 动作数据
     */
    class ActionData : public Object
    {
    public:
        static ActionDataPtr create(const String &name, int32_t duration);

        virtual ~ActionData();

    protected:
        ActionData(const String &name, int32_t duration);

    public:
        typedef std::vector<KeyFrameDataPtr>    KeyFrames;
        typedef KeyFrames::iterator             KeyFramesItr;
        typedef KeyFrames::const_iterator       KeyFramesConstItr;

        typedef std::map<String, KeyFrames>     Bones;
        typedef Bones::iterator                 BonesItr;
        typedef Bones::const_iterator           BonesConstItr;
        typedef std::pair<String, KeyFrames>    BonesValue;

        String      mName;                  /// 动作名称
        int32_t     mDuration;              /// 动作时长，单位：毫秒
        Bones       mBonesTranslation;      /// 动作相关骨骼的平移变换关键帧数据
        Bones       mBonesRotation;         /// 动作相关骨骼的旋转变换关键帧数据
        Bones       mBonesScaling;          /// 动作相关骨骼的缩放变换关键帧数据
    };
}


#endif  /*__T3D_ACTION_DATA_H__*/
