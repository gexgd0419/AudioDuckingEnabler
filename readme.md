# AudioDuckingEnabler

An NVDA add-on that makes it possible to use audio ducking when using a portable copy of NVDA or when UAC is disabled.

Install this add-on, restart NVDA, and audio ducking will work.

## System requirements

Windows version: Windows 8 or later.

NVDA version: 2016.1 or later. 2016.1 is the first NVDA version that supports audio ducking.

## Why audio ducking is not supported in portable copies by default

This is a limitation imposed by Windows.

Applications that want to use the audio ducking feature must have [UIAccess privilege](https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-securityoverview), which requires them to be digitally signed and be installed in the Program Files folder. As only administrators can install applications in such locations, this prevents features such as audio ducking from being abused.

What this add-on does is to modify the function that Windows uses to verify if the application has UIAccess privilege, and lie to the system that NVDA always has UIAccess privilege, bypassing the privilege check. Therefore, NVDA will be able to use the audio ducking feature even when without UIAccess privilege, for example, when running as a portable copy.

However, if Microsoft decides to "fix" this in the future, this add-on may stop working.
