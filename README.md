# Parser-Asm-VM

编译环境：VS2013，C++11

简单的实现了一个编译程序，一个汇编程序和一个简单的16位虚拟机
一条龙实现从源代码到汇编程序再到虚拟机机器指令程序的转换过程
并可以在该虚拟机上运行该虚拟机机器指令程序
由于语法分析程序的Bug，源文件和汇编文件最后都需要使用‘#’结束，才能顺利的完成编译和汇编的过程

将Parser设置为运行项目
在资源文件目录下的Text.txt和data.txt分别为源文件和编译后的汇编代码
运行Parser，就可以将Text.txt编译输出汇编代码到data.asm

该Parser程序支持if,if-else,for,while,do-while,assign语句和一些简单的算术运算语句

将Asm设置为运行项目
在资源文件目录下的data.asm和data.bin分别为汇编代码和目标代码
将Parser生成的data.asm文件中的汇编代码复制到该项目下的data.asm文件里面，并以'#'结束
运行Asm，就可以将data.asm汇编输出虚拟机指令代码到data.bin

该Asm程序支持load,store,push,pop,je,jne,jb,jg,jmp,halt和一些简单的算术运算指令，支持立即数和直接寻址，支持字和字节的访问。
VM指令集的设计主要参考了RISC，有256个8位寄存器，一个16位地址的RAM内存空间。

VM项目是早期的虚拟机实现，最新的VM程序已经和Asm项目合并在一起
