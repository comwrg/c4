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

## 四则运算分析
```
gcc -m32 -O0 -S demo.c
```
---
```c
#include <stdio.h>

int main() {
    int a = 1, b = 2, c = 3, d;
    d = a + b + c;
    printf("%d\n", d);
    return 2333;
}
```
```assembly
_main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp       ; allocate space to local variables
	movl	$1, -12(%ebp)   ; a = 1
	movl	$2, -16(%ebp)   ; b = 2
	movl	$3, -20(%ebp)   ; c = 3
	movl	-12(%ebp), %ecx ; ecx = a
	movl	-16(%ebp), %edx ; edx = b
	addl	%edx, %ecx      ; ecx += edx
	movl	-20(%ebp), %edx ; edx = c
	addl	%ecx, %edx      ; edx += ecx
	movl	%edx, -24(%ebp) ; d = edx
	subl	$8, %esp
	pushl	-24(%ebp)
	leal	lC0-L1$pb(%eax), %eax
	pushl	%eax
	call	_printf
	addl	$16, %esp
	movl	$2333, %eax
	leave
	ret
```
---
```c
#include <stdio.h>

int main() {
    int a = 1, b = 2, c = 3, d;
    d = a + b * c;
    printf("%d\n", d);
    return 2333;
}
```
```assembly
_main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$1, -12(%ebp)   ; a = 1
	movl	$2, -16(%ebp)   ; b = 2
	movl	$3, -20(%ebp)   ; c = 3
	movl	-16(%ebp), %edx ; edx = b
	imull	-20(%ebp), %edx ; edx *= c
	movl	-12(%ebp), %ecx ; ecx = a
	addl	%ecx, %edx      ; edx += ecx
	movl	%edx, -24(%ebp)
	subl	$8, %esp
	pushl	-24(%ebp)
	leal	lC0-L1$pb(%eax), %eax
	pushl	%eax
	call	_printf
	addl	$16, %esp
	movl	$2333, %eax
	leave
	ret
```
总结起来就是先算运算符级别高的那个， 然后继续往下算

---
```c
#include <stdio.h>

int main() {
    int a = 1,
        b = 2,
        c = 3,
        d = 4,
        e = 5,
        f = 6,
        g    ;


    g = a*b + c*d + e*f;
    printf("%d\n", g);
    return 2333;
}
```
```assembly
_main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$1, -12(%ebp)   ; a
	movl	$2, -16(%ebp)   ; b
	movl	$3, -20(%ebp)   ; c
	movl	$4, -24(%ebp)   ; d
	movl	$5, -28(%ebp)   ; e
	movl	$6, -32(%ebp)   ; f
	movl	-12(%ebp), %edx ; edx = a
	movl	%edx, %ecx      ; ecx = edx
	imull	-16(%ebp), %ecx ; ecx *= b, ecx = a*b now
	movl	-20(%ebp), %edx ; edx = c
	imull	-24(%ebp), %edx ; edx *= d, edx = c*d now
	addl	%edx, %ecx      ; ecx += edx, ecx = a*b + c*d now
	movl	-28(%ebp), %edx ; edx = e
	imull	-32(%ebp), %edx ; edx *= f
	addl	%ecx, %edx      ; edx += ecx
	movl	%edx, -36(%ebp)
	subl	$8, %esp
	pushl	-36(%ebp)
	leal	lC0-L1$pb(%eax), %eax
	pushl	%eax
	call	_printf
	addl	$16, %esp
	movl	$2333, %eax
	leave
```
然而还是不知道咋写。。
