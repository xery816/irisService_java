# 虹膜识别 Java Demo 运行说明

## 📋 脚本说明

本项目提供了多个 PowerShell 脚本来简化构建和运行流程：

### 🚀 主要脚本

| 脚本文件 | 功能说明 | 使用场景 |
|---------|---------|---------|
| `run.ps1` | **构建 + 运行**（推荐） | 首次运行或代码修改后 |
| `build.ps1` | 仅构建项目 | 只想编译不运行 |
| `run-only.ps1` | 仅运行应用 | 已构建完成，快速启动 |
| `clean.ps1` | 清理构建文件 | 遇到构建问题时清理 |

---

## 🎯 快速开始

### 方法 1：使用脚本运行（推荐）

1. **打开 PowerShell**（以管理员身份运行）

2. **允许脚本执行**（首次需要）：
   ```powershell
   Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
   ```

3. **运行主脚本**：
   ```powershell
   cd "F:\iris_java\Windows SDK_3.23.6_x64_Eng"
   .\run.ps1
   ```

4. **等待构建完成**，应用会自动启动 ✅

---

### 方法 2：在 IntelliJ IDEA 中运行

#### 步骤 1：导入项目
1. 打开 IDEA
2. **File → Open**
3. 选择：`F:\iris_java\Windows SDK_3.23.6_x64_Eng\pidsdk-java-v1.2\pidsdk-java`
4. 等待 Gradle 同步完成

#### 步骤 2：配置运行配置
1. 点击右上角 **"Edit Configurations..."**
2. 点击 **"+"** → 选择 **"Application"**
3. 配置如下：

   ```
   Name: Iris Demo App
   
   Main class: cn.simbok.demo.App
   
   VM options: 
   -Djava.library.path=F:\iris_java\Windows SDK_3.23.6_x64_Eng\win_pidsdk_3.23.6_x64\bin
   
   Working directory: 
   F:\iris_java\Windows SDK_3.23.6_x64_Eng\win_pidsdk_3.23.6_x64\bin
   
   Use classpath of module: 
   SimbokSDKDemo.app.main
   ```

4. 点击 **"Apply"** → **"OK"**
5. 点击绿色运行按钮 ▶️

---

## 📦 环境要求

### 必需组件
- ✅ **JDK 1.8+**（推荐使用带 JavaFX 的 JDK）
  - 推荐：[Amazon Corretto JDK 11](https://aws.amazon.com/corretto/)
  - 或：[Liberica JDK Full](https://bell-sw.com/pages/downloads/)（包含 JavaFX）

- ✅ **虹膜识别硬件设备**
  - 需要连接到计算机
  - 驱动已正确安装

- ✅ **Windows x64 操作系统**

### 可选组件
- IntelliJ IDEA（用于开发）
- Gradle（项目已包含 gradlew）

---

## 🎮 应用操作说明

运行成功后，会看到 JavaFX 图形界面：

### 操作步骤

1. **初始化** 
   - 点击 "初始化" 按钮
   - 等待设备连接成功
   - 预览区域会显示实时图像

2. **注册用户**
   - 在 "用户ID" 输入框输入用户名（如：user001）
   - 点击 "注册" 按钮
   - 按照语音提示调整眼睛位置
   - 等待注册完成

3. **识别用户**
   - 点击 "识别" 按钮
   - 看向摄像头
   - 识别结果会显示在提示区域

4. **管理用户**
   - 点击 "刷新" 按钮查看用户列表
   - 选择用户后点击 "删除" 按钮删除用户

5. **停止操作**
   - 点击 "停止" 按钮停止当前操作

---

## ⚠️ 常见问题

### 问题 1：找不到虹膜设备（错误码 1）
**解决方案**：
- 检查硬件是否正确连接
- 检查设备管理器中设备状态
- 尝试重新插拔 USB 连接

### 问题 2：授权过期（错误码 15）
**解决方案**：
- 联系供应商更新授权
- 检查许可文件是否存在

### 问题 3：找不到 JavaFX 类
**解决方案**：
```powershell
# 安装带 JavaFX 的 JDK
# 或下载 OpenJFX 并配置 --module-path
```

### 问题 4：DLL 加载失败
**解决方案**：
```powershell
# 确保所有 DLL 在 bin 目录下
cd F:\iris_java\Windows SDK_3.23.6_x64_Eng\win_pidsdk_3.23.6_x64\bin
dir *.dll  # 检查 DLL 文件
```

### 问题 5：脚本无法执行
**解决方案**：
```powershell
# 以管理员身份运行 PowerShell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

---

## 📁 项目结构

```
F:\iris_java\Windows SDK_3.23.6_x64_Eng\
├── run.ps1                          # 主运行脚本
├── build.ps1                        # 构建脚本
├── run-only.ps1                     # 快速运行脚本
├── clean.ps1                        # 清理脚本
├── README-运行说明.md               # 本文件
│
├── win_pidsdk_3.23.6_x64\          # C++ SDK
│   └── bin\                         # 运行时库和可执行文件
│       ├── libirisengine.dll        # 主引擎
│       ├── param_common.cfg         # 通用配置
│       └── param_dev.cfg            # 设备配置
│
└── pidsdk-java-v1.2\               # Java 封装
    └── pidsdk-java\
        ├── app\                     # JavaFX 演示应用
        └── libIrisHelper\           # JNI 封装库
```

---

## 🔧 高级配置

### 修改 JNI 库路径

编辑 `pidsdk-java-v1.2\pidsdk-java\app\build.gradle.kts`：

```kotlin
application {
    mainModule.set("cn.simbok.demo")
    mainClass.set("cn.simbok.demo.App")

    // 修改这里的路径
    val jniPath = "你的SDK路径/bin"
    applicationDefaultJvmArgs = arrayListOf(
        "-Djava.library.path=$jniPath"
    )
}
```

### 修改仓库地址

如果无法访问内部仓库，编辑 `gradle.properties`：

```properties
# 使用阿里云镜像
REPOS_SERVER=https://maven.aliyun.com/repository/public
```

---

## 📞 技术支持

- **开发商**：北京思搏克智能科技有限公司
- **SDK 版本**：3.23.6
- **Java SDK 版本**：1.2

---

## 📝 开发参考

详细开发文档请参考：
- `win_pidsdk_3.23.6_x64\doc\IrisEngine Windows&Linux SDK开发指南V1.13.pdf`
- `pidsdk-java-v1.2\pidsdk-java\doc\README-Windows.md`

---

**祝使用愉快！** 🎉

