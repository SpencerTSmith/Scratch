global read_temporal_asm
global read_non_temporal_asm

section .text

; Linux calling convention. Out point in rdi, out count in rsi,
; in pointer in rdx, in count in rcx
; Counts must be multiples of each other and of a cachline

read_temporal_asm:
  mov rax, rdx ; rax will hold in pointer
  mov r9,  rcx  ; r9 will hold the in count

  ; Loop over, writing the buffer
  align 64
.outer:
  mov rdx, rax ; reset the in pointer
  mov rcx, r9  ; reset the in count

.inner:
  ; Load a cacheline from in
  vmovdqu ymm0, [rdx]
  vmovdqu ymm1, [rdx + 32]

  ; Put it into out
  vmovdqu [rdi],      ymm0
  vmovdqu [rdi + 32], ymm1

  add rdx, 64 ; advance in pointer
  add rdi, 64 ; advance out pointer

  sub rcx, 64
  jnz .inner

  sub rsi, r9
  jnz .outer

  ret

read_non_temporal_asm:
  mov rax, rdx ; rax will hold in pointer
  mov r9,  rcx  ; r9 will hold the in count

  ; Loop over, writing the buffer
  align 64
.outer:
  mov rdx, rax ; reset the in pointer
  mov rcx, r9  ; reset the in count

.inner:
  ; Load a cacheline from in
  vmovdqu ymm0, [rdx]
  vmovdqu ymm1, [rdx + 32]

  ; Put it into out
  vmovntdq [rdi],      ymm0
  vmovntdq [rdi + 32], ymm1

  add rdx, 64 ; advance in pointer
  add rdi, 64 ; advance out pointer

  sub rcx, 64
  jnz .inner

  sub rsi, r9
  jnz .outer

  ret
