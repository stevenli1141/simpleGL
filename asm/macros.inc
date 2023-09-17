    .macro export label:req
    .global \label
\label:
    .endm

    .macro exportbin file:req label:req
    .global \label
\label:
    .incbin "\file"
    .align 8
    .endm

    .macro setvar var:req val:req
    .byte 0x1, \var
    .word \val
    .align 4
    .endm

    .macro setsysvar var:req val:req
    .byte 0x2, \var
    .align 4
    .long \val
    .endm

    .macro loadvar var:req sysvar:req
    .byte 0x3, \var
    .word \sysvar
    .align 4
    .endm

    .macro add var1:req var2:req var3:req
    .byte 0x4, \var1, \var2, \var3
    .endm

    .macro sub var1:req var2:req var3:req
    .byte 0x5, \var1, \var2, \var3
    .endm

    .macro mult var1:req var2:req var3:req
    .byte 0x6, \var1, \var2, \var3
    .endm

    .macro div var1:req var2:req var3:req
    .byte 0x7, \var1, \var2, \var3
    .endm

    .macro mod var1:req var2:req var3:req
    .byte 0x8, \var1, \var2, \var3
    .endm

    .macro cmp var1:req var2:req var3:req
    .byte 0x9, \var1, \var2, \var3
    .endm

    .macro setflag flag:req val:req
    .byte 0xA, \val
    .word \flag
    .endm

    .macro goto label:req start:req
    .byte 0xB
    .align 4
    .long \label - \start
    .align 4
    .endm

    .macro goto_if_eq label:req var:req val:req start:req
    .byte 0xC, \var
    .word \val
    .align 4
    .long \label - \start
    .align 4
    .endm

    .macro goto_if_flag_eq label:req flag:req val:req start:req
    .byte 0xD, \val
    .word \flag
    .long \label - \start
    .endm

    .macro return
    .byte 0xE
    .align 4
    .endm

    .macro start script:req arg:req scriptstart:req
    .byte 0xF, \arg
    .align 4
    .long \script - \scriptstart
    .endm

    .macro waitfor var:req val:req
    .byte 0x10, \var
    .align 4
    .long \val
    .endm

    .macro yield
    .byte 0x11
    .align 4
    .endm

    .macro lock
    .byte 0x12, 1
    .align 4
    .endm

    .macro unlock
    .byte 0x12, 0
    .align 4
    .endm

    .macro special cmd:req
    .byte 0x13
    .align 4
    .endm

    .macro loadscene scene:req
    .byte 0x14
    .align 2
    .word \scene
    .align 4
    .endm

    .macro moveuserto rate:req x:req y:req z:req theta:req mask:req
    .byte 0x15, \rate, \mask
    .align 4
    .float \x
    .float \y
    .float \z
    .float \theta
    .endm

    .macro setusertransform x:req y:req z:req theta:req mask:req
    .byte 0x16, \mask
    .align 4
    .float \x
    .float \y
    .float \z
    .float \theta
    .endm

    .macro setuservalue prop:req val:req
    .byte 0x17, \prop
    .word \val
    .align 4
    .endm

    .macro moveentityto_with_mask entity:req rate:req x:req y:req z:req theta:req mask:req
    .byte 0x18, \entity, \rate, \mask
    .align 4
    .float \x
    .float \y
    .float \z
    .float \theta
    .endm

    .macro moveentityto entity:req rate:req x:req y:req z:req theta:req
    moveentityto_with_mask \entity, \rate, \x, \y, \z, \theta, TRANSFORM_XYZ_THETA
    .endm

    .macro setentitytransform entity:req x:req y:req z:req theta:req
    .byte 0x19, \entity
    .align 4
    .float \x
    .float \y
    .float \z
    .float \theta
    .endm

    .macro setentityvalue entity:req prop:req val:req
    .byte 0x1B, \entity, \prop, \val
    .align 4
    .endm

    .macro movecamerato rate:req x:req y:req z:req theta:req phi:req
    .byte 0x1C, \rate
    .align 4
    .float \x
    .float \y
    .float \z
    .float \theta
    .float \phi
    .endm

    .macro setcameratransform x:req y:req z:req theta:req phi:req
    .byte 0x1D
    .align 4
    .float \x
    .float \y
    .float \z
    .float \theta
    .float \phi
    .endm

    .macro setfade var:req color:req period:req
    .byte 0x21, \var, \color, \period
    .align 4
    .endm

    .macro fadeto var:req color:req
    setfade \var, \color, 0
    .endm

    .macro removefade
    fadeto 0, 0
    .endm

    .macro prepare_text text:req format:req start:req
    .byte 0x25, \format
    .align 4
    .long \text - \start
    .endm

    .macro prepare_shared_text text:req
    .byte 0x26, \text
    .align 4
    .endm

    .macro set_subobject_transform entity:req subindex:req x:req y:req z:req theta:req phi:req
    .byte 0x2A, \entity, \subindex
    .align 4
    .float \x
    .float \y
    .float \z
    .float \theta
    .float \phi
    .endm

    .macro move_subobject_to entity:req subindex:req rate:req x:req y:req z:req theta:req phi:req
    .byte 0x2B, \rate, \entity, \subindex
    .float \x
    .float \y
    .float \z
    .float \theta
    .float \phi
    .endm

    .macro random var:req range:req
    .byte 0x31, \var, \range
    .align 4
    .endm

# More complex commonly used macros

    .macro fade_black_load_map map:req x: req y:req z:req theta:req beginfade:req endfade:req scriptstart:req
    setvar 0x1, 0
    setvar 0x2, 8
\beginfade:
    goto_if_eq \endfade, 0x1, 128, \scriptstart
    fadeto 0x1, COLOR_BLACK
    add 0x1, 0x2, 0x1
    yield
    goto \beginfade, \scriptstart
\endfade:
    setusertransform \x, \y, \z, \theta, TRANSFORM_ALL
    loadmap \map
    removefade
    .endm

    .macro jump_to time:req h:req x:req y:req z:req theta:req mask:req begin:req end:req scriptstart:req
    setvar 0x1, \time
    setvar 0x2, 1
    setvar 0x3, 0
    setuservalue ENTITY_PROP_STATE, ENTITY_STATE_JUMP
\begin:
    goto_if_eq \end, 0x1, 0, \scriptstart
    jump 0x1, \time, \h, \x, \y, \z, \theta, \mask
    sub 0x1, 0x2, 0x1
    yield
    goto \begin, \scriptstart
\end:
    setuservalue ENTITY_PROP_STATE, ENTITY_STATE_NORMAL
    .endm

    .macro getparticlesystem var:req
    loadvar \var, SYSVAR_LATEST_PARTICLE_SYSTEM
    .endm

    .macro getdynamicsystem var:req
    loadvar \var, SYSVAR_LATEST_DYNAMIC_OBJECT
    .endm
