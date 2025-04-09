# AudioDuckingEnabler 启用音频闪避

这个插件允许在便携版 NVDA 中使用音频闪避功能，以及在系统 UAC 禁用的情况下使用音频闪避功能。

安装插件，重启 NVDA，之后音频闪避功能就可以工作了。

## 系统要求

Windows 版本：Windows 8 及更高版本。

NVDA 版本：2016.1 及更高版本。2016.1 是第一个支持音频闪避功能的 NVDA 版本。

## 为什么默认情况下音频闪避在便携版中不能使用

这是 Windows 施加的限制。

需要使用音频闪避功能的应用程序必须具有 [UIAccess 权限](https://learn.microsoft.com/zh-cn/windows/win32/winauto/uiauto-securityoverview)，这要求应用必须具有数字签名，并且必须被安装在 Program Files 文件夹下。由于只有管理员才能在这种位置安装应用，这个限制可以避免音频闪避之类的功能被应用滥用。

这个插件的原理是修改系统用于验证应用是否具有 UIAccess 权限的函数，并欺骗系统，令其认为 NVDA 总是具有 UIAccess 权限，以此绕开权限检查。这样一来，NVDA 即使没有 UIAccess 权限也能够使用音频闪避功能，例如在使用便携版 NVDA 的情况下。

不过如果微软将来决定“修补”这个漏洞，该插件也会随之失效。
