
extern Addresses : qword

.code

	; dinput8
	hk_DirectInput8Create proc
		jmp [Addresses + 0]
	hk_DirectInput8Create endp

	hk_DllCanUnloadNow proc
		jmp [Addresses + 8]
	hk_DllCanUnloadNow endp

	hk_DllGetClassObject proc
		jmp [Addresses + 16]
	hk_DllGetClassObject endp

	hk_DllRegisterServer proc
		jmp [Addresses + 24]
	hk_DllRegisterServer endp

	hk_DllUnregisterServer proc
		jmp [Addresses + 32]
	hk_DllUnregisterServer endp

	hk_GetdfDIJoystick proc
		jmp [Addresses + 40]
	hk_GetdfDIJoystick endp


	;version
	hk_GetFileVersionInfoA proc
		jmp [Addresses]
	hk_GetFileVersionInfoA endp

	hk_GetFileVersionInfoByHandle proc
		jmp [Addresses + 8]
	hk_GetFileVersionInfoByHandle endp

	hk_GetFileVersionInfoExA proc
		jmp [Addresses + 16]
	hk_GetFileVersionInfoExA endp

	hk_GetFileVersionInfoExW proc
		jmp [Addresses + 24]
	hk_GetFileVersionInfoExW endp

	hk_GetFileVersionInfoSizeA proc
		jmp [Addresses + 32]
	hk_GetFileVersionInfoSizeA endp

	hk_GetFileVersionInfoSizeExA proc
		jmp [Addresses + 40]
	hk_GetFileVersionInfoSizeExA endp

	hk_GetFileVersionInfoSizeExW proc
		jmp [Addresses + 48]
	hk_GetFileVersionInfoSizeExW endp

	hk_GetFileVersionInfoSizeW proc
		jmp [Addresses + 56]
	hk_GetFileVersionInfoSizeW endp

	hk_GetFileVersionInfoW proc
		jmp [Addresses + 64]
	hk_GetFileVersionInfoW endp

	hk_VerFindFileA proc
		jmp [Addresses + 72]
	hk_VerFindFileA endp

	hk_VerFindFileW proc
		jmp [Addresses + 80]
	hk_VerFindFileW endp

	hk_VerInstallFileA proc
		jmp [Addresses + 88]
	hk_VerInstallFileA endp

	hk_VerInstallFileW proc
		jmp [Addresses + 96]
	hk_VerInstallFileW endp

	hk_VerLanguageNameA proc
		jmp [Addresses + 104]
	hk_VerLanguageNameA endp

	hk_VerLanguageNameW proc
		jmp [Addresses + 112]
	hk_VerLanguageNameW endp

	hk_VerQueryValueA proc
		jmp [Addresses + 120]
	hk_VerQueryValueA endp

	hk_VerQueryValueW proc
		jmp [Addresses + 128]
	hk_VerQueryValueW endp


	;xinput9_1_0
	hk_DllMain proc
		jmp [Addresses]
	hk_DllMain endp

	hk_XInputGetCapabilities proc
		jmp [Addresses + 8]
	hk_XInputGetCapabilities endp

	hk_XInputGetDSoundAudioDeviceGuids proc
		jmp [Addresses + 16]
	hk_XInputGetDSoundAudioDeviceGuids endp

	hk_XInputGetState proc
		jmp [Addresses + 24]
	hk_XInputGetState endp

	hk_XInputSetState proc
		jmp [Addresses + 32]
	hk_XInputSetState endp


end
