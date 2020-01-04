# rt-thread env 修改芯片类型
在拿到到的rtt源码BSP中，很多开发板的env并没有提供选择芯片类型的选项，而是直接固定了芯片型号，比如STM32F103C8，如何将芯片型号更换为RC呢？在实际使用过程中，通过MDK直接修改芯片型号，似乎并不起作用。

所以，我们需要修改下配置文件，过程如下：

 1. 修改board目录下Kconfig脚本文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191212081845267.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3RpZ2Vyb3Rz,size_16,color_FFFFFF,t_70)
 2. 修改board目录下board.h下Flash和RAM大小
![在这里插入图片描述](https://img-blog.csdnimg.cn/2019121208213892.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3RpZ2Vyb3Rz,size_16,color_FFFFFF,t_70)
 3. 修改board目录下SConscript脚本文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191212082413241.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3RpZ2Vyb3Rz,size_16,color_FFFFFF,t_70)
 4. 修改board\linker_scripts目录下的三个链接文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191212082635154.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3RpZ2Vyb3Rz,size_16,color_FFFFFF,t_70)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191212082704347.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3RpZ2Vyb3Rz,size_16,color_FFFFFF,t_70)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191212082821330.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3RpZ2Vyb3Rz,size_16,color_FFFFFF,t_70)
 5. 最后，在修改MDK中的芯片类型就OK了

![在这里插入图片描述](https://img-blog.csdnimg.cn/20191212083112869.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3RpZ2Vyb3Rz,size_16,color_FFFFFF,t_70)

