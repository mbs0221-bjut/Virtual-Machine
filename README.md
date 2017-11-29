# Parser-Asm-VM

编译环境：VS2013，C++11

一个编译程序，一个汇编程序和一个简单的16位虚拟机

设置Parser工程为主项目，在所在目录的Text.txt文件中按照设计的语言写程序，运行工程即可编译该文件为Text.asm文件；
将Text.asm文件拷贝到Asm工程文件夹，在文本最后一行加上#作为结束符
将Asm工程作为主项目，运行工程即可将Text.asm文件汇编成对应虚拟机支持的名为Text.bin的可执行文件
运行开始会输出汇编过程，最后可以看到虚拟机执行过程

都是很简单的虚拟机，支持必要的add, sub, mul, div, mod, load, store, jmp, jne, jge, jbe, jg, jb, halt等指令
并实现了简单的按字、字节访存

TODO：
  在Asm中实现过程调用指令，实现参数传递；
  在Parser中实现带参函数调用，实现Break，Continue指令等
