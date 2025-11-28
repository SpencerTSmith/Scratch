global read_no_fetch_asm
global read_prefetch_asm

section .text

; Increment cachline buffer at every offset
; If offsets are random (and also within the buffer!) it should demonstrate some benefit of prefetching

; Linux calling convention
; *data in rdi
; *offsets in rsi
; offsets_count in rdx
; fake_op_count in rcx

read_no_fetch_asm:
  xor r8, r8 ; Will hold current index into offsets
  mov r10, rcx

  align 64
.loop:
  mov rcx, r10
  mov r9, [rsi + r8]  ; Grab offset
  lea r9, [rdi + r9]  ; add offset into *data

  vmovdqu ymm0, [r9]
  vmovdqu ymm1, [r9 + 32]

  ; Fake operation to take up time
.fake:
  vpaddd ymm0, ymm0, ymm1
  vpaddd ymm1, ymm1, ymm0

  sub rcx, 1
  jnz .fake

  add r8, 8
  sub rdx, 1
  jnz .loop

  ret

read_prefetch_asm:
  xor r8, r8 ; Will hold current index into offsets
  mov r10, rcx

  align 64
.loop:
  mov rcx, r10
  mov r9, [rsi + r8] ; Grab offset
  lea r9, [rdi + r9]  ; add offset into *data

  vmovdqu ymm0, [r9]
  vmovdqu ymm1, [r9 + 32]

  mov r9, [rsi + r8 + 8]
  prefetcht0 [rdi + r9]

  ; Fake operation to take up time
.fake:
  vpaddd ymm0, ymm0, ymm1
  vpaddd ymm1, ymm1, ymm0

  sub rcx, 1
  jnz .fake

  add r8, 8
  sub rdx, 1
  jnz .loop

  ret
