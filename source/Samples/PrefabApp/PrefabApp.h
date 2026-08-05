/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __PREFAB_APP_H__
#define __PREFAB_APP_H__


#include "../Common/SampleApp.h"


class PrefabApp : public SampleWindowApp
{
public:
    PrefabApp();
    ~PrefabApp() override;

protected:
    TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;
    void applicationWillTerminate() override;

    bool runPrefabTests();
    void logStep(const char *name, bool ok, const String &detail = "");
    /// 把测试报告落到工作目录，方便无控制台的窗口程序查看结果
    void writeReport();
    /// 打印场景树到日志，并写入 result.txt 报告（label 标明当前步骤）
    void dumpHierarchy(Tiny3D::Scene *scene, const char *label);

    /// 第 5 步 loadScene 成功后，给重载场景补相机 / 灯光 / 方块，方便肉眼核对覆盖位姿
    bool setupPreview(Tiny3D::Scene *scene, Tiny3D::GameObject *prefabRoot,
        const Tiny3D::Vector3 &lookAt);

    void buildCamera(Tiny3D::Transform3D *parent, const Tiny3D::Vector3 &lookAt);
    void buildLights(Tiny3D::Scene *scene, Tiny3D::Transform3D *parent);

    String mWorkDir;
    String mReport;
    int mPassCount {0};
    int mFailCount {0};

    Tiny3D::MeshPtr mPreviewMesh {nullptr};
};


#endif  /*__PREFAB_APP_H__*/
