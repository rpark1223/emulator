# um-emulator
Emulator for universal machine

I. Architecture:
   A. Our um_emulator's run function is called from main in order to run
      the whole program. The um_emulator module utilizes the Bitpack module to
      extract the relevant opcodes, registers, and values from each word read
      from the .um file; it keeps these secrets and uses them to decide which
      instruction to perform on which registers/segments in memory. The
      um_emulator module also hides the secret of how registers and the
      program counter are represented, which is as a C-array of uint32_ts.
      The actual execution of the instructions in memory occurs in our
      segments module, which hides the secret of how our memory is represented
      ; its functions are called by the um_emulator module accordingly. With
      regard to the secret of memory representation, segments hides the fact
      that our segments are represented as UArray_Ts in a Seq_T, and that
      their identifiers are contained in another Seq_T when unmapped.
