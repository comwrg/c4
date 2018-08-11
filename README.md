# c4
<table>
   <tr>
      <td> </td>
      <td align="center"> master </td>
      <td align="center"> dev </td>
   </tr>
   <tr>
      <td align="center"> Travis CI </td>
      <td align="center">
         <a href="https://travis-ci.org/comwrg/c4"><img src="https://travis-ci.org/comwrg/c4.svg?branch=master"></a>
      </td>
      <td align="center">
         <a href="https://travis-ci.org/comwrg/c4"><img src="https://travis-ci.org/comwrg/c4.svg?branch=dev"></a>
      </td>
   </tr>
   <tr>
      <td align="center"> Codecov </td>
      <td align="center">
         <a href="https://codecov.io/gh/comwrg/c4"><img src="https://codecov.io/gh/comwrg/c4/branch/master/graph/badge.svg" /></a>
      </td>
      <td align="center">
         <a href="https://codecov.io/gh/comwrg/c4"><img src="https://codecov.io/gh/comwrg/c4/branch/dev/graph/badge.svg" /></a>
      </td>
   </tr>
</table>

参考一下：
* https://github.com/rswier/c4
* http://lotabout.me/2015/write-a-C-interpreter-1/

太无聊了，就想了下实现一下简单的C编译器把

嗯。。硬这头皮跟教程写到一半， 发现完全写不下去， 
1. 不好调试， 教程是一部分一部分讲的， 你要全部看完， 程序才可以运行等起来， 
   有一些运行效果， 所以在写到一半的情况下不清楚自己写的是对是错， 
   等全部写完， 估计修BUG得修几天
2. 代码量大, 功能对于新手来说还是太多， 想一下子全部都实现出来， 难度还是太大

然后我现在想了一个办法， 先实现最最最最简单的功能， 再慢慢加功能。

2018年 8月11日 星期六 09时17分30秒 CST
这代码能把我写晕， debug的想死   
发现一个新坑   
```c
int **p;
/**
*(int *) p + 4 和 
*(int **) p + 4 是不一样的

一个是把地址转成整型加4, 相当于下面加1
一个是把加4个单位, 相当于上面加16

*/
```
## Knowledge of assembly

### eax, ax, ah, al
<table>
   <tr>
      <td> 00000000 </td>
      <td> 00000000 </td>
      <td> 00000000 </td>
      <td> 00000000 </td>
   </tr>
   <tr>
      <td colspan="4" align="center"> eax </td>
   </tr>
   <tr>
      <td colspan="2"> </td>
      <td colspan="2" align="center"> ax </td>
   </tr>
   <tr>
      <td colspan="2"> </td>
      <td align="center"> ah </td>
      <td align="center"> al </td>
   </tr>
</table>

## c -> assembly

### return
```assembly
; return 2333;
movl    $2333, %eax
ret
```

### local variable
```assembly
; int a = 0;
subl	$16, %esp
movl	$0, -4(%ebp)
```
局部变量实现是在栈上分配一个空间   
第一行将`esp`减去16， 即在栈上分配16个字节   
第二行把前4个字节赋值0   

现在出现了一个问题， 为啥分配16个字节， 而不是4个字节， Google发现是*字节对齐*的原因。

* https://www.zhihu.com/question/20871464

