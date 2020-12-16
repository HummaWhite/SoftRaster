# SoftRaster
软光栅渲染器

![](https://raw.staticdn.net/HummaWhite/SoftRaster/master/texture/Test%202020_8_8%2023_08_01.png)

![](https://raw.staticdn.net/HummaWhite/SoftRaster/master/texture/Test%202020_8_8%2022_40_50.png)

### 编译

```
g++ -c main.cpp Camera.cpp stb_image/stb_image.cpp -std=c++17 -g
g++ main.o Camera.o stb_image.o -lgdi32 -lole32 -loleaut32 -luuid -lwinmm -lmsimg32 -o test.exe
```

stb_image库请自行获取并配置

### 实现的功能

+ 向量、矩阵基础运算
+ 简单Obj模型数据读取
+ 可编程管线
+ 顶点处理：VertexShader、CVV完整裁剪
+ 光栅化：正/背面剔除、扫描线（配合Bresenham画线算法确定起止位置）、透视校正插值
+ 片元处理：FragmentShader、深度测试
+ 纹理：近邻与双线性过滤
+ 缓存：双缓冲、写入纹理（纹理即缓存）、模仿OpenGL FBO的FrameBufferAdapter
+ 多线程处理

### Demo

采用PBR光照的犹他壶

### Shader样例

```C++
struct ShaderSample
{
	// VS到FS之间传递的数据类型，目前只能选择手动给每一个数据插值
	struct VSToFS
	{
		VSToFS() {}
		VSToFS(VSToFS& from, VSToFS& to, float weight)
		{
			//裁剪阶段插值
			//通过构造函数生成插值后的实例
			pos = lerp(from.pos, to.pos, weight);
			// ...
		}

		VSToFS(VSToFS& va, VSToFS& vb, VSToFS& vc, Vec3 weight)
		{
			//光栅化阶段的三角形重心插值
			pos = triLerp(va.pos, vb.pos, vc.pos, weight);
			// ...
		}

        // VS到FS之间传递的数据
		Vec3 pos;
		Vec2 texCoord;
		Vec3 norm;
	};

	// 输入VS的数据类型
	struct VSIn
	{
		Vec3 pos;
		Vec2 texCoord;
		Vec3 norm;
	};

	// Vertex Shader
	Pipeline::VSOut<VSToFS> processVertex(VSIn& in)
	{
		Pipeline::VSOut<VSToFS> out;
        
        // 各种计算过程
        // ...

        // 写入数据
		out.data.pos = ...;
		out.data.texCoord = ...;
        // sr_Position类似gl_Position
        out.sr_Position = ...;
        
		return out;
	}

	// Fragment Shader
	void processFragment(FrameBufferAdapter& adapter, Pipeline::FSIn<VSToFS>& in)
	{
		Vec3 result(0.0f);
        
        // in的成员有x、 y（int型片元屏幕坐标）, z、 w（float型片元深度）以及data
        // data即从VS传来的自定义数据（类型为VSToFS）
        result = in.data.norm;
        
        // 采样纹理
        Vec4 texColor = texture(tex, { 0.5f, 0.5f }, LINEAR);
        // ...

        // 向FrameBufferAdapter的0号颜色附件写入结果
		adapter.writeColor(0, result);
        // 向FrameBufferAdapter的深度附件写入结果，如果不些则默认写入管线中的值
		adapter.writeDepth(in.z);
	}

	// uniform变量，可直接从外部设置
	Mat4 model;
	Mat4 view;
	Mat4 proj;
	Vec3 viewPos;
    TextureRGB24 *tex = nullptr;
    // ...
};
```

