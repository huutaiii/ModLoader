
extern Addresses : qword

.code

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

end
