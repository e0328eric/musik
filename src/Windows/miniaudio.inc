extrn ma_decoder_init_file_w
extrn ma_decoder_uninit
extrn ma_device_config_init
extrn ma_device_init
extrn ma_device_uninit
extrn ma_device_start
extrn ma_device_stop
extrn ma_decoder_get_length_in_pcm_frames
extrn ma_decoder_get_cursor_in_pcm_frames
extrn ma_decoder_seek_to_pcm_frame
extrn ma_decoder_read_pcm_frames

MA_DECODER_SIZE = 552
MA_DEVICE_SIZE  = 3312

MA_SUCCESS      = 0

;; musik errorkind
MUSIK_OPEN_FILE_FAILED    = 1
MUSIK_INIT_DEVICE_FAILED  = 2
MUSIK_START_DEVICE_FAILED = 3
MUSIK_STOP_DEVICE_FAILED  = 4
MUSIK_ERROR_KIND_COUNT    = 5

section ".text" code readable executable

;; [in]  rcx <- filename (utf-16)
initMusik:
    push rcx
    invoke malloc, MA_DECODER_SIZE
    mov  QWORD [musik.decoder], rax
    invoke malloc, MA_DEVICE_SIZE
    mov  QWORD [musik.device], rax

    pop  rcx
    invoke ma_decoder_init_file_w, rcx, 0, QWORD [musik.decoder]
    cmp  rax, MA_SUCCESS
    jne  @F
    mov  r12, MUSIK_OPEN_FILE_FAILED
    jmp  musikInitFailed
@@:
    ret

deinitMusik:
    invoke free, QWORD [musik.device]
    invoke free, QWORD [musik.decoder]
    ret

section ".musikb" data readable writable
musik Musik
